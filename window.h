#pragma once

#include "includes.h"
#include "inputmanager.h"

class InputManager;

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

	//void SetInputManager(InputManager* inputManager);

	HWND GetHWND(void) { return m_hwnd; }
	int GetWidth(void) const { return m_desc.width; }
	int GetHeight(void) const { return m_desc.height; }
	int GetLeft(void) const { return m_desc.posx; }
	int GetTop(void) const { return m_desc.posy; }

	// return the caption of the window
	const std::wstring& GetCaption() const { return m_desc.caption; }

	// informs us, if there was a message about exit
	bool IsExit(void) const { return m_isexit; }
	// informs us about windows activity
	bool IsActive(void) const { return m_active; }
	// informs us about changin of the window
	// Attention: after calling, notifies the window about handling of the event
	bool IsResize(void)
	{
		bool ret = m_isresize;
		m_isresize = false;
		return ret;
	}

	// events handling
	LRESULT WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	private:
		void m_UpdateWindowState();
		
		HWND m_hwnd;
		DescWindow m_desc;
		//InputManager* m_pInputManater;
		LPCWSTR m_applicationName;
		HINSTANCE m_hinstance;
		static Window* m_winInstance;

	private:
		bool m_maximized;
		bool m_minimized;
		bool m_active;
		bool m_isresize;
		bool m_isexit;
};

static LRESULT CALLBACK StaticWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return Window::Get()->WndProc(hwnd, message, wParam, lParam);
}