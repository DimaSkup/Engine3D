// ********************************************************************************
// Filename:     EntityManagerDeserializer.h
// Description:  contains functional for deserialization of the EntityManager data
//               as well as the components
// 
// Created:      26.06.24
// ********************************************************************************
#pragma once

#include "EntityManager.h"
#include "SerializationHelperTypes.h"

namespace ECS
{

class EntityManagerDeserializer
{
public:
	EntityManagerDeserializer() {}

	void Deserialize(
		EntityManager& entityMgr,
		const std::string& dataFilepath);

private:
	void ReadDataHeader(
		std::ifstream& fin,
		SerializedDataHeader& header);

	void DeserializeDataOfEnttMgr(
		std::ifstream& fin,
		EntityManager& entityMgr);
};

}