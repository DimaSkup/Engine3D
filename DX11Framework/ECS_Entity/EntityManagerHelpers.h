// *********************************************************************************
// Filename:      EntityManagerHelpers.h
// Description:   constains helper functional for the EntityManager
// 
// Created:       13.05.24
// *********************************************************************************
#pragma once


#if 0
const std::string GetUniqueEntityIDBasedOn(
	const std::string& ID,
	const std::vector<std::string>& inIDsArr)
{
	// if there is such an ID in the array of IDs inside the EntityManager
	// we have to modify the input one before storing

	assert(!ID.empty());

	// check if we already have such an ID
	if (std::binary_search(inIDsArr.begin(), inIDsArr.end(), ID))
	{
		std::string newID;
		UINT num = 0;

		// generate new ID while it coincides with some another
		do
		{
			newID = { ID + "_" + std::to_string(num) };
			++num;

		} while (std::binary_search(inIDsArr.begin(), inIDsArr.end(), newID));

		return newID;
	}

	// just return the input ID since it is a unique
	else
	{
		return ID;
	}
}

///////////////////////////////////////////////////////////

const uint32_t GetIndexForNewEntityByID(
	const std::string& ID,
	const std::vector<std::string>& inIDsArr)
{
	//
	// search an index of the element after the input textID
	//

	assert(!ID.empty());

	const auto it = std::upper_bound(inIDsArr.begin(), inIDsArr.end(), ID);

	// there is no entities after this one
	if (it == inIDsArr.end())
	{
		return (inIDsArr.size()) ? (inIDsArr.size() - 1) : 0;
	}

	// we add a new element somewhere between of records in the data arrays so
	// define an index for it
	else
	{
		return static_cast<uint32_t>(std::distance(inIDsArr.begin(), it));
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

#endif