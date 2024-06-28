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
#include "../UI/Text/TextStore.h"
#include "../Engine/SystemState.h"
#include "ECS_Entity/EntityManager.h"

//////////////////////////////////
// Class name: UserInterfaceClass
//////////////////////////////////
class UserInterfaceClass
{
private:
	struct InitParamsForDebugStrings
	{
		const UINT maxDebugStringSize_ = 32;           // max length of debug string (debug strings with data about fps, position, rotation, etc.)
		const POINT startDrawAt{ 10, 10 };             // start position where we render the first string
		const int strideY = 20;                        // each text string is rendered by 20 pixels lower that the previous one
		const DirectX::XMFLOAT3 textColor{ 1, 1, 1 };  // white
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

	// Public modification API
	void SetStringByKey(ID3D11Device* pDevice, 
		const std::string & key, 
		const std::string & str, 
		const POINT & drawAt);

	void Update(
		ID3D11DeviceContext* pDeviceContext,
		const SystemState & systemState);


	// Public rendering API
	void Render(
		ID3D11DeviceContext* pDeviceContext, 
		EntityManager& entityMgr,
		const DirectX::XMMATRIX & WVO);

	

	//bool UpdateRenderCounts(ID3D11DeviceContext* pDeviceContext, int, int, int);

private:
	void RenderMainMenuBar(
		ID3D11DeviceContext* pDeviceContext, 
		EntityManager& entityMgr);

	//////////////////////////////////////////
	//  INITIALIZE STRINGS
	//////////////////////////////////////////
	void PrepareTextForDebugStringsToInit(
		const UINT videoCardMemory,
		const std::string & videoCardName,
		_Inout_ std::vector<std::string> & initStrArr);

	void PrepareTextIDsForStringsToInit(
		const size_t numOfStrings,
		_Inout_ std::vector<std::string> & textIDs);

	void PrepareDrawAtPositionsToInit(
		const POINT & startDrawAt,
		const UINT gapBetweenStrings, 
		const UINT fontHeight, 
		const int windowWidth,
		const int windowHeight,
		const size_t positionsCount,
		_Inout_ std::vector<POINT> & drawAtPositionsArr);

	
	void InitializeDebugStrings(ID3D11Device* pDevice,
		const size_t numOfStrings,
		const UINT maxStrSize,
		FontClass & font,
		const std::vector<std::string> & textDataToInit,
		const std::vector<std::string> & textIDsToInit,
		const std::vector<POINT> & drawAtPosToInit);

	//////////////////////////////////////////
	//  UPDATE STRINGS
	//////////////////////////////////////////

	void UpdateDebugStrings(ID3D11DeviceContext* pDeviceContext,
		const SystemState & systemState);

	//////////////////////////////////////////
	//  RENDER STRINGS
	//////////////////////////////////////////
	void RenderDebugText(ID3D11DeviceContext* pDeviceContext,
		const DirectX::XMMATRIX & WVO,
		const DirectX::XMFLOAT3 & textColor);

private:
	// default window dimensions
	UINT windowWidth_ = 800;
	UINT windowHeight_ = 600;

	//UpdateDataStorage updateDataForStrings_;
	FontShaderClass fontShader_;    // a common font shader class for rendering font onto the screen
	FontClass       font1_;         // a font class object (represents a font style)
	TextStore       debugStrings_;  // constains strings with debug data: fps, position/rotation, etc.
	
};