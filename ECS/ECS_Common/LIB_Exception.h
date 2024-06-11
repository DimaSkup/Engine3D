///////////////////////////////////////////////////////////////////////////////////////////
// Filename:     LIB_Exception.h
// Descption:    a wrapper class for manual exceptions of the DoorsEngine
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <comdef.h>         // for using the _com_error class which defines an error object
#include "StringHelper.h"  


#define THROW_ERROR(msg)                                       throw LIB_Exception(0, msg, __FILE__, __FUNCTION__, __LINE__)
#define ASSERT_NOT_FAILED(hr, msg)      if (FAILED(hr))        throw LIB_Exception(hr, msg, __FILE__, __FUNCTION__, __LINE__)
#define ASSERT_TRUE(condition, msg)     if (condition != true) throw LIB_Exception(0, msg, __FILE__, __FUNCTION__, __LINE__)
#define ASSERT_NOT_NULLPTR(ptr, msg)    if (ptr == nullptr)    throw LIB_Exception(0, msg, __FILE__, __FUNCTION__, __LINE__)
#define ASSERT_NOT_ZERO(value, msg)     if (value == 0)        throw LIB_Exception(0, msg, __FILE__, __FUNCTION__, __LINE__)
#define ASSERT_NOT_EMPTY(is_empty, msg) if (is_empty == true)  throw LIB_Exception(0, msg, __FILE__, __FUNCTION__, __LINE__)

class LIB_Exception
{
public:
	LIB_Exception(HRESULT hr, const std::string& msg, const std::string& file, const std::string& function, int line);

	inline const wchar_t * getStr() const
	{
		return errorMsg.c_str();
	}

private:
	std::wstring errorMsg;
};