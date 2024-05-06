// *********************************************************************************
// Filename:      ModelsStoreCreationHelphers.h
// Description:   functional for creation of model(s) for the ModelsStore;
// 
// Created:       06.05.24
// *********************************************************************************

#pragma once

#include <string>
#include <vector>






const std::string GenerateTextID_BasedOn(
	const std::string & inTextID,
	const std::vector<std::string> & inTextIDsArr)
{
	// if there is such a textID so we have to modify the input one for storing

	std::string newID {inTextID};
	
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
	// index of model's data
	uint32_t idx = 0;

	return idx;
}