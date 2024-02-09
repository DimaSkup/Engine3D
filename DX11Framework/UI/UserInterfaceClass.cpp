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

		// allocate memory for the vertices array which will be used as vertex data buffer for
		// updating of the text vertex buffers
		const UINT verticesInSymbol = 4;
		initData.verticesArrToUpdate.resize(initParams.maxDebugStringSize_ * verticesInSymbol);


		/////////////////////////////////////

		// initialize a font shader class which will be used 
		// for rendering all the text data onto the screen;
		fontShader_.Initialize(pDevice, pDeviceContext);

		// initialize the first font object
		const bool result = font1_.Initialize(pDevice, pDeviceContext, fontDataFilePath, fontTextureFilePath);
		COM_ERROR_IF_FALSE(result, "can't initialize the first font object");

		/////////////////////////////////////


		// prepare indices array of text strings to update
		PrepareIndicesOfStringsToUpdate(initData.indicesOfStringsToUpdate);

		// prepare prefixes for strings
		InitializePrefixesForStrings(initData.debugStrPrefixes);
		
		// prepare positions where we will draw text strings
		PrepareDrawAtPositions(initParams.startDrawAt,
			initParams.strideY,
			windowWidth,
			windowHeight,
			initData.debugStrPrefixes.size(),     // how many positions we have
			initData.drawAtPositionsArr);         // fill in this array with positions data

		// prepare text strings array for initialization of text class objects
		PrepareInitDataForDebugStrings(
			initData.finalTextData,
			videoCardMemory,
			videoCardName);

		InitializeDebugStrings(pDevice,
			pDeviceContext,
			initParams.maxDebugStringSize_,
			font1_,
			fontShader_,
			initData.finalTextData,         // an array with initial strings
			initData.drawAtPositionsArr);   // an array with initial positions of the strings
			

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
	UpdateDataStorage & updateDataStorage = updateDataForStrings_;
	std::vector<std::string> dataToUpdate;

	try
	{
		PrepareRawDataForStringsToUpdate(systemState, dataToUpdate);

		PrepareStringsToUpdate(
			updateDataStorage.debugStrPrefixes,
			dataToUpdate,
			updateDataStorage.indicesOfStringsToUpdate,  // array of indices to text string for updating
			updateDataStorage.finalTextData);            // array of text data for updating

		UpdateDebugStrings(pDeviceContext,
			updateDataStorage.finalTextData,
			updateDataStorage.drawAtPositionsArr,        // an array of positions for text strings
			updateDataStorage.indicesOfStringsToUpdate,
			updateDataStorage.verticesArrToUpdate,       // temporal buffer for vertices data 
			debugStrArr_);
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
	this->RenderDebugText(pDeviceContext, WVO, updateDataForStrings_.textColor);

	return;
}






////////////////////////////////////////////////////////////////////////////////////////////
//
//                                PRIVATE FUNCTIONS
//
////////////////////////////////////////////////////////////////////////////////////////////

void UserInterfaceClass::PrepareDrawAtPositions(
	const POINT & startDrawAt,
	const int strideY,
	const int windowWidth,
	const int windowHeight,
	const size_t positionsCount,
	_Inout_ std::vector<POINT> & drawAtPositionsArr)
{
	assert(windowWidth > 0);
	assert(windowHeight > 0);
	assert(positionsCount > 0);

	// prepare an array with positions data for the text strings placing onto the screen
	drawAtPositionsArr.resize(positionsCount);

	POINT drawAt;
	drawAt.x = (windowWidth / -2) + startDrawAt.x;
	drawAt.y = (windowHeight / 2) - startDrawAt.y;

	for (UINT i = 0; i < positionsCount; ++i)
	{
		drawAtPositionsArr[i] = drawAt;
		drawAt.y -= strideY;    // the following string will be rendered by stringY pixels below
	}

	return;
}

///////////////////////////////////////////////////////////

void UserInterfaceClass::PrepareInitDataForDebugStrings(
	_Inout_ std::vector<std::string> & initStrArr,
	const UINT videoCardMemory,
	const std::string & videoCardName)
{
	COM_ERROR_IF_ZERO(videoCardName.size(), "the input str with video card name is empty");
	COM_ERROR_IF_ZERO(videoCardMemory, "the input value of the video card memory == 0");

	// setup the video card info string and video card memory string
	const std::string videoStringData{ "Video Card: " + videoCardName };
	const std::string memoryStringData{ "Video Memory: " + std::to_string(videoCardMemory) + "MB" };
	
	// prepare initial data for debug text strings
	initStrArr =
	{
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
}

///////////////////////////////////////////////////////////

void UserInterfaceClass::InitializePrefixesForStrings(
	_Inout_ std::vector<std::string> & debugStrPrefixes)
{
	// prepare prefixes for debug strings
	debugStrPrefixes =
	{
		"video_string_data",
		"video_memory_data",
		"Fps: ",
		"X: ", "Y: ", "Z: ",                         // position strings
		"rX (pich): ", "rY (yaw): ", "rZ (roll): ",  // rotation strings
		"Models drawn: ",
		"Cells drawn: ",
		"Cells culled: ",
		"Vertices drawn: ",
		"Triangles drawn: ",
	};

	return;
}

///////////////////////////////////////////////////////////

void UserInterfaceClass::InitializeDebugStrings(ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,
	const UINT maxStrSize,
	FontClass & font,
	FontShaderClass & fontShader,
	const std::vector<std::string> & initStrArr,
	const std::vector<POINT> & drawAtPositions)
{
	assert(maxStrSize > 0);
	assert(initStrArr.size() == drawAtPositions.size());

	// allocate memory for the necessary number of text strings
	debugStrArr_.resize(initStrArr.size(), TextClass());

	// initialize each debug text string
	for (UINT i = 0; i < debugStrArr_.size(); ++i)
	{
		// initialize the fps text string
		const bool result = debugStrArr_[i].Initialize(pDevice, pDeviceContext,
			maxStrSize,          // max size for this string
			&font, &fontShader,
			initStrArr[i],       // initialize a string with this text
			drawAtPositions[i]);  // draw the string at this positions
		COM_ERROR_IF_FALSE(result, "can't init the string");
	}

	return;

} // end InitializeDebugStrings


///////////////////////////////////////////////////////////

void UserInterfaceClass::PrepareIndicesOfStringsToUpdate(
	_Inout_ std::vector<UINT> & indicesOfStringsToUpdate)
{
	// prepare an array which we will be used to define an index of sentence to update
	indicesOfStringsToUpdate =
	{
		// 1, 2,            // we don't strings with video card name/memory
		2,                  // fps
		3, 4, 5,            // x/y/z posisition
		6, 7, 8,            // x/y/z rotation,
		9, 10, 11, 12, 13   // render count data
	};

	return;
}

///////////////////////////////////////////////////////////

void UserInterfaceClass::PrepareRawDataForStringsToUpdate(
	const SystemState & systemState,
	_Inout_ std::vector<std::string> & dataForUpdating)
{
	dataForUpdating =
	{
		"0", "0",   // we don't want to update string with video card name/memory
		std::to_string(systemState.fps),

		// position data
		std::to_string(XMVectorGetX(systemState.editorCameraPosition)),
		std::to_string(XMVectorGetY(systemState.editorCameraPosition)),
		std::to_string(XMVectorGetZ(systemState.editorCameraPosition)),

		// rotation data
		std::to_string(XMVectorGetX(systemState.editorCameraRotation)),
		std::to_string(XMVectorGetY(systemState.editorCameraRotation)),
		std::to_string(XMVectorGetZ(systemState.editorCameraRotation)),

		// render counts data
		std::to_string(systemState.renderedModelsCount),
		std::to_string(systemState.cellsDrawn),
		std::to_string(systemState.cellsCulled),
		std::to_string(systemState.renderedVerticesCount),
		std::to_string(systemState.renderedVerticesCount / 3)
	};
}

///////////////////////////////////////////////////////////

void UserInterfaceClass::PrepareStringsToUpdate(
	const std::vector<std::string> & strPrefixes,
	const std::vector<std::string> & dataForUpdating,
	const std::vector<UINT> & textStrIndicesToUpdate,
	_Inout_ std::vector<std::string> & finalTextStringsToUpdate)
{
	
	for (UINT i = 0; i < textStrIndicesToUpdate.size(); ++i)
	{
		// by this index we will update the string
		const UINT index = textStrIndicesToUpdate[i];

		// final string = prefix + data
		finalTextStringsToUpdate[index] = strPrefixes[index] + dataForUpdating[index];
	}

	return;
}

///////////////////////////////////////////////////////////

void UserInterfaceClass::UpdateDebugStrings(ID3D11DeviceContext* pDeviceContext,
	const std::vector<std::string> & finalStringsToUpdate,
	const std::vector<POINT> & drawAtPositions,
	const std::vector<UINT> & indicesOfStringsToUpdate,
	_Inout_ std::vector<VERTEX_FONT> & tempVerticesBuffer,
	_Inout_ std::vector<TextClass> & debugTextObjArr)
{
	//
	// update the debug string to render it onto the screen
	//

	// reset the vertices temp buffer for proper updating of the vertex buffer
	std::fill(tempVerticesBuffer.begin(), tempVerticesBuffer.end(), VERTEX_FONT());

	for (size_t i = 0; i < indicesOfStringsToUpdate.size(); ++i)
	{
		// by this index we will update the string
		const UINT index = indicesOfStringsToUpdate[i];

		// update the sentence with the new string information
		debugTextObjArr[index].Update(pDeviceContext,
			tempVerticesBuffer,                // a temporal buffer for vertices data for updating
			finalStringsToUpdate[index],       // new string data
			drawAtPositions[index]);           // position on the screen
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


#endif 

void UserInterfaceClass::RenderDebugText(ID3D11DeviceContext* pDeviceContext, 
	const DirectX::XMMATRIX & WVO,
	const DirectX::XMFLOAT3 & textColor)
{
	// THIS FUNCTION renders all the UI debug text strings onto the screen

	// render the fps string
	for (size_t i = 0; i < debugStrArr_.size(); ++i)
	{
		debugStrArr_[i].Render(pDeviceContext, WVO, textColor);
	}

	return;
}