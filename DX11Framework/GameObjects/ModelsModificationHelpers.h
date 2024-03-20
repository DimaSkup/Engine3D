///////////////////////////////////////////////////////////////////////////////////////////
// Filename:     ModelsModificationHelpers.h
// Description:  contains private functional for modification of
//               the models data during each frame;
//
// Created:      13.02.24
///////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;


void PrepareModificationVectors(
	const std::vector<UINT> & inModelsToUpdate,
	const std::vector<DirectX::XMVECTOR> & inModification,
	std::vector<DirectX::XMVECTOR> & outModificationVectorsToUpdate)
{
	// store the modification vectors to update
	for (UINT model_index : inModelsToUpdate)
		outModificationVectorsToUpdate.push_back(inModification[model_index]);
}



///////////////////////////////////////////////////////////////////////////////////////////
//                       POSITIONS PRIVATE MODIFICATION API
///////////////////////////////////////////////////////////////////////////////////////////

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
	const UINT numOfModelsToUpdate,
	const float deltaTime,
	const std::vector<DirectX::XMVECTOR> & inPositionsToUpdate,
	const std::vector<DirectX::XMVECTOR> & inPosModifications,
	std::vector<DirectX::XMVECTOR> & outPositionsToUpdate)
{
	// compute the new positions
	for (UINT idx = 0; idx < numOfModelsToUpdate; ++idx)
	{
		// new_pos_vector = old_pos_vector + modification_vector
		outPositionsToUpdate.push_back(DirectX::XMVectorAdd(
			inPositionsToUpdate[idx], inPosModifications[idx] * deltaTime));
	}
}

///////////////////////////////////////////////////////////

void ApplyPositions(
	const std::vector<UINT> & inModelsToUpdate,
	const std::vector<DirectX::XMVECTOR> & inPositionsToUpdate,
	std::vector<DirectX::XMVECTOR> & outUpdatedPositions)
{
	assert(outUpdatedPositions.size() >= inModelsToUpdate.size());

	UINT data_idx = 0;

	// store new positions values into the models_store positions array
	for (UINT model_index : inModelsToUpdate)
		outUpdatedPositions[model_index] = inPositionsToUpdate[data_idx++];
}



///////////////////////////////////////////////////////////////////////////////////////////
//                        ROTATIONS PRIVATE MODIFICATION API
///////////////////////////////////////////////////////////////////////////////////////////

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
	const UINT numOfModelsToUpdate,
	const float deltaTime,
	const std::vector<DirectX::XMVECTOR> & inRotationsToUpdate,
	const std::vector<DirectX::XMVECTOR> & inModifications,
	std::vector<DirectX::XMVECTOR> & outRotationsToUpdate)
{
	// compute the new rotations
	for (UINT idx = 0; idx < numOfModelsToUpdate; ++idx)
	{


		// old_rotation_vector * rotation_quaternion
		//const DirectX::XMVECTOR rotatedVector = XMVector3Rotate(inRotationsToUpdate[idx], XMQuaternionRotationRollPitchYawFromVector(inModifications[idx] * deltaTime));
		const DirectX::XMVECTOR rotationVec(DirectX::XMVectorScale(inModifications[idx], deltaTime));
		const DirectX::XMVECTOR quat (DirectX::XMQuaternionRotationRollPitchYawFromVector(rotationVec));

		outRotationsToUpdate.push_back(XMVector3Rotate(
			inRotationsToUpdate[idx], 
			quat
		));
	}
}

///////////////////////////////////////////////////////////

void ApplyRotations(
	const std::vector<UINT> & inModelsToUpdate,
	const std::vector<DirectX::XMVECTOR> & inRotationsToUpdate,
	std::vector<DirectX::XMVECTOR> & outUpdatedRotations)
{
	assert(outUpdatedRotations.size() >= inModelsToUpdate.size());

	UINT data_idx = 0;

	// store new rotations values into the models_store rotations array
	for (UINT model_index : inModelsToUpdate)
		outUpdatedRotations[model_index] = inRotationsToUpdate[data_idx++];
}




///////////////////////////////////////////////////////////////////////////////////////////
//                     WORLD MATRICES PRIVATE MODIFICATION API
///////////////////////////////////////////////////////////////////////////////////////////

void PrepareTranslationMatrices(
	const UINT numOfModelsToUpdate,
	const std::vector<DirectX::XMVECTOR> & inPositions,
	std::vector<DirectX::XMMATRIX> & outTranslationMatrices)
{
	// compute translation matrices
	for (UINT data_idx = 0; data_idx < numOfModelsToUpdate; ++data_idx)
		outTranslationMatrices.push_back(DirectX::XMMatrixTranslationFromVector(inPositions[data_idx]));
}

///////////////////////////////////////////////////////////

void PrepareRotationMatrices(
	const UINT numOfModelsToUpdate,
	const std::vector<DirectX::XMVECTOR> & inRotations,
	std::vector<DirectX::XMMATRIX> & outRotationMatricesToUpdate)
{
	// compute rotation matrices
	for (UINT data_idx = 0; data_idx < numOfModelsToUpdate; ++data_idx)
		outRotationMatricesToUpdate.push_back(DirectX::XMMatrixRotationRollPitchYawFromVector(inRotations[data_idx]));
}

///////////////////////////////////////////////////////////

void ComputeWorldMatricesToUpdate(
	const UINT numOfModelsToUpdate,
	const std::vector<DirectX::XMMATRIX> & inTranslationsMatrices,
	const std::vector<DirectX::XMMATRIX> & inRotationsMatrices,
	std::vector<DirectX::XMMATRIX> & outWorldMatricesToUpdate)
{
	// world_matrix = translation_mat * rotation_mat
	for (UINT data_idx = 0; data_idx < numOfModelsToUpdate; ++data_idx)
	{
		outWorldMatricesToUpdate.push_back 
		(
			inRotationsMatrices[data_idx] * inTranslationsMatrices[data_idx]
		);
	}
}

///////////////////////////////////////////////////////////

void ApplyWorldMatrices(
	const std::vector<UINT> & inModelsToUpdate,
	const std::vector<DirectX::XMMATRIX> & inWorldMatrices,
	std::vector<DirectX::XMMATRIX> & outUpdatedWorldMatrices)
{
	UINT data_idx = 0;

	// apply new values of the world matrices 
	for (UINT model_index : inModelsToUpdate)
		outUpdatedWorldMatrices[model_index] = inWorldMatrices[data_idx++];
}
