#pragma once
#pragma warning (disable : 4996) // because we use the wcsrtombs function

#include <string>

class StringConverter
{
public:
	// converters
	static std::wstring StringToWide(std::string str);
	static std::string ToString(std::wstring wstr);
	static std::string ToString(const wchar_t* wcstr);
private:
	static std::string ToStringHelper(wchar_t const* wcstr);
};