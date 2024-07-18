#pragma once
#pragma warning (disable : 4996) // because we use the wcsrtombs function

#include <string>
#include <vector>

namespace ImgReader
{


class StringHelper
{
public:
	// converters
	inline static std::wstring StringToWide(const std::string& str) { return std::wstring(str.begin(), str.end()); }
	inline static std::string ToString(const std::wstring& wstr) { return ToStringHelper(wstr.c_str()); }
	inline static std::string ToString(const wchar_t* wcstr) { return ToStringHelper(wcstr); };

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


};  // namespace ECS