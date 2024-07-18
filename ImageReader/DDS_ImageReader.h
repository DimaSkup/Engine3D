// *********************************************************************************
// Filename:     DDS_ImageReader.h
// Description:  textures loader/initializer of the .dds format;
// 
// *********************************************************************************
#pragma once

#include <d3d11.h>
#include <string>

#include "Common/Types.h"

class DDS_ImageReader final
{
public:
	DDS_ImageReader() {}

	bool LoadTextureFromFile(
		const std::string & filePath,
		ID3D11Device* pDevice,
		ID3D11Resource** ppTexture,
		ID3D11ShaderResourceView** ppTextureView,
		u32& textureWidth,
		u32& textureHeight);
};
