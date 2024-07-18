// *********************************************************************************
// Filename: textureclass.cpp
// *********************************************************************************
#include "textureclass.h"


#include <D3DX11tex.h>
#include "ImageReader.h"

#include "../Engine/macros.h"
#include "../Engine/Log.h"


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
	const std::string & filePath)
	:
	path_(filePath)
{
	// create and initialize a texture from a file by filePath

	try
	{
		ASSERT_NOT_EMPTY(filePath.empty(), "the input file path is empty");
		LoadFromFile(pDevice, filePath);
	}
	catch (EngineException & e)
	{
		Log::Error(e, true);
		THROW_ERROR("can't create a texture from file: " + filePath);
	}
}

///////////////////////////////////////////////////////////

TextureClass::TextureClass(
	ID3D11Device* pDevice, 
	const Color & color)
{
	// THIS FUNC creates a 1x1 texture with input color value

	try
	{
		Initialize1x1ColorTexture(pDevice, color);
	}
	catch (EngineException& e)
	{
		Log::Error(e, true);
		THROW_ERROR("can't create a texture by input color data");
	}
}

///////////////////////////////////////////////////////////

TextureClass::TextureClass(
	ID3D11Device* pDevice, 
	const Color* pColorData, 
	const UINT width,
	const UINT height)
{
	// THIS FUNC creates a width_x_height texture with input color data

	try
	{
		ASSERT_NOT_NULLPTR(pColorData, "the input ptr to color data == nullptr");
		ASSERT_TRUE((bool)(width & height), "texture dimensions must be greater that zero");

		InitializeColorTexture(pDevice, pColorData, width, height);
	}
	catch (EngineException& e)
	{
		Log::Error(e, true);
		THROW_ERROR("can't create a texture by given color data");
	}	
}

///////////////////////////////////////////////////////////

TextureClass::TextureClass(ID3D11Device* pDevice,
	const std::string& path,
	const uint8_t* pData,
	const size_t size)
	:
	path_(path)
{
	// a constructor for loading embedded compressed textures;
	// 
	// source texture can be compressed or embedded so we firstly load its 
	// content into the memory and then passed here ptr to this loaded data;


	try
	{
		ASSERT_NOT_EMPTY(path.empty(), "path to the compressed/embedded texture is empty");
		ASSERT_NOT_NULLPTR(pData, "the input ptr to the data == nullptr");
		ASSERT_NOT_ZERO(size, "the input size == 0");

		ImageReader imageReader;

		bool result = imageReader.LoadTextureFromMemory(pDevice,
			pData,
			size,
			&pTexture_,
			&pTextureView_);
		ASSERT_TRUE(result, "can't load a texture from memory");
	}
	catch (EngineException& e)
	{
		Log::Error(e, true);
		THROW_ERROR("can't create an embedded compressed texture");
	}
}

///////////////////////////////////////////////////////////

TextureClass::TextureClass(const TextureClass & src)
{
	// using of the operator=
	*this = src;
}

///////////////////////////////////////////////////////////

TextureClass::~TextureClass()
{
	_RELEASE(pTexture_);
	_RELEASE(pTextureView_);   
}




// ***********************************************************************************
//
//                              PUBLIC FUNCTIONS
//
// ***********************************************************************************


TextureClass & TextureClass::operator=(const TextureClass & src)
{
	// guard self assignment
	if (this == &src)
		return *this;

	ID3D11Device* pDevice = nullptr;
	ID3D11DeviceContext* pDeviceContext = nullptr;

	// copy common data of the texture
	path_ = src.path_;
	width_ = src.width_;
	height_ = src.height_;

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
	pTexture_ = static_cast<ID3D11Texture2D*>(p2DTexture);

	// copy the data from the source texture
	pDeviceContext->CopyResource(pTexture_, src.pTexture_);

	
	// ----------------  CREATE A NEW SHADER RESOURCE VIEW  ------------------------

	CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	src.pTextureView_->GetDesc(&srvDesc);

	hr = pDevice->CreateShaderResourceView(pTexture_, &srvDesc, &pTextureView_);
	ASSERT_NOT_FAILED(hr, "Failed to create shader resource view from texture generated from color data");

	return *this;

}

///////////////////////////////////////////////////////////

POINT TextureClass::GetTextureSize()
{
	ID3D11Texture2D* p2DTexture = static_cast<ID3D11Texture2D*>(pTexture_);
	D3D11_TEXTURE2D_DESC desc;
	
	p2DTexture->GetDesc(&desc);
	
	return { (LONG)desc.Width, (LONG)desc.Height };
}


// ************************************************************************************
//
//                            PRIVATE FUNCTIONS
//
// ************************************************************************************

void TextureClass::LoadFromFile(
	ID3D11Device* pDevice,
	const std::string & filePath)
{
	// Loads the texture file into the shader resource variable called pTextureResource_.
	// The texture can now be used to render with
	
	try
	{
		path_ = filePath;

		ImageReader imageReader;

		const bool result = imageReader.LoadTextureFromFile(filePath,
			pDevice,
			&pTexture_,
			&pTextureView_,
			width_,
			height_);

		// if we didn't manage to initialize a texture from the file ...
		if (!result)
		{
			// ... we create a 1x1 color texture for this texture object
			Initialize1x1ColorTexture(pDevice, Colors::UnloadedTextureColor);
		}
	}
	catch (EngineException & e)
	{
		Log::Error(e, true);
		ASSERT_TRUE(false, "can't initialize a texture from file");
	}
}


///////////////////////////////////////////////////////////

void TextureClass::Initialize1x1ColorTexture(
	ID3D11Device* pDevice,
	const Color & colorData)
{
	InitializeColorTexture(pDevice, &colorData, 1, 1);
}

///////////////////////////////////////////////////////////

void TextureClass::InitializeColorTexture(ID3D11Device* pDevice,
	const Color* pColorData,
	const UINT width,
	const UINT height)
{
	// Initialize a color texture using input color data (pColorData) and
	// the input width/height

	width_ = width;
	height_ = height;

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
}

///////////////////////////////////////////////////////////
