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
private:
	struct InitParamsForDebugStrings
	{
		const UINT maxDebugStringSize_ = 32;             // max length of debug string (debug strings with data about fps, position, rotation, etc.)
		const DirectX::XMFLOAT3 textColor{ 1, 1, 1 };  // white
	};

	struct UpdateDataStorage
	{
		std::vector<std::string> textStringsArr;
		std::vector<POINT> drawAtPositionsArr;
		const DirectX::XMFLOAT3 textColor{ 1, 1, 1 }; // white
	};
	
public:
	UserInterfaceClass();
	~UserInterfaceClass();

private:  // restrict a copying of this class instance
	UserInterfaceClass(const UserInterfaceClass & obj);    
	UserInterfaceClass & operator=(const UserInterfaceClass & obj);

public:
	void Initialize(ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext,
		const std::string & fontDataFilePath,      // a path to file with data about this type of font
		const std::string & fontTextureFilePath,   // a path to texture file for this font
		const UINT windowWidth,
		const UINT windowHeight,
		const UINT videoCardMemory,
		const std::string & videoCardName);        

	void Update(ID3D11DeviceContext* pDeviceContext, const SystemState & systemState);
	void Render(ID3D11DeviceContext* pDeviceContext, const DirectX::XMMATRIX & WVO);

	//bool UpdateRenderCounts(ID3D11DeviceContext* pDeviceContext, int, int, int);

private:
	//////////////////////////////////////////
	//  INITIALIZE STRINGS
	//////////////////////////////////////////
	void PrepareInitDataForDebugStrings(
		_Inout_ std::vector<std::string> & initStrArr,
		_Inout_ std::vector<POINT> & drawAtPositions,
		const UINT videoCardMemory,
		const std::string & videoCardName);

	void InitializeDebugStrings(ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext,
		const UINT windowWidth,
		const UINT windowHeight,
		const UINT maxStrSize,
		FontClass & font,
		FontShaderClass & fontShader,
		const std::vector<std::string> & initStrArr,
		const std::vector<POINT> & drawAtPositions,
		const DirectX::XMFLOAT3 & color);

	//////////////////////////////////////////
	//  UPDATE STRINGS
	//////////////////////////////////////////
	void UpdateDebugStrings(ID3D11DeviceContext* pDeviceContext, 
		const std::vector<std::string> & textStringsArr,
		const std::vector<POINT> & drawAtPositions,
		const DirectX::XMFLOAT3 & color);

	//////////////////////////////////////////
	//  RENDER STRINGS
	//////////////////////////////////////////
	void RenderDebugText(ID3D11DeviceContext* pDeviceContext, const DirectX::XMMATRIX & WVO);

private:
	FontShaderClass fontShader_;    // a common font shader class for rendering font onto the screen
	FontClass       font1_;         // a font class object (represents a font style)
	
	
	int        previousFps_ = -1;                   // the previous frame fps
	DirectX::XMVECTOR previousPosition_;            // the position of the camera during the previous frame
	DirectX::XMVECTOR previousRotation_;            // the rotation of the camera during the previous frame

	std::vector<TextClass> debugStrArr_;
	UpdateDataStorage updateDataForStrings_;
	//TextClass fpsString_;                           // a string with info about fps
	//std::vector<TextClass> videoStringsArr_;       // info about video stuff (adapter, memory, etc)
	//std::vector<TextClass> positionStringsArr_;    // info about the current position of the camera
	//std::vector<TextClass> renderCountStringsArr_; // info about rendered models counts
};