///////////////////////////////////////////////////////////////////////////////////////////
// Filename:     COMException.cpp
// Descption:    implementation of functional for the COMException class 
///////////////////////////////////////////////////////////////////////////////////////////
#include "COMException.h"

COMException::COMException(HRESULT hr, const std::string& msg, const std::string& file, const std::string& function, int line)
{
	// generate an error string with data about some exception so later we can
	// use this string for the Log::Error() function

	errorMsg = L"\nErrorMsg: " + StringHelper::StringToWide(std::string(msg));

	if (hr != NULL)
	{
		_com_error error(hr);

		// show us a message box indicating the file and line number where the error
		// occured, as well as a texture description of the error, and the name of 
		// the function generated the error
		//DXTrace(file.c_str(), (DWORD)line, hr, error.ErrorMessage(), true);

		errorMsg += L"\n";
		errorMsg += error.ErrorMessage();
	}

	errorMsg += L"\nFile:     " + StringHelper::StringToWide(file);
	errorMsg += L"\nFunction: " + StringHelper::StringToWide(function) + L"()";
	errorMsg += L"\nLine:     " + StringHelper::StringToWide(std::to_string(line));
	errorMsg += L"\n\n";
}