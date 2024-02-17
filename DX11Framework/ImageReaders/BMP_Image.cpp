////////////////////////////////////////////////////////////////////////////////////////////
// Filename:     BMP_Image.cpp
// Description:  implementation of functional for the BMP_Image class;
//
// Created:      17.02.24
////////////////////////////////////////////////////////////////////////////////////////////
#include "BMP_Image.h"

#include "../Engine/log.h"
#include "DDS_ImageReader.h"
#include <fstream>



BMP_Image::BMP_Image()
{
}


BMP_Image::BMP_Image(int32_t width, int32_t height, bool has_alpha)
{
	// create an empty BMP image with such params
}




////////////////////////////////////////////////////////////////////////////////////////////
//                               PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////

// load texture's data from file by filePath and initialize a texture resource and
// a shader resource view with this data
bool BMP_Image::LoadTextureFromFile(
	const std::string & bmpFilePath,
	ID3D11Device* pDevice,
	ID3D11Resource** ppTexture,
	ID3D11ShaderResourceView** ppTextureView,
	UINT & textureWidth,
	UINT & textureHeight)
{
	DDS_ImageReader ddsImage;

	ddsImage.LoadTextureFromFile(bmpFilePath, pDevice, ppTexture, ppTextureView, textureWidth, textureHeight);
	return true;
}

///////////////////////////////////////////////////////////

void BMP_Image::Read(const std::string & filePath)
{
	// read BMP image data from a file by filePath

	
	//BITMAPCOLORHEADER

	std::ifstream fin{ filePath, std::ios_base::binary };
	if (fin)
	{
		// read in the bitmap file header
		fin.read((char*)&bitmapFileHeader_, sizeof(BITMAPFILEHEADER));
		if (bitmapFileHeader_.bfType != 0x4D42)   // if != "BM"
		{
			throw std::runtime_error("ERROR! Unrecognized file format.");
		}

		// read in the bitmap info header
		fin.read((char*)&bitmapInfoHeader_, sizeof(BITMAPINFOHEADER));

		// the BITMAPCOLORHEADER is used only for transparent images
		if (bitmapInfoHeader_.biBitCount == 32)
		{
			// check if the file has bit mask color information
			if (bitmapInfoHeader_.biSize >= (sizeof(BITMAPINFOHEADER) + sizeof(BITMAPCOLORHEADER)))
			{
				fin.read((char*)&bitmapColorHeader_, sizeof(BITMAPCOLORHEADER));

				// check if the pixel data is store as BGRA and if the color space type is sRGB
				CheckColorHeader(bitmapColorHeader_);
			}
			else
			{
				Log::Error(LOG_MACRO, "The file \"" + filePath + "\" doesn't seem to contain bit mask information");
				throw std::runtime_error("ERROR! Unrecognized file format");
			}
		}

		// -------------------------------------------------------- //

		// jump to the pixel data location
		fin.seekg(bitmapFileHeader_.bfOffBits, fin.beg);

		// adjust the header fields for output;
		// some editors will put extra info in the image file, we only save the header and the data.
		if (bitmapInfoHeader_.biBitCount == 32)
		{
			bitmapInfoHeader_.biSize = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPCOLORHEADER);
			bitmapFileHeader_.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(BITMAPCOLORHEADER);
		}
		else if (bitmapInfoHeader_.biBitCount == 24)
		{
			bitmapInfoHeader_.biSize = sizeof(BITMAPINFOHEADER);
			bitmapFileHeader_.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
		}
		else
		{
			Log::Error(LOG_MACRO, "The file \"" + filePath + "\" has unsupported bit depth");
			throw std::runtime_error("ERROR! Unsupported bit depth");
		}

		bitmapFileHeader_.bfSize = bitmapFileHeader_.bfOffBits;

		// check height of the image
		if (bitmapInfoHeader_.biHeight < 0)
		{
			Log::Error(LOG_MACRO, "The program can treat only BMP images with the origin in the bottom left corner!");
			throw std::runtime_error("The program can treat only BMP images with the origin in the bottom left corner!");
		}

		// prepare enough memory for the image data
		data_.resize(bitmapInfoHeader_.biWidth * bitmapInfoHeader_.biHeight * bitmapInfoHeader_.biBitCount / 8);

		// here we check if we need to take into account row padding
		if (bitmapInfoHeader_.biWidth % 4 == 0)
		{
			fin.read((char*)data_.data(), data_.size());
			bitmapFileHeader_.bfSize += data_.size();
		}
		else
		{
			row_stride_ = bitmapInfoHeader_.biWidth * bitmapInfoHeader_.biBitCount / 8;
			uint32_t new_stride = MakeStrideAligned(4);
			std::vector<uint8_t> padding_row(new_stride - row_stride_);

			for (int y = 0; y < bitmapInfoHeader_.biHeight; ++y)
			{
				fin.read((char*)(data_.data() + row_stride_ * y), row_stride_);
				fin.read((char*)padding_row.data(), padding_row.size());
			}

			bitmapFileHeader_.bfSize += data_.size() + bitmapInfoHeader_.biHeight * padding_row.size();
		}
	}
	else
	{
		Log::Error(LOG_MACRO, "Unable to open the input image file: " + filePath);
		throw std::runtime_error("Unabled to open the input image file: " + filePath);
	}
}

///////////////////////////////////////////////////////////

void BMP_Image::Write(const std::string & filePath)
{
	// write BMP image data to disk into a file by filePath

	std::ofstream fout{ filePath, std::ios_base::binary };

	BITMAPINFOHEADER bitmapInfoHeader = bitmapInfoHeader_;

	if (fout)
	{
		// if our bit depth is 32
		if (bitmapInfoHeader.biBitCount == 32)
		{
			WriteHeadersAndData(fout);
		}
		else if (bitmapInfoHeader.biBitCount == 24)
		{
			// if we have a propper padding
			if (bitmapInfoHeader.biWidth % 4 == 0)
			{
				WriteHeadersAndData(fout);
			}
			else
			{
				uint32_t new_stride = MakeStrideAligned(4);
				std::vector<uint8_t> padding_row(new_stride - row_stride_);

				WriteHeaders(fout);

				const uint8_t* pData = data_.data();
				const uint32_t rowStride = row_stride_;

				for (int y = 0; y < bitmapInfoHeader.biHeight; ++y)
				{
					fout.write((const char*)(pData + rowStride * y), rowStride);
					fout.write((const char*)padding_row.data(), padding_row.size());
				}
			}
		}
		else
		{
			Log::Error(LOG_MACRO, "The program can treat only 24 or 32 bits per pixel BMP files");
			throw std::runtime_error("The program can treat only 24 or 32 bits per pixel BMP files");
		}
	}
	else
	{
		Log::Error(LOG_MACRO, "Unable to open the output image file");
		throw std::runtime_error("Unable to open the output image file");
	}
}

///////////////////////////////////////////////////////////






////////////////////////////////////////////////////////////////////////////////////////////
//                              PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////

void BMP_Image::CheckColorHeader(BITMAPCOLORHEADER & bitmapColorHeader)
{
	// check if the pixel data is stored as BGRA and if the color space type is sRGB

	BITMAPCOLORHEADER expectedColorHeader;

	if (expectedColorHeader.red_mask != bitmapColorHeader.red_mask ||
		expectedColorHeader.blue_mask != bitmapColorHeader.blue_mask ||
		expectedColorHeader.green_mask != bitmapColorHeader.green_mask ||
		expectedColorHeader.alpha_mask != bitmapColorHeader.alpha_mask)
	{
		Log::Error(LOG_MACRO, "Unexpected color mask format! The program expects the pixel data to be in the BGRA format");
		throw std::runtime_error("Unexpected color mask format! The program expects the pixel data to be in the BGRA format");
	}
	
	if (expectedColorHeader.color_space_type != bitmapColorHeader.color_space_type)
	{
		Log::Error(LOG_MACRO, "Unexpected color space type! The program expects sRGB values");
		throw std::runtime_error("Unexpected color space type! The program expects sRGB values");
	}
}

///////////////////////////////////////////////////////////

void BMP_Image::WriteHeaders(std::ofstream & fout)
{
	fout.write((const char*)&bitmapFileHeader_, sizeof(BITMAPFILEHEADER));
	fout.write((const char*)&bitmapInfoHeader_, sizeof(BITMAPINFOHEADER));

	if (bitmapInfoHeader_.biBitCount == 32)
	{
		fout.write((const char*)&bitmapColorHeader_, sizeof(BITMAPCOLORHEADER));
	}
}

///////////////////////////////////////////////////////////

void BMP_Image::WriteHeadersAndData(std::ofstream & fout)
{
	WriteHeaders(fout);
	fout.write((const char*)data_.data(), data_.size());
}

///////////////////////////////////////////////////////////

uint32_t BMP_Image::MakeStrideAligned(const uint32_t align_stride)
{
	// add 1 to the row_stride_ until it is divisible with the align_stride
	uint32_t new_stride = row_stride_;

	while (new_stride % align_stride != 0)
	{
		new_stride++;
	}

	return new_stride;
}