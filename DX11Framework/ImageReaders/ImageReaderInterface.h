#pragma once

#include <vector>
#include <string>

typedef unsigned char BYTE;

class ImageReaderInterface
{
public:
	virtual bool ReadImage(const std::string & filePath, const std::vector<BYTE> & imageData) = 0;
};