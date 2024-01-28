#include "DDS_ImageReader.h"

#include <d3d11.h>
#include <d3dx11tex.h>
#include <string>

#include "../Engine/StringHelper.h"
#include "../Engine/macros.h"
#include "../Engine/log.h"

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

	std::wstring wFilePath{ StringHelper::StringToWide(filePath) };




	// Load the texture in
	HRESULT hr = D3DX11CreateShaderResourceViewFromFile(pDevice,
		wFilePath.c_str(),   // src file path
		nullptr,             // ptr load info
		nullptr,             // ptr pump
		ppTextureView,      // pp shader resource view
		nullptr);            // pHresult


	// initialize a texture resource using the shader resource view
	(*ppTextureView)->GetResource(ppTexture);

#if 0
	ID3D11Texture2D* p2DTexture = nullptr;

	textureDesc.Width = textureWidth;   // we've gotten width/height in the LoadTarga32Bit function
	textureDesc.Height = textureHeight;
	textureDesc.MipLevels = pDesc.Texture2D.MipLevels;
	textureDesc.ArraySize = pDesc.Texture2DArray.ArraySize;
	textureDesc.Format = pDesc.Format;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	// create the empty texture
	hr = pDevice->CreateTexture2D(&textureDesc, nullptr, &p2DTexture);
	COM_ERROR_IF_FAILED(hr, "can't create an empty 2D texture: " + filePath);

	// store a ptr to the 2D texture 
	*ppTexture = static_cast<ID3D11Texture2D*>(p2DTexture);

#endif

	if (FAILED(hr))
	{
		std::string errorMsg{ "can't load a DDS texture: " + filePath };
		Log::Error(LOG_MACRO, errorMsg.c_str());

		return false;
	}

	return true;

} // end LoadTextureFromFile