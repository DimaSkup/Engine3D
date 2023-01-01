#pragma once

#include <comdef.h>
#include "StringConverter.h"  // for using the _com_error class which defines an error object

#define COM_ERROR_IF_FAILED(hr, msg) if (FAILED(hr)) throw COMException(hr, msg, __FILE__, __FUNCTION__, __LINE__)
#define COM_ERROR_IF_FALSE(condition, msg) if (!condition) throw COMException(0, msg, __FILE__, __FUNCTION__, __LINE__)

class COMException
{
public:
	COMException(HRESULT hr, const std::string& msg, const std::string& file, const std::string& function, int line)
	{
		

		errorMsg = L"\nErrorMsg: " + StringConverter::StringToWide(std::string(msg));

		if (hr != NULL)
		{
			_com_error error(hr);

			errorMsg += L"\n";
			errorMsg += error.ErrorMessage();
		}
		
		errorMsg += L"\nFile:     " + StringConverter::StringToWide(file);
		errorMsg += L"\nFunction: " + StringConverter::StringToWide(function);
		errorMsg += L"\nLine:     " + StringConverter::StringToWide(std::to_string(line));
	}

	const wchar_t * getStr() const
	{
		return errorMsg.c_str();
	}

private:
	std::wstring errorMsg;
};