// ************************************************************************************
// Filename:     ModelsModificationHelpers.h
// Description:  contains private functional for modification of
//               the models data during each frame;
//
// Created:      13.02.24
// ************************************************************************************

#pragma once

#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>
#include "../Engine/COMException.h"

using namespace DirectX;


void GetIndicesOfModelsToUpdate(
	const std::vector<std::string> & inTextIDs,                 // main text IDs array
	const std::vector<std::string> & inTextIDsOfModelsToUpdate, // text IDs of models which will be updated
	std::vector<UINT> & outIdxsOfModelsToUpdate)                // container for models indices
{
	// get indices of models by its textIDs

	assert(!inTextIDs.empty());
	assert(!inTextIDsOfModelsToUpdate.empty());
	assert(outIdxsOfModelsToUpdate.empty());

	const auto iter_inTextIDsBegin = inTextIDs.begin();
	const auto iter_inTextIDsEnd = inTextIDs.end();

	for (const std::string & textID : inTextIDsOfModelsToUpdate)
	{
		const auto it = std::find(iter_inTextIDsBegin, iter_inTextIDsEnd, textID);

		if (it != iter_inTextIDsEnd)
		{
			// store an index of the model
			outIdxsOfModelsToUpdate.push_back((UINT)std::distance(iter_inTextIDsBegin, it));
		}
		else
		{
			COM_ERROR_IF_FALSE(false, "there is no such textID in the main array: " + textID);
		}
	}
}

///////////////////////////////////////////////////////////

void PrepareModificationVectors(
	const std::vector<UINT> & inModelsToUpdate,
	const std::vector<DirectX::XMVECTOR> & inModification,
	std::vector<DirectX::XMVECTOR> & outModificationVectorsToUpdate)
{
	// store the modification vectors to update
	for (UINT model_index : inModelsToUpdate)
		outModificationVectorsToUpdate.push_back(inModification[model_index]);
}

void ComputeModificationVectors(
	const float deltaTime,
	_Inout_ std::vector<DirectX::XMVECTOR> & inModificatorsArr)
{
	for (DirectX::XMVECTOR & modificator : inModificatorsArr)
	{
		modificator = DirectX::XMVectorScale(modificator, deltaTime*10.0f);
	}
}



// ************************************************************************************
//                       POSITIONS PRIVATE MODIFICATION API
// ************************************************************************************

void PreparePositionsToUpdate(
	const std::vector<UINT> & inModelsToUpdate,
	const std::vector<DirectX::XMVECTOR> & inPositions, 
	std::vector<DirectX::XMVECTOR> & outPositionToUpdate)
{
	// store the positions to update
	for (UINT model_index : inModelsToUpdate)
		outPositionToUpdate.push_back(inPositions[model_index]);
}

///////////////////////////////////////////////////////////

void ComputePositions(
	const float deltaTime,
	const std::vector<DirectX::XMVECTOR> & inPosToUpdate,
	const std::vector<DirectX::XMVECTOR> & inPosModifications,
	std::vector<DirectX::XMVECTOR> & outPosToUpdate)
{
	// compute the new positions by input positions and position modificators

	assert(inPosToUpdate.size() == inPosModifications.size());
	
	for (UINT idx = 0; idx < inPosToUpdate.size(); ++idx)
	{
		// new_pos_vector = old_pos_vector + (modification_vector * delta_time)
		outPosToUpdate.push_back(DirectX::XMVectorAdd(inPosToUpdate[idx], inPosModifications[idx] * deltaTime));
	}
}

///////////////////////////////////////////////////////////

void ApplyPositions(
	const std::vector<UINT> & inModelsToUpdate,
	const std::vector<DirectX::XMVECTOR> & inPositionsToUpdate,
	std::vector<DirectX::XMVECTOR> & outUpdatedPositions)
{
	// store new positions values into the models_store positions array

	assert(outUpdatedPositions.size() >= inModelsToUpdate.size());

	UINT data_idx = 0;

	for (UINT model_index : inModelsToUpdate)
		outUpdatedPositions[model_index] = inPositionsToUpdate[data_idx++];
}



// ************************************************************************************
//                        ROTATIONS PRIVATE MODIFICATION API
// ************************************************************************************

void PrepareRotationsToUpdate(
	const std::vector<UINT> & inModelsToUpdate,
	const std::vector<DirectX::XMVECTOR> & inRotations,
	std::vector<DirectX::XMVECTOR> & outRotationsToUpdate)
{
	// store the positions to update
	for (UINT model_index : inModelsToUpdate)
		outRotationsToUpdate.push_back(inRotations[model_index]);
}

///////////////////////////////////////////////////////////

void ComputeRotations(
	const float deltaTime,
	const std::vector<DirectX::XMVECTOR> & inRotToUpdate,
	const std::vector<DirectX::XMVECTOR> & inRotModificators,
	std::vector<DirectX::XMVECTOR> & outRotationsToUpdate)
{
	// compute the new rotations by input rotations and rotation modificators

	assert(inRotToUpdate.size() == inRotModificators.size());

	const XMVECTOR minRange{ -XM_PI, -XM_PI, -XM_PI };
	const XMVECTOR maxRange{ XM_PI, XM_PI, XM_PI };

	for (UINT idx = 0; idx < inRotToUpdate.size(); ++idx)
	{
		// old_rotation_vector * rotation_quaternion

		const DirectX::XMVECTOR newRotation = XMVector3Rotate(
			inRotToUpdate[idx],
			inRotModificators[idx]);//DirectX::XMVectorScale(inRotModificators[idx], deltaTime));

		outRotationsToUpdate.push_back(XMVectorClamp(newRotation, minRange, maxRange));
	}
}

///////////////////////////////////////////////////////////

void ApplyRotations(
	const std::vector<UINT> & inModelsToUpdate,
	const std::vector<DirectX::XMVECTOR> & inRotationsToUpdate,
	std::vector<DirectX::XMVECTOR> & outUpdatedRotations)
{
	// store new rotations values into the models_store rotations array

	assert(outUpdatedRotations.size() >= inModelsToUpdate.size());

	UINT data_idx = 0;
	
	for (UINT model_index : inModelsToUpdate)
		outUpdatedRotations[model_index] = inRotationsToUpdate[data_idx++];
}




// ************************************************************************************
//                     WORLD MATRICES PRIVATE MODIFICATION API
// ************************************************************************************

void ComputeAndApplyWorldMatrices(
	const std::vector<UINT> & inModelsToUpdate,
	const std::vector<DirectX::XMVECTOR>& inScales,
	const std::vector<DirectX::XMVECTOR>& inRotations,
	const std::vector<DirectX::XMVECTOR>& inTranslations,
	std::vector<DirectX::XMMATRIX> & outUpdatedWorldMatrices)
{
	// compute and apply new world matrices for models by idxs from inModelsToUpdate

	assert(inModelsToUpdate.size() == inScales.size());
	assert(inScales.size() == inTranslations.size());
	assert(inTranslations.size() == inRotations.size());
	assert(inRotations.size() < outUpdatedWorldMatrices.size());

	UINT data_idx = 0;

	for (UINT model_idx : inModelsToUpdate)
	{
		/*
		outUpdatedWorldMatrices[model_idx] = 
			DirectX::XMMatrixScalingFromVector(inScales[data_idx]) *
			DirectX::XMMatrixRotationRollPitchYawFromVector(inRotations[data_idx]) *
			DirectX::XMMatrixTranslationFromVector(inTranslations[data_idx]);
			*/
		
		outUpdatedWorldMatrices[model_idx] *= DirectX::XMMatrixAffineTransformation(
			inScales[data_idx],
			outUpdatedWorldMatrices[model_idx].r[3],
			inRotations[data_idx],
			inTranslations[data_idx]);
			

		++data_idx;
	}
		
}
