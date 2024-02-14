#include "DDS_ImageReader.h"

#include <d3d11.h>
#include <d3dx11tex.h>
#include <string>

#include "../Engine/StringHelper.h"
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

	HRESULT hr = S_OK;
	const std::wstring wFilePath{ StringHelper::StringToWide(filePath) };

	// create a shader resource view from the texture file
	hr = D3DX11CreateShaderResourceViewFromFile(pDevice,
		wFilePath.c_str(),   // src file path
		nullptr,             // ptr load info
		nullptr,             // ptr pump
		ppTextureView,       // pp shader resource view
		nullptr);            // pHresult

	COM_ERROR_IF_FAILED(hr, "can't load a DDS texture: " + filePath);


	// initialize a texture resource using the shader resource view
	(*ppTextureView)->GetResource(ppTexture);

	// load information about the texture
	D3DX11_IMAGE_INFO imageInfo;
	hr = D3DX11GetImageInfoFromFile(wFilePath.c_str(), nullptr, &imageInfo, nullptr);
	COM_ERROR_IF_FAILED(hr, "can't read the image info from file: " + filePath);

	// initializa the texture width and height values
	textureWidth = imageInfo.Width;
	textureHeight = imageInfo.Height;

	return true;

} // end LoadTextureFromFile