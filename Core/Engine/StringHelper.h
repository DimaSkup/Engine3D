#pragma once
#pragma warning (disable : 4996) // because we use the wcsrtombs function

#include <string>
#include <vector>

class StringHelper
{
public:
	// converters
	static std::wstring StringToWide(std::string str);
	static std::string ToString(std::wstring wstr);
	static std::string ToString(const wchar_t* wcstr);

	// get some value from the filepath
	static std::string GetDirectoryFromPath(const std::string & filepath);
	static std::string GetFileExtension(const std::string & filename);
	static std::string GetFileNameFromPath(const std::string & filePath);

	static std::string Join(
		const std::vector<std::string>& strArr, 
		const std::string& glue = ", ");

	template<typename T>
	static std::vector<std::string> ConvertNumbersIntoStrings(const std::vector<T>& numbersArr);

private:
	static std::string ToStringHelper(wchar_t const* wcstr);
};