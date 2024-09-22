#include "StringHelper.h"
#include "log.h"

#include <stdexcept>
#include <vector>


// ************************************************************************************
//
//                          PUBLIC STATIC FUNCTIONS
//
// ************************************************************************************

std::string StringHelper::GetDirPath(const std::string & filepath)
{
	size_t offset1 = filepath.find_last_of('\\');
	size_t offset2 = filepath.find_last_of('/');

	bool cond1 = (offset1 == std::string::npos);
	bool cond2 = (offset2 == std::string::npos);

	size_t end = 0;

	if (cond1 && cond2)
	{
		// if no slash or backslash
		end = 0;
	}
	if (cond1)
	{
		end = offset2;
	}
	else if (cond2)
	{
		end = offset1;
	}
	else
	{
		// if both exists, need to use the greater offset
		end = max(offset1, offset2);
	}

	return filepath.substr(0, end);
} 

///////////////////////////////////////////////////////////

std::string StringHelper::GetFileName(const std::string & filePath)
{
	// get file name without extension from the input path

	if (filePath.empty()) return " ";

	size_t offset1 = filePath.find_last_of('\\');
	size_t offset2 = filePath.find_last_of('/');

	bool cond1 = (offset1 == std::string::npos);
	bool cond2 = (offset2 == std::string::npos);

	size_t start = 0;
	size_t end = 0;

	// if we have extension we use a pos before "." (period) symbol
	// or the end of str in another case
	end = filePath.find_last_of('.');
	end = (end == std::string::npos) ? filePath.size()-1 : end-1;
	
	if (cond1 && cond2)
	{
		// if no slash or backslash
		start = end;
	}
	else if (cond1)
	{
		start = offset2;
	}
	else if (cond2)
	{
		start = offset1;
	}
	else
	{
		// if both exists we need to use the greater offset as starting pos for substr
		start = max(offset1, offset2);
	}
	
	// start + 1 -- to skip slash
	return filePath.substr(start+1, end-start);
}

///////////////////////////////////////////////////////////

std::string StringHelper::GetFileExt(const std::string& filepath)
{
	// get only extension from the input filename or filepath

	size_t offset = filepath.find_last_of('.');

	// if we have no matches
	if (offset == std::string::npos)
	{
		return "";
	}

	// returns an extension of a file
	return std::string(filepath.substr(offset + 1));   // +1 because we have to skip a "." (period) symbol
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


std::string StringHelper::ToStringHelper(const std::wstring& wstr)
{
	// converts a wide character string into a usual ASCII character string 
	// (currently, wide characters are supposed to be in the ASCII range)  

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

