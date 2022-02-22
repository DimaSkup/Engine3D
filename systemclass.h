#pragma once

///////////////////////////////////////////////////////////////////////////////
// Filename: systemclass.h
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////
// PRE-PROCESSING DIRECTIVES
/////////////////////////////
#define WIN32_LEAN_AND_MEAN

/////////////////////////////
// INCLUDES
/////////////////////////////
#include <windows.h>
#include <string>
#include <ctime>


/////////////////////////////
// MY CLASS INCLUDES
/////////////////////////////
#include "log.h"
#include "inputclass.h"
#include "graphicsclass.h"

//////////////////////////////////////////////////////////
// Class name: SystemClass
//////////////////////////////////////////////////////////
class SystemClass
{
public:
	SystemClass();
	SystemClass(const SystemClass&);
	~SystemClass();

	void Initialize();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Frame();
	void InitializeWindows(int&, int&);
	void ShutdownWindows();

private:
	LPCWSTR m_applicationName;
	HINSTANCE m_instance;
	HWND m_hwnd;

	InputClass* m_input;
	GraphicsClass* m_Graphics;
};

/////////////////////////////
// FUNCTION PROTOTYPES
/////////////////////////////
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

/////////////////////////////
// GLOBALS
/////////////////////////////
static SystemClass* ApplicationHandle = nullptr;