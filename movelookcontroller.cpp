#include "movelookcontroller.h"

bool MoveLookController::Initialize(void)
{
	// ------------------------------ INPUT ----------------------------------------- //
	m_pInput = new InputClass();
	if (!m_pInput)
	{
		Log::Get()->Error(THIS_FUNC, "can't create the input class object");
		return false;
	}

	// set this input class object as a listener of input devices events
	Window::Get()->GetInputManager()->AddInputListener(m_pInput);
}

// here we handle mouse moving event
void MoveLookController::OnMouseMoved(InputClass* input)
{
	DirectX::XMFLOAT2 position {input->GetMousePos()}
}