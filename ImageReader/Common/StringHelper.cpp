#include "StringHelper.h"
#include "log.h"

#include <filesystem>
#include <stdexcept>
#include <vector>


namespace ImgReader
{

// ************************************************************************************
//
//                          PUBLIC STATIC FUNCTIONS
//
// ************************************************************************************

std::string StringHelper::GetDirectoryFromPath(const std::string& filepath)
{
	size_t offset1 = filepath.find_last_of('\\');
	size_t offset2 = filepath.find_last_of('/');

	bool cond1 = (offset1 == std::string::npos);
	bool cond2 = (offset2 == std::string::npos);

	// if no slash or backslash
	if (cond1 && cond2)
	{
		return " ";
	}
	if (cond1)
	{
		return filepath.substr(0, offset2);
	}
	if (cond2)
	{
		return filepath.substr(0, offset1);
	}

	// if both exists, need to use the greater offset
	return filepath.substr(0, max(offset1, offset2));
}

///////////////////////////////////////////////////////////

std::string StringHelper::GetFileNameFromPath(const std::string& filePath)
{
	size_t offset1 = filePath.find_last_of('\\');
	size_t offset2 = filePath.find_last_of('/');

	bool cond1 = (offset1 == std::string::npos);
	bool cond2 = (offset2 == std::string::npos);

	// if no slash or backslash
	if (cond1 && cond2)
	{
		return " ";
	}
	if (cond1)
	{
		return filePath.substr(offset2, filePath.size() - 1);
	}
	if (cond2)
	{
		return filePath.substr(offset1, filePath.size() - 1);
	}

	// if both exists, need to use the greater offset
	return filePath.substr(max(offset1, offset2), filePath.size() - 1);
}

///////////////////////////////////////////////////////////

std::string StringHelper::GetFileExtension(const std::string& filename)
{
	// find the last "." (period) symbol
	size_t offset = filename.find_last_of('.');

	// if we have no matches
	if (offset == std::string::npos)
	{
		return " ";
	}

	// returns an extension of a file by the filePath path
	return std::string(filename.substr(offset + 1));   // +1 because we have to skip a "." (period) symbol
}


///////////////////////////////////////////////////////////

std::string StringHelper::Join(
	const std::vector<std::string>& strArr,
	const std::string& glue)
{
	// concatenate all the input strings into a single string and return it
	std::string finalStr = "";

	for (const std::string& str : strArr)
		finalStr += { str + glue };

	return finalStr;
}

///////////////////////////////////////////////////////////

template<typename T>
std::vector<std::string> StringHelper::ConvertNumbersIntoStrings(
	const std::vector<T>& numbersArr)      // type T is supposed to be numerical
{
	// in:       array of numbers of some numerical type
	// return:   array of numbers convertes into strings

	std::vector<std::string> outStrArr;

	for (const T num : numbersArr)
		outStrArr.push_back(std::to_string(num));

	return outStrArr;
}


// ************************************************************************************
//
//                            PRIVATE FUNCTIONS
//
// ************************************************************************************

// converts a wide character string into a usual character string
std::string StringHelper::ToStringHelper(const std::wstring& wstr)
{
	if (wstr.empty())
	{
		Log::Error("input string is empty");
		Log::Error("can't convert input string from wchar_t* into std::string");
		return " ";
	}
	else
	{
		std::string outStr(wstr.size(), '\0');

		for (ptrdiff_t i = 0; i < std::ssize(wstr); ++i)
			outStr[i] = (char)wstr[i];

		return outStr;
	}
}



} // namespace ImgReader