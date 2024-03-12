#pragma once

#include <windows.h>
#include "log.h"
#include "EventListener.h"

class EventHandler
{
public:
	EventHandler() {}

	void AddEventListener(EventListener* eventListener)
	{
		assert(eventListener != nullptr);
		pEventListener = eventListener;
	}

	EventListener* pEventListener = nullptr;


public:

	void HandleEvent(HWND hwnd,
		UINT uMsg,
		WPARAM wParam,
		LPARAM lParam)
	{
		static bool isResizing = false;

		switch (uMsg)
		{
			// this message is sent when an application becomes activated or deactivated
			case WM_ACTIVATE:  
			{
				if (LOWORD(wParam) == WA_INACTIVE)
				{
					// set that the application is paused
					pEventListener->EventActivate(EventListener::APP_STATE::DEACTIVATED);
				}
				else
				{
					// set that the application is activated
					pEventListener->EventActivate(EventListener::APP_STATE::ACTIVATED);
				}
				return;
			}
			// this message is sent when the used grabs the resize bars
			case WM_ENTERSIZEMOVE:
			{
				pEventListener->EventActivate(EventListener::APP_STATE::DEACTIVATED);
				isResizing = true;
				return;
			}
			// this message is sent when the used releases the resize bars;
			// here we reset everything based on the new window dimensions;
			case WM_EXITSIZEMOVE:
			{
				pEventListener->EventActivate(EventListener::APP_STATE::ACTIVATED);
				pEventListener->EventWindowResize(hwnd, uMsg, wParam, lParam);
				isResizing = false;
				return;
			}
			case WM_MOVE:
			{
				pEventListener->EventWindowMove(hwnd, uMsg, wParam, lParam);
				return;
			}
			case WM_SIZE:
			{
				pEventListener->EventWindowResize(hwnd, uMsg, wParam, lParam);
				return;
			}
			case WM_SIZING:
			{
				pEventListener->EventWindowSizing(hwnd, uMsg, wParam, lParam);
				return;
			}

			// --- keyboard messages --- //
			case WM_KEYDOWN:
			case WM_KEYUP:
			case WM_CHAR:
			{
				pEventListener->EventKeyboard(hwnd, uMsg, wParam, lParam);
				return;
			}
			// --- mouse messages --- //
			case WM_MOUSEMOVE:
			case WM_LBUTTONDOWN: case WM_LBUTTONUP:
			case WM_MBUTTONDOWN: case WM_MBUTTONUP:
			case WM_RBUTTONDOWN: case WM_RBUTTONUP:
			case WM_MOUSEWHEEL:
			case WM_INPUT: 
			{
				pEventListener->EventMouse(hwnd, uMsg, wParam, lParam);
				return ;
			}
		}
	}
};
