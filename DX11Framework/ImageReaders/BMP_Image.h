////////////////////////////////////////////////////////////////////////////////////////////
// Filename:     BMP_Image.h
// Description:  has functional for reading of bmp image data;
//               1. can initialize texture resource and shader resource view using
//                  loaded bmp image data
//               2. read read bmp image data and return this raw data as the result
//
// Created:      17.02.24
////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <d3d11.h>
#include <string>
#include <vector>

typedef unsigned int UINT;

struct BITMAPCOLORHEADER
{
	uint32_t red_mask{ 0x00ff0000 };         // Bit mask for the red channel
	uint32_t green_mask{ 0x0000ff00 };       // Bit mask for the green channel
	uint32_t blue_mask{ 0x000000ff };        // Bit mask for the blue channel
	uint32_t alpha_mask{ 0xff000000 };       // Bit mask for the alpha channel
	uint32_t color_space_type{ 0x73524742 }; // Default "sRGB" (0x73524742)
	uint32_t unused[16]{ 0 };                // Unused data for sRGB color space
};

class BMP_Image
{
public:
	BMP_Image();
	BMP_Image(int32_t width, int32_t height, bool has_alpha = true);

	// load texture's data from file by filePath and initialize a texture resource and
	// a shader resource view with this data
	bool LoadTextureFromFile(const std::string & filePath,
		ID3D11Device* pDevice,
		ID3D11Resource** ppTexture,
		ID3D11ShaderResourceView** ppTextureView,
		UINT & textureWidth,
		UINT & textureHeight);

	// read an image data from the file by filePath and store it into the imageData array
	bool ReadRawImageData(const std::string & filePath, 
		_Inout_ std::vector<float> & imageData,
		UINT & textureWidth,
		UINT & textureHeight);

	void Read(const std::string & filePath);   // read from a file by filePath
	void Write(const std::string & filePath);  // write into a file by filePath

private:
	void CheckColorHeader(BITMAPCOLORHEADER & bitmapColorHeader);
	void WriteHeaders(std::ofstream & fout);
	void WriteHeadersAndData(std::ofstream & fout);
	uint32_t MakeStrideAligned(const uint32_t align_stride);

private:
	BITMAPFILEHEADER  bitmapFileHeader_;
	BITMAPINFOHEADER  bitmapInfoHeader_;
	BITMAPCOLORHEADER bitmapColorHeader_;
	std::vector<uint8_t> data_;

	uint32_t row_stride_{ 0 };
};