#include "DDS_ImageReader.h"

#include <d3d11.h>
#include <d3dx11tex.h>
#include <string>

#include "../Engine/StringHelper.h"
#include "../Engine/log.h"
#include "DDSTextureLoader11.h"

using namespace DirectX;



bool DDS_ImageReader::LoadTextureFromFile(const std::string & filePath,
	ID3D11Device* pDevice,
	ID3D11Resource** ppTexture,
	ID3D11ShaderResourceView** ppTextureView,
	UINT & textureWidth,
	UINT & textureHeight)
{
	// this function loads a DDS texture from the file by filePath
	// and initializes input parameters: texture resource, shader resource view,
	// width and height of the texture;

	assert(!filePath.empty());
	assert(pDevice != nullptr);
	assert(ppTexture != nullptr);
	assert(ppTextureView != nullptr);

	try
	{
		HRESULT hr = S_OK;
		const std::wstring wFilePath{ StringHelper::StringToWide(filePath) };

		// CREATE USING FUNCTIONAL FROM DirectXTK
#if 0
		hr = CreateDDSTextureFromFileEx(
				pDevice, 
				wFilePath.c_str(),                                     // szFileName
				0,                                                     // maxsize: if maxsize parameter non-zero, then all mipmap levels larger than the maxsize are ignored before creating the Direct3D 11 resource
				D3D11_USAGE_DEFAULT,                                   // usage	
				D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET, // bindFlags
			 	0,                                                     // cpu access flags
			    D3D11_RESOURCE_MISC_GENERATE_MIPS,                     // misc flags
				DDS_LOADER_FLAGS::DDS_LOADER_DEFAULT,                  // loadFlags
				ppTexture,
			    ppTextureView,
				nullptr);                                              // alphaMode

		ASSERT_NOT_FAILED(hr, "can't create a texture from file: " + filePath);

		/*
		ID3D11Texture2D* pTexture = dynamic_cast<ID3D11Texture2D*>(*ppTexture);
		ID3D11Resource* pRes = nullptr;
		D3D11_TEXTURE2D_DESC texDesc;
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

		pTexture->GetDesc(&texDesc);

		
		srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Format = texDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

		hr = pDevice->CreateShaderResourceView(pTexture, &srvDesc, ppTextureView);
		ASSERT_NOT_FAILED(hr, "can't create a shader resource view for texture: " + filePath);
		*/

#elif 1
		// create a shader resource view from the texture file
		hr = D3DX11CreateShaderResourceViewFromFile(pDevice,
			wFilePath.c_str(),   // src file path
			nullptr,             // ptr load info
			nullptr,             // ptr pump
			ppTextureView,       // pp shader resource view
			nullptr);            // pHresult
		ASSERT_NOT_FAILED(hr, "can't load a DDS texture: " + filePath);

		// initialize a texture resource using the shader resource view
		(*ppTextureView)->GetResource(ppTexture);

#endif
		// load information about the texture
		D3DX11_IMAGE_INFO imageInfo;
		hr = D3DX11GetImageInfoFromFile(wFilePath.c_str(), nullptr, &imageInfo, nullptr);
		ASSERT_NOT_FAILED(hr, "can't read the image info from file: " + filePath);

		// initializa the texture width and height values
		textureWidth = imageInfo.Width;
		textureHeight = imageInfo.Height;
	}
	catch (EngineException& e)
	{
		Log::Error(e, true);
		Log::Error(LOG_MACRO, "can't load texture from file: " + filePath);
	}

	return true;

}