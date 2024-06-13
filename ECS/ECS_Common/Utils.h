// *********************************************************************************
// Filename:     Utils.h
// Description:  contains some common utils for the Entity-Component-System
// *********************************************************************************
#pragma once
#include "ECS_Types.h"
#include "../ECS_Common/StringHelper.h"
#include <vector>
#include <algorithm>

namespace Utils
{
	template<typename T>
	static std::string JoinArrIntoStr(
		const std::vector<T>& arrOfNum,
		const std::string& glue = ", ")
	{
		std::vector<std::string> arrNumAsStr = ECS::StringHelper::ConvertNumbersIntoStrings<T>(arrOfNum);
		return ECS::StringHelper::Join(std::move(arrNumAsStr), glue);
	}

	template<class T>
	static void AppendArray(std::vector<T>& head, const std::vector<T>& tail)
	{
		head.insert(head.end(), tail.begin(), tail.end());
	}

	static ptrdiff_t GetPosForID(
		const std::vector<EntityID>& enttsIDs,
		const EntityID& enttID)
	{
		// get insertion position (index) into array for input entity ID
		// return: pos of ID
		return std::distance(enttsIDs.begin(), std::upper_bound(enttsIDs.begin(), enttsIDs.end(), enttID));
	}

	static ptrdiff_t GetIdxOfID(
		const std::vector<EntityID>& enttsIDs,
		const EntityID& enttID)
	{
		// get current position (index) into array for of ID
		// return: pos (data idx) of ID
		return std::distance(enttsIDs.begin(), std::upper_bound(enttsIDs.begin(), enttsIDs.end(), enttID)) - 1;
	}

	template<class T>
	static void InsertAtPos(
		std::vector<T>& arr,
		const ptrdiff_t pos,
		const T& val)
	{
		// insert into the arr at pos (index) input val
		arr.insert(arr.begin() + pos, val);
	}

	template<typename T>
	static bool ArrHasVal(
		std::vector<T>& arr,
		const T& val)
	{
		// check if the input array contains value if so we return true or false in another case
		return (std::find(arr.begin(), arr.end(), val) != arr.end());
	}

} // namespace Utils

