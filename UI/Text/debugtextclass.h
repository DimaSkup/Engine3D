////////////////////////////////////////////////////////////////////
// Filename:      debugtestclass.h
// Description:   this class is responsible for the output of debug
//                data (fps, cpu, etc.) onto the screen
//
// Revising:      10.09.22
////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include "textclass.h"
#include "../../LogSystem/Log.h"


//////////////////////////////////
// Class name: DebugTextClass
//////////////////////////////////
class DebugTextClass
{
public:
	DebugTextClass(void);
	DebugTextClass(const DebugTextClass& copy);
	~DebugTextClass(void);

	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hwnd,
		            int screenWidth, int screenHeight,
		            DirectX::XMMATRIX baseViewMatrix);
	bool Render(ID3D11DeviceContext* deviceContext,
		        DirectX::XMMATRIX worldMatrix,
		        DirectX::XMMATRIX orthoMatrix);
	void Shutdown(void);



	bool SetMousePosition(DirectX::XMFLOAT2 pos);   // set the mouse position data for rendering it on the screen
	bool SetDisplayParams(int width, int height);

	// set the fps and cpu data for rendering it on the screen
	bool SetFps(int fps);
	bool SetCpu(int cpu);
	bool SetCameraPosition(DirectX::XMFLOAT3 position);
	bool SetCameraOrientation(DirectX::XMFLOAT2 orientation);
	bool SetRenderCount(int renderCount);

private:
	void SetSentencePosByKey(std::string key, int posX, int posY);

private:
	TextClass* m_pText;
	std::map<std::string, POINT> sentencesPos;
};
