// ********************************************************************************
// Filename:      SysUtils.h
// Description:   contains some utils which are common for the ECS systems
// 
// Created:       06.09.24
// ********************************************************************************
#pragma once

#include <vector>
#include "../../Common/Utils.h"
#include "../../Common/log.h"

namespace SysUtils
{


static bool RecordsExist(
	const std::vector<EntityID>& idsFromComponent,
	const std::vector<EntityID>& ids)
{
	// check if array of ids from some component contains ids from input arr (ids)
	// 
	// return: true -- if all the ids are already stored in the component
	//         false -- if some id are not stored in the component 

	std::vector<bool> exists(std::ssize(ids));

	for (u32 idx = 0; const EntityID& id : ids)
		exists[idx++] = Utils::BinarySearch(idsFromComponent, id);

	// if some ids isn't placed in the component we print a msg about it
	if (Utils::BinarySearch(exists, false))
	{
		ptrdiff_t idx = Utils::FindIdxOfVal(exists, false);
		ECS::Log::Error("there is no record by id (" + std::to_string(ids[idx]) + ") in the component");
		return false;
	}

	// all the ids are exists in the component
	return true;
}

///////////////////////////////////////////////////////////

static bool RecordsNotExist(
	const std::vector<EntityID>& idsFromComponent,
	const std::vector<EntityID>& ids)
{
	// check if array of ids from some component DON'T contain ids from input arr (ids)
	// 
	// return: true -- if there are no such ids in the component's ids arr
	//         false -- there is some id in the component

	std::vector<bool> exists(std::ssize(ids));

	for (u32 idx = 0; const EntityID & id : ids)
		exists[idx++] = Utils::BinarySearch(idsFromComponent, id);

	// if some ids is placed in the component we print a msg about it
	if (Utils::BinarySearch(exists, true))
	{
		ptrdiff_t idx = Utils::FindIdxOfVal(exists, true);
		ECS::Log::Error("there is a record by id (" + std::to_string(ids[idx]) + ") in the component");
		return false;
	}

	// all the ids DON'T exist in the component
	return true;
}




} // namespace Utils