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

typedef unsigned int UINT;




void PrepareIDsOfModelsToRender(
	const UINT inVertexBufferIdx,
	const std::vector<uint32_t> & inVertexBufferToModelRelations,
	std::vector<uint32_t> & outIDsToRender)
{
	// get a bunch of models which are related to the same 
	// vertex buffer by (inVertexBufferIdx) and get its IDs (ids of models)

	for (UINT model_idx = 0; model_idx < inVertexBufferToModelRelations.size(); ++model_idx)
	{
		// if the model by model_idx is related to the input vertex buffer idx
		if (inVertexBufferToModelRelations[model_idx] == inVertexBufferIdx)
			outIDsToRender.push_back(model_idx);
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

