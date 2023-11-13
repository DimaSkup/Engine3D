////////////////////////////////////////////////////////////////////////////////////////////
// Filename:     UserInterfaceclass.h
// Description:  a functional for initialization, 
//               updating and rendering of the UI
// 
// Created:      22.05.23
////////////////////////////////////////////////////////////////////////////////////////////
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
	UserInterfaceClass(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	~UserInterfaceClass();

private:  // restrict a copying of this class instance
	UserInterfaceClass(const UserInterfaceClass & obj);    
	UserInterfaceClass & operator=(const UserInterfaceClass & obj);

public:
	bool Initialize(D3DClass* pD3D, 
		int windowWidth, int windowHeight, 
		const DirectX::XMMATRIX & baseViewMatrix,
		FontShaderClass* pFontShader);             // a font shader for rendering text onto the screen

	bool Frame(ID3D11DeviceContext* pDeviceContext, const SystemState* systemState, const DirectX::XMFLOAT3 & position, const DirectX::XMFLOAT3 & rotation);
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
	FontShaderClass* pFontShader_ = nullptr;    // a common font shader class for rendering font onto the screen
	FontClass* pFont1_ = nullptr;
	TextClass* pFpsString_ = nullptr;               // info about fps


	const UINT numVideoStrings_ = 2;
	const UINT numPositionStrings_ = 6;
	const UINT numRenderCountStrings_ = 3;

	std::vector<TextClass*> videoStringsArr_;       // info about video stuff (adapter, memory, etc)
	std::vector<TextClass*> positionStringsArr_;    // info about the current position of the camera
	std::vector<TextClass*> renderCountStringsArr_; // info about rendered models counts
	
	int previousFps_ = -1;                          // the previous frame fps
	int previousPosition_[6] = { -1 };              // the previous frame position
	
};