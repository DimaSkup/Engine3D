///////////////////////////////////////////////////////////////////////////////////////////
// Filename:     LIB_Exception.cpp
// Descption:    implementation of functional for the LIB_Exception class 
///////////////////////////////////////////////////////////////////////////////////////////
#include "LIB_Exception.h"

LIB_Exception::LIB_Exception(HRESULT hr, const std::string& msg, const std::string& file, const std::string& function, int line)
{
	// generate an error string with data about some exception so later we can
	// use this string for the ECS::Log::Error() function

	errorMsg = L"\nErrorMsg: " + StringHelper::StringToWide(std::string(msg));

	if (hr != NULL)
	{
		_com_error error(hr);

		errorMsg += L"\n";
		errorMsg += error.ErrorMessage();
	}

	errorMsg += L"\nFile:     " + StringHelper::StringToWide(file);
	errorMsg += L"\nFunction: " + StringHelper::StringToWide(function) + L"()";
	errorMsg += L"\nLine:     " + StringHelper::StringToWide(std::to_string(line));
	errorMsg += L"\n\n";
}