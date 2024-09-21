///////////////////////////////////////////////////////////////////////////////////////////
// Filename:     EngineException.h
// Descption:    a wrapper class for manual exceptions of the DoorsEngine
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "StringHelper.h"

#include <comdef.h>         // for using the _com_error class which defines an error object
#include <DxErr.h>
#include <source_location>
#include <string>


class EngineException
{
public:
	EngineException(
		HRESULT hr, 
		const std::string& msg, 
		const std::string& file, 
		const std::string& function,
		int line);

	EngineException(
		const std::string& msg,
		const std::source_location& location = std::source_location::current(),
		const HRESULT hr = S_OK);

	inline const wchar_t * getStr() const {	return errorMsg_.c_str(); }

private:
	void MakeExceptionMsg(
		HRESULT hr,
		const std::string& msg,
		const std::string& file,
		const std::string& function,
		const int line);

private:
	std::wstring errorMsg_ { L"" };
};