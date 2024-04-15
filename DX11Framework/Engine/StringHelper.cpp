#include "StringHelper.h"

#include <filesystem>
#include <stdexcept>
#include <vector>

/////////////////////////////////////////////////////////////////////////////////////////
//
//                          PUBLIC STATIC FUNCTIONS
//
/////////////////////////////////////////////////////////////////////////////////////////


// ------------------------------    CONVERTERS    ------------------------------------- //

// converts a usual character string into the wide string
std::wstring StringHelper::StringToWide(std::string str)
{
	std::wstring wideString(str.begin(), str.end());
	return wideString;
}

///////////////////////////////////////////////////////////

std::string StringHelper::ToString(std::wstring wstr)
{
	return StringHelper::ToStringHelper(wstr.c_str());
}

///////////////////////////////////////////////////////////

std::string StringHelper::ToString(const wchar_t* wcstr)
{
	return StringHelper::ToStringHelper(wcstr);
}


// ----------------------------------    GETTERS    ------------------------------------- //

std::string StringHelper::GetDirectoryFromPath(const std::string & filepath)
{
	size_t offset1 = filepath.find_last_of('\\');
	size_t offset2 = filepath.find_last_of('/');

	// if no slash or backslash
	if (offset1 == std::string::npos && offset2 == std::string::npos)
	{
		return "";
	}

	if (offset1 == std::string::npos)
	{
		return filepath.substr(0, offset2);
	}
	if (offset2 == std::string::npos)
	{
		return filepath.substr(0, offset1);
	}

	// if both exists, need to use the greater offset
	return filepath.substr(0, std::max(offset1, offset2));
} 

///////////////////////////////////////////////////////////

std::string StringHelper::GetFileNameFromPath(const std::string & filePath)
{
	size_t offset1 = filePath.find_last_of('\\');
	size_t offset2 = filePath.find_last_of('/');

	// if no slash or backslash
	if (offset1 == std::string::npos && offset2 == std::string::npos)
	{
		return "";
	}

	if (offset1 == std::string::npos)
	{
		return filePath.substr(offset2, filePath.size() - 1);
	}
	if (offset2 == std::string::npos)
	{
		return filePath.substr(offset1, filePath.size() - 1);
	}

	// if both exists, need to use the greater offset
	return filePath.substr(std::max(offset1, offset2), filePath.size() - 1);
}

///////////////////////////////////////////////////////////

std::string StringHelper::GetFileExtension(const std::string & filename)
{
	// find the last "." (period) symbol
	size_t offset = filename.find_last_of('.');

	// if we have no matches
	if (offset == std::string::npos)
	{
		return {};
	}

	// returns an extension of a file by the filePath path
	return std::string(filename.substr(offset + 1));   // +1 because we have to skip a "." (period) symbol
}


/////////////////////////////////////////////////////////////////////////////////////////
//
//                            PRIVATE FUNCTIONS
//
/////////////////////////////////////////////////////////////////////////////////////////

// converts a wide character string into a usual character string
std::string StringHelper::ToStringHelper(const wchar_t* wcstr)
{
	auto s = std::mbstate_t();

	// define a count of charaters
	const size_t charCount = std::wcsrtombs(nullptr, &wcstr, 0, &s);
	if (charCount == static_cast<std::size_t>(-1))
	{
		throw std::logic_error("Illegal byte sequence");
	}

	// +1 because we need to add a null terminator which isn't part of size
	std::vector<char> buffer(charCount + 1, '\0');

	// convert from wchar_t* to char*
	std::wcsrtombs(buffer.data(), &wcstr, charCount, &s);

	return std::string(buffer.data());
}

