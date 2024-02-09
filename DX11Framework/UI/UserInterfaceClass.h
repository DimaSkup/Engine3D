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
		const UINT maxDebugStringSize_ = 32;           // max length of debug string (debug strings with data about fps, position, rotation, etc.)
		const POINT startDrawAt{ 10, 10 };             // start position where we render the first string
		const int strideY = 20;                        // each text string is rendered by 20 pixels lower that the previous one
		const DirectX::XMFLOAT3 textColor{ 1, 1, 1 };  // white
	};

	struct UpdateDataStorage
	{
		const DirectX::XMFLOAT3 textColor{ 1, 1, 1 }; // white

		std::vector<POINT> drawAtPositionsArr;
		std::vector<UINT> indicesOfStringsToUpdate;
		std::vector<VERTEX_FONT> verticesArrToUpdate;
		std::vector<std::string> debugStrPrefixes;      // prefixes for debug strings
		std::vector<std::string> finalTextData;   // array of strings: [prefix + updated_value] (for instance: "fps: " + "100" = "fps: 100")
		
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
	void PrepareDrawAtPositions(
		const POINT & startDrawAt,
		const int strideY, 
		const int windowWidth,
		const int windowHeight,
		const size_t positionsCount,
		_Inout_ std::vector<POINT> & drawAtPositionsArr);

	void PrepareInitDataForDebugStrings(
		_Inout_ std::vector<std::string> & initStrArr,		
		const UINT videoCardMemory,
		const std::string & videoCardName);

	void InitializePrefixesForStrings(
		_Inout_ std::vector<std::string> & debugStrPrefixes);

	void InitializeDebugStrings(ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext,
		const UINT maxStrSize,
		FontClass & font,
		FontShaderClass & fontShader,
		const std::vector<std::string> & initStrArr,
		const std::vector<POINT> & drawAtPositions);

	//////////////////////////////////////////
	//  UPDATE STRINGS
	//////////////////////////////////////////
	void PrepareIndicesOfStringsToUpdate(
		_Inout_ std::vector<UINT> & indicesOfStringsToUpdate);

	void PrepareRawDataForStringsToUpdate(
		const SystemState & systemState,
		_Inout_ std::vector<std::string> & dataForUpdating);

	void PrepareStringsToUpdate(
		const std::vector<std::string> & strPrefixes,
		const std::vector<std::string> & dataForUpdating,
		const std::vector<UINT> & textStrIndicesToUpdate,
		_Inout_ std::vector<std::string> & finalTextStringsToUpdate);

	void UpdateDebugStrings(ID3D11DeviceContext* pDeviceContext,
		const std::vector<std::string> & finalStringsToUpdate,
		const std::vector<POINT> & drawAtPositions,
		const std::vector<UINT> & indicesOfStringsToUpdate,
		_Inout_ std::vector<VERTEX_FONT> & tempVerticesBuffer,
		_Inout_ std::vector<TextClass> & debugTextObjArr);

	//////////////////////////////////////////
	//  RENDER STRINGS
	//////////////////////////////////////////
	void RenderDebugText(ID3D11DeviceContext* pDeviceContext,
		const DirectX::XMMATRIX & WVO,
		const DirectX::XMFLOAT3 & textColor);

private:
	UpdateDataStorage updateDataForStrings_;
	FontShaderClass fontShader_;    // a common font shader class for rendering font onto the screen
	FontClass       font1_;         // a font class object (represents a font style)
	
	std::vector<TextClass> debugStrArr_;              // constains strings with debug data: fps, position/rotation, etc.
	
};