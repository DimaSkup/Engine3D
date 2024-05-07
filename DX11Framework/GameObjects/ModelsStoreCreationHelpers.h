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
	std::vector<std::string> & outTextIDsToFill)
{
	// make empty place for some count (shiftFactor) of text IDs (inTextIDs) 
	// in the input textIDs' array (outTextIDsToFill) and fill it in with data

#if _DEBUG || DEBUG

	const UINT inTextIDsSize = inTextIDs.size();
	const UINT outTextIDsSize = outTextIDsToFill.size();
	
	assert(shiftFactor == inTextIDsSize);
	assert(fromIdx < outTextIDsSize);
	assert(inTextIDsSize <= outTextIDsSize);
	assert(shiftFactor <= outTextIDsSize);

#endif

	std::shift_right(outTextIDsToFill.begin() + fromIdx, outTextIDsToFill.end(), shiftFactor);
	std::copy(inTextIDs.begin(), inTextIDs.end(), outTextIDsToFill.begin() + fromIdx);
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

#if _DEBUG || DEBUG
	const UINT inPosArrSize = inPositions.size();
	const UINT outPosArrSize = outPosArr.size();

	assert(shiftFactor == inPosArrSize);
	assert(fromIdx < outPosArrSize);
	assert(inPosArrSize <= outPosArrSize);
#endif

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

	const UINT inRotArrSize = inRotations.size();
	const UINT outRotArrSize = outRotArr.size();

	assert(shiftFactor == inRotArrSize);
	assert(fromIdx < outRotArrSize);
	assert(inRotArrSize <= outRotArrSize);


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

	const UINT inScaleArrSize = inScales.size();
	const UINT outScaleArrSize = outScaleArr.size();

	assert(shiftFactor == inScaleArrSize);
	assert(fromIdx < outScaleArrSize);
	assert(inScaleArrSize <= outScaleArrSize);


	std::shift_right(outScaleArr.begin() + fromIdx, outScaleArr.end(), shiftFactor);
	std::copy(inScales.begin(), inScales.end(), outScaleArr.begin() + fromIdx);
}

///////////////////////////////////////////////////////////

void ShiftRightPositionModificatorAndFillWithData(
	const UINT shiftFactor,
	const UINT fromIdx,
	const std::vector<DirectX::XMVECTOR> & inPosModif,
	std::vector<DirectX::XMVECTOR> & outPosModif)
{
	// make empty place for some count (shiftFactor) of 
	// position modificators (inPosModif) in the 
	// input pos modificators array (outPosModif) and fill it with data

	const UINT inPosModifArrSize = inPosModif.size();
	const UINT outPosModifArrSize = outPosModif.size();

	assert(shiftFactor == inPosModifArrSize);
	assert(fromIdx < outPosModifArrSize);
	assert(inPosModifArrSize <= outPosModifArrSize);

	std::shift_right(outPosModif.begin() + fromIdx, outPosModif.end(), shiftFactor);
	std::copy(inPosModif.begin(), inPosModif.end(), outPosModif.begin() + fromIdx);
}

///////////////////////////////////////////////////////////

void ShiftRightRotationModificatorAndFillWithData(
	const UINT shiftFactor,
	const UINT fromIdx,
	const std::vector<DirectX::XMVECTOR> & inRotModif,
	std::vector<DirectX::XMVECTOR> & outRotModif)
{
	// make empty place for some count (shiftFactor) of 
	// position modificators (inPosModif) in the 
	// input pos modificators array (outPosModif) and fill it with data

	const UINT inPosModifArrSize = inRotModif.size();
	const UINT outPosModifArrSize = outRotModif.size();

	assert(shiftFactor == inPosModifArrSize);
	assert(fromIdx < outPosModifArrSize);
	assert(inPosModifArrSize <= outPosModifArrSize);

	std::shift_right(outRotModif.begin() + fromIdx, outRotModif.end(), shiftFactor);
	std::copy(inRotModif.begin(), inRotModif.end(), outRotModif.begin() + fromIdx);
}

///////////////////////////////////////////////////////////

void ShiftRightScalesModifatorAndFillWithData(
	const UINT shiftFactor,
	const UINT fromIdx,
	const std::vector<DirectX::XMVECTOR>& inPosModif,
	std::vector<DirectX::XMVECTOR>& outPosModif)
{
	// make empty place for some count (shiftFactor) of 
	// position modificators (inPosModif) in the 
	// input pos modificators array (outPosModif) and fill it with data

	const UINT inPosModifArrSize = inPosModif.size();
	const UINT outPosModifArrSize = outPosModif.size();

	assert(shiftFactor == inPosModifArrSize);
	assert(fromIdx < outPosModifArrSize);
	assert(inPosModifArrSize <= outPosModifArrSize);

	std::shift_right(outPosModif.begin() + fromIdx, outPosModif.end(), shiftFactor);
	std::copy(inPosModif.begin(), inPosModif.end(), outPosModif.begin() + fromIdx);
}

///////////////////////////////////////////////////////////