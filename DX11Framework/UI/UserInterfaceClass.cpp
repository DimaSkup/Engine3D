////////////////////////////////////////////////////////////////////////////////////////////
// Filename:     UserInterfaceclass.cpp
// Description:  a functional for initialization, 
//               updating and rendering of the UI
// 
// Created:      25.05.23
////////////////////////////////////////////////////////////////////////////////////////////
#include "UserInterfaceClass.h"
#include "../../ShaderClass/ShadersContainer.h"   // to get a pointer to the FontShaderClass



UserInterfaceClass::UserInterfaceClass(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	try
	{
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
		Log::Error(THIS_FUNC, e.what());
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
	int windowWidth, int windowHeight,
	const DirectX::XMMATRIX & baseViewMatrix,
	FontShaderClass* pFontShader)   // a font shader for rendering text onto the screen
{
	assert(pFontShader != nullptr);

	Log::Debug(THIS_FUNC_EMPTY);

	try
	{
		bool result = false;

		// setup a pointer to the font shader class so we can use it at any part of the UserInterfaceClass
		pFontShader_ = pFontShader;

		// initialize the first font object
		result = pFont1_->Initialize(pD3D->GetDevice(), pD3D->GetDeviceContext(), "data/ui/font01.txt", L"data/ui/font01.dds");
		COM_ERROR_IF_FALSE(result, "can't initialize the first font object");

		// initialize the fps text string
		result = pFpsString_->Initialize(pD3D->GetDevice(), pD3D->GetDeviceContext(),
			windowWidth, windowHeight,
			16, pFont1_, pFontShader_,
			"Fps: 0", 10, 50, 0.0f, 1.0f, 0.0f);
		COM_ERROR_IF_FALSE(result, "can't initialize the fps text string");

		// initialize the video text strings
		result = this->InitializeVideoStrings(pD3D, windowWidth, windowHeight);
		COM_ERROR_IF_FALSE(result, "can't initialize the video text strings");

		// initialize the position text strings
		result = this->InitializePositionStrings(pD3D, windowWidth, windowHeight);
		COM_ERROR_IF_FALSE(result, "can't initialize the position text strings");

		// initialize the render count text strings
		result = this->InitializeRenderCountStrings(pD3D, windowWidth, windowHeight);
		COM_ERROR_IF_FALSE(result, "can't initialize the render count strings");

		Log::Debug(THIS_FUNC, "USER INTERFACE is initialized");

		// initialize the base view matrix
		baseViewMatrix_ = baseViewMatrix;
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(THIS_FUNC, "can't initialize the UserInterfaceClass");
		return false;
	}


	return true;
}

///////////////////////////////////////////////////////////

bool UserInterfaceClass::Frame(ID3D11DeviceContext* pDeviceContext, 
	const SystemState* pSystemState,
	const DirectX::XMFLOAT3 & position,
	const DirectX::XMFLOAT3 & rotation)
{
	COM_ERROR_IF_FALSE(pSystemState, "the system state object == nullptr");

	bool result = false;

	try
	{
		// update the fps string
		result = UpdateFpsString(pDeviceContext, pSystemState->fps);
		COM_ERROR_IF_FALSE(result, "can't update the fps string");

		// update the position strings
		result = UpdatePositionStrings(pDeviceContext, position, rotation);
		COM_ERROR_IF_FALSE(result, "can't update the position strings");

		// update the render count strings
		result = UpdateRenderCounts(pDeviceContext,
			pSystemState->renderCount,
			pSystemState->cellsDrawn, 
			pSystemState->cellsCulled);
		COM_ERROR_IF_FALSE(result, "can't update the render count strings");
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		Log::Error(THIS_FUNC, "can't update some text string");
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////

bool UserInterfaceClass::Render(D3DClass* pD3D,
	const XMMATRIX & worldMatrix, 
	const XMMATRIX & orthoMatrix)
{
	// turn off the Z buffer and enable alpha blending to begin 2D rendering
	pD3D->TurnZBufferOff();
	pD3D->TurnOnAlphaBlending();

	ID3D11DeviceContext* pDeviceContext = pD3D->GetDeviceContext();

	// render the fps string
	pFpsString_->Render(pDeviceContext, worldMatrix, baseViewMatrix_, orthoMatrix);

	// render the video card strings
	videoStringsArr_[0]->Render(pDeviceContext, worldMatrix, baseViewMatrix_, orthoMatrix);
	videoStringsArr_[1]->Render(pDeviceContext, worldMatrix, baseViewMatrix_, orthoMatrix);

	// render the position and rotation strings
	for (size_t i = 0; i < 6; i++)
	{
		positionStringsArr_[i]->Render(pDeviceContext, worldMatrix, baseViewMatrix_, orthoMatrix);
	}

	// render the render count strings
	for (size_t i = 0; i < 3; i++)
	{
		renderCountStringsArr_[i]->Render(pDeviceContext, worldMatrix, baseViewMatrix_, orthoMatrix);
	}
	
	pD3D->TurnOffAlphaBlending();  // turn off alpha blending now that the text has been rendered
	pD3D->TurnZBufferOn();         // turn the Z buffer back on now that the 2D rendering has completed

	return true;
}






////////////////////////////////////////////////////////////////////////////////////////////
//
//                                PRIVATE FUNCTIONS
//
////////////////////////////////////////////////////////////////////////////////////////////


bool UserInterfaceClass::InitializeVideoStrings(D3DClass* pD3D, 
	int screenWidth, 
	int screenHeight)
{
	// initialize the video text strings with initial data

	Log::Debug(THIS_FUNC_EMPTY);

	errno_t error = 0;
	bool result = false;
	char videoCardName[128] { '\0' };
	char videoString[144] { '\0' };
	char memoryString[32] { '\0' };
	char tempString[16] { '\0' };
	int videoCardMemory = 0;

	try
	{
		// setup the video card info string
		pD3D->GetVideoCardInfo(videoCardName, videoCardMemory);
		error = strcpy_s(videoString, "Video Card: ");
		COM_ERROR_IF_FALSE(error == 0, "can't copy the string");

		error = strcat_s(videoString, videoCardName);
		COM_ERROR_IF_FALSE(error == 0, "can't concatenate the string");


		// setup the video card memory string
		error = _itoa_s(videoCardMemory, tempString, 10);
		COM_ERROR_IF_FALSE(error == 0, "can't convert from integer into ascii");

		error = strcpy_s(memoryString, "Video Memory: ");
		COM_ERROR_IF_FALSE(error == 0, "can't copy the string");

		error = strcat_s(memoryString, tempString);
		COM_ERROR_IF_FALSE(error == 0, "can't concatenate the string");

		error = strcat_s(memoryString, "MB");
		COM_ERROR_IF_FALSE(error == 0, "can't concatenate the string");


		// initialize the video text strings
		result = videoStringsArr_[0]->Initialize(pD3D->GetDevice(), pD3D->GetDeviceContext(),
			screenWidth, screenHeight,
			256, pFont1_, pFontShader_, videoString, 10, 10, 1.0f, 1.0f, 1.0f);
		COM_ERROR_IF_FALSE(result, "can't initialize a string with a video card name");

		result = videoStringsArr_[1]->Initialize(pD3D->GetDevice(), pD3D->GetDeviceContext(),
			screenWidth, screenHeight,
			32, pFont1_, pFontShader_, memoryString, 10, 30, 1.0f, 1.0f, 1.0f);
		COM_ERROR_IF_FALSE(result, "can't initialize a string with a video card memory");
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(THIS_FUNC, "can't initialize debug strings with video data");
		return false;
	}

	return true;
}  // end InitializeVideoStrings

///////////////////////////////////////////////////////////

bool UserInterfaceClass::InitializePositionStrings(D3DClass* pD3D, 
	int screenWidth,
	int screenHeight)
{
	// initialize the position text string with initial data

	Log::Debug(THIS_FUNC_EMPTY);
	bool result = false;
	int stringLength = 16;

	// temporal pointers to the device and device context
	ID3D11Device* pDevice = pD3D->GetDevice();
	ID3D11DeviceContext* pDeviceContext = pD3D->GetDeviceContext();

	try
	{
		// init X position string
		result = positionStringsArr_[0]->Initialize(pDevice, pDeviceContext,
			screenWidth, screenHeight,
			stringLength, pFont1_, pFontShader_, "X: 0", 10, 100, 1.0f, 1.0f, 1.0f);
		COM_ERROR_IF_FALSE(result, "can't initialize the text string with X position data");

		// init Y position string
		result = positionStringsArr_[1]->Initialize(pDevice, pDeviceContext,
			screenWidth, screenHeight,
			stringLength, pFont1_, pFontShader_, "Y: 0", 10, 120, 1.0f, 1.0f, 1.0f);
		COM_ERROR_IF_FALSE(result, "can't initialize the text string with Y position data");

		// init Z position string
		result = positionStringsArr_[2]->Initialize(pDevice, pDeviceContext,
			screenWidth, screenHeight,
			stringLength, pFont1_, pFontShader_, "Z: 0", 10, 140, 1.0f, 1.0f, 1.0f);
		COM_ERROR_IF_FALSE(result, "can't initialize the text string with Z position data");



		// init X rotation string
		result = positionStringsArr_[3]->Initialize(pDevice, pDeviceContext,
			screenWidth, screenHeight,
			stringLength, pFont1_, pFontShader_, "rX: 0", 10, 180, 1.0f, 1.0f, 1.0f);
		COM_ERROR_IF_FALSE(result, "can't initialize the text string with X rotation data");

		// init Y rotation string
		result = positionStringsArr_[4]->Initialize(pDevice, pDeviceContext,
			screenWidth, screenHeight,
			stringLength, pFont1_, pFontShader_, "rY: 0", 10, 200, 1.0f, 1.0f, 1.0f);
		COM_ERROR_IF_FALSE(result, "can't initialize the text string with Y rotation data");

		// init Z rotation string
		result = positionStringsArr_[5]->Initialize(pDevice, pDeviceContext,
			screenWidth, screenHeight,
			stringLength, pFont1_, pFontShader_, "rZ: 0", 10, 220, 1.0f, 1.0f, 1.0f);
		COM_ERROR_IF_FALSE(result, "can't initialize the text string with Z rotation data");
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(THIS_FUNC, "can't initialize debug strings with position/rotation data");
		return false;
	}

	return true;
} // InitializePositionStrings()

///////////////////////////////////////////////////////////

bool UserInterfaceClass::InitializeRenderCountStrings(D3DClass* pD3D, 
	int screenWidth, 
	int screenHeight)
{
	// initialize the render count text strings with initial data

	Log::Debug(THIS_FUNC_EMPTY);
	bool result = false;
	int maxStringLength = 32;
	
	try
	{
		result = renderCountStringsArr_[0]->Initialize(pD3D->GetDevice(), pD3D->GetDeviceContext(),
			screenWidth, screenHeight,
			maxStringLength, pFont1_, pFontShader_, "Polys drawn: 0", 10, 260, 1.0f, 1.0f, 1.0f);
		COM_ERROR_IF_FALSE(result, "can't initialize the render count string");


		result = renderCountStringsArr_[1]->Initialize(pD3D->GetDevice(), pD3D->GetDeviceContext(),
			screenWidth, screenHeight,
			maxStringLength, pFont1_, pFontShader_, "Cells drawn: 0", 10, 280, 1.0f, 1.0f, 1.0f);
		COM_ERROR_IF_FALSE(result, "can't initialize the render count string");


		result = renderCountStringsArr_[2]->Initialize(pD3D->GetDevice(), pD3D->GetDeviceContext(),
			screenWidth, screenHeight,
			maxStringLength, pFont1_, pFontShader_, "Cells culled: 0", 10, 300, 1.0f, 1.0f, 1.0f);
		COM_ERROR_IF_FALSE(result, "can't initialize the render count string");

	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(THIS_FUNC, "can't initialize debug strings with render counts data");
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////

bool UserInterfaceClass::UpdateFpsString(ID3D11DeviceContext* pDeviceContext, int fps)
{
	// update the fps value to render it onto the screen

	DirectX::XMFLOAT4 fpsTextColor { 0.0f, 0.0f, 0.0f, 1.0f };  // black by default
	bool result = false;

	// check if the fps from the previous frame was the same, if so don't need to update the text string
	if (previousFps_ == fps)
	{
		return true;
	}

	// store the fps for checking next frame
	previousFps_ = fps;

	// truncate the fps to below 100,000
	if (fps > 99999)
	{
		fps = 99999;
	}

	// make a new fps string
	std::string newFpsValue{ "Fps: " + std::to_string(fps) };  

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
	result = pFpsString_->Update(pDeviceContext, newFpsValue, { 10.0f, 50.0f }, fpsTextColor);
	COM_ERROR_IF_FALSE(result, "can't update the fps string");

	return true;
}

///////////////////////////////////////////////////////////

bool UserInterfaceClass::UpdatePositionStrings(ID3D11DeviceContext* pDeviceContext,
	const DirectX::XMFLOAT3 & position, 
	const DirectX::XMFLOAT3 & rotation)
{
	// update the position strings to render it onto the screen

	char tempString[16] = { '\0' };
	char finalString[16] = { '\0' };
	bool result = false;
	DirectX::XMFLOAT4 whiteColor{ 1.0f, 1.0f, 1.0f, 1.0f };

	int positionX = static_cast<int>(position.x);
	int positionY = static_cast<int>(position.y);
	int positionZ = static_cast<int>(position.z);
	int rotationX = static_cast<int>(rotation.x);
	int rotationY = static_cast<int>(rotation.y);
	int rotationZ = static_cast<int>(rotation.z);

	// update the position strings if the value has changed since the last frame
	if (positionX != previousPosition_[0])
	{
		previousPosition_[0] = positionX;
		_itoa_s(previousPosition_[0], tempString, 10);
		strcpy_s(finalString, "X: ");
		strcat_s(finalString, tempString);
		result = positionStringsArr_[0]->Update(pDeviceContext, finalString, { 10.0f, 100.0f }, whiteColor);
		COM_ERROR_IF_FALSE(result, "can't update the text string with X position data");
	}

	if (positionY != previousPosition_[1])
	{
		previousPosition_[1] = positionY;
		_itoa_s(previousPosition_[1], tempString, 10);
		strcpy_s(finalString, "Y: ");
		strcat_s(finalString, tempString);
		result = positionStringsArr_[1]->Update(pDeviceContext, finalString, { 10.0f, 120.0f }, whiteColor);
		COM_ERROR_IF_FALSE(result, "can't update the text string with Y position data");
	}

	if (positionZ != previousPosition_[2])
	{
		previousPosition_[2] = positionZ;
		_itoa_s(positionZ, tempString, 10);
		strcpy_s(finalString, "Z: ");
		strcat_s(finalString, tempString);
		result = positionStringsArr_[2]->Update(pDeviceContext, finalString, { 10.0f, 140.0f }, whiteColor);
		COM_ERROR_IF_FALSE(result, "can't update the text string with Z position data");
	}

	if (rotationX != previousPosition_[3])
	{
		previousPosition_[3] = rotationX;
		_itoa_s(rotationX, tempString, 10);
		strcpy_s(finalString, "rX: ");
		strcat_s(finalString, tempString);
		result = positionStringsArr_[3]->Update(pDeviceContext, finalString, { 10.0f, 180.0f }, whiteColor);
		COM_ERROR_IF_FALSE(result, "can't update the text string with X rotation data");
	}

	if (rotationY != previousPosition_[4])
	{
		previousPosition_[4] = rotationY;
		_itoa_s(rotationY, tempString, 10);
		strcpy_s(finalString, "rY: ");
		strcat_s(finalString, tempString);
		result = positionStringsArr_[4]->Update(pDeviceContext, finalString, { 10.0f, 200.0f }, whiteColor);
		COM_ERROR_IF_FALSE(result, "can't update the text string with Y rotation data");
	}

	if (rotationZ != previousPosition_[5])
	{
		previousPosition_[5] = rotationZ;
		_itoa_s(rotationZ, tempString, 10);
		strcpy_s(finalString, "rZ: ");
		strcat_s(finalString, tempString);
		result = positionStringsArr_[5]->Update(pDeviceContext, finalString, { 10.0f, 220.0f }, whiteColor);
		COM_ERROR_IF_FALSE(result, "can't update the text string with Z rotation data");
	}

	return true;
}

///////////////////////////////////////////////////////////

bool UserInterfaceClass::UpdateRenderCounts(ID3D11DeviceContext* pDeviceContext, 
	int renderCount, 
	int nodesDrawn, 
	int nodesCulled)
{
	// update the render count strings to show it on the screen

	std::string finalString{ "" };
	bool result = false;
	DirectX::XMFLOAT4 whiteColor{ 1.0f, 1.0f, 1.0f, 1.0f };

	// update the string with the number of rendered polygons
	finalString = "Polys Drawn: ";
	finalString += std::to_string(renderCount);

	result = renderCountStringsArr_[0]->Update(pDeviceContext, finalString, { 10, 260 }, whiteColor);
	COM_ERROR_IF_FALSE(result, "can't update the string with the number of rendered polygons");


	// update the string with the number of rendered terrain cells
	finalString = "Cells Drawn: ";
	finalString += std::to_string(nodesDrawn);

	result = renderCountStringsArr_[1]->Update(pDeviceContext, finalString, { 10, 280 }, whiteColor);
	COM_ERROR_IF_FALSE(result, "can't update the string with the number of rendered terrain cells");


	// update the string with the number of culled terrain cells
	finalString = "Cells Culled: ";
	finalString += std::to_string(nodesCulled);

	result = renderCountStringsArr_[2]->Update(pDeviceContext, finalString, { 10, 300 }, whiteColor);
	COM_ERROR_IF_FALSE(result, "can't update the string with the number of culled terrain cells");



	return true;
}