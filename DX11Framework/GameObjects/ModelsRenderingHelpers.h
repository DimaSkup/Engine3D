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
	const UINT chunksCount, 
	const std::vector<DirectX::XMFLOAT3> & minChunksDimensions,
	const std::vector<DirectX::XMFLOAT3> & maxChunksDimensions,
	const std::vector<std::vector<uint32_t>> relationsChunksToModels,
	FrustumClass & frustum,
	std::vector<uint32_t> & outIDsToRender)
{
	// go through each chunk and check if we see it 
	// if so we set all the related models for rendering

	for (UINT chunk_idx = 0; chunk_idx < chunksCount; ++chunk_idx)
	{
		if (frustum.CheckRectangle22(minChunksDimensions[chunk_idx], maxChunksDimensions[chunk_idx]))
		{
			// add all the related models of this chunk for rendering
			outIDsToRender.insert(outIDsToRender.end(), relationsChunksToModels[chunk_idx].begin(), relationsChunksToModels[chunk_idx].end());
		}
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

void PrepareTexturesSRV_OfModelsToRender(
	const std::vector<uint32_t> & inIDs,
	const std::vector<TextureClass*> & textures,
	std::vector<ID3D11ShaderResourceView* const*> & texturesSRVs)
{
	// get a bunch of pointer to textures by input IDs

	ID3D11ShaderResourceView* const* pDiffuseTexture = textures[inIDs[0]]->GetTextureResourceViewAddress();

	//for (const uint32_t idx : inIDs)
	texturesSRVs.insert(texturesSRVs.end(), inIDs.size(), pDiffuseTexture);
}

