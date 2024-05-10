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
	while (std::binary_search(inTextIDsArr.begin(), inTextIDsArr.end(), newID))
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

template<typename T>
void ShiftRightAndFillWithData(
	const UINT shiftFactor,
	const UINT fromIdx,
	const std::vector<T>& inDataArr,
	std::vector<T>& outDataArr)
{
	// make empty place for some count (shiftFactor) of input data (inDataArr) 
	// in the output data array (outDataArr) and fill it in with data

	assert(shiftFactor == inDataArr.size());
	assert(fromIdx < outDataArr.size());
	assert(inDataArr.size() <= outDataArr.size());
	assert(shiftFactor <= outDataArr.size());

	std::shift_right(outDataArr.begin() + fromIdx, outDataArr.end(), shiftFactor);
	std::copy(inDataArr.begin(), inDataArr.end(), outDataArr.begin() + fromIdx);
}