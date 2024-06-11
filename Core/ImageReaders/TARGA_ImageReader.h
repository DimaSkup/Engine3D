#pragma once

#include <d3d11.h>
#include <vector>
#include <string>

class TARGA_ImageReader final
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

	bool LoadTextureFromFile(const std::string & filePath,
		ID3D11Device* pDevice,
		ID3D11Resource** ppTexture_,
		ID3D11ShaderResourceView** ppTextureView_,
		UINT & textureWidth,
		UINT & textureHeight);

private:
	bool LoadTarga32Bit(const std::string & filePath,
		std::vector<UCHAR> & targaDataArr,   // raw image data
		UINT & textureWidth,
		UINT & textureHeight);
};