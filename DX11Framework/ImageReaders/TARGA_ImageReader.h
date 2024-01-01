#pragma once

#include <d3d11.h>

#include "../Engine/COMException.h"
#include "../Engine/log.h"
#include "ImageReaderInterface.h"


class TARGA_ImageReader final : public ImageReaderInterface
{
private:
	// we define the Targa file header structure here to 
	// make reading in the data easier (for .tga format)
	struct TargaHeader
	{
		UCHAR data1[12]{ '\0' };
		USHORT width = 0;
		USHORT height = 0;
		UCHAR bpp{ '\0' };
		UCHAR data2{ '\0' };
	};

public:
	TARGA_ImageReader();

	virtual void ReadTextureFromImage(const std::string & filePath,
		ID3D11Device* pDevice,
		ID3D11Resource* pTexture_,
		ID3D11ShaderResourceView* pTextureView_);

	bool LoadTargaTexture(const std::string & filePath, 
		ID3D11Device* pDevice,
		ID3D11Resource* pTexture_,
		ID3D11ShaderResourceView* pTextureView_);

private:
	bool LoadTarga32Bit(const std::string & filePath, std::vector<UCHAR> & targaDataArr);

private:
	UINT textureWidth_ = 0;
	UINT textureHeight_ = 0;
};