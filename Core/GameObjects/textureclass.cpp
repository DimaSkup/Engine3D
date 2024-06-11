// *********************************************************************************
// Filename: textureclass.cpp
// *********************************************************************************
#include "textureclass.h"

//#include <comdef.h>
//#include <iostream>
#include <D3DX11tex.h>
#include "../ImageReaders/ImageReader.h"


#pragma warning (disable : 4996)



// *********************************************************************************
//
//                             CONSTRUCTORS / DESTRUCTOR
//
// *********************************************************************************

TextureClass::TextureClass()
{
	// default constructor
}

///////////////////////////////////////////////////////////

TextureClass::TextureClass(
	ID3D11Device* pDevice, 
	const std::string & filePath, 
	const aiTextureType type)
{
	// create and initialize a texture using its filePath

	// check input params
	ASSERT_NOT_NULLPTR(pDevice, "the input ptr to the device == nullptr");
	ASSERT_TRUE(!filePath.empty(), "the input file path is empty");

	try
	{
		LoadTextureFromFile(pDevice, filePath, type);
	}
	catch (EngineException & e)
	{
		Log::Error(e, true);
		ASSERT_TRUE(false, "can't create a texture from file: " + filePath);
	}
}

///////////////////////////////////////////////////////////

TextureClass::TextureClass(
	ID3D11Device* pDevice, 
	const Color & color, 
	const aiTextureType type)
{
	// THIS FUNC creates 1x1 texture with input color value

	// check input params
	ASSERT_NOT_NULLPTR(pDevice, "the input ptr to the device == nullptr");

	try
	{
		Initialize1x1ColorTexture(pDevice, color, type);
	}
	catch (EngineException& e)
	{
		Log::Error(e, true);
		ASSERT_TRUE(false, "can't create a texture by given color data");
	}
}

///////////////////////////////////////////////////////////

TextureClass::TextureClass(
	ID3D11Device* pDevice, 
	const Color* pColorData, 
	const UINT width,
	const UINT height,
	const aiTextureType type)
{
	// THIS FUNC creates width_x_height texture with input color data

	// check input params
	ASSERT_NOT_NULLPTR(pDevice, "the input ptr to the device == nullptr");
	ASSERT_NOT_NULLPTR(pColorData, "the input ptr to color data == nullptr");
	ASSERT_NOT_ZERO(width, "the width of texture must be greater that zero");
	ASSERT_NOT_ZERO(height, "the height of texture must be greater that zero");

	try
	{
		InitializeColorTexture(pDevice, pColorData, width, height, type);
	}
	catch (EngineException& e)
	{
		Log::Error(e, true);
		ASSERT_TRUE(false, "can't create a texture by given color data");
	}	
}

///////////////////////////////////////////////////////////

TextureClass::TextureClass(ID3D11Device* pDevice,
	const uint8_t* pData,
	const size_t size,
	const aiTextureType type)
{
	// THIS FUNC creates a texture by input raw color data (pData)

	// check input params
	ASSERT_NOT_NULLPTR(pDevice, "the input ptr to the device == nullptr");
	ASSERT_NOT_NULLPTR(pData, "the input ptr to the data == nullptr");

	type_ = type;

	ImageReader imageReader;

	bool result = imageReader.LoadTextureFromMemory(pDevice,
		pData,
		size,
		&pTexture_,
		&pTextureView_);
	ASSERT_TRUE(result, "can't load a texture from memory");
}

TextureClass::TextureClass(const TextureClass & src)
{
	// using of the operator=
	*this = src;
}

TextureClass::~TextureClass()
{
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


	// -----------------------  CREATE A NEW TEXTURE  ------------------------------

	CD3D11_TEXTURE2D_DESC textureDesc;
	ID3D11Texture2D* p2DTexture = nullptr;

	// get the description of the another texture
	static_cast<ID3D11Texture2D*>(src.pTexture_)->GetDesc(&textureDesc);

	// create a new 2D texture
	HRESULT hr = pDevice->CreateTexture2D(&textureDesc, nullptr, &p2DTexture);
	ASSERT_NOT_FAILED(hr, "Failed to initialize texture from color data");

	// store a ptr to a new 2D texture
	this->pTexture_ = static_cast<ID3D11Texture2D*>(p2DTexture);

	// copy the data from the source texture
	pDeviceContext->CopyResource(this->pTexture_, src.pTexture_);

	
	// ----------------  CREATE A NEW SHADER RESOURCE VIEW  ------------------------

	CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	src.pTextureView_->GetDesc(&srvDesc);

	hr = pDevice->CreateShaderResourceView(this->pTexture_, &srvDesc, &pTextureView_);
	ASSERT_NOT_FAILED(hr, "Failed to create shader resource view from texture generated from color data");

	return *this;

}

///////////////////////////////////////////////////////////

ID3D11ShaderResourceView* TextureClass::GetTextureResourceView() const
{
	// Called by other objects that need access to the texture shader resource so that
	// they can use the texture for rendering
	return pTextureView_;
}

ID3D11ShaderResourceView* const* TextureClass::GetTextureResourceViewAddress() const
{
	return &pTextureView_;
}

#if 0
WCHAR* TextureClass::GetName() const
{
	return pTextureName_;
}
#endif

///////////////////////////////////////////////////////////

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
	return type_;
}

///////////////////////////////////////////////////////////

POINT TextureClass::GetTextureSize()
{
	ID3D11Texture2D* p2DTexture = static_cast<ID3D11Texture2D*>(pTexture_);
	D3D11_TEXTURE2D_DESC desc;
	
	p2DTexture->GetDesc(&desc);
	
	return { (LONG)desc.Width, (LONG)desc.Height };
}

///////////////////////////////////////////////////////////

void TextureClass::SetType(aiTextureType newType)
{
	// change a type of the texture
	type_ = newType;
	return;
}



///////////////////////////////////////////////////////////////////////////////////////////
//
//                               PRIVATE FUNCTIONS
//
///////////////////////////////////////////////////////////////////////////////////////////



void TextureClass::LoadTextureFromFile(ID3D11Device* pDevice,
	const std::string & filePath,
	const aiTextureType type)
{
	// Loads the texture file into the shader resource variable called pTextureResource_.
	// The texture can now be used to render with
	
	try
	{
		type_ = type;

		ImageReader imageReader;

		const bool result = imageReader.LoadTextureFromFile(filePath,
			pDevice,
			&pTexture_,
			&pTextureView_,
			textureWidth_,
			textureHeight_);

		// if we didn't manage to initialize a texture from the file ...
		if (!result)
		{
			// ... we create a 1x1 color texture for this texture object
			Initialize1x1ColorTexture(pDevice, Colors::UnloadedTextureColor, type_);
		}
	}
	catch (EngineException & e)
	{
		Log::Error(e, false);
		ASSERT_TRUE(false, "can't initialize a texture from file");
	}
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
	const UINT width,
	const UINT height,
	const aiTextureType type)
{
	// Initialize a color texture using input color data (pColorData) and
	// the input width/height

	type_ = type;
	textureWidth_ = width;
	textureHeight_ = height;

	ID3D11Texture2D* p2DTexture = nullptr;
	D3D11_TEXTURE2D_DESC textureDesc;
	D3D11_SUBRESOURCE_DATA initialData{};

	// setup description for this texture
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.ArraySize = 1;
	textureDesc.MipLevels = 0;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.MiscFlags = 0;

	// setup initial data for this texture
	initialData.pSysMem = pColorData;
	initialData.SysMemPitch = width * sizeof(Color);

	// create a new 2D texture
	HRESULT hr = pDevice->CreateTexture2D(&textureDesc, &initialData, &p2DTexture);
	ASSERT_NOT_FAILED(hr, "Failed to initialize texture from color data");

	// store a ptr to the 2D texture 
	pTexture_ = static_cast<ID3D11Texture2D*>(p2DTexture);

	// setup description for a shader resource view (SRV)
	CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc(D3D11_SRV_DIMENSION_TEXTURE2D, textureDesc.Format);

	// create a new SRV from texture
	hr = pDevice->CreateShaderResourceView(pTexture_, &srvDesc, &pTextureView_);
	ASSERT_NOT_FAILED(hr, "Failed to create shader resource view from texture generated from color data");

	return;

}

///////////////////////////////////////////////////////////
