// *********************************************************************************
// Filename: textureclass.cpp
// *********************************************************************************
#include "textureclass.h"

#include <D3DX11tex.h>
#include "ImageReader.h"
#include "../../ImageReader/Common/LIB_Exception.h"

#include "../Engine/Log.h"
#include "../Common/MemHelpers.h"
#include "../Common/Assert.h"


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
	const std::string & filePath) :
	path_(filePath)
{
	// create and initialize a texture from a file by filePath

	try
	{
		LoadFromFile(pDevice, filePath);
	}
	catch (EngineException & e)
	{
		Log::Error(e);
		throw EngineException("can't create a texture from file: " + filePath);
	}
}

///////////////////////////////////////////////////////////

TextureClass::TextureClass(
	const std::string& path,
	ID3D11Resource* pTexture,
	ID3D11ShaderResourceView* pTextureView,
	const UINT width,
	const UINT height) :
	path_(path),
	pTexture_(pTexture),
	pTextureView_(pTextureView),
	width_(width),
	height_(height)
{
	// check input params
	bool isValid = (!path.empty());
	isValid &= (pTexture != nullptr);
	isValid &= (pTextureView != nullptr);
	isValid &= (width && height);
	Assert::True(isValid, "input data isn't valid");
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
		Log::Error(e);
		throw EngineException("can't create a texture by input color data");
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
		Assert::NotNullptr(pColorData, "the input ptr to color data == nullptr");
		Assert::True((bool)(width & height), "texture dimensions must be greater that zero");

		InitializeColorTexture(pDevice, pColorData, width, height);
	}
	catch (EngineException& e)
	{
		Log::Error(e);
		throw EngineException("can't create a texture by given color data");
	}	
}

///////////////////////////////////////////////////////////

TextureClass::TextureClass(
	ID3D11Device* pDevice,
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
		ImgReader::ImageReader imageReader;
		ImgReader::ImageReader::DXTextureData texData(path, &pTexture_, &pTextureView_);

		imageReader.LoadTextureFromMemory(pDevice, pData, size, texData);

		width_ = texData.textureWidth;
		height_ = texData.textureHeight;
	}
	catch (ImgReader::LIB_Exception& e)
	{
		Log::Error(e.GetStr());

		// if we didn't manage to initialize texture's data from the memory
		// we create a 1x1 color texture for this texture object
		Initialize1x1ColorTexture(pDevice, Colors::UnloadedTextureColor);
	}
	catch (EngineException& e)
	{
		Log::Error(e);
		Log::Error("can't create an embedded compressed texture");

		Initialize1x1ColorTexture(pDevice, Colors::UnloadedTextureColor);
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
	SafeRelease(&pTexture_);
	SafeRelease(&pTextureView_);
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
	Assert::NotFailed(hr, "Failed to initialize texture from color data");

	// store a ptr to a new 2D texture
	pTexture_ = static_cast<ID3D11Texture2D*>(p2DTexture);

	// copy the data from the source texture
	pDeviceContext->CopyResource(pTexture_, src.pTexture_);

	
	// ----------------  CREATE A NEW SHADER RESOURCE VIEW  ------------------------

	CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	src.pTextureView_->GetDesc(&srvDesc);

	hr = pDevice->CreateShaderResourceView(pTexture_, &srvDesc, &pTextureView_);
	Assert::NotFailed(hr, "Failed to create shader resource view from texture generated from color data");

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
	// create a texture's resource and shader resource view loading texture data from the file
	try
	{
		ImgReader::ImageReader imageReader;
		ImgReader::ImageReader::DXTextureData data(filePath, &pTexture_, &pTextureView_);

		imageReader.LoadTextureFromFile(pDevice, data);

		path_ = filePath;
		width_ = data.textureWidth;
		height_ = data.textureHeight;
	}
	catch (ImgReader::LIB_Exception& e)
	{
		Log::Error(e.GetStr());

		// if we didn't manage to initialize a texture from the file 
		// we create a 1x1 color texture for this texture object
		Initialize1x1ColorTexture(pDevice, Colors::UnloadedTextureColor);
	}
	catch (EngineException & e)
	{
		Log::Error(e);
		throw EngineException("can't initialize a texture from file");
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

void TextureClass::InitializeColorTexture(
	ID3D11Device* pDevice,
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
	Assert::NotFailed(hr, "Failed to initialize texture from color data");

	// store a ptr to the 2D texture 
	pTexture_ = static_cast<ID3D11Texture2D*>(p2DTexture);

	// setup description for a shader resource view (SRV)
	CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc(D3D11_SRV_DIMENSION_TEXTURE2D, textureDesc.Format);

	// create a new SRV from texture
	hr = pDevice->CreateShaderResourceView(pTexture_, &srvDesc, &pTextureView_);
	Assert::NotFailed(hr, "Failed to create shader resource view from texture generated from color data");
}

///////////////////////////////////////////////////////////
