////////////////////////////////////////////////////////////////////////////////////////////
// Filename:     BMP_Image.h
// Description:  has functional for reading of bmp image data;
//               1. can initialize texture resource and shader resource view using
//                  loaded bmp image data
//               2. (not working) read read bmp image data and return this raw data as the result
//
// Created:      17.02.24
////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <d3d11.h>
#include <string>
#include <vector>

#include "Common/Types.h"


namespace ImgReader
{

struct BITMAPCOLORHEADER
{
	u32 red_mask        { 0x00ff0000 };       // Bit mask for the red channel
	u32 green_mask      { 0x0000ff00 };       // Bit mask for the green channel
	u32 blue_mask       { 0x000000ff };       // Bit mask for the blue channel
	u32 alpha_mask      { 0xff000000 };       // Bit mask for the alpha channel
	u32 color_space_type{ 0x73524742 };       // Default "sRGB" (0x73524742)
	u32 unused[16]{ 0 };                      // Unused data for sRGB color space
};

class BMP_Image
{
public:
	BMP_Image() {}
	BMP_Image(u32 width, u32 height, bool has_alpha = true) {}

	// load texture's data from file by filePath and initialize a texture resource and
	// a shader resource view with this data
	void LoadTextureFromFile(
		const std::string & filePath,
		ID3D11Device* pDevice,
		ID3D11Resource** ppTexture,
		ID3D11ShaderResourceView** ppTextureView,
		u32& textureWidth,
		u32& textureHeight);

	void Read(const std::string & filePath);   // read from a file by filePath
	void Write(const std::string & filePath);  // write into a file by filePath

	void CopyRawDataInto(std::vector<u8> & outData);

	inline void GetDimensions(UINT & width, UINT & height)
	{
		width = bitmapInfoHeader_.biWidth;
		height = bitmapInfoHeader_.biHeight;
	}

	inline const std::vector<u8> & GetRawData() const {	return data_; }


private:
	void CheckColorHeader(BITMAPCOLORHEADER & bitmapColorHeader);
	void WriteHeaders(std::ofstream & fout);
	void WriteHeadersAndData(std::ofstream & fout);
	u32 MakeStrideAligned(const u32 align_stride);

private:
	BITMAPFILEHEADER  bitmapFileHeader_{0};
	BITMAPINFOHEADER  bitmapInfoHeader_{0};
	BITMAPCOLORHEADER bitmapColorHeader_;
	std::vector<u8> data_;

	u32 row_stride_{ 0 };
};


} // namespace ImgReader