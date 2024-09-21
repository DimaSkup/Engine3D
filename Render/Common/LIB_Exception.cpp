///////////////////////////////////////////////////////////////////////////////////////////
// Filename:     LIB_Exception.cpp
// Descption:    implementation of functional for the LIB_Exception class 
///////////////////////////////////////////////////////////////////////////////////////////
#include "LIB_Exception.h"

namespace Render
{


LIB_Exception::LIB_Exception(
	HRESULT hr,
	const std::string& msg, 
	const std::string& file, 
	const std::string& function, 
	const int line)
{
	// generate an error string with data about some LIB_Exception so later we can
	// use this string for the ECS::Log::Error() function

	MakeExceptionMsg(hr, msg, file, function, line);
}

///////////////////////////////////////////////////////////

LIB_Exception::LIB_Exception(
	const std::string& msg,
	const std::source_location& location,
	const HRESULT hr)
{
	// generate an error string with data about some LIB_Exception so later we can
	// use this string for the logger printing functions

	MakeExceptionMsg(hr, msg, location.file_name(), location.function_name(), location.line());
}

// *********************************************************************************

void LIB_Exception::MakeExceptionMsg(
	HRESULT hr,
	const std::string& msg,
	const std::string& file,
	const std::string& function,
	const int line)
{
	errorMsg_ = L"\nErrorMsg: " + StringHelper::StringToWide(std::string(msg));

	if (hr != NULL)
	{
		_com_error error(hr);

		errorMsg_ += L"\n";
		errorMsg_ += error.ErrorMessage();
	}

	errorMsg_ += L"\nFile:     " + StringHelper::StringToWide(file);
	errorMsg_ += L"\nFunction: " + StringHelper::StringToWide(function) + L"()";
	errorMsg_ += L"\nLine:     " + StringHelper::StringToWide(std::to_string(line));
	errorMsg_ += L"\n\n";
}

}