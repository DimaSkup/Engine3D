#pragma once

#include <memory>

#include "RenderWindow.h"
#include "../Engine/EventHandler.h"

class WindowContainer
{
public:
	WindowContainer();
	~WindowContainer();

	void SetEventHandler(EventHandler* pEventHandler);

	LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static WindowContainer* Get() { return pWindowContainer_; };   // returns a pointer to the current WindowContainer instance


public: 
	static WindowContainer* pWindowContainer_;
	RenderWindow  renderWindow_;
	EventHandler* pEventHandler_ = nullptr;
};