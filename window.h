#pragma once

#include "includes.h"

struct DescWindow
{
	DescWindow()
	{
		posx = 20;
		posy = 20;
		width = 800;
		height = 600;
		caption = L"";
		resizing = false;
		fullScreen = false;
	}

	int posx;
	int posy;
	int width;
	int height;
	std::wstring caption;
	bool resizing;
	bool fullScreen;
};

class Window
{
public:
	Window(void);

	static Window* Get(void) { return m_winInstance; }
	
	bool Initialize(const DescWindow& desc);
	void RunEvent(void);
	void Shutdown(void);

	HWND GetHWND(void) { return m_hwnd; }
	int GetWidth(void) const { return m_desc.width; }
	int GetHeight(void) const { return m_desc.height; }



	bool IsExit(void) const { return m_isExit; }

	LRESULT WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	HWND m_hwnd;
	DescWindow m_desc;
	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	static Window* m_winInstance;

private:
	bool m_maximized;
	bool m_minimized;
	bool m_isResizing;
	bool m_isExit;
};

static LRESULT CALLBACK StaticWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return Window::Get()->WndProc(hwnd, message, wParam, lParam);
}