#pragma once

#include <string>
#include <vector>

namespace ImgReader
{

class StringHelper
{
public:
	// converters
	template<typename T>
	static std::vector<std::string> ConvertNumbersIntoStrings(const std::vector<T>& numbersArr);

	inline static std::wstring StringToWide(const std::string& str) { return { str.begin(), str.end() }; };
	inline static std::string ToString(const std::wstring& wstr) { return ToStringHelper(wstr); };

	// get some value from the filepath
	static std::string GetDirectoryFromPath(const std::string& filepath);
	static std::string GetFileExtension(const std::string& filename);
	static std::string GetFileNameFromPath(const std::string& filePath);

	static std::string Join(
		const std::vector<std::string>& strArr,
		const std::string& glue = ", ");


private:
	static std::string ToStringHelper(const std::wstring& wstr);
};

} // namespace ImgReader