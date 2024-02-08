////////////////////////////////////////////////////////////////////////////////////////////
// Filename:     UserInterfaceclass.cpp
// Description:  a functional for initialization, 
//               updating and rendering of the UI
// 
// Created:      25.05.23
////////////////////////////////////////////////////////////////////////////////////////////
#include "UserInterfaceClass.h"

using namespace DirectX;


UserInterfaceClass::UserInterfaceClass()
{
}

UserInterfaceClass::~UserInterfaceClass()
{
	debugStrArr_.clear();
}




////////////////////////////////////////////////////////////////////////////////////////////
//
//                                 PUBLIC FUNCTIONS
//
////////////////////////////////////////////////////////////////////////////////////////////

// initialize the graphics user interface (GUI)
void UserInterfaceClass::Initialize(ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,
	const std::string & fontDataFilePath,      // a path to file with data about this type of font
	const std::string & fontTextureFilePath,   // a path to texture file for this font
	const UINT windowWidth,
	const UINT windowHeight,
	const UINT videoCardMemory,
	const std::string & videoCardName) 
{
	Log::Debug(LOG_MACRO);

	try
	{
		const InitParamsForDebugStrings initParams;
		UpdateDataStorage & initData = updateDataForStrings_;
		FontShaderClass* pFontShader = &fontShader_;
		FontClass* pFont = &font1_;
		
		// initialize a font shader class which will be used 
		// for rendering all the text data onto the screen;
		fontShader_.Initialize(pDevice, pDeviceContext);

		/////////////////////////////////////

		// initialize the first font object
		const bool result = font1_.Initialize(pDevice, pDeviceContext, fontDataFilePath, fontTextureFilePath);
		COM_ERROR_IF_FALSE(result, "can't initialize the first font object");

		
		this->PrepareInitDataForDebugStrings(
			initData.textStringsArr,
			initData.drawAtPositionsArr,
			videoCardMemory,
			videoCardName);

		this->InitializeDebugStrings(pDevice, 
			pDeviceContext,
			windowWidth, 
			windowHeight,
			initParams.maxDebugStringSize_,
			font1_,
			fontShader_,
			initData.textStringsArr,        // an array with initial strings
			initData.drawAtPositionsArr,    // an array with initial positions of the strings
			initParams.textColor);

		Log::Debug(LOG_MACRO, "USER INTERFACE is initialized");
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		Log::Error(LOG_MACRO, "can't initialize the UserInterfaceClass");
		return;
	}


	return;
}

///////////////////////////////////////////////////////////

void UserInterfaceClass::Update(ID3D11DeviceContext* pDeviceContext, 
	const SystemState & systemState)
{
	// each frame we call this function for updating the UI

	bool result = false;
	const UpdateDataStorage & updateDataStorage = updateDataForStrings_;

	try
	{

		UpdateDebugStrings(pDeviceContext,
			updateDataStorage.textStringsArr,
			updateDataStorage.drawAtPositionsArr,
			updateDataStorage.textColor);

#if 0
		// update the fps string
		UpdateFpsString(pDeviceContext, systemState.fps, textColor);
		

		// update the position strings
		UpdatePositionStrings(pDeviceContext,
			systemState.editorCameraPosition,
			systemState.editorCameraRotation,
			textColor);
		
		// update the render count strings
		UpdateRenderCounts(pDeviceContext, 
		{
			systemState.renderedModelsCount,
			systemState.cellsDrawn,
			systemState.cellsCulled,
			systemState.renderedVerticesCount,
			systemState.renderedVerticesCount / 3
		},
			textColor);

#endif
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		Log::Error(LOG_MACRO, "can't update some text string");
		return;
	}

	return;
}

///////////////////////////////////////////////////////////

void UserInterfaceClass::Render(ID3D11DeviceContext* pDeviceContext,
	const DirectX::XMMATRIX & WVO)
{
	//
	// this functions renders all the UI elements onto the screen
	//

	// render the debug text data onto the screen
	this->RenderDebugText(pDeviceContext, WVO);

	return;
}






////////////////////////////////////////////////////////////////////////////////////////////
//
//                                PRIVATE FUNCTIONS
//
////////////////////////////////////////////////////////////////////////////////////////////

void UserInterfaceClass::PrepareInitDataForDebugStrings(
	_Inout_ std::vector<std::string> & initStrArr,
	_Inout_ std::vector<POINT> & drawAtPositions,
	const UINT videoCardMemory,
	const std::string & videoCardName)
{
	COM_ERROR_IF_ZERO(videoCardName.size(), "the input str with video card name is empty");
	COM_ERROR_IF_ZERO(videoCardMemory, "the input value of the video card memory == 0");

	// setup the video card info string and video card memory string
	const std::string videoStringData{ "Video Card: " + videoCardName };
	const std::string memoryStringData{ "Video Memory: " + std::to_string(videoCardMemory) + "MB" };

	const int videoString_MaxSize = 256;
	const int memoryString_MaxSize = 32;

	POINT drawAt{ 10, 10 };                          // start position where we render the first string
	const int strideY = 20;                           // each text string is rendered by 20 pixels lower that the previous one

	initStrArr = {
		videoStringData,
		memoryStringData,
		"Fps: 0",
		"X: 0", "Y: 0", "Z: 0",                         // position strings
		"rX (pich): 0", "rY (yaw): 0", "rZ (roll): 0",  // rotation strings
		"Models drawn: 0",
		"Cells drawn: 0",
		"Cells culled: 0",
		"Vertices drawn: 0",
		"Triangles drawn: 0",
	};

	drawAtPositions.resize(initStrArr.size());

	for (UINT i = 0; i < drawAtPositions.size(); ++i)
	{
		drawAtPositions[i] = drawAt;
		drawAt.y += strideY;         // the following string will be rendered by stringY pixels below
	}
}

void UserInterfaceClass::InitializeDebugStrings(ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,
	const UINT windowWidth,
	const UINT windowHeight,
	const UINT maxStrSize,
	FontClass & font,
	FontShaderClass & fontShader,
	const std::vector<std::string> & initStrArr,
	const std::vector<POINT> & drawAtPositions,
	const DirectX::XMFLOAT3 & textColor)
{
	assert(windowWidth > 0);
	assert(windowHeight > 0);
	assert(maxStrSize > 0);
	assert(initStrArr.size() == drawAtPositions.size());

	// allocate memory for the necessary number of text strings
	debugStrArr_.resize(initStrArr.size(), TextClass());

	// initialize each debug text string
	for (UINT i = 0; i < debugStrArr_.size(); ++i)
	{
		// initialize the fps text string
		const bool result = debugStrArr_[i].Initialize(pDevice, pDeviceContext,
			windowWidth, windowHeight,
			maxStrSize,          // max size for this string
			&font, &fontShader,
			initStrArr[i],       // initialize a string with this text
			drawAtPositions[i],  // draw the string at this positions
			textColor);
		COM_ERROR_IF_FALSE(result, "can't init the string");
	}

	return;

} // end InitializeDebugStrings


///////////////////////////////////////////////////////////

void UserInterfaceClass::UpdateDebugStrings(ID3D11DeviceContext* pDeviceContext,
	const std::vector<std::string> & textStringsArr,
	const std::vector<POINT> & drawAtPositions,
	const DirectX::XMFLOAT3 & color)
{
	// update the debug string to render it onto the screen

	for (size_t i = 0; i < debugStrArr_.size(); ++i)
	{
		// update the sentence with the new string information
		debugStrArr_[i].Update(pDeviceContext,
		textStringsArr[i],          // new string
		drawAtPositions[i], // position
		color);             // text color

	}
	
	return;
}

///////////////////////////////////////////////////////////
#if 0
void UserInterfaceClass::UpdatePositionStrings(ID3D11DeviceContext* pDeviceContext,
	const DirectX::XMVECTOR & position,
	const DirectX::XMVECTOR & rotation,
	const DirectX::XMFLOAT3 & color)
{
	// update the GUI strings with position/rotation data to render it onto the screen

	std::string finalString{ "" };
	bool result = false;
	const UINT strideY = 20;
	POINT drawAt{ 10, 100 };

	const std::vector<std::string> prefixForStr
	{
		"X: ", "Y: ", "Z: ",
		"rX (pich): ", "rY (yaw): ", "rZ (roll): "
	};


	/////////////////////////////////////////////
	//  UPDATE POSITION STRINGS
	/////////////////////////////////////////////
	
	// update the position strings if the value has changed since the last frame
	const XMVECTOR positionEqualFlags = XMVectorEqual(position, previousPosition_);

	for (UINT i = 0; i < 3; ++i)
	{
		if (!positionEqualFlags.m128_f32[i])
		{
			// prepare a string with data
			finalString = prefixForStr[i];
			finalString += std::to_string(position.m128_f32[i]);  

			// update the string with new one
			result = positionStringsArr_[i].Update(pDeviceContext, finalString, drawAt, color);
			COM_ERROR_IF_FALSE(result, "can't update the text string with position data");
		}

		// the next string will be rendered by strideY pixels below
		drawAt.y += strideY;
	}

	/////////////////////////////////////////////
	//  UPDATE ROTATION STRINGS
	/////////////////////////////////////////////

	const XMVECTOR rotationEqualFlags = XMVectorEqual(rotation, previousRotation_);

	for (UINT i = 0; i < 3; ++i)
	{
		if (!rotationEqualFlags.m128_f32[i])
		{
			// prepare a string with data
			finalString = prefixForStr[i + 3];
			finalString += std::to_string(rotation.m128_f32[i]);

			// update the string with new one
			result = positionStringsArr_[i].Update(pDeviceContext, finalString, drawAt, color);
			COM_ERROR_IF_FALSE(result, "can't update the text string with rotation data");
		}

		// the next string will be rendered by strideY pixels below
		drawAt.y += strideY;
	}

	// update the values of the previous position/rotation
	previousPosition_ = position;
	previousRotation_ = rotation;

	return;
}

///////////////////////////////////////////////////////////

void UserInterfaceClass::UpdateRenderCounts(ID3D11DeviceContext* pDeviceContext,
	const std::vector<UINT> renderCountsDataArr,
	const DirectX::XMFLOAT3 & color)
{
	// update the render count strings to show it on the screen

	bool result = false;
	const int strideY = 20;   // each text string is rendered by 20 pixels lower that the previous one
	POINT drawAt{ 10, 260 };
	std::string finalString{ "" };

	const std::vector<std::string> prefixStrArr
	{
		"Models drawn: ",
		"Cells drawn: ",
		"Cells culled: ",
		"Vertices drawn: ",
		"Triangles drawn: ",
	};

	////////////////////////////////////////////////

	for (UINT i = 0; i < renderCountStringsArr_.size(); ++i)
	{
		// update the string with new render count data
		finalString = prefixStrArr[i];
		finalString += std::to_string(renderCountsDataArr[i]);

		result = renderCountStringsArr_[i].Update(pDeviceContext,
			finalString,
			drawAt, 
			color);
		COM_ERROR_IF_FALSE(result, "can't update the string with some render count data");

		drawAt.y += strideY;
	}

	return;
}

///////////////////////////////////////////////////////////
#endif 

void UserInterfaceClass::RenderDebugText(ID3D11DeviceContext* pDeviceContext, 
	const DirectX::XMMATRIX & WVO)
{
	// THIS FUNCTION renders all the UI debug text strings onto the screen

	// render the fps string
	for (size_t i = 0; i < debugStrArr_.size(); ++i)
	{
		debugStrArr_[i].Render(pDeviceContext, WVO);
	}

	return;
}