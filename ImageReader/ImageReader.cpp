// *********************************************************************************
// Filename:    ImageReader.cpp
// Description: constains implementation of functional for the ImageReader class;
// *********************************************************************************
#include "ImageReader.h"

#include "Common/StringHelper.h"
#include "Common/LIB_Exception.h"
#include "Common/log.h"
#include "Common/Assert.h"

#include <d3dx11tex.h>

namespace ImgReader
{

// ************************************************************************************
// 
//                            PUBLIC METHODS
// 
// ************************************************************************************


void ImageReader::LoadTextureFromFile(ID3D11Device* pDevice, DXTextureData& texData)
{
	try
	{	
		CheckInputParams(texData);

		const std::string textureExt = StringHelper::GetFileExtension(texData.filePath);

		if (textureExt == "png")
		{
			LoadPNGTexture(pDevice, texData);
		}
		else if (textureExt == "dds")
		{
			LoadDDSTexture(pDevice, texData);
		}
		else if (textureExt == "tga")
		{
			LoadTGATexture(pDevice, texData);
		}
		else if (textureExt == "bmp")
		{
			LoadBMPTexture(pDevice, texData);
		}
		else
		{
			throw LIB_Exception("UNKNOWN IMAGE EXTENSION");
		}
	}
	catch (LIB_Exception & e)
	{
		const std::string errMgs{ "can't load a texture from file: " + texData.filePath };

		Log::Error(e);
		Log::Error(errMgs);
		throw LIB_Exception(errMgs);
	}
}

///////////////////////////////////////////////////////////

void ImageReader::LoadTextureFromMemory(
	ID3D11Device* pDevice,
	const uint8_t* pData,
	const size_t size,
	DXTextureData& outTexData)
{

	try
	{
		CheckInputParams(outTexData);
		Assert::True((bool)pDevice && (bool)pData && (size > 0), "some of input params are invalid");

		HRESULT hr = DirectX::CreateWICTextureFromMemory(
			pDevice,
			pData,
			size,
			outTexData.ppTexture,
			outTexData.ppTextureView);
		Assert::NotFailed(hr, "can't create a texture from memory");

		// initialize the texture width and height values
		D3D11_TEXTURE2D_DESC desc;
		ID3D11Texture2D* pTex = (ID3D11Texture2D*)(*outTexData.ppTexture);
		pTex->GetDesc(&desc);

		outTexData.textureWidth = desc.Width;
		outTexData.textureHeight = desc.Height;
	}
	catch (LIB_Exception & e)
	{
		const std::string errMsg = "can't load texture's data from memory";
		Log::Error(e);
		Log::Error(errMsg);
		throw LIB_Exception(errMsg);

	}
}



// ************************************************************************************
// 
//                            PRIVATE METHODS
// 
// ************************************************************************************

void ImageReader::CheckInputParams(const DXTextureData& data)
{
	Assert::True(
		(!data.filePath.empty()) && 
		data.ppTexture && 
		data.ppTextureView, "some of input params are invalid");
}

///////////////////////////////////////////////////////////

void ImageReader::LoadPNGTexture(ID3D11Device* pDevice, DXTextureData& data)
{
	const std::wstring wFilePath{ StringHelper::StringToWide(data.filePath) };

	const HRESULT hr = DirectX::CreateWICTextureFromFile(
		pDevice,
		wFilePath.c_str(),
		data.ppTexture,
		data.ppTextureView);

	Assert::NotFailed(hr, "can't create a PNG texture from file: " + data.filePath);

	// initialize the texture width and height values
	D3D11_TEXTURE2D_DESC desc;
	ID3D11Texture2D* pTex = (ID3D11Texture2D*)(*data.ppTexture);
	pTex->GetDesc(&desc);

	data.textureWidth = desc.Width;
	data.textureHeight = desc.Height;
}

///////////////////////////////////////////////////////////

void ImageReader::LoadDDSTexture(ID3D11Device* pDevice, DXTextureData& data)
{
	ddsImgReader_.LoadTextureFromFile(
		data.filePath,
		pDevice,
		data.ppTexture,
		data.ppTextureView,
		data.textureWidth,
		data.textureHeight);
}

///////////////////////////////////////////////////////////

void ImageReader::LoadTGATexture(ID3D11Device* pDevice, DXTextureData& data)
{
	targaImgReader_.LoadTextureFromFile(
		data.filePath,
		pDevice,
		data.ppTexture,
		data.ppTextureView,
		data.textureWidth,
		data.textureHeight);
}

///////////////////////////////////////////////////////////

void ImageReader::LoadBMPTexture(ID3D11Device* pDevice, DXTextureData& data)
{
	bmpImgReader_.LoadTextureFromFile(
		data.filePath,
		pDevice,
		data.ppTexture,
		data.ppTextureView,
		data.textureWidth,
		data.textureHeight);
}


} // namespace ImgReader