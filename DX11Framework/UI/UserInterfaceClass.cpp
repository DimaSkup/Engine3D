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
		const UINT gapBetweenStringsInPixels = 2;
		const InitParamsForDebugStrings initParams;
		FontClass & font = font1_;
		
		// data arrays for initialization
		std::vector<std::string> textContents;
		std::vector<std::string> textIDs;           // text IDs for associative navigation
		std::vector<POINT>       drawAtPositions;
		

		/////////////////////////////////////

		// initialize a font shader class which will be used 
		// for rendering all the text data onto the screen;
		fontShader_.Initialize(pDevice, pDeviceContext);

		// initialize the first font object
		const bool result = font.Initialize(pDevice, pDeviceContext, fontDataFilePath, fontTextureFilePath);
		COM_ERROR_IF_FALSE(result, "can't initialize the first font object");

		/////////////////////////////////////


		// prepare text strings array for initialization of text class objects
		PrepareTextForDebugStringsToInit(
			videoCardMemory,
			videoCardName,
			textContents);

		const size_t numOfStrings = textContents.size();

		// prepare text IDs which will be used for associative navigation
		PrepareTextIDsForStringsToInit(
			numOfStrings,
			textIDs);

		// prepare positions where we will draw text strings
		PrepareDrawAtPositionsToInit(initParams.startDrawAt,
			gapBetweenStringsInPixels,  // the distance in pixels between the bottom of the previous string and the top of the following one
			font.GetFontHeight(),       // height in pixels of this font type
			windowWidth,
			windowHeight,
			numOfStrings,               // how many positions we have
			drawAtPositions);           // fill in this array with positions data

		
		InitializeDebugStrings(pDevice,
			numOfStrings,
			initParams.maxDebugStringSize_,
			font,
			textContents,
			textIDs,
			drawAtPositions);
			
			

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
	
	try
	{
		this->UpdateDebugStrings(pDeviceContext, systemState);
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
	this->RenderDebugText(pDeviceContext,
		WVO,                               // world * base_view * ortho
		{ 1, 1, 1 });                      // text color: white

	return;
}






////////////////////////////////////////////////////////////////////////////////////////////
//
//                                PRIVATE FUNCTIONS
//
////////////////////////////////////////////////////////////////////////////////////////////



void UserInterfaceClass::PrepareTextForDebugStringsToInit(
	const UINT videoCardMemory,
	const std::string & videoCardName,
	_Inout_ std::vector<std::string> & initStrArr)
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

void UserInterfaceClass::PrepareTextIDsForStringsToInit(
	const size_t numOfStrings,
	_Inout_ std::vector<std::string> & textIDs)
{
	textIDs =
	{
		"video_card_name",
		"video_card_memory",
		"fps",
		"x_pos", "y_pos", "z_pos",  // position
		"x_rot", "y_rot", "z_rot",  // rotation
		"models_drawn",
		"cells_drawn",              // the number of terrain cells which were rendered
		"cells_culled",
		"vertices_drawn",
		"triangles_drawn",
	};

	// check if we prepared proper number of text IDs
	assert((numOfStrings == textIDs.size()) && "you have to prepare ther proper number of text IDs");
}

///////////////////////////////////////////////////////////

void UserInterfaceClass::PrepareDrawAtPositionsToInit(
	const POINT & startDrawAt,
	const UINT gapBetweenStrings,
	const UINT fontHeight,
	const int windowWidth,
	const int windowHeight,
	const size_t positionsCount,
	_Inout_ std::vector<POINT> & drawAtPositionsArr)
{
	assert(gapBetweenStrings > 0);
	assert(fontHeight > 0);
	assert(windowWidth > 0);
	assert(windowHeight > 0);
	assert(positionsCount > 0);

	const UINT strideY = fontHeight + gapBetweenStrings;
	POINT drawAt;

	// prepare an array with positions data for the text strings placing onto the screen
	drawAtPositionsArr.resize(positionsCount);

	// compute the starting position on the screen
	drawAt.x = (windowWidth / -2) + startDrawAt.x;
	drawAt.y = (windowHeight / 2) - startDrawAt.y;

	for (UINT i = 0; i < positionsCount; ++i)
	{
		drawAtPositionsArr[i] = drawAt;
		drawAt.y -= strideY;    // the following string will be rendered by strideY pixels below
	}

	return;
}

///////////////////////////////////////////////////////////

void UserInterfaceClass::InitializeDebugStrings(ID3D11Device* pDevice,
	const size_t numOfStrings,
	const UINT maxStrSize,
	FontClass & font,
	const std::vector<std::string> & textDataToInit,
	const std::vector<std::string> & textIDsToInit,
	const std::vector<POINT> & drawAtPosToInit)
{
	assert(numOfStrings > 0);
	assert(maxStrSize > 0);
	assert(textDataToInit.size() == numOfStrings);
	assert(textIDsToInit.size() == numOfStrings);
	assert(drawAtPosToInit.size() == numOfStrings);

	for (size_t str_idx = 0; str_idx < numOfStrings; ++str_idx)
	{
		// initialize the fps text string
		debugStrings_.CreateSentence(pDevice,
			font,
			maxStrSize,                  // max size for this string
			textDataToInit[str_idx],     // text content
			textIDsToInit[str_idx],      // text ID for this string
			drawAtPosToInit[str_idx]);   // upper left position   
	}

	debugStrings_.CreateSentence(pDevice,
		font, 
		maxStrSize, 
		"KEKLOL", 
		"kek_lol",
		{ 200, 200 });

	

	return;

} // end InitializeDebugStrings


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
	const SystemState & systemState)
{
	//
	// update the debug string to render it onto the screen
	//


	// prepare text IDs (by these ids we fill find strings and update it with new data)
	const std::vector<std::string> textIDsToUpdate =
	{
		"fps",
		"x_pos", "y_pos", "z_pos",  // position
		"x_rot", "y_rot", "z_rot",  // rotation
	};

	// prepare text prefixes to update
	const std::vector<std::string> prefixes =
	{
		"Fps: ",

		"X: ", "Y: ", "Z: ",                         // position
		"rX (pich): ", "rY (yaw): ", "rZ (roll): ",  // rotation
	};

	// convert into the string format all the data to update 
	const std::vector<std::string> dataToUpdate =
	{
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
#if 0
		std::to_string(systemState.renderedModelsCount),
		std::to_string(systemState.cellsDrawn),
		std::to_string(systemState.cellsCulled),
		std::to_string(systemState.renderedVerticesCount),
		std::to_string(systemState.renderedVerticesCount / 3)
#endif
	};


	const size_t numOfStrToUpdate = textIDsToUpdate.size();

	assert(prefixes.size() == numOfStrToUpdate);
	assert(dataToUpdate.size() == numOfStrToUpdate);


	// prepare new text content
	std::vector<std::string> textContentsToUpdate(numOfStrToUpdate);
	
	for (size_t idx = 0; idx < numOfStrToUpdate; ++idx)
	{
		textContentsToUpdate[idx] = { prefixes[idx] + dataToUpdate[idx] };
	}

	debugStrings_.Update(pDeviceContext,
		font1_,
		textIDsToUpdate,
		textContentsToUpdate);

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


///////////////////////////////////////////////////////////////////////////////////////////////
//                              PRIVATE RENDERING API
///////////////////////////////////////////////////////////////////////////////////////////////

void UserInterfaceClass::RenderDebugText(ID3D11DeviceContext* pDeviceContext, 
	const DirectX::XMMATRIX & WVO,
	const DirectX::XMFLOAT3 & textColor)
{
	// THIS FUNCTION renders all the UI debug text strings onto the screen

	ID3D11ShaderResourceView* const* ppFontTexture = font1_.GetTextureResourceViewAddress();

	debugStrings_.Render(pDeviceContext,
			&fontShader_,
			ppFontTexture,
			WVO, 
			textColor);
}