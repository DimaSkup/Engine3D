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
		const std::string & fontDataFilePath,      // a path to file with data about this type of font
		const std::string & fontTextureFilePath,   // a path to texture file for this font
		const int windowWidth, 
		const int windowHeight, 
		const DirectX::XMMATRIX & baseViewMatrix,
		FontShaderClass* pFontShader);             // a font shader for rendering text onto the screen

	bool Frame(ID3D11DeviceContext* pDeviceContext, const SystemState* systemState);
	bool Render(D3DClass* pD3D, const XMMATRIX & worldMatrix, const XMMATRIX & orthoMatrix);

	//bool UpdateRenderCounts(ID3D11DeviceContext* pDeviceContext, int, int, int);

private:
	bool InitializeVideoStrings(const int screenWidth, const int screenHeight, const int videoCardMemory, const std::string & videoCardName);
	bool InitializePositionStrings(const int screenWidth, const int screenHeight);
	bool InitializeRenderCountStrings(const int screenWidth, const int screenHeight);

	bool UpdateFpsString(ID3D11DeviceContext* pDeviceContext, const int fpsCount);
	bool UpdatePositionStrings(ID3D11DeviceContext* pDeviceContext, const DirectX::XMFLOAT3 & position, const DirectX::XMFLOAT3 & rotation);
	bool UpdateRenderCounts(ID3D11DeviceContext* pDeviceContext, const int renderCount, const int nodesDrawn, const int nodesCulled);

private:
	
	ID3D11Device* pDevice_ = nullptr;
	ID3D11DeviceContext* pDeviceContext_ = nullptr;
	FontShaderClass* pFontShader_ = nullptr;    // a common font shader class for rendering font onto the screen
	FontClass* pFont1_ = nullptr;
	TextClass* pFpsString_ = nullptr;                // info about fps

	int        previousFps_ = -1;                    // the previous frame fps
	const int  maxDebugStringSize = 32;              // max length of debug string (debug strings with data about fps, position, rotation, etc.)
	const UINT numVideoStrings_ = 2;
	const UINT numPositionStrings_ = 6;
	const UINT numRenderCountStrings_ = 3;

	const DirectX::XMFLOAT4 defaultDebugTextColor{ 1, 1, 1, 1 };  // white

	DirectX::XMMATRIX baseViewMatrix_;          // is used for proper rendering of the UI
	DirectX::XMFLOAT3 previousPosition_;            // the position of the camera during the previous frame
	DirectX::XMFLOAT3 previousRotation_;            // the rotation of the camera during the previous frame

	std::vector<TextClass*> videoStringsArr_;       // info about video stuff (adapter, memory, etc)
	std::vector<TextClass*> positionStringsArr_;    // info about the current position of the camera
	std::vector<TextClass*> renderCountStringsArr_; // info about rendered models counts
};