#pragma once

#include <DirectXMath.h>
#include <vector>

#include "../Common/MathHelper.h"

typedef unsigned int UINT;

class ChunkHelper
{
public:

	void ComputeChunksDimensions(
		const UINT chunksCount,                             // the number of all chunks
		const UINT chunksCountInRow,                        // the number of chunks in line in each direction (positive: x/y/z; negative: x/y/z)
		const UINT chunkWidth,                              // width/depth of chunks 
		std::vector<DirectX::XMVECTOR> & outMinDimensions,
		std::vector<DirectX::XMVECTOR> & outMaxDimensions,
		std::vector<DirectX::XMVECTOR> & outChunksCenterPositions);

	void ComputeChunksColors(
		const UINT chunksCount,
		std::vector<DirectX::XMFLOAT4> & outColorsForChunks);
};