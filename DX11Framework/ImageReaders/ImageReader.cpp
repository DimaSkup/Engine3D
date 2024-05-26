// *********************************************************************************
// Filename:    ImageReader.h
// Description: constains implementation of functional for the ImageReader class;
// *********************************************************************************
#include "ImageReader.h"

// image readers for different types
#include "DDS_ImageReader.h"
#include "TARGA_ImageReader.h"
#include "PNG_ImageReader.h"
#include "WICTextureLoader11.h"
#include "BMP_Image.h"

#include "../Engine/StringHelper.h"
#include "../Engine/EngineException.h"
#include "../Engine/log.h"


ImageReader::ImageReader()
{
}

bool ImageReader::LoadTextureFromFile(const std::string & filePath,
	ID3D11Device* pDevice,
	ID3D11Resource** ppTexture,
	ID3D11ShaderResourceView** ppTextureView,
	UINT & textureWidth,
	UINT & textureHeight)
{
	// check input params
	ASSERT_NOT_EMPTY(filePath.empty(), "path to the image is empty");
	ASSERT_NOT_NULLPTR(pDevice, "ptr to the device == nullptr");
	ASSERT_NOT_NULLPTR(ppTexture, "double ptr to the texture resource == nullptr");
	ASSERT_NOT_NULLPTR(ppTextureView, "double ptr to the texture view == nullptr");

	try
	{	
		const std::string textureExt = StringHelper::GetFileExtension(filePath);

		// if we have a PNG image format
		if (textureExt == "png")
		{
			const std::wstring wFilePath{ StringHelper::StringToWide(filePath) };

			const HRESULT hr = DirectX::CreateWICTextureFromFile(pDevice,
				wFilePath.c_str(),
				ppTexture,
				ppTextureView);
			ASSERT_NOT_FAILED(hr, "can't create a PNG texture from file: " + filePath);
		}

		// if we have a DirectDraw Surface (DDS) container format
		else if (textureExt == "dds")
		{
			DDS_ImageReader DDS_ImageReader;

			//const HRESULT hr = DirectX::CreateWICTextureFromFile
			bool result = DDS_ImageReader.LoadTextureFromFile(filePath,
				pDevice,
				ppTexture,
				ppTextureView,
				textureWidth,
				textureHeight);
			
			ASSERT_TRUE(result, "can't load a DDS texture");
		}

		// if we have a Targa file format
		else if (textureExt == "tga")
		{
			TARGA_ImageReader targa_ImageReader;

			bool result = targa_ImageReader.LoadTextureFromFile(filePath,
				pDevice, 
				ppTexture, 
				ppTextureView, 
				textureWidth, 
				textureHeight);

			ASSERT_TRUE(result, "can't load a Targa texture");
		}

		// if we have a bitmap image file
		else if (textureExt == "bmp")
		{
			BMP_Image bmp_Image;

			bool result = bmp_Image.LoadTextureFromFile(filePath,
				pDevice,
				ppTexture,
				ppTextureView,
				textureWidth,
				textureHeight);
			ASSERT_TRUE(result, "can't load a BMP texture");
		}

		else
		{
			ASSERT_TRUE(false, "UNKNOWN IMAGE EXTENSION");
		}
	}
	catch (EngineException & e)
	{
		Log::Error(e, true);
		Log::Error(LOG_MACRO, "can't initialize the texture: " + filePath);
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////

bool ImageReader::LoadTextureFromMemory(ID3D11Device* pDevice,
	const uint8_t* pData,
	const size_t size,
	ID3D11Resource** ppTexture,
	ID3D11ShaderResourceView** ppTextureView)
{
	// check input params
	assert(pDevice != nullptr);
	assert(pData != nullptr);
	assert(size > 0);
	assert(ppTexture != nullptr);
	assert(ppTextureView != nullptr);

	try
	{
		HRESULT hr = DirectX::CreateWICTextureFromMemory(pDevice,
			pData,
			size,
			ppTexture,
			ppTextureView);
		ASSERT_NOT_FAILED(hr, "can't create a texture from memory");
	}
	catch (EngineException & e)
	{
		Log::Error(e, false);
		return false;
	}

	return true;
}


#if 0
bool ImageReader::ReadRawImageData(const std::string & filePath,
	UINT & imageWidth,
	UINT & imageHeight,
	std::vector<uint8_t> & imageData)
{
	// read an image data from the file by filePath and
	// store it into the imageData array

	try
	{
		ASSERT_NOT_ZERO(filePath.length(), "the input path to the image file is empty");

		const std::string textureExt = StringHelper::GetFileExtension(filePath);

		// if we have a bitmap image file
		if (textureExt == "bmp")
		{
			BMP_Image bmp_Image;

			// read the bmp image from the file
			bmp_Image.Read(filePath);
			
			bmp_Image.CopyRawDataInto(imageData); 
			bmp_Image.GetDimensions(imageWidth, imageHeight);
		}
		else
		{
			ASSERT_TRUE(false, "UNKNOWN IMAGE FORMAT");
		}
	}
	catch (EngineException & e)
	{
		Log::Error(e, true);
		return false;
	}

	return true;
}

#endif