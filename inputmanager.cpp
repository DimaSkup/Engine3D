////////////////////////////////////////////////////////////////////
// Filename: inputmanager.cpp
// Revising: 01.06.22
////////////////////////////////////////////////////////////////////
#include "inputmanager.h"

void InputManager::Initialize(void)
{
	m_curX = m_curY = m_curMouseWheel = 0;
	Log::Get()->Debug(THIS_FUNC_EMPTY);
	return;
}

void InputManager::Shutdown(void)
{
	if (!m_inputHandlers.empty())
	{
		m_inputHandlers.clear();
	}

	Log::Get()->Debug(THIS_FUNC_EMPTY);
}

void InputManager::AddInputHandler(InputHandler* handler)
{
	m_inputHandlers.push_back(handler);
	Log::Get()->Debug(THIS_FUNC, "a new input handler is added");
	return;
}

void InputManager::SetWindowZone(const RECT& clientRect)
{
	m_wndRect.left = clientRect.left;
	m_wndRect.top = clientRect.top;
	m_wndRect.right = clientRect.right;
	m_wndRect.bottom = clientRect.bottom;

	Log::Get()->Debug(THIS_FUNC, "the window zone is updated");
	return;
}

void InputManager::ManageInput(MSG message, WPARAM wParam, LPARAM lParam)
{
	if (m_inputHandlers.empty())
	{
		Log::Get()->Error(THIS_FUNC, "the input handlers list is empty!");
		return;
	}



	return;
}

