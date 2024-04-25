///////////////////////////////////////////////////////////////////////////////////////////
// Filename:     ModelsRenderingHelpers.h
// Description:  contains private functional for rendering of 
//               the models data during each frame;
//
// Created:      03.03.24
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>

#include "textureclass.h"
#include "../Render/frustumclass.h"

typedef unsigned int UINT;




void PrepareIDsOfModelsToRender(
	const DirectX::XMFLOAT3 & cameraPos,
	const float cameraDepth,
	const UINT chunksCount, 
	const std::vector<DirectX::XMVECTOR> & minChunksDimensions,
	const std::vector<DirectX::XMVECTOR> & maxChunksDimensions,
	const std::vector<DirectX::XMVECTOR> & chunksCenterPositions,
	const std::vector<std::vector<uint32_t>> relationsChunksToModels,
	FrustumClass & frustum,
	std::vector<uint32_t> & outIDsToRender)
{
	// go through each chunk and check if we see it 
	// if so we set all the related models for rendering

	const float squareVisibilityDepth = std::powf(cameraDepth, 2);

	const XMVECTOR camPosVec = XMLoadFloat3(&cameraPos);
	std::vector<float> squareOfDistToCenterArr;
	std::vector<UINT> chunksIdxsInVisibilityRange;
	std::vector<UINT> chunksIdxsInFrustum;

	// compute the quare of distances to the chunks center positions
	for (const XMVECTOR & pos : chunksCenterPositions)
	{
		const XMVECTOR camToChunkCenter(XMVectorSubtract(pos,camPosVec));  // vector from the camera current position to the chunk's center
		squareOfDistToCenterArr.push_back(XMVectorGetX(XMVector4Dot(camToChunkCenter, camToChunkCenter)));
	}

	// define what chunks in the range of our visibility
	for (UINT chunk_idx = 0; chunk_idx < chunksCount; ++chunk_idx)
	{
		if (squareVisibilityDepth > squareOfDistToCenterArr[chunk_idx])
			chunksIdxsInVisibilityRange.push_back(chunk_idx);
	}

	// define if some particular chunk (which is in visibility range) is also in the frustum
	for (const UINT chunk_idx : chunksIdxsInVisibilityRange)
	{
		if (frustum.CheckRectangle22(minChunksDimensions[chunk_idx], maxChunksDimensions[chunk_idx]))
			chunksIdxsInFrustum.push_back(chunk_idx);
	}

	// add all the models of the visibile chunks for rendering
	for (const UINT chunk_idx : chunksIdxsInFrustum)
	{
		outIDsToRender.insert(outIDsToRender.end(), relationsChunksToModels[chunk_idx].begin(), relationsChunksToModels[chunk_idx].end());
	}
}

///////////////////////////////////////////////////////////

void GetRelatedInputModelsToVertexBuffer(
	const UINT vertexBufferIdx,
	const std::vector<UINT> & modelIdxs,
	const std::vector<UINT> & relatedToVertexBufferByIdx,
	_Out_ std::vector<uint32_t> & outModelsIdxs)    
{
	// THIS FUNC defines indices of input models which are related 
	// to vertex buffer by vertexBufferIdx

	for (UINT model_idx : modelIdxs)
	{
		if (vertexBufferIdx == relatedToVertexBufferByIdx[model_idx])
			outModelsIdxs.push_back(model_idx);
	}
}

///////////////////////////////////////////////////////////

void PrepareWorldMatricesToRender(
	const std::vector<uint32_t> & inIDs,
	const std::vector<DirectX::XMMATRIX> & inWorldMatrices,
	std::vector<DirectX::XMMATRIX> & outWorldMatrices)
{
	// get a bunch of world matrices by input models IDs (inIDs)

	for (const uint32_t idx : inIDs)
		outWorldMatrices.push_back(inWorldMatrices[idx]);
}

///////////////////////////////////////////////////////////

void PrepareTexturesSRV_ToRender(
	const std::map<aiTextureType, TextureClass*> & texturesMap,
	std::vector<ID3D11ShaderResourceView* const*> & texturesSRVs)
{
	// get a bunch of pointer to textures by input IDs

	texturesSRVs.clear();

	for (auto& texture : texturesMap)
	{
		// get double pointer to a shader resource view
		ID3D11ShaderResourceView* const* ppSRV = texture.second->GetTextureResourceViewAddress();
		texturesSRVs.push_back(ppSRV);
	}
}

