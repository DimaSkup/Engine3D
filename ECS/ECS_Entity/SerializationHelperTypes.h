#pragma once

#include <vector>


struct SerializedDataHeaderRecord
{
	size_t dataBlockMarker = 0;   // KEY:   markers for data blocks
	size_t dataBlockPos = 0;      // VALUE: positions of each data block in the file
};

struct SerializedDataHeader
{
	size_t recordsCount = 0;

	std::vector<SerializedDataHeaderRecord> records;

	size_t& GetDataBlockPos(const size_t idx)
	{
		return records[idx].dataBlockPos;
	}
};