#pragma once

#include <stdint.h>
#include <fstream>
#include <string>
#include <iostream>

#include "ImageReaderInterface.h"
#include "../Engine/log.h"


class PNG_Reader : public ImageReaderInterface
{
private:
	struct PNG_Chunk
	{
		uint32_t length = 0;             // a 4-byte unsigned integer giving the number of bytes in the chunk's data field. The length counts only the data field, not itself or other parts.
		uint32_t type = 0;               // a 4-byte chunk type code.
		void* chunk_data = nullptr;      // the data bytes appropriate to the chunk type, if any. This field can be of zero length
		uint32_t crc = 0;                // a 4-byte CRC (Cyclic Redundancy Check)
	};

	struct IHDR_Header
	{
		// uint32_t = 4 bytes; uint8_t = 1 byte

		uint32_t width = 0;              // width and height give the image dimensions in pixels
		uint32_t height = 0;
		uint8_t bitDepth = 0;
		uint8_t colorType = 0;
		uint8_t compressMethod = 0;
		uint8_t filterMethod = 0;
		uint8_t interlaceMethod = 0;
	};

public:
	PNG_Reader();
	~PNG_Reader();

	virtual bool ReadImage(const std::string & filePath, const std::vector<BYTE> & imageData) override;

private:
	void CheckFileSignature(FILE* pFile);

private:
	std::ifstream fin;
};