// *********************************************************************************
// Filename:     TransformSystemSerializeDeserializeHelpers.h
// Description:  contains helper functional for serialization and deserialization
//               of data from the Transform and WorldMatrix components
// Created:      13.06.24
// *********************************************************************************
#pragma once

#include "../../ECS_Common/ECS_Types.h"
#include "../../ECS_Common/LIB_Exception.h"

#include "../../ECS_Components/Transform.h"

#include <fstream>


void SerializeTransformData(std::ofstream& fout, Transform& t)
{
	const std::vector<EntityID>& ids = t.ids_;
	const std::vector<XMFLOAT3>& pos = t.positions_;
	const std::vector<XMFLOAT3>& dir = t.directions_;
	const std::vector<XMFLOAT3>& scale = t.scales_;

	// write how much data we have for each data array so later we will use 
	// this number for deserialization
	const std::string dataCountStr = "transform_data_count: " + std::to_string(std::ssize(ids));
	
	// write data into the data file
	fout.write(dataCountStr.c_str(), dataCountStr.size() * sizeof(char));
	fout.write((const char*)(ids.data()), ids.size() * sizeof(EntityID));
	fout.write((const char*)(pos.data()), pos.size() * sizeof(XMFLOAT3));
	fout.write((const char*)(dir.data()), dir.size() * sizeof(XMFLOAT3));
	fout.write((const char*)(scale.data()), scale.size() * sizeof(XMFLOAT3));
}

///////////////////////////////////////////////////////////

void DeserializeTransformData(std::ifstream& fin, Transform& t)
{
	UINT dataCount = 0;
	std::string ignore;

	fin >> ignore >> dataCount;

	// if we read wrong data block
	ASSERT_TRUE(ignore == "transform_data_count:", "read wrong data during deserialization of the Transform component data from a file");

	// ------------------------------------------

	std::vector<EntityID>& ids = t.ids_;
	std::vector<XMFLOAT3>& pos = t.positions_;
	std::vector<XMFLOAT3>& dir = t.directions_;
	std::vector<XMFLOAT3>& scales = t.scales_;

	// if earlier there was some data in the Transform component we crear it 
	ids.clear();
	pos.clear();
	dir.clear();
	scales.clear();

	// if we have any data for deserialization
	if (dataCount > 0)
	{
		// prepare enough amount of memory for data
		ids.resize(dataCount);
		pos.resize(dataCount);
		dir.resize(dataCount);
		scales.resize(dataCount);

		// deserialize the transform data into the data arrays
		fin.read((char*)(ids.data()), dataCount * sizeof(EntityID));
		fin.read((char*)(pos.data()), dataCount * sizeof(XMFLOAT3));
		fin.read((char*)(dir.data()), dataCount * sizeof(XMFLOAT3));
		fin.read((char*)(scales.data()), dataCount * sizeof(XMFLOAT3));
	}
}

///////////////////////////////////////////////////////////
