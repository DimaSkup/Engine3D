#pragma once

///////////////////////////////////////////////////////////////////////////////
// Filename: systemclass.h
///////////////////////////////////////////////////////////////////////////////

#include "includes.h"
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

	bool Initialize(void);
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Frame();
	void InitializeWindows(int&, int&);	// initialize the WinAPI
	void ShutdownWindows();

private:
	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;

	InputClass* m_input;
	GraphicsClass* m_graphics;
};

/////////////////////////////
// FUNCTION PROTOTYPES
/////////////////////////////
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

/////////////////////////////
// GLOBALS
/////////////////////////////
static SystemClass* ApplicationHandle = nullptr;