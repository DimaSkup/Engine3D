///////////////////////////////////////////////////////////////////////////////////////////
// Filename:     COMException.h
// Descption:    a wrapper class for manual exceptions of the DoorsEngine
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <comdef.h>
#include "StringHelper.h"  // for using the _com_error class which defines an error object
#include <DxErr.h>


#define COM_ERROR_IF_FAILED(hr, msg) if (FAILED(hr))       throw COMException(hr, msg, __FILE__, __FUNCTION__, __LINE__)
#define COM_ERROR_IF_FALSE(condition, msg) if (!condition) throw COMException(0, msg, __FILE__, __FUNCTION__, __LINE__)
#define COM_ERROR_IF_NULLPTR(ptr, msg) if (ptr == nullptr) throw COMException(0, msg, __FILE__, __FUNCTION__, __LINE__)
#define COM_ERROR_IF_ZERO(value, msg) if (value == 0)      throw COMException(0, msg, __FILE__, __FUNCTION__, __LINE__)

class COMException
{
public:
	COMException(HRESULT hr, const std::string& msg, const std::string& file, const std::string& function, int line);

	inline const wchar_t * getStr() const
	{
		return errorMsg.c_str();
	}

private:
	std::wstring errorMsg;
};