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
	bool Initialize(D3DClass* pD3D, const SETTINGS::settingsParams* systemParams, const DirectX::XMMATRIX & baseViewMatrix);

	bool Frame(ID3D11DeviceContext* pDeviceContext, const SETTINGS::settingsParams* systemParams, const SystemState* systemState, const DirectX::XMFLOAT3 & position, const DirectX::XMFLOAT3 & rotation, int renderCount, int nodesDrawn, int nodesCulled);
	bool Render(D3DClass* pD3D, const XMMATRIX & worldMatrix, const XMMATRIX & orthoMatrix);

	//bool UpdateRenderCounts(ID3D11DeviceContext* pDeviceContext, int, int, int);

private:
	bool InitializeVideoStrings(D3DClass* pD3D, int screenWidth, int screenHeight);
	bool InitializePositionStrings(D3DClass* pD3D, int screenWidth, int screenHeight);
	bool InitializeRenderCountStrings(D3DClass* pD3D, int screenWidth, int screenHeight);

	bool UpdateFpsString(ID3D11DeviceContext* pDeviceContext, int fpsCount);
	bool UpdatePositionStrings(ID3D11DeviceContext* pDeviceContext, const DirectX::XMFLOAT3 & position, const DirectX::XMFLOAT3 & rotation);
	bool UpdateRenderCounts(ID3D11DeviceContext* pDeviceContext, int renderCount, int nodesDrawn, int nodesCulled);

private:
	DirectX::XMMATRIX baseViewMatrix_;          // is used for proper rendering of the UI
	FontClass* pFont1_ = nullptr;
	TextClass* pFpsString_ = nullptr;           // info about fps
	TextClass* pVideoStrings_ = nullptr;        // info about video stuff (adapter, memory, etc)
	TextClass* pPositionStrings_ = nullptr;     // info about the current position of the camera
	TextClass* pRenderCountStrings_ = nullptr;  // info about rendered models counts

	int previousFps_ = -1;                       // the previous frame fps
	int previousPosition_[6] = { -1 };           // the previous frame position
	
};