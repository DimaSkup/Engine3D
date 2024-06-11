#pragma once

#include <d3d11.h>
#include <string>

class DDS_ImageReader final
{
public:
	DDS_ImageReader() {}

	bool LoadTextureFromFile(const std::string & filePath,
		ID3D11Device* pDevice,
		ID3D11Resource** ppTexture,
		ID3D11ShaderResourceView** ppTextureView,
		UINT & textureWidth,
		UINT & textureHeight);
};
