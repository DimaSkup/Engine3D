#pragma once

#include <d3d11.h>
#include "ImageReaderInterface.h"


class DDS_ImageReader : public ImageReaderInterface
{
public:
	DDS_ImageReader() {}

	virtual bool LoadTextureFromFile(const std::string & filePath,
		ID3D11Device* pDevice,
		ID3D11Resource** ppTexture,
		ID3D11ShaderResourceView** ppTextureView,
		UINT & textureWidth,
		UINT & textureHeight) override;
};
