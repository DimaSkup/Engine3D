#include "ImageReader.h"

#include "DDS_ImageReader.h"
#include "TARGA_ImageReader.h"
#include "PNG_ImageReader.h"
#include "WICTextureLoader11.h"

#include "../Engine/StringHelper.h"
#include "../Engine/COMException.h"
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
	assert(!filePath.empty());
	assert(pDevice != nullptr);


	bool result = false;

	try
	{	
		std::string textureExt = StringHelper::GetFileExtension(filePath);

		// if we have a PNG image format
		if (textureExt == "png")
		{
			std::wstring wFilePath{ StringHelper::StringToWide(filePath) };

			HRESULT hr = DirectX::CreateWICTextureFromFile(pDevice,
				wFilePath.c_str(),
				ppTexture,
				ppTextureView);
			COM_ERROR_IF_FAILED(hr, "can't create a PNG texture from file: " + filePath);
		}
		// if we have a DirectDraw Surface (DDS) container format
		else if (textureExt == "dds")
		{
			std::unique_ptr<DDS_ImageReader> pDDS_ImageReader = std::make_unique<DDS_ImageReader>();

			result = pDDS_ImageReader->LoadTextureFromFile(filePath,
				pDevice,
				ppTexture,
				ppTextureView,
				textureWidth,
				textureHeight);

			COM_ERROR_IF_FALSE(result, "can't load a DDS texture");
		}
		// if we have a Targa file format
		else if (textureExt == "tga")
		{
			std::unique_ptr<TARGA_ImageReader> pTarga_ImageReader = std::make_unique<TARGA_ImageReader>();

			result = pTarga_ImageReader->LoadTextureFromFile(filePath,
				pDevice, 
				ppTexture, 
				ppTextureView, 
				textureWidth, 
				textureHeight);

			COM_ERROR_IF_FALSE(result, "can't load a Targa texture");
		}
		else
		{
			COM_ERROR_IF_FALSE(false, "UNKNOWN EXTENSION");
		}
	}
	catch (COMException & e)
	{

		std::string errorMsg{ "can't initialize the texture: " + filePath };
		Log::Error(e, true);
		Log::Error(THIS_FUNC, errorMsg.c_str());

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
	try
	{
		HRESULT hr = DirectX::CreateWICTextureFromMemory(pDevice,
			pData,
			size,
			ppTexture,
			ppTextureView);
		COM_ERROR_IF_FAILED(hr, "can't create a texture from memory");
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		return false;
	}



	return true;
}

// read an image data from the file by filePath and store it into the imageData array
bool ImageReader::ReadRawImageData(const std::string & filePath,
	const std::vector<BYTE> & imageData)
{
	return true;
}