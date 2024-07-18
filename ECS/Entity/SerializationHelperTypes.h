#pragma once

#include <vector>


struct SerializedDataHeaderRecord
{
	u32 dataBlockMarker = 0;   // KEY:   markers for data blocks
	u32 dataBlockPos = 0;      // VALUE: positions of each data block in the file
};

struct SerializedDataHeader
{
	u32 recordsCount = 0;

	std::vector<SerializedDataHeaderRecord> records;

	u32& GetDataBlockPos(const u32 idx)
	{
		return records[idx].dataBlockPos;
	}
};