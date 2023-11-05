#include "StringConverter.h"



/////////////////////////////////////////////////////////////////////////////////////////
//
//                          PUBLIC STATIC FUNCTIONS
//
/////////////////////////////////////////////////////////////////////////////////////////


// ----------------------------    CONVERTERS    ------------------------------------- //

// converts a usual character string into the wide string
std::wstring StringConverter::StringToWide(std::string str)
{
	std::wstring wideString(str.begin(), str.end());
	return wideString;
}

std::string StringConverter::ToString(std::wstring wstr)
{
	return StringConverter::ToStringHelper(wstr.c_str());
}

std::string StringConverter::ToString(const wchar_t* wcstr)
{
	return StringConverter::ToStringHelper(wcstr);
}



/////////////////////////////////////////////////////////////////////////////////////////
//
//                            PRIVATE FUNCTIONS
//
/////////////////////////////////////////////////////////////////////////////////////////

// converts a wide character string into a usual character string
std::string StringConverter::ToStringHelper(const wchar_t* wcstr)
{
	auto s = std::mbstate_t();

	// define a count of charaters
	const size_t charCount = std::wcsrtombs(nullptr, &wcstr, 0, &s);
	if (charCount == static_cast<std::size_t>(-1))
	{
		throw std::logic_error("Illegal byte sequence");
	}

	// +1 because we need to add a null terminator which isn't part of size
	char* buffer = new char[charCount + 1];
	buffer[charCount] = '\0'; // add a null-pointer

	std::wcsrtombs(buffer, &wcstr, charCount, &s);
	std::string str{ buffer };

	return str;
}

