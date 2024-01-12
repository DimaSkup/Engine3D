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


	if (FAILED(hr))
	{
		std::string errorMsg{ "can't load a DDS texture: " + filePath };
		Log::Error(THIS_FUNC, errorMsg.c_str());

		return false;
	}

	return true;

} // end LoadTextureFromFile