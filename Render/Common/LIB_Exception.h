///////////////////////////////////////////////////////////////////////////////////////////
// Filename:     LIB_Exception.h
// Descption:    a wrapper class for manual LIB_Exceptions of the DoorsEngine
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <comdef.h>         // for using the _com_error class which defines an error object
#include <source_location>
#include "StringHelper.h"  

namespace Render
{

class LIB_Exception
{
public:
	LIB_Exception(
		HRESULT hr,
		const std::string& msg,
		const std::string& file, 
		const std::string& function, 
		const int line);

	LIB_Exception(
		const std::string& msg,
		const std::source_location& location = std::source_location::current(),
		const HRESULT hr = S_OK);

	inline const wchar_t* getStr() const { return errorMsg_.c_str(); }

private:
	void MakeExceptionMsg(
		HRESULT hr,
		const std::string& msg,
		const std::string& file,
		const std::string& function,
		const int line);


private:
	std::wstring errorMsg_{ L"" };
};

} // namespace Render