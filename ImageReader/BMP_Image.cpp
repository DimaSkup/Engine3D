////////////////////////////////////////////////////////////////////////////////////////////
// Filename:     BMP_Image.cpp
// Description:  implementation of functional for the BMP_Image class;
//
// Created:      17.02.24
////////////////////////////////////////////////////////////////////////////////////////////
#include "BMP_Image.h"

#include "Common/log.h"
#include "DDS_ImageReader.h"
#include <fstream>

#include <DirectXColors.h>

using namespace ImgReader;





// ********************************************************************************
//                             PUBLIC METHODS
// ********************************************************************************

void BMP_Image::CopyRawDataInto(_Inout_ std::vector<u8> & outData)
{
	try
	{
		outData.resize(data_.size());
		std::copy(data_.begin(), data_.end(), outData.begin());
	}
	catch (const std::bad_alloc & e)
	{
		Log::Error(LOG_MACRO, e.what());
		ASSERT_TRUE(false, "can't allocate memory for the raw bmp image data");
	}
}

///////////////////////////////////////////////////////////

bool BMP_Image::LoadTextureFromFile(
	const std::string & bmpFilePath,
	ID3D11Device* pDevice,
	ID3D11Resource** ppTexture,
	ID3D11ShaderResourceView** ppTextureView,
	u32& textureWidth,
	u32& textureHeight)
{
	// load texture's data from file by filePath and initialize a texture resource and
	// a shader resource view with this data
#if 1
	// because we can use the same loading both for dds and bmp files
	// we just use the DDS_ImageReader for processing the input file by bmpFilePath
	DDS_ImageReader ddsImage;

	ddsImage.LoadTextureFromFile(bmpFilePath, 
		pDevice,
		ppTexture,
		ppTextureView,
		textureWidth, 
		textureHeight);
#endif


#if 0

	HRESULT hr = S_OK;
	bool result = false;

	UINT rowPitch = 0;


	
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

	ID3D11Texture2D* p2DTexture = nullptr;
	ID3D11DeviceContext* pDeviceContext = nullptr;

	// holds the raw Targa data read straight in from the file
	//std::vector<UCHAR> targaDataArr;

	// ----------------------------------------------------- //

	// get the device context
	pDevice->GetImmediateContext(&pDeviceContext);

	// load the image data into memory (into the data array) 
	Read(bmpFilePath);
	const UINT bytesOfPixel = bitmapInfoHeader_.biBitCount / 8;
	GetDimensions(textureWidth, textureHeight);

	// next we need to setup our description of the DirectX texture that we will load
	// the Targa data into. We use the height and width from the Targa image data, and 
	// set the format to be a 32-bit RGBA texture. We set the SampleDesc to default.
	// Then we set the Usage to D3D11_USAGE_DEFAULT which is better performing memory.
	// And finally, we set the MipLevels, BindFlags, and MiscFlags to the settings 
	// required for Mipmapped textures. Once the description is complete, we call
	// CreateTexture2D() to create an empty texture for us. The next step will be to 
	// copy the Targa data into that empty texture.

	//CD3D11_TEXTURE2D_DESC textureDesc(DXGI_FORMAT_R8G8B8A8_UNORM, textureWidth, textureHeight);
#if 0
	D3D11_TEXTURE2D_DESC textureDesc;
	textureDesc.Width = textureWidth;   // we've gotten width/height in the LoadTarga32Bit function
	textureDesc.Height = textureHeight;
	textureDesc.MipLevels = 0;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
#endif
#if 0
	// create the empty texture
	//(u32)(DirectX::Colors::Cyan)
	std::vector<u32> dataArr(textureWidth * textureHeight, 0);



	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = dataArr.data();
	initData.SysMemPitch = rowPitch;
	hr = pDevice->CreateTexture2D(&textureDesc, &initData, &p2DTexture);
	ASSERT_NOT_FAILED(hr, "can't create an empty 2D texture: " + bmpFilePath);
#endif
	static const std::vector<u32> pixelData{ 100, 0xffc99aff };

	D3D11_SUBRESOURCE_DATA initData = { pixelData.data(), pixelData.size() * sizeof(u32), 0 };

	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = desc.Height = 10;
	desc.ArraySize = 1;
	desc.MipLevels = 0;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	//ComPtr<ID3D11Texture2D> tex;
	hr = pDevice->CreateTexture2D(&desc, nullptr, &p2DTexture);
	ASSERT_NOT_FAILED(hr, "can't create n 2D texture: " + bmpFilePath);
	

	// set the row pitch of the image data
	rowPitch = (desc.Width) * sizeof(u32);

	// copy the targa image data into the texture
	pDeviceContext->UpdateSubresource(p2DTexture, 0, nullptr, &initData, rowPitch, 0);

	// setup the shader resource view description
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	// after the texture is loaded, we create a shader resource view which allows us to have
	// a pointer to set the texture in shaders.
	hr = pDevice->CreateShaderResourceView(p2DTexture, &srvDesc, ppTextureView);
	ASSERT_NOT_FAILED(hr, "can't create the shader resource view: " + bmpFilePath);

	// generate mipmaps for this texture
	pDeviceContext->GenerateMips(*ppTextureView);


	// store a ptr to the 2D texture 
	*ppTexture = static_cast<ID3D11Texture2D*>(p2DTexture);
#endif


	return true;
}

///////////////////////////////////////////////////////////

void BMP_Image::Read(const std::string & filePath)
{
	// THIS FUNCTION reads  BMP image data from a file by filePath;
	// stores the params of the image into bitmap headers, and
	// initializes the data array with loaded data;

	
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
			bitmapFileHeader_.bfSize += (DWORD)data_.size();
		}
		else
		{
			row_stride_ = bitmapInfoHeader_.biWidth * bitmapInfoHeader_.biBitCount / 8;
			u32 new_stride = MakeStrideAligned(4);
			std::vector<u8> padding_row(new_stride - row_stride_);

			for (int y = 0; y < bitmapInfoHeader_.biHeight; ++y)
			{
				fin.read((char*)(data_.data() + row_stride_ * y), row_stride_);
				fin.read((char*)padding_row.data(), padding_row.size());
			}

			bitmapFileHeader_.bfSize += (DWORD)data_.size() + (DWORD)bitmapInfoHeader_.biHeight * (DWORD)padding_row.size();
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
				u32 new_stride = MakeStrideAligned(4);
				std::vector<u8> padding_row(new_stride - row_stride_);

				WriteHeaders(fout);

				const u8* pData = data_.data();
				const u32 rowStride = row_stride_;

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

u32 BMP_Image::MakeStrideAligned(const u32 align_stride)
{
	// add 1 to the row_stride_ until it is divisible with the align_stride
	u32 new_stride = row_stride_;

	while (new_stride % align_stride != 0)
	{
		new_stride++;
	}

	return new_stride;
}