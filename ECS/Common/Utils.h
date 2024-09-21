// *********************************************************************************
// Filename:     Utils.h
// Description:  contains some common utils for the Entity-Component-System
// *********************************************************************************
#pragma once
#include "Types.h"
#include "../Common/StringHelper.h"

#include <vector>
#include <algorithm>
#include <fstream>

namespace Utils
{
	// ****************************************************************************
	// file read/write API

	template<typename T>
	inline static void FileWrite(std::ofstream& fout, const std::vector<T>& arr)
	{
		// write all the arr content into the file stream
		fout.write((const char*)arr.data(), arr.size() * sizeof(T));
	}

	// ----------------------------------------------------

	template<typename T>
	inline static void FileWrite(std::ofstream& fout, T* pData, const u32 count = 1)
	{
		// write into the file stream bytes in quantity of count;
		// NOTE: if count == 1 it means that we want to write only one basic (int, u32, ect.) variable
		fout.write((const char*)pData, count * sizeof(T));
	}

	// ----------------------------------------------------

	template<typename T>
	inline static void FileRead(std::ifstream& fin, std::vector<T>& arr)
	{
		// read in data from the file stream into the arr
		fin.read((char*)arr.data(), arr.size() * sizeof(T));
	}

	// ----------------------------------------------------

	template<typename T>
	inline static void FileRead(std::ifstream& fin, T* pData, const u32 count = 1)
	{
		// read in from the file stream an array bytes in quantity of count
		// NOTE: if count == 1 it means that we want to read only one basic (int, u32, ect.) variable
		fin.read((char*)pData, count * sizeof(T));
	}


	// ****************************************************************************
	// different help purposes API

	template<typename T>
	static std::string JoinArrIntoStr(
		const std::vector<T>& arrOfNum,
		const std::string& glue = ", ")
	{
		std::vector<std::string> arrNumAsStr = ECS::StringHelper::ConvertNumbersIntoStrings<T>(arrOfNum);
		return ECS::StringHelper::Join(std::move(arrNumAsStr), glue);
	}

	// ----------------------------------------------------

	template<class T>
	static void AppendArray(std::vector<T>& head, const std::vector<T>& tail)
	{
		head.insert(head.end(), tail.begin(), tail.end());
	}


	// ****************************************************************************
	// sorted insertion API

	inline static ptrdiff_t GetPosForID(
		const std::vector<EntityID>& enttsIDs,
		const EntityID& enttID)
	{
		// get position (index) into array for sorted insertion of ID
		// 
		// input:  1. a SORTED array of IDs
		//         2. an ID for which we want to find an insertion pos
		// return:    an insertion idx for ID

		return std::distance(enttsIDs.begin(), std::upper_bound(enttsIDs.begin(), enttsIDs.end(), enttID));
	}

	// ----------------------------------------------------

	template<class T>
	static void InsertAtPos(
		std::vector<T>& arr,
		const ptrdiff_t pos,
		const T& val)
	{
		// std::insert() wrapper
		// insert val into the arr at pos (index) 
		arr.insert(arr.begin() + pos, val);
	}

	template<class T>
	static void InsertAtPos(
		std::vector<T>& arr,
		const ptrdiff_t pos,
		const T&& val)
	{
		// std::insert() wrapper
		// insert val into the arr at pos (index) 
		arr.insert(arr.begin() + pos, val);
	}

	

	// ****************************************************************************
	// check existing API

	template<typename T>
	inline static bool BinarySearch(
		const std::vector<T>& arr,
		const T& value)
	{
		// std::binary_search() wrapper
		// input:  1. a SORTED arr
		//         2. a searched value
		// return: true - if there is such a value; or false - in another case

		return std::binary_search(arr.begin(), arr.end(), value);
	}

	// ------------------------------------------

	template<class T>
	inline bool CheckValuesExistInArr(
		const std::vector<T>& inOrigArr,
		const std::vector<T>& inValuesArr)
	{
		// check if each value (from inValueArr) exists in the origin arr
		auto itBeg = inOrigArr.begin();
		auto itEnd = inOrigArr.end();
		bool isExist = true;

		for (u32 idx = 0; const T & val : inValuesArr)
			isExist &= std::binary_search(itBeg, itEnd, val);

		return isExist;
	}

	// ----------------------------------------------------

	template<typename T>
	inline static bool ArrHasVal(
		const std::vector<T>& arr,
		const T& val)
	{
		// input:  1. an array of RANDOMLY placed values
		//         2. a searched value
		// return: true - if there is such a value; or false - in another case

		return std::find(arr.begin(), arr.end(), val) != arr.end();
	}

	// ****************************************************************************
	// search for index API

	template<class T>
	inline static ptrdiff_t GetIdxInSortedArr(
		const std::vector<T>& arr,
		const T& val)
	{
		// get current position (index) into the SORTED array for input value
		// NOTE:   we subtract 1 from the final result because of upper_bound which 
		//         returns the idx right after the searched value
		// return: pos (data idx) of searched value

		return std::distance(arr.begin(), std::upper_bound(arr.begin(), arr.end(), val)) - 1;
	}

	// ------------------------------------------

	template<class T>
	inline void GetIdxsInSortedArr(
		const std::vector<T>& inOrigArr,
		const std::vector<T>& inValuesArr,
		std::vector<ptrdiff_t>& outIdxsArr)
	{
		// get arr of idxs to values

		auto itBeg = inOrigArr.begin();
		auto itEnd = inOrigArr.end();

		outIdxsArr.resize(inValuesArr.size());

		for (u32 idx = 0; const T & val : inValuesArr)
			outIdxsArr[idx++] = std::distance(itBeg, std::upper_bound(itBeg, itEnd, val)) - 1;
	}

	// ----------------------------------------------------

	template<typename T>
	inline static ptrdiff_t FindIdxOfVal(
		const std::vector<T>& arr,
		const T& value)
	{
		// input:  1. an array of RANDOMLY placed values
		//         2. a searched value
		// 
		// return: idx into the array of searched value

		return std::distance(arr.begin(), std::find(arr.begin(), arr.end(), value));
	}

} // namespace Utils

