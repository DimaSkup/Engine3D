///////////////////////////////////////////////////////////////////////////////////////////
// Filename:     LightStoreUpdateHelpers.h
// Description:  contains updating functional for the LightStore class
//
// Created:      15.02.24
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <vector>
#include <DirectXMath.h>

typedef unsigned int UINT;



///////////////////////////////////////////////////////////////////////////////////////////
//                    PRIVATE UPDATE API FOR POINT LIGHT SOURCES
///////////////////////////////////////////////////////////////////////////////////////////


void SelectPointLightsToUpdate(
	const UINT numOfPointLights,
	const std::vector<UINT> & inIDs,
	std::vector<UINT> & outPointLightsToUpdate)
{
	// define what point lights will be updated
	for (UINT idx = 0; idx < numOfPointLights; ++idx)
		outPointLightsToUpdate.push_back(inIDs[idx]);
}

/////////////////////////////////////////////////

void PreparePointLightsPositionsToUpdate(
	const std::vector<UINT> & inLightsToUpdate,
	const std::vector<DirectX::XMVECTOR> & inPositions,
	std::vector<DirectX::XMVECTOR> & outPositionsToUpdate)
{
	// make an array of positions data of the point lights which will be updated
	for (const UINT idx : inLightsToUpdate)
		outPositionsToUpdate.push_back(inPositions[idx]);
}

/////////////////////////////////////////////////

void PreparePointLightsPositionModificatorsToUpdate(
	const std::vector<UINT> & inLightsToUpdate,
	const std::vector<DirectX::XMVECTOR> & inPosModificators,
	std::vector<DirectX::XMVECTOR> & outPosModificators)
{
	// make an array of positions modificators data of the point lights which will be updated
	for (const UINT idx : inLightsToUpdate)
		outPosModificators.push_back(inPosModificators[idx]);
}

/////////////////////////////////////////////////

void ComputePointLightsPositionsToUpdate(
	const std::vector<DirectX::XMVECTOR> & inPositions,
	const std::vector<DirectX::XMVECTOR> & inPosModificators,
	std::vector<DirectX::XMVECTOR> & outUpdatedPositions)
{
	// compute new positions for the point lights which will be updated;
	// (updated_pos_vector = old_pos_vector * pos_modificator_vector)
	for (UINT data_idx = 0; data_idx < inPositions.size(); ++data_idx)
		outUpdatedPositions.push_back(DirectX::XMVectorAdd(inPositions[data_idx], inPosModificators[data_idx]));
}
