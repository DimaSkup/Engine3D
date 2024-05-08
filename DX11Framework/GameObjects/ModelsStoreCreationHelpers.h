// *********************************************************************************
// Filename:      ModelsStoreCreationHelphers.h
// Description:   functional for creation of model(s) for the ModelsStore;
// 
// Created:       06.05.24
// *********************************************************************************

#pragma once

#include <string>
#include <vector>
#include <algorithm>




const std::string GenerateTextID_BasedOn(
	const std::string & inTextID,
	const std::vector<std::string> & inTextIDsArr)
{
	// if there is such a textID in the array of textIDs inside the ModelsStore
	// we have to modify the input one before storing

	assert(!inTextID.empty());

	std::string newID {inTextID};
	
	// generate new ID while it coincides with some other
	while (std::binary_search(inTextIDsArr.begin(), inTextIDsArr.end(), inTextID))
	{
		static UINT num = 2;
		newID = { inTextID + "_" + std::to_string(num) };
		++num;
	}

	return newID;
}

///////////////////////////////////////////////////////////

const uint32_t DefineIndexForNewModelWithTextID(
	const std::string& inTextID,
	const std::vector<std::string>& inTextIDsArr)
{
	//
	// search an index of the element after the input textID
	//

	assert(!inTextID.empty());

	const auto it = std::upper_bound(inTextIDsArr.begin(), inTextIDsArr.end(), inTextID);

	// there is no models after this one
	if (it == inTextIDsArr.end())
	{
		return (inTextIDsArr.size()) ? (inTextIDsArr.size() - 1) : 0;
	}

	// we add a new element somewhere between of records in the data arrays so
	// define an index for it
	else
	{
		return static_cast<uint32_t>(std::distance(inTextIDsArr.begin(), it));
	}
}

///////////////////////////////////////////////////////////

void ShiftRightTextIDsAndFillWithData(
	const UINT shiftFactor,
	const UINT fromIdx,
	const std::vector<std::string> & inTextIDs,
	std::vector<std::string> & outTextIDs)
{
	// make empty place for some count (shiftFactor) of text IDs (inTextIDs) 
	// in the input textIDs' array (outTextIDsToFill) and fill it in with data

	assert(shiftFactor == inTextIDs.size());
	assert(fromIdx < outTextIDs.size());
	assert(inTextIDs.size() <= outTextIDs.size());
	assert(shiftFactor <= outTextIDs.size());

	std::shift_right(outTextIDs.begin() + fromIdx, outTextIDs.end(), shiftFactor);
	std::copy(inTextIDs.begin(), inTextIDs.end(), outTextIDs.begin() + fromIdx);
}

///////////////////////////////////////////////////////////

void ShiftRightPositionsAndFillWithData(
	const UINT shiftFactor,
	const UINT fromIdx,
	const std::vector<DirectX::XMVECTOR> & inPositions,
	std::vector<DirectX::XMVECTOR> & outPosArr)
{
	// make empty place for some count (shiftFactor) of positions (inPositions) 
	// in the input positions array (outPosArr) and fill it in with data

	assert(shiftFactor == inPositions.size());
	assert(fromIdx < outPosArr.size());
	assert(inPositions.size() <= outPosArr.size());

	std::shift_right(outPosArr.begin() + fromIdx, outPosArr.end(), shiftFactor);
	std::copy(inPositions.begin(), inPositions.end(), outPosArr.begin() + fromIdx);
}

///////////////////////////////////////////////////////////

void ShiftRightRotationsAndFillWithData(
	const UINT shiftFactor,
	const UINT fromIdx,
	const std::vector<DirectX::XMVECTOR>& inRotations,
	std::vector<DirectX::XMVECTOR>& outRotArr)
{
	// make empty place for some count (shiftFactor) of rotations (inRotations) 
	// in the input rotations array (outRotArr) and fill it in with data

	assert(shiftFactor == inRotations.size());
	assert(fromIdx < outRotArr.size());
	assert(inRotations.size() <= outRotArr.size());

	std::shift_right(outRotArr.begin() + fromIdx, outRotArr.end(), shiftFactor);
	std::copy(inRotations.begin(), inRotations.end(), outRotArr.begin() + fromIdx);
}

///////////////////////////////////////////////////////////

void ShiftRightScalesAndFillWithData(
	const UINT shiftFactor,
	const UINT fromIdx,
	const std::vector<DirectX::XMVECTOR>& inScales,
	std::vector<DirectX::XMVECTOR>& outScaleArr)
{
	// make empty place for some count (shiftFactor) of scales (inScales) 
	// in the input scales array (outScaleArr) and fill it in with data

	assert(shiftFactor == inScales.size());
	assert(fromIdx < outScaleArr.size());
	assert(inScales.size() <= outScaleArr.size());

	std::shift_right(outScaleArr.begin() + fromIdx, outScaleArr.end(), shiftFactor);
	std::copy(inScales.begin(), inScales.end(), outScaleArr.begin() + fromIdx);
}

///////////////////////////////////////////////////////////

void ShiftRightPositionModificatorsAndFillWithData(
	const UINT shiftFactor,
	const UINT fromIdx,
	const std::vector<DirectX::XMVECTOR> & inPosModif,
	std::vector<DirectX::XMVECTOR> & outPosModif)
{
	// make empty place for some count (shiftFactor) of 
	// position modificators (inPosModif) in the 
	// input pos modificators array (outPosModif) and fill it with data

	assert(shiftFactor == inPosModif.size());
	assert(fromIdx < outPosModif.size());
	assert(inPosModif.size() <= outPosModif.size());

	std::shift_right(outPosModif.begin() + fromIdx, outPosModif.end(), shiftFactor);
	std::copy(inPosModif.begin(), inPosModif.end(), outPosModif.begin() + fromIdx);
}

///////////////////////////////////////////////////////////

void ShiftRightRotationModificatorsAndFillWithData(
	const UINT shiftFactor,
	const UINT fromIdx,
	const std::vector<DirectX::XMVECTOR> & inRotModif,
	std::vector<DirectX::XMVECTOR> & outRotModif)
{
	// make empty place for some count (shiftFactor) of 
	// rotation modificators (inPosModif) in the 
	// input rot modificators array (outRotModif) and fill it with data

	assert(shiftFactor == inRotModif.size());
	assert(fromIdx < outRotModif.size());
	assert(inRotModif.size() <= outRotModif.size());

	std::shift_right(outRotModif.begin() + fromIdx, outRotModif.end(), shiftFactor);
	std::copy(inRotModif.begin(), inRotModif.end(), outRotModif.begin() + fromIdx);
}

///////////////////////////////////////////////////////////

void ShiftRightScaleModifatorsAndFillWithData(
	const UINT shiftFactor,
	const UINT fromIdx,
	const std::vector<DirectX::XMVECTOR>& inScaleModif,
	std::vector<DirectX::XMVECTOR>& outScaleModif)
{
	// make empty place for some count (shiftFactor) of 
	// position modificators (inPosModif) in the 
	// input pos modificators array (outPosModif) and fill it with data

	assert(shiftFactor == inScaleModif.size());
	assert(fromIdx < outScaleModif.size());
	assert(inScaleModif.size() <= outScaleModif.size());

	std::shift_right(outScaleModif.begin() + fromIdx, outScaleModif.end(), shiftFactor);
	std::copy(inScaleModif.begin(), inScaleModif.end(), outScaleModif.begin() + fromIdx);
}

///////////////////////////////////////////////////////////

void ShiftRightWorldMatricesAndFillWithData(
	const UINT shiftFactor,
	const UINT fromIdx,
	const std::vector<DirectX::XMMATRIX>& inWorldMatrices,
	std::vector<DirectX::XMMATRIX>& outWorldMatrices)
{
	// make empty place for some count (shiftFactor) of 
	// world matrices (inWorldMatrices) in the 
	// input world matrices array (outWorldMatrices) and fill it with data

	assert(shiftFactor == inWorldMatrices.size());
	assert(fromIdx < outWorldMatrices.size());
	assert(inWorldMatrices.size() <= outWorldMatrices.size());

	std::shift_right(outWorldMatrices.begin() + fromIdx, outWorldMatrices.end(), shiftFactor);
	std::copy(inWorldMatrices.begin(), inWorldMatrices.end(), outWorldMatrices.begin() + fromIdx);
}

///////////////////////////////////////////////////////////

void ShiftRightTextureTransformationsAndFillWithData(
	const UINT shiftFactor,
	const UINT fromIdx,
	const std::vector<DirectX::XMMATRIX>& inTexTransform,
	std::vector<DirectX::XMMATRIX>& outTexTransform)
{
	// make empty place for some count (shiftFactor) of 
	// texture transformations (inTexTransforms) in the 
	// input texture transformations array (outTexTransform) and fill it with data

	assert(shiftFactor == inTexTransform.size());
	assert(fromIdx < outTexTransform.size());
	assert(inTexTransform.size() <= outTexTransform.size());

	std::shift_right(outTexTransform.begin() + fromIdx, outTexTransform.end(), shiftFactor);
	std::copy(inTexTransform.begin(), inTexTransform.end(), outTexTransform.begin() + fromIdx);
}

///////////////////////////////////////////////////////////

void ShiftRightMaterialsAndFillWithData(
	const UINT shiftFactor,
	const UINT fromIdx,
	const std::vector<Material>& inMaterials,
	std::vector<Material>& outMaterials)
{
	// make empty place for some count (shiftFactor) of  materials (inMaterials) 
	// in the input materials array (outMaterials) and fill it with data

	assert(shiftFactor == inMaterials.size());
	assert(fromIdx < outMaterials.size());
	assert(inMaterials.size() <= outMaterials.size());

	std::shift_right(outMaterials.begin() + fromIdx, outMaterials.end(), shiftFactor);
	std::copy(inMaterials.begin(), inMaterials.end(), outMaterials.begin() + fromIdx);
}

///////////////////////////////////////////////////////////

void ShiftRightRelationsModelToVB_AndFillWithData(
	const UINT shiftFactor,
	const UINT fromIdx,
	const std::vector<UINT>& inRelations,
	std::vector<UINT>& outRelations)
{
	// make empty place for some count (shiftFactor) of relations between
	// models and some vertex buffer (inRelations) 
	// in the input relations array (outRelations) and fill it with data

	assert(shiftFactor == inRelations.size());
	assert(fromIdx < outRelations.size());
	assert(inRelations.size() <= outRelations.size());

	std::shift_right(outRelations.begin() + fromIdx, outRelations.end(), shiftFactor);
	std::copy(inRelations.begin(), inRelations.end(), outRelations.begin() + fromIdx);
}
