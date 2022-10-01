#include "StringConverter.h"

// converts a usual character string into the wide string
std::wstring StringConverter::StringToWide(std::string str)
{
	std::wstring wideString(str.begin(), str.end());
	return wideString;
}
