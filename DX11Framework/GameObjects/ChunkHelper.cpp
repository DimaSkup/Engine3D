#include "ChunkHelper.h"
#include "../Engine/log.h"

using namespace DirectX;

void ChunkHelper::ComputeChunksDimensions(
	const UINT chunksCount,                             // the number of all chunks
	const UINT chunksCountInRow,                        // the number of chunks in line in each direction (positive: x/y/z; negative: x/y/z)
	const UINT chunkWidth,                              // width/depth of chunks 
	std::vector<DirectX::XMVECTOR> & outMinDimensions,
	std::vector<DirectX::XMVECTOR> & outMaxDimensions,
	std::vector<DirectX::XMVECTOR> & outChunksCenterPositions)

{
	// THIS FUNCTION computes the minimal and maximal dimension points for each chunk 

	Log::Debug(LOG_MACRO, "computation of chunk dimensions");

	outMinDimensions.resize(chunksCount);
	outMaxDimensions.resize(chunksCount);
	outChunksCenterPositions.resize(chunksCount);

	UINT chunk_idx = 0;

	const float fChunkWidth = static_cast<float>(chunkWidth);
	const float fChunksCountInRow = static_cast<float>(chunksCountInRow);

	const int halfChunksRow = static_cast<int>(0.5f * chunksCountInRow);
	const int startIdx = -halfChunksRow;
	
	// build chunks around the Origin (0, 0, 0)
	for (int width_idx = startIdx; width_idx < halfChunksRow; ++width_idx) // by X-axis
	{
		for (int depth_idx = startIdx; depth_idx < halfChunksRow; ++depth_idx) // by Z-axis
		{
			
			// set minimal and maximal dimensions for the chunk by chunk_idx
			outMinDimensions[chunk_idx] = { (width_idx * fChunkWidth),        -fChunkWidth,      (depth_idx * fChunkWidth) };
			outMaxDimensions[chunk_idx] = { (width_idx + 1.0f) * fChunkWidth,  fChunkWidth,      (depth_idx + 1.0f) * fChunkWidth };

			// set center position of the chunk by chunk_idx
			outChunksCenterPositions[chunk_idx] = 0.5f * (outMaxDimensions[chunk_idx] + outMinDimensions[chunk_idx]);

			++chunk_idx;

		} // for
	} // for

#if 0
	// IF WE WANT TO CREATE CHUNKS ALSO BY Y-AXIS 

	// build chunks around the Origin (0, 0, 0)
	for (int width_idx = startIdx; width_idx < halfChunksRow; ++width_idx)
	{
		for (int height_idx = startIdx; height_idx < halfChunksRow; ++height_idx)
		{
			for (int depth_idx = startIdx; depth_idx < halfChunksRow; ++depth_idx)
			{
				// set minimal and maximal dimensions for the chunk by chunk_idx
				outMinDimensions[chunk_idx] = { (width_idx * fChunkWidth),        (height_idx*fChunkWidth),      (depth_idx * fChunkWidth) };
				outMaxDimensions[chunk_idx] = { (width_idx + 1.0f) * fChunkWidth, (height_idx + 1)*fChunkWidth,  (depth_idx + 1.0f) * fChunkWidth };

				// set center position of the chunk by chunk_idx
				outChunksCenterPositions[chunk_idx] = 0.5f * (outMaxDimensions[chunk_idx] + outMinDimensions[chunk_idx]);

				++chunk_idx;

			} // for
		} // for
	} // for
#endif
}

///////////////////////////////////////////////////////////

void ChunkHelper::ComputeChunksColors(const UINT chunksCount,
	std::vector<DirectX::XMFLOAT4> & outColorsForChunks)
{
	// THIS FUNCTION creates unique colous for each chunk (for debug purposes)

	outColorsForChunks.resize(chunksCount);

	for (UINT idx = 0; idx < chunksCount; ++idx)
	{
		const float red = MathHelper::RandF();
		const float green = MathHelper::RandF();
		const float blue = MathHelper::RandF();

		outColorsForChunks[idx] = { red, green, blue, 1.0f };
	}
}