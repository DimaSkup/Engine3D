// *********************************************************************************
// Filename:     PNG_ImageReader.h
// Description:  textures loader of the .png format;
// 
// *********************************************************************************
#pragma once


#include <fstream>
#include <string>
#include <iostream>
#include <d3d11.h>

#include "Common/log.h"
#include "Common/Types.h"


class PNG_ImageReader final
{
private:
	struct PNG_Chunk
	{
		u32 length = 0;                  // a 4-byte unsigned integer giving the number of bytes in the chunk's data field. The length counts only the data field, not itself or other parts.
		u32 type = 0;                    // a 4-byte chunk type code.
		void* chunk_data = nullptr;      // the data bytes appropriate to the chunk type, if any. This field can be of zero length
		u32 crc = 0;                     // a 4-byte CRC (Cyclic Redundancy Check)
	};

	struct IHDR_Header
	{
		u32 width = 0;              // width and height give the image dimensions in pixels
		u32 height = 0;
		u8 bitDepth = 0;
		u8 colorType = 0;
		u8 compressMethod = 0;
		u8 filterMethod = 0;
		u8 interlaceMethod = 0;
	};

public:
	PNG_ImageReader() {};
	~PNG_ImageReader() {};

	bool LoadTextureFromFile(
		const std::string & filePath,
		ID3D11Device* pDevice,
		ID3D11Resource** ppTexture,
		ID3D11ShaderResourceView** ppTextureView,
		u32& textureWidth,
		u32& textureHeight);

private:
	void CheckFileSignature(FILE* pFile);

private:
	std::ifstream fin;
};