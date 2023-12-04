///////////////////////////////////////////////////////////////////////////////////////////
// Filename: textureclass.cpp
///////////////////////////////////////////////////////////////////////////////////////////
#include "textureclass.h"
#include <iostream>
#include <vector>

#pragma warning (disable : 4996)



///////////////////////////////////////////////////////////////////////////////////////////
//
//                             CONSTRUCTORS / DESTRUCTOR
//
///////////////////////////////////////////////////////////////////////////////////////////


TextureClass::TextureClass(ID3D11Device* pDevice, const std::string & filePath, aiTextureType type)
{
	// create and initialize a texture using its filePath

	// check input params
	COM_ERROR_IF_NULLPTR(pDevice, "the input ptr to the device == nullptr");
	COM_ERROR_IF_FALSE(!filePath.empty(), "the input file path is empty");

	try
	{
		this->InitializeTextureFromFile(pDevice, filePath, type);
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(THIS_FUNC, e.what());
		COM_ERROR_IF_FALSE(false, "can't allocate memory for the texture class elements");
	}
	
}

TextureClass::TextureClass(ID3D11Device* pDevice, const Color & color, aiTextureType type)
{
	this->Initialize1x1ColorTexture(pDevice, color, type);
}

TextureClass::TextureClass(ID3D11Device* pDevice, const Color* pColorData, UINT width, UINT height, aiTextureType type)
{
	this->InitializeColorTexture(pDevice, pColorData, width, height, type);
}

TextureClass::TextureClass(const TextureClass & src)
{
	// using of the operator=
	*this = src;
}

TextureClass::~TextureClass()
{
	_DELETE_ARR(pTargaData_);
	_DELETE_ARR(pTextureName_);

	//_RELEASE(pTextureResource_);
	_RELEASE(pTexture_);
	_RELEASE(pTextureView_);   
}




///////////////////////////////////////////////////////////////////////////////////////////
//
//                                PUBLIC FUNCTIONS
//
///////////////////////////////////////////////////////////////////////////////////////////


TextureClass & TextureClass::operator=(const TextureClass & src)
{
	// guard self assignment
	if (this == &src)
		return *this;

	ID3D11Device* pDevice = nullptr;
	ID3D11DeviceContext* pDeviceContext = nullptr;

	// copy common data of the texture
	type_ = src.type_;
	textureWidth_ = src.textureWidth_;
	textureHeight_ = src.textureHeight_;

	// get ptrs to the device and device context
	src.pTexture_->GetDevice(&pDevice);
	pDevice->GetImmediateContext(&pDeviceContext);


	/////////////////////////  CREATE A NEW TEXTURE  /////////////////////////

	CD3D11_TEXTURE2D_DESC textureDesc;
	ID3D11Texture2D* p2DTexture = nullptr;

	// get the description of the another texture
	static_cast<ID3D11Texture2D*>(src.pTexture_)->GetDesc(&textureDesc);

	// create a new 2D texture
	HRESULT hr = pDevice->CreateTexture2D(&textureDesc, nullptr, &p2DTexture);
	COM_ERROR_IF_FAILED(hr, "Failed to initialize texture from color data");

	// store a ptr to a new 2D texture
	pTexture_ = static_cast<ID3D11Texture2D*>(p2DTexture);

	// copy the data from the source texture
	pDeviceContext->CopyResource(this->pTexture_, src.pTexture_);

	
	/////////////////////  CREATE A NEW SHADER RESOURCE VIEW  //////////////////////

	CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	src.pTextureView_->GetDesc(&srvDesc);

	hr = pDevice->CreateShaderResourceView(this->pTexture_, &srvDesc, &pTextureView_);
	COM_ERROR_IF_FAILED(hr, "Failed to create shader resource view from texture generated from color data");

	return *this;

} // end operator=

///////////////////////////////////////////////////////////

ID3D11ShaderResourceView* TextureClass::GetTextureResourceView() const
{
	// Called by other objects that need access to the texture shader resource so that
	// they can use the texture for rendering
	return pTextureView_;
}

ID3D11ShaderResourceView** TextureClass::GetTextureResourceViewAddress()
{
	return &pTextureView_;
}

WCHAR* TextureClass::GetName() const
{
	return pTextureName_;
}

// return the width of the texture
UINT TextureClass::GetWidth() const
{
	return textureWidth_;
}

UINT TextureClass::GetHeight() const
{
	return textureHeight_;
}

aiTextureType TextureClass::GetType() const
{
	return this->type_;
}


POINT TextureClass::GetTextureSize()
{
	ID3D11Texture2D* p2DTexture = static_cast<ID3D11Texture2D*>(pTexture_);
	D3D11_TEXTURE2D_DESC desc;
	
	p2DTexture->GetDesc(&desc);
	
	return { (LONG)desc.Width, (LONG)desc.Height };
}




///////////////////////////////////////////////////////////////////////////////////////////
//
//                               PRIVATE FUNCTIONS
//
///////////////////////////////////////////////////////////////////////////////////////////



bool TextureClass::InitializeTextureFromFile(ID3D11Device* pDevice,
	//const WCHAR* filename,
	const std::string & filePath,
	aiTextureType type)
{
	// Loads the texture file into the shader resource variable called pTextureResource_.
	// The texture can now be used to render with
	
	//pTextureName_ = new WCHAR[100]{ L'\0' };  // maximum symbols = 100
	bool result = false;

	try
	{
		std::string textureExt = GetTextureExtension(filePath);

		// if we have a DirectDraw Surface (DDS) container format
		if (textureExt == "dds")
		{
			result = LoadDDSTexture(filename, pDevice);
			COM_ERROR_IF_FALSE(result, "can't load a DDS texture");
		}
		// if we have a Targa file format
		else if (textureExt == "tga")
		{
			result = LoadTargaTexture(filename, pDevice);
			COM_ERROR_IF_FALSE(result, "can't load a Targa texture");
		}
	}
	catch (COMException & e)
	{
		std::string errorMsg{ "can't initialize the texture: " + StringHelper::ToString(filename) };
		Log::Error(e, true);
		Log::Error(THIS_FUNC, errorMsg.c_str());

		return false;
	}




	// initialize the texture name
	wcscpy(pTextureName_, filename);

	return true;

} // end InitializeTextureFromFile

///////////////////////////////////////////////////////////

bool TextureClass::LoadDDSTexture(const WCHAR* filename, ID3D11Device* pDevice)
{
	// Load the texture in
	HRESULT hr = D3DX11CreateShaderResourceViewFromFile(pDevice, filename,
		nullptr, nullptr,
		&pTextureView_, nullptr);


	if (FAILED(hr))
	{
		Log::Error("%s() (%d): %s %S", __FUNCTION__, __LINE__,
			"can't create the shader resource view from the file: ", filename);
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////

bool TextureClass::LoadTargaTexture(const std::string & filename,
	ID3D11Device* pDevice)
{
	HRESULT hr = S_OK;
	bool result = false;

	UINT width = 0;
	UINT height = 0;
	UINT rowPitch = 0;

	D3D11_TEXTURE2D_DESC textureDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

	ID3D11Texture2D* pTexture_ = nullptr;
	ID3D11DeviceContext* pDeviceContext = nullptr;

	// ----------------------------------------------------- //

	// get the device context
	pDevice->GetImmediateContext(&pDeviceContext);

	// load the targa image data into memory (into the pTargaData_ array) 
	result = LoadTarga32Bit(strFilename.c_str());

	// next we need to setup our description of the DirectX texture that we will load
	// the Targa data into. We use the height and width from the Targa image data, and 
	// set the format to be a 32-bit RGBA texture. We set the SampleDesc to default.
	// Then we set the Usage to D3D11_USAGE_DEFAULT which is better performing memory.
	// And finally, we set the MipLevels, BindFlags, and MiscFlags to the settings 
	// required for Mipmapped textures. Once the description is complete, we call
	// CreateTexture2D() to create an empty texture for us. The next step will be to 
	// copy the Targa data into that empty texture.

	textureDesc.Width = textureWidth_;
	textureDesc.Height = textureHeight_;
	textureDesc.MipLevels = 0;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	// create the empty texture
	hr = pDevice->CreateTexture2D(&textureDesc, nullptr, &pTexture_);
	COM_ERROR_IF_FAILED(hr, "can't create the empty texture: " + strFilename);

	// set the row pitch of the targa image data
	rowPitch = (textureWidth_ * 4) * sizeof(UCHAR);

	// copy the targa image data into the texture
	pDeviceContext->UpdateSubresource(pTexture_, 0, nullptr, pTargaData_, rowPitch, 0);

	// setup the shader resource view description
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	// after the texture is loaded, we create a shader resource view which allows us to have
	// a pointer to set the texture in shaders.
	hr = pDevice->CreateShaderResourceView(pTexture_, &srvDesc, &pTextureView_);
	COM_ERROR_IF_FAILED(hr, "can't create the shader resource view: " + strFilename);

	// generate mipmaps for this texture
	pDeviceContext->GenerateMips(pTextureView_);

	// release the targa image data now that the image data has been loaded into the texture.
	_DELETE_ARR(pTargaData_);
	_RELEASE(pTexture_);

	return true;
}

///////////////////////////////////////////////////////////

bool TextureClass::LoadTarga32Bit(const char* filename)
{
	// this is a Targa image loading function. NOTE that Targa images are stored upside down
	// and need to be flipped before using. So here we will open the file, read it into
	// an array, and then take that array data and load it into the pTargaData_ array in
	// the correct order. Note we are purposely only dealing with 32-bit Targa files that
	// have alpha channels, this function will reject Targa's that are saved as 24-bit


	errno_t error = -1;
	//UCHAR bpp = 0;           // bites per pixel (supposed to be 32)
	UINT imageSize = 0;
	UINT index = 0;          // index into the targa destination data array (for the pTargaData_ array)
	UINT k = 0;              // index into the targa image data (for the pTargaImage array)
	UINT count = 0;

	TargaHeader targaFileHeader;
	UCHAR* pTargaImage = nullptr;
	FILE* pFile = nullptr;
	std::string strFilename{ filename };

	try
	{
		// open the targa file for reading in binary
		error = fopen_s(&pFile, filename, "rb");
		COM_ERROR_IF_FALSE(error == 0, "can't open the targa file for reading in binary: " + strFilename);

		// read in the file header
		count = static_cast<UINT>(fread(&targaFileHeader, sizeof(TargaHeader), 1, pFile));
		COM_ERROR_IF_FALSE(count == 1, "can't read in the file header: " + strFilename);

		// get the important information from the header
		textureWidth_ = static_cast<UINT>(targaFileHeader.width);
		textureHeight_ = static_cast<UINT>(targaFileHeader.height);
		//bpp = targaFileHeader.bpp;

		// check that it is 32 bit and not 24 bit
		COM_ERROR_IF_FALSE(targaFileHeader.bpp == static_cast<UCHAR>(32), "this targa texture is not 32-bit: " + strFilename);

		// calculate the size of the 32 bit image data
		imageSize = textureWidth_ * textureHeight_ * 4;

		try
		{
			// allocate memory for the targa image data
			pTargaImage = new UCHAR[imageSize]{ 0 };

			// allocate memory for the targa destination data
			pTargaData_ = new UCHAR[imageSize]{ 0 };
		}
		catch (std::bad_alloc & e)
		{
			Log::Error(THIS_FUNC, e.what());
			Log::Error(THIS_FUNC, "can't allocate memory for the target image data");
			return false;
		}

		// read in the targa image data
		count = static_cast<UINT>(fread(pTargaImage, 1, imageSize, pFile));
		COM_ERROR_IF_FALSE(count == imageSize, "can't read in the targa image data from file: " + strFilename);

		// close the file
		error = fclose(pFile);
		COM_ERROR_IF_FALSE(error == 0, "can't close the file: " + strFilename);

		

		// setup the index into the targa image data
		k = (imageSize) - (textureWidth_ * 4);

		// now copy the targa image data into the targa destination array in the correct
		// order since the targa format is stored upside down and also is not in RGBA order.
		for (UINT j = 0; j < textureHeight_; j++)
		{
			for (UINT i = 0; i < textureWidth_; i++)
			{
				pTargaData_[index + 0] = pTargaImage[k + 2];  // red
				pTargaData_[index + 1] = pTargaImage[k + 1];  // green
				pTargaData_[index + 2] = pTargaImage[k + 0];  // blue
				pTargaData_[index + 3] = pTargaImage[k + 3];  // alpha

				// increment the indexes into the targa data
				k += 4;
				index += 4;
			}

			// set the targa image data index back to the preceding row at the beginning
			// of the column since its reading is upside down
			k -= (textureWidth_ * 8);
		}

		// release the targa image data now that it was copied into the destination array
		_DELETE_ARR(pTargaImage);
	}
	catch (COMException & e)
	{
		_DELETE_ARR(pTargaData_);
		_DELETE_ARR(pTargaImage);
		fclose(pFile);              // close the targa file

		Log::Error(e, true);
		return false;
	}
	catch (std::bad_alloc & e)
	{
		_DELETE_ARR(pTargaData_);
		_DELETE_ARR(pTargaImage);
		fclose(pFile);              // close the targa file

		Log::Error(THIS_FUNC, e.what());
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////

void TextureClass::Initialize1x1ColorTexture(ID3D11Device* pDevice,
	const Color & colorData,
	aiTextureType type)
{
	InitializeColorTexture(pDevice, &colorData, 1, 1, type);
}

///////////////////////////////////////////////////////////

void TextureClass::InitializeColorTexture(ID3D11Device* pDevice,
	const Color* pColorData,
	UINT width,
	UINT height,
	aiTextureType type)
{
	// check input params
	COM_ERROR_IF_NULLPTR(pDevice, "ptr to device == nullptr");
	COM_ERROR_IF_NULLPTR(pColorData, "ptr to color data == nullptr");

	this->type_ = type;
	this->textureWidth_ = width;
	this->textureHeight_ = height;

	CD3D11_TEXTURE2D_DESC textureDesc(DXGI_FORMAT_R8G8B8A8_UNORM, width, height);
	ID3D11Texture2D* p2DTexture = nullptr;
	D3D11_SUBRESOURCE_DATA initialData{};

	initialData.pSysMem = pColorData;
	initialData.SysMemPitch = width * sizeof(Color);

	HRESULT hr = pDevice->CreateTexture2D(&textureDesc, &initialData, &p2DTexture);
	COM_ERROR_IF_FAILED(hr, "Failed to initialize texture from color data");

	// store a ptr to the 2D texture 
	pTexture_ = static_cast<ID3D11Texture2D*>(p2DTexture);

	CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc(D3D11_SRV_DIMENSION_TEXTURE2D, textureDesc.Format);

	hr = pDevice->CreateShaderResourceView(pTexture_, &srvDesc, &pTextureView_);
	COM_ERROR_IF_FAILED(hr, "Failed to create shader resource view from texture generated from color data");

	return;

} // end InitializeColorTexture

///////////////////////////////////////////////////////////