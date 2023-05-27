////////////////////////////////////////////////////////////////////
// Filename:     UserInterfaceclass.h
// Description:  a functional for initialization, 
//               updating and rendering of the UI
// 
// Created:      22.05.23
////////////////////////////////////////////////////////////////////
#pragma once


//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include "../Render/d3dclass.h"
#include "../UI/Text/textclass.h"
#include "../Engine/SystemState.h"

using namespace DirectX;


//////////////////////////////////
// Class name: UserInterfaceClass
//////////////////////////////////
class UserInterfaceClass
{
public:
	UserInterfaceClass();
	~UserInterfaceClass();

private:  // restrict a copying of this class instance
	UserInterfaceClass(const UserInterfaceClass & obj);    
	UserInterfaceClass & operator=(const UserInterfaceClass & obj);

public:
	bool Initialize(D3DClass* pD3D, const SETTINGS::settingsParams* systemParams);

	bool Frame(ID3D11DeviceContext* pDeviceContext, const SETTINGS::settingsParams* systemParams, const SystemState* systemState);
	bool Render(D3DClass* pD3D, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX orthoMatrix);

	//bool UpdateRenderCounts(ID3D11DeviceContext* pDeviceContext, int, int, int);

private:
	bool UpdateFpsString(ID3D11DeviceContext* pDeviceContext, int fpsCount);

private:
	FontClass* pFont1_ = nullptr;
	TextClass* pFpsString_ = nullptr;           // info about fps
	TextClass* pVideoStrings_ = nullptr;        // info about video stuff (adapter, memory, etc)
	TextClass* pPositionStrings_ = nullptr;     // info about the current position of the camera
	TextClass* pRenderCountStrings_ = nullptr;  // info about rendered models counts

	int previousFps = -1;                       // the previous frame fps
	int previousPosition[6] = { 0 };
	
};