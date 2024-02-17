#pragma once

#include <vector>
#include <string>
#include <d3d11.h>
#include <memory>

typedef unsigned char BYTE;

class ImageReader
{
public:
	ImageReader();

	bool LoadTextureFromFile(const std::string & filePath,
		ID3D11Device* pDevice,
		ID3D11Resource** ppTexture,
		ID3D11ShaderResourceView** ppTextureView,
		UINT & textureWidth,
		UINT & textureHeight);

	bool ImageReader::LoadTextureFromMemory(ID3D11Device* pDevice,
		const uint8_t* pData,
		const size_t size,
		ID3D11Resource** ppTexture,
		ID3D11ShaderResourceView** ppTextureView);

	// read an image data from the file by filePath and store it into the imageData array
	bool ReadRawImageData(const std::string & filePath,
		UINT & imageWidth,
		UINT & imageHeight,
		_Inout_ std::vector<float> & imageData);
};