////////////////////////////////////////////////////////////////////////////////////////////
// Filename:     UserInterfaceclass.cpp
// Description:  a functional for initialization, 
//               updating and rendering of the UI
// 
// Created:      25.05.23
////////////////////////////////////////////////////////////////////////////////////////////
#include "UserInterfaceClass.h"


UserInterfaceClass::UserInterfaceClass(ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext)
{
	assert((pDevice != nullptr) && "the ptr to device must not be nullptr!");
	assert((pDeviceContext != nullptr) && "the ptr to device context must not be nullptr!");

	try
	{
		pDevice_ = pDevice;
		pDeviceContext_ = pDeviceContext;

		// create the first font object
		pFont1_ = new FontClass;                      

		// create the fps text string
		pFpsString_ = new TextClass(pDevice, pDeviceContext); 

		// set how many debug strings we will have
		videoStringsArr_.resize(numVideoStrings_, nullptr);
		positionStringsArr_.resize(numPositionStrings_, nullptr);
		renderCountStringsArr_.resize(numRenderCountStrings_, nullptr);

		// create the text objects for the video strings
		for (UINT i = 0; i < videoStringsArr_.size(); i++)
		{
			videoStringsArr_[i] = new TextClass(pDevice, pDeviceContext);
		}

		// create the text objects for the position strings
		for (UINT i = 0; i < positionStringsArr_.size(); i++)
		{
			positionStringsArr_[i] = new TextClass(pDevice, pDeviceContext);
		}

		// create the text objects for the render count strings
		for (UINT i = 0; i < renderCountStringsArr_.size(); i++)
		{
			renderCountStringsArr_[i] = new TextClass(pDevice, pDeviceContext);
		}
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(LOG_MACRO, e.what());
		COM_ERROR_IF_FALSE(false, "can't allocate memory for the UI elements");
	}
}

UserInterfaceClass::~UserInterfaceClass()
{
	// release memory from the font class obj
	_DELETE(pFont1_);

	// release memory from the string about fps
	_DELETE(pFpsString_);

	// release memory from strings about video adapters
	for (TextClass* pText : videoStringsArr_)
	{
		_DELETE(pText);
	}
	videoStringsArr_.clear();

	// release memory from strings about camera position
	for (TextClass* pText : positionStringsArr_)
	{
		_DELETE(pText);
	}
	positionStringsArr_.clear();

	// release memory from strings about numbers of rendered/culled models
	for (TextClass* pText : renderCountStringsArr_)
	{
		_DELETE(pText);
	}
	renderCountStringsArr_.clear();
}




////////////////////////////////////////////////////////////////////////////////////////////
//
//                                 PUBLIC FUNCTIONS
//
////////////////////////////////////////////////////////////////////////////////////////////

// initialize the graphics user interface (GUI)
bool UserInterfaceClass::Initialize(D3DClass* pD3D, 
	const std::string & fontDataFilePath,      // a path to file with data about this type of font
	const std::string & fontTextureFilePath,   // a path to texture file for this font
	const int windowWidth, 
	const int windowHeight,
	FontShaderClass* pFontShader)   // a font shader for rendering text onto the screen
{
	assert(pFontShader != nullptr);

	Log::Debug(LOG_MACRO);

	try
	{
		bool result = false;
		const POINT fpsStringPos{ 10, 50 };
		const DirectX::XMFLOAT3 fpsStringColor{ 0.0f, 1.0f, 0.0f };  // green
		std::string videoCardName{ "" };
		int videoCardMemory = 0;
		

		// setup a pointer to the font shader class so we can use it at any part of the UserInterfaceClass
		pFontShader_ = pFontShader;

		/////////////////////////////////////

		// initialize the first font object
		result = pFont1_->Initialize(pDevice_, pDeviceContext_, fontDataFilePath, fontTextureFilePath);
		COM_ERROR_IF_FALSE(result, "can't initialize the first font object");

		// initialize the fps text string
		result = pFpsString_->Initialize(pD3D->GetDevice(), pD3D->GetDeviceContext(),
			windowWidth, windowHeight,
			maxDebugStringSize,          // max size for this string
			pFont1_, pFontShader_,    
			"Fps: 0", fpsStringPos,      // initial string / initial position
			fpsStringColor);
		COM_ERROR_IF_FALSE(result, "can't initialize the fps text string");


		// initialize the video text strings
		pD3D->GetVideoCardInfo(videoCardName, videoCardMemory);

		result = this->InitializeVideoStrings(windowWidth, windowHeight, videoCardMemory, videoCardName);
		COM_ERROR_IF_FALSE(result, "can't initialize the video text strings");


		// initialize the position text strings
		result = this->InitializePositionStrings(windowWidth, windowHeight);
		COM_ERROR_IF_FALSE(result, "can't initialize the position text strings");

		// initialize the render count text strings
		result = this->InitializeRenderCountStrings(windowWidth, windowHeight);
		COM_ERROR_IF_FALSE(result, "can't initialize the render count strings");



		Log::Debug(LOG_MACRO, "USER INTERFACE is initialized");
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		Log::Error(LOG_MACRO, "can't initialize the UserInterfaceClass");
		return false;
	}


	return true;
}

///////////////////////////////////////////////////////////

bool UserInterfaceClass::Frame(const SystemState* pSystemState)
{
	// each frame we call this function for updating the UI

	COM_ERROR_IF_NULLPTR(pSystemState, "the system state object == nullptr");

	bool result = false;

	try
	{
		// update the fps string
		result = UpdateFpsString(pSystemState->fps);
		COM_ERROR_IF_FALSE(result, "can't update the fps string");

		// update the position strings
		result = UpdatePositionStrings(pSystemState->editorCameraPosition, 
			pSystemState->editorCameraRotation);
		COM_ERROR_IF_FALSE(result, "can't update the position strings");

		// update the render count strings
		result = UpdateRenderCounts(pSystemState->renderedModelsCount,
			pSystemState->cellsDrawn, 
			pSystemState->cellsCulled,
			pSystemState->renderedVerticesCount,
			pSystemState->renderedVerticesCount / 3);
		COM_ERROR_IF_FALSE(result, "can't update the render count strings");
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		Log::Error(LOG_MACRO, "can't update some text string");
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////

bool UserInterfaceClass::Render(D3DClass* pD3D, DataContainerForShaders* pDataForShaders)
{
	//
	// this functions renders all the UI elements onto the screen
	//

	// turn off the Z buffer and enable alpha blending to begin 2D rendering
	pD3D->TurnZBufferOff();
	pD3D->TurnOnAlphaBlending();


	this->RenderDebugText(pDataForShaders);


	pD3D->TurnOffAlphaBlending();  // turn off alpha blending now that the text has been rendered
	pD3D->TurnZBufferOn();         // turn the Z buffer back on now that the 2D rendering has completed

	return true;
}






////////////////////////////////////////////////////////////////////////////////////////////
//
//                                PRIVATE FUNCTIONS
//
////////////////////////////////////////////////////////////////////////////////////////////


bool UserInterfaceClass::InitializeVideoStrings(const int screenWidth,
	const int screenHeight,
	const int videoCardMemory,
	const std::string & videoCardName)
{
	// initialize the video text strings with initial data

	Log::Debug(LOG_MACRO);

	errno_t error = 0;
	bool result = false;
	char videoStringData[144] { '\0' };
	char memoryStringData[32] { '\0' };
	char tempString[16] { '\0' };

	const int videoString_MaxSize = 256;
	const int memoryString_MaxSize = 32;
	const POINT videoStringPos{ 10, 10 };
	const POINT memoryStringPos{ 10, 30 };
	const DirectX::XMFLOAT3 textColor{ 0, 1, 0 };


	try
	{
		// setup the video card info string
		error = strcpy_s(videoStringData, "Video Card: ");
		COM_ERROR_IF_FALSE(error == 0, "can't copy the string");

		error = strcat_s(videoStringData, videoCardName.c_str());
		COM_ERROR_IF_FALSE(error == 0, "can't concatenate the string");


		// setup the video card memory string
		error = _itoa_s(videoCardMemory, tempString, 10);
		COM_ERROR_IF_FALSE(error == 0, "can't convert from integer into ascii");

		error = strcpy_s(memoryStringData, "Video Memory: ");
		COM_ERROR_IF_FALSE(error == 0, "can't copy the string");

		error = strcat_s(memoryStringData, tempString);
		COM_ERROR_IF_FALSE(error == 0, "can't concatenate the string");

		error = strcat_s(memoryStringData, "MB");
		COM_ERROR_IF_FALSE(error == 0, "can't concatenate the string");


		// initialize the video text strings
		result = videoStringsArr_[0]->Initialize(pDevice_, pDeviceContext_,
			screenWidth, screenHeight,
			videoString_MaxSize,
			pFont1_, pFontShader_, 
			videoStringData,
			videoStringPos,
			textColor);
		COM_ERROR_IF_FALSE(result, "can't initialize a string with a video card name");

		result = videoStringsArr_[1]->Initialize(pDevice_, pDeviceContext_,
			screenWidth, screenHeight,
			memoryString_MaxSize, 
			pFont1_, pFontShader_,
			memoryStringData,
			memoryStringPos,
			textColor);
		COM_ERROR_IF_FALSE(result, "can't initialize a string with a video card memory");
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(LOG_MACRO, "can't initialize debug strings with video data");
		return false;
	}

	return true;
}  // end InitializeVideoStrings

///////////////////////////////////////////////////////////

bool UserInterfaceClass::InitializePositionStrings(const int screenWidth,
	const int screenHeight)
{
	// initialize the position text string with initial data

	Log::Debug(LOG_MACRO);


	bool result = false;
	POINT drawAt{ 10, 100 };                          // start position where we render the first string
	const int strideY = 20;                           // each text string is rendered by 20 pixels lower that the previous one

	try
	{
		// ----------------------- position string ----------------------- //

		// init X position string
		result = positionStringsArr_[0]->Initialize(pDevice_, pDeviceContext_,
			screenWidth, screenHeight,
			maxDebugStringSize, pFont1_, 
			pFontShader_, "X: 0",
			drawAt, defaultDebugTextColor);
		COM_ERROR_IF_FALSE(result, "can't initialize the text string with X position data");
		drawAt.y += strideY;

		// init Y position string
		result = positionStringsArr_[1]->Initialize(pDevice_, pDeviceContext_,
			screenWidth, screenHeight,
			maxDebugStringSize, pFont1_,
			pFontShader_, "Y: 0", 
			drawAt, defaultDebugTextColor);
		COM_ERROR_IF_FALSE(result, "can't initialize the text string with Y position data");
		drawAt.y += strideY + strideY;  // make stride by Y two times

		// init Z position string
		result = positionStringsArr_[2]->Initialize(pDevice_, pDeviceContext_,
			screenWidth, screenHeight,
			maxDebugStringSize, pFont1_, 
			pFontShader_, "Z: 0", 
			drawAt, defaultDebugTextColor);
		COM_ERROR_IF_FALSE(result, "can't initialize the text string with Z position data");
		drawAt.y += strideY;



		// ----------------------- rotation string ----------------------- //

		// init X rotation string
		result = positionStringsArr_[3]->Initialize(pDevice_, pDeviceContext_,
			screenWidth, screenHeight,
			maxDebugStringSize, pFont1_, 
			pFontShader_, "rX (pich): 0", 
			drawAt, defaultDebugTextColor);
		COM_ERROR_IF_FALSE(result, "can't initialize the text string with X rotation data");
		drawAt.y += strideY;

		// init Y rotation string
		result = positionStringsArr_[4]->Initialize(pDevice_, pDeviceContext_,
			screenWidth, screenHeight,
			maxDebugStringSize, pFont1_,
			pFontShader_, "rY (yaw): 0", 
			drawAt, defaultDebugTextColor);
		COM_ERROR_IF_FALSE(result, "can't initialize the text string with Y rotation data");
		drawAt.y += strideY;

		// init Z rotation string
		result = positionStringsArr_[5]->Initialize(pDevice_, pDeviceContext_,
			screenWidth, screenHeight,
			maxDebugStringSize, pFont1_, 
			pFontShader_, "rZ (roll): 0", 
			drawAt, defaultDebugTextColor);
		COM_ERROR_IF_FALSE(result, "can't initialize the text string with Z rotation data");
		drawAt.y += strideY;

	} 
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(LOG_MACRO, "can't initialize debug strings with position/rotation data");
		return false;
	}

	return true;

} // InitializePositionStrings()

///////////////////////////////////////////////////////////

bool UserInterfaceClass::InitializeRenderCountStrings(const int screenWidth, 
	const int screenHeight)
{
	// initialize the render count text strings with initial data

	Log::Debug(LOG_MACRO);

	bool result = false;
	const int strideY = 20;   // each text string is rendered by 20 pixels lower that the previous one
	POINT drawAt{ 10, 260 };
	
	try
	{
		result = renderCountStringsArr_[0]->Initialize(pDevice_, pDeviceContext_,
			screenWidth, screenHeight,
			maxDebugStringSize, pFont1_, 
			pFontShader_, "Models drawn: 0", 
			drawAt, defaultDebugTextColor);
		COM_ERROR_IF_FALSE(result, "can't initialize the string (models draw)");
		drawAt.y += strideY;

		result = renderCountStringsArr_[1]->Initialize(pDevice_, pDeviceContext_,
			screenWidth, screenHeight,
			maxDebugStringSize, pFont1_,
			pFontShader_, "Cells drawn: 0",
			drawAt, defaultDebugTextColor);
		COM_ERROR_IF_FALSE(result, "can't initialize the string (cells drawn)");
		drawAt.y += strideY;

		result = renderCountStringsArr_[2]->Initialize(pDevice_, pDeviceContext_,
			screenWidth, screenHeight,
			maxDebugStringSize, pFont1_,
			pFontShader_, "Cells culled: 0",
			drawAt, defaultDebugTextColor);
		COM_ERROR_IF_FALSE(result, "can't initialize the string (cells culled)");
		drawAt.y += strideY;

		result = renderCountStringsArr_[3]->Initialize(pDevice_, pDeviceContext_,
			screenWidth, screenHeight,
			maxDebugStringSize, pFont1_,
			pFontShader_, "Vertices drawn: 0",
			drawAt, defaultDebugTextColor);
		COM_ERROR_IF_FALSE(result, "can't initialize the string (vertices drawn)");
		drawAt.y += strideY;

		result = renderCountStringsArr_[4]->Initialize(pDevice_, pDeviceContext_,
			screenWidth, screenHeight,
			maxDebugStringSize, pFont1_,
			pFontShader_, "Triangles drawn: 0",
			drawAt, defaultDebugTextColor);
		COM_ERROR_IF_FALSE(result, "can't initialize the string (triangles drawn)");
		drawAt.y += strideY;

	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(LOG_MACRO, "can't initialize debug strings with render counts data");
		return false;
	}

	return true;

} // end InitializeRenderCountStrings

///////////////////////////////////////////////////////////

bool UserInterfaceClass::UpdateFpsString(const int fps)
{
	// update the fps value to render it onto the screen

	// check if the fps from the previous frame was the same, if so don't need to update the text string
	if (previousFps_ == fps)
	{
		return true;
	}

	////////////////////////////////////////////////

	DirectX::XMFLOAT3 fpsTextColor{ 0.0f, 0.0f, 0.0f };  // black by default
	bool result = false;

	// store the new fps value for checking next frame
	previousFps_ = fps;

	// make a new fps string
	std::string newFpsStr{ "Fps: " + std::to_string(fps) };  

	// setup the fps text color
	if (fps >= 60)
	{
		fpsTextColor.y = 1.0f; // set to green
	}
	else if (fps < 30)
	{
		fpsTextColor.x = 1.0f; // set to red
	}
	else  // fps is between 30 and 60 so set the color to yellow
	{
		fpsTextColor.x = 1.0f;
		fpsTextColor.y = 1.0f;
	}

	// update the sentence with the new string information
	result = pFpsString_->Update(pDeviceContext_, newFpsStr, { 10, 50 }, fpsTextColor);
	COM_ERROR_IF_FALSE(result, "can't update the fps string");

	return true;
}

///////////////////////////////////////////////////////////

bool UserInterfaceClass::UpdatePositionStrings(const DirectX::XMFLOAT3 & position, 
	const DirectX::XMFLOAT3 & rotation)
{
	// update the GUI strings with position/rotation data to render it onto the screen

	char finalString[32] = { '\0' };
	bool result = false;
	const DirectX::XMFLOAT3 whiteColor{ 1.0f, 1.0f, 1.0f };


	/////////////////////////////////////////////
	//  UPDATE POSITION STRINGS
	/////////////////////////////////////////////

	// update the position strings if the value has changed since the last frame
	if (position.x != previousPosition_.x)
	{
		// update the value of previous position
		previousPosition_.x = position.x;  

		// prepare a string with data
		strcpy_s(finalString, "X: ");
		strcat_s(finalString, std::to_string(position.x).c_str());  // get float position as string and concatenate it to the finalString

		// update the string with new one
		result = positionStringsArr_[0]->Update(pDeviceContext_, finalString, { 10, 100 }, whiteColor);
		COM_ERROR_IF_FALSE(result, "can't update the text string with X position data");
	}

	if (position.y != previousPosition_.y)
	{
		previousPosition_.y = position.y;

		strcpy_s(finalString, "Y: ");
		strcat_s(finalString, std::to_string(position.y).c_str());

		result = positionStringsArr_[1]->Update(pDeviceContext_, finalString, { 10, 120 }, whiteColor);
		COM_ERROR_IF_FALSE(result, "can't update the text string with Y position data");
	}

	if (position.z != previousPosition_.z)
	{
		previousPosition_.z = position.z;
		
		strcpy_s(finalString, "Z: ");
		strcat_s(finalString, std::to_string(position.z).c_str());

		result = positionStringsArr_[2]->Update(pDeviceContext_, finalString, { 10, 140 }, whiteColor);
		COM_ERROR_IF_FALSE(result, "can't update the text string with Z position data");
	}


	/////////////////////////////////////////////
	//  UPDATE ROTATION STRINGS
	/////////////////////////////////////////////

	if (rotation.x != previousRotation_.x)
	{
		previousRotation_.x = rotation.x;

		strcpy_s(finalString, "rX (pitch): ");
		strcat_s(finalString, std::to_string(rotation.x).c_str());

		result = positionStringsArr_[3]->Update(pDeviceContext_, finalString, { 10, 180 }, whiteColor);
		COM_ERROR_IF_FALSE(result, "can't update the text string with X rotation data");
	}

	if (rotation.y != previousRotation_.y)
	{
		previousRotation_.y = rotation.y;
		
		strcpy_s(finalString, "rY (yaw): ");
		strcat_s(finalString, std::to_string(rotation.y).c_str());

		result = positionStringsArr_[4]->Update(pDeviceContext_, finalString, { 10, 200 }, whiteColor);
		COM_ERROR_IF_FALSE(result, "can't update the text string with Y rotation data");
	}

	if (rotation.z != previousRotation_.z)
	{
		previousRotation_.z = rotation.z;

		strcpy_s(finalString, "rZ (roll): ");
		strcat_s(finalString, std::to_string(rotation.z).c_str());

		result = positionStringsArr_[5]->Update(pDeviceContext_, finalString, { 10, 220 }, whiteColor);
		COM_ERROR_IF_FALSE(result, "can't update the text string with Z rotation data");
	}

	return true;
}

///////////////////////////////////////////////////////////

bool UserInterfaceClass::UpdateRenderCounts(const int renderCount, 
	const int nodesDrawn, 
	const int nodesCulled,
	const int renderedVerticesCount,
	const int renderedTrianglesCount)
{
	// update the render count strings to show it on the screen

	bool result = false;
	const int strideY = 20;   // each text string is rendered by 20 pixels lower that the previous one
	POINT drawAt{ 10, 260 };

	////////////////////////////////////////////////

	// update the string with the number of rendered polygons
	std::string finalString = "Models Drawn: ";
	finalString += std::to_string(renderCount);

	result = renderCountStringsArr_[0]->Update(pDeviceContext_, finalString, drawAt, defaultDebugTextColor);
	COM_ERROR_IF_FALSE(result, "can't update the string with the number of rendered polygons");

	drawAt.y += strideY;

	////////////////////////////////////////////////


	// update the string with the number of rendered terrain cells
	finalString = "Cells Drawn: ";
	finalString += std::to_string(nodesDrawn);

	result = renderCountStringsArr_[1]->Update(pDeviceContext_, finalString, drawAt, defaultDebugTextColor);
	COM_ERROR_IF_FALSE(result, "can't update the string with the number of rendered terrain cells");

	drawAt.y += strideY;

	////////////////////////////////////////////////


	// update the string with the number of culled terrain cells
	finalString = "Cells Culled: ";
	finalString += std::to_string(nodesCulled);

	result = renderCountStringsArr_[2]->Update(pDeviceContext_, finalString, drawAt, defaultDebugTextColor);
	COM_ERROR_IF_FALSE(result, "can't update the string with the number of culled terrain cells");
	
	drawAt.y += strideY;

	////////////////////////////////////////////////


	// update the string with the number of rendered vertices
	finalString = "Vertices drawn: ";
	finalString += std::to_string(renderedVerticesCount);

	result = renderCountStringsArr_[3]->Update(pDeviceContext_, finalString, drawAt, defaultDebugTextColor);
	COM_ERROR_IF_FALSE(result, "can't update the string with the number of culled terrain cells");

	drawAt.y += strideY;

	////////////////////////////////////////////////


	// update the string with the number of rendered triangles
	finalString = "Triangles drawn: ";
	finalString += std::to_string(renderedTrianglesCount);

	result = renderCountStringsArr_[4]->Update(pDeviceContext_, finalString, drawAt, defaultDebugTextColor);
	COM_ERROR_IF_FALSE(result, "can't update the string with the number of culled terrain cells");

	drawAt.y += strideY;



	return true;
}

///////////////////////////////////////////////////////////

void UserInterfaceClass::RenderDebugText(DataContainerForShaders* pDataForShaders)
{
	// THIS FUNCTION renders all the UI debug text strings onto the screen

	// render the fps string
	pFpsString_->Render(pDeviceContext_, pDataForShaders);

	// render the video card strings
	videoStringsArr_[0]->Render(pDeviceContext_, pDataForShaders);
	videoStringsArr_[1]->Render(pDeviceContext_, pDataForShaders);

	// render the position and rotation strings
	for (size_t i = 0; i < positionStringsArr_.size(); i++)
	{
		positionStringsArr_[i]->Render(pDeviceContext_, pDataForShaders);
	}

	// render the render count strings
	for (size_t i = 0; i < renderCountStringsArr_.size(); i++)
	{
		renderCountStringsArr_[i]->Render(pDeviceContext_, pDataForShaders);
	}


	return;
}