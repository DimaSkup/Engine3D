#pragma once


#include "ImageReaderInterface.h"

class ImageReader : public ImageReaderInterface
{
public:
	ImageReader();

	virtual bool LoadTextureFromFile(const std::string & filePath,
		ID3D11Device* pDevice,
		ID3D11Resource** ppTexture,
		ID3D11ShaderResourceView** ppTextureView,
		UINT & textureWidth,
		UINT & textureHeight) override;

	// read an image data from the file by filePath and store it into the imageData array
	bool ReadRawImageData(const std::string & filePath, const std::vector<BYTE> & imageData);
};