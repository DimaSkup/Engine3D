// ********************************************************************************
// Filename:     EntityManagerSerializer.h
// Description:  contains functional for serialization of the EntityManager data
//               as well as the components
// 
// Created:      26.06.24
// ********************************************************************************
#pragma once

#include "EntityManager.h"
#include "SerializationHelperTypes.h"

class EntityManagerSerializer
{
public:
	EntityManagerSerializer() {}

	void Serialize(
		EntityManager& entityMgr,
		const std::string& dataFilepath);

private:
	void WriteDataHeader(
		std::ofstream& fout,
		SerializedDataHeader& header);

	void SerializeDataOfEnttMgr(
		std::ofstream& fout,
		EntityManager& entityMgr);
};