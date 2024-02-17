///////////////////////////////////////////////////////////////////////////////////////////
// Filename: textureclass.cpp
///////////////////////////////////////////////////////////////////////////////////////////
#include "textureclass.h"

#include <comdef.h>
#include <iostream>
#include <D3DX11tex.h>
#include "../ImageReaders/ImageReader.h"


#pragma warning (disable : 4996)



///////////////////////////////////////////////////////////////////////////////////////////
//
//                             CONSTRUCTORS / DESTRUCTOR
//
///////////////////////////////////////////////////////////////////////////////////////////

TextureClass::TextureClass()
{
	// default constructor
}

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
		Log::Error(LOG_MACRO, e.what());
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

TextureClass::TextureClass(ID3D11Device* pDevice,
	const uint8_t* pData,
	const size_t size,
	const aiTextureType type)
{
	this->type_ = type;

	
	ImageReader imageReader;

	bool result = imageReader.LoadTextureFromMemory(pDevice,
		pData,
		size,
		&pTexture_,
		&pTextureView_);
	
}

TextureClass::TextureClass(const TextureClass & src)
{
	// using of the operator=
	*this = src;
}

TextureClass::~TextureClass()
{
	//_DELETE_ARR(pTextureName_);

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
	return this->type_;
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
	this->type_ = newType;
	return;
}



///////////////////////////////////////////////////////////////////////////////////////////
//
//                               PRIVATE FUNCTIONS
//
///////////////////////////////////////////////////////////////////////////////////////////



bool TextureClass::InitializeTextureFromFile(ID3D11Device* pDevice,
	const std::string & filePath,
	aiTextureType type)
{
	// Loads the texture file into the shader resource variable called pTextureResource_.
	// The texture can now be used to render with
	
	try
	{
		ImageReader imageReader;

		const bool result = imageReader.LoadTextureFromFile(filePath,
			pDevice,
			&pTexture_,
			&pTextureView_,
			textureWidth_,
			textureHeight_);


		// if we didn't manage to initialize a texture from the file
		if (!result)
		{
			// if it failed we create a 1x1 color texture for this texture object
			this->Initialize1x1ColorTexture(pDevice, Colors::UnloadedTextureColor, this->type_);
		}

		this->type_ = type;
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		Log::Error(LOG_MACRO, "can't initialize a texture from file");
		return false;
	}

	return true;

} // end InitializeTextureFromFile


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
