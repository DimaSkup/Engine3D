////////////////////////////////////////////////////////////////////
// Filename: debugtextclass.cpp
// Revising: 12.09.22
////////////////////////////////////////////////////////////////////
#include "debugtextclass.h"

DebugTextClass::DebugTextClass(void)
{
	pText_ = nullptr;
}

// we don't use the copy constructor and destructor in this class
DebugTextClass::DebugTextClass(const DebugTextClass& copy) {}
DebugTextClass::~DebugTextClass(void) {}


////////////////////////////////////////////////////////////////////
//
//                    PUBLIC FUNCTIONS
//
////////////////////////////////////////////////////////////////////

bool DebugTextClass::Initialize(ID3D11Device* device, 
	                            ID3D11DeviceContext* deviceContext, HWND hwnd,
	                            int screenWidth, int screenHeight,
	                            DirectX::XMMATRIX baseViewMatrix)
{
	Log::Get()->Debug(THIS_FUNC_EMPTY);

	bool result = false;

	// --- configure sentences positions on the screen --- //
	SetSentencePosByKey("MouseXPos", 10, 10);
	SetSentencePosByKey("MouseYPos", 10, 27);
	SetSentencePosByKey("Fps", 10, 44);
	SetSentencePosByKey("Cpu", 10, 61);
	SetSentencePosByKey("DisplayWH", 10, 78);
	SetSentencePosByKey("CameraPosition", 10, 95);
	SetSentencePosByKey("CameraOrientation", 10, 112);
	SetSentencePosByKey("RenderCount", 10, 129);

	// --- create and initialize the text class object --- //
	pText_ = new TextClass();
	if (!pText_)
	{
		Log::Get()->Error(THIS_FUNC, "can't create a text class object");
		return false;
	}

	result = pText_->Initialize(device, deviceContext, hwnd, 
		                         screenWidth, screenHeight,
		                         baseViewMatrix);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't initialize the text class object");
		return false;
	}
	


	return true;
}  // Initialize()


// render all the debug sentence onto the screen
bool DebugTextClass::Render(ID3D11DeviceContext* deviceContext,
	                        DirectX::XMMATRIX worldMatrix,
	                        DirectX::XMMATRIX orthoMatrix)
{
	bool result = false;

	result = pText_->Render(deviceContext, worldMatrix, orthoMatrix);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't render onto the screen using the text class");
		return false;
	}

	return true;
} // Render()

void DebugTextClass::Shutdown(void)
{
	_SHUTDOWN(pText_);

	if (!sentencesPos_.empty())
	{
		sentencesPos_.clear();
	}

	Log::Get()->Debug(THIS_FUNC_EMPTY);
}










bool DebugTextClass::SetDebugParams(const SETTINGS::settingsParams* systemParams, const SystemState* systemState)
{
	bool result = false;

	// setup the output debug text

	// set the mouse position
	result = this->SetMousePosition(systemState->mouseX, systemState->mouseY);
	if (!result)
	{
		Log::Error(THIS_FUNC, "can't set up the mouse position for the debug text output");
		return false;
	}

	// set the frames per second
	result = this->SetFps(systemState->fps);
	if (!result)
	{
		Log::Error(THIS_FUNC, "can't set the fps parameter for output");
		return false;
	}


	// set the screen parameters
	result = this->SetDisplayParams(systemParams->WINDOW_WIDTH, systemParams->WINDOW_HEIGHT);
	if (!result)
	{
		Log::Error(THIS_FUNC, "can't set the display params for output");
		return false;
	}

	// set the camera position
	result = this->SetCameraPosition(systemState->editorCameraPosition);
	if (!result)
	{
		Log::Error(THIS_FUNC, "can't set the camera position for output");
		return false;
	}

	// set the camera orientation
	result = this->SetCameraOrientation(systemState->editorCameraRotation);
	if (!result)
	{
		Log::Error(THIS_FUNC, "can't set the camera orientation for output");
		return false;
	}

	// set the number of rendered models
	result = this->SetRenderCount(systemState->renderCount);
	if (!result)
	{
		Log::Error(THIS_FUNC, "can't set the number of rendered models for output");
		return false;
	}

	// set the cpu usage
	result = this->SetCpu(systemState->cpu);
	if (!result)
	{
		Log::Error(THIS_FUNC, "can't set the cpu performance for output");
		return false;
	}

	return true;
}


// Takes the fps integer value given to it and then converts it to a string. Once the fps
// count is in a string format it gets concatenated to another string so it has a prefix
// indicating that it is the fps speed. After that it is stored in the sentence structure
// for rendering. The SetFps() function also sets the colour of the fps string to green 
// if above 60 fps, yellow if below 60 fps, and red if below 30 fps
bool DebugTextClass::SetFps(int fps)
{
	//std::string tempString;
	//std::string fpsString;
	char tempString[16];
	char fpsString[16];
	float red = 0.0f, green = 0.0f, blue = 0.0f;
	bool result = false;
	std::string fpsKey = "Fps";


	// truncate the fps to below 10,000
	if (fps > 9999)
	{
		fps = 9999;
	}

	// convert the fps integer to string format
	_itoa_s(fps, tempString, 10);

	// setup the fps string
	strcpy_s(fpsString, "Fps: ");
	strcat_s(fpsString, tempString);

	// if fps is 60 or above set the fps colour to green
	if (fps >= 60)
	{
		red = 0.0f;
		green = 1.0f;
		blue = 0.0f;
	}

	// if fps is below 60 set the fps colour to yello
	if (fps < 60)
	{
		red = 1.0f;
		green = 1.0f;
		blue = 0.0f;
	}

	// if fps is below 30 set the fps colour to red
	if (fps < 30)
	{
		red = 1.0f;
		green = 0.0f;
		blue = 0.0f;
	}


	result = pText_->SetSentenceByKey(fpsKey, fpsString,
		                               sentencesPos_[fpsKey].x,
		                               sentencesPos_[fpsKey].y,
		                               red, green, blue);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't update the sentence with FPS data");
		return false;
	}


	return true;
} // SetFps()



  // this function is similar to the SetFps() function. It takes the cpu value and converts
  // it to a string which is the store in the sentence structure and rendered
bool DebugTextClass::SetCpu(int cpu)
{
	char tempString[16];
	char cpuString[16];
	bool result = false;
	std::string cpuKey = "Cpu";


	// convert the cpu integer to string format
	_itoa_s(cpu, tempString, 10);

	// setup the cpu string
	strcpy_s(cpuString, "Cpu: ");
	strcat_s(cpuString, tempString);
	strcat_s(cpuString, "%");

	// set the sentence with CPU data for output it on the screen

	result = pText_->SetSentenceByKey(cpuKey, cpuString,
		                               sentencesPos_[cpuKey].x,
		                               sentencesPos_[cpuKey].y,
		                               1.0f, 1.0f, 1.0f);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't update the sentence with FPS data");
		return false;
	}

	return true;
}



// takes the current mouse position coordinates, makes about 
// it sentences and sets these sentences to print
bool DebugTextClass::SetMousePosition(int mouseX, int mouseY)
{
	bool result = false;
	POINT mousePos{ mouseX, mouseY };
	std::string mouseXKey{ "MouseXPos" };
	std::string mouseYKey{ "MouseYPos" };
	std::string strMouse{ "Mouse X: " + std::to_string(mousePos.x) };

	// HACK
	if (mouseX < -5000)
	{
		//pos = { 0, 0 };
	}

	// output mouse X coord data
	result = pText_->SetSentenceByKey(mouseXKey, strMouse,
		                               sentencesPos_[mouseXKey].x, 
		                               sentencesPos_[mouseXKey].y, 
		                               1.0f, 1.0f, 1.0f);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't set the sentence with mouse X data");
		return false;
	}

	// output mouse Y coord data
	strMouse = "Mouse Y: " + std::to_string(mousePos.y);
	result = pText_->SetSentenceByKey(mouseYKey, strMouse,
		                               sentencesPos_[mouseYKey].x,
		                               sentencesPos_[mouseYKey].y,
		                               1.0f, 1.0f, 1.0f);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't set the sentence with mouse Y data");
		return false;
	}

	return true;
} // SetMousePosition()

  // takes display width and height and sets it for output on the screen
bool DebugTextClass::SetDisplayParams(int width, int height)
{
	bool result = false;
	std::string displayParamsLine{ "Display: " };
	std::string displayKey{ "DisplayWH" };

	// make a final string with display params
	displayParamsLine += std::to_string(width) + "x" + std::to_string(height);

	result = pText_->SetSentenceByKey(displayKey, displayParamsLine,
		                               sentencesPos_[displayKey].x,
		                               sentencesPos_[displayKey].y,
		                               1.0f, 1.0f, 1.0f);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't set the sentence with display params");
		return false;
	}

	return true;
} // SetDisplayParams()


  // print the current camera position coordinates
bool DebugTextClass::SetCameraPosition(const DirectX::XMFLOAT3 & position)
{
	bool result = false;
	std::string displayParamsLine{ "" };
	std::string cameraPosKey{ "CameraPosition" };

	// for better comprehension we show position.z as a movement by Y-axis and
	// position.y as movement by Z-axis
	displayParamsLine = "x: " + std::to_string(position.x) + "; " +
		                "y: " + std::to_string(position.z) + "; " +
		                "z: " + std::to_string(position.y) + ";";

	// update the sentence
	result = pText_->SetSentenceByKey(cameraPosKey, displayParamsLine,
		sentencesPos_[cameraPosKey].x,
		sentencesPos_[cameraPosKey].y,
		1.0f, 1.0f, 1.0f);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't set the sentence with camera position coordinates");
		return false;
	}

	return true;
} // SetCameraOrientation()


// print the current rotation angle of the camera (in degrees)
bool DebugTextClass::SetCameraOrientation(const DirectX::XMFLOAT3 & orientation)
{
	bool result = false;
	std::string displayParamsLine{ "" };
	std::string cameraOrientKey{ "CameraOrientation" };

	displayParamsLine = "x angle: " + std::to_string(orientation.x) + "; y angle: " + std::to_string(orientation.y);

	// update the sentence
	result = pText_->SetSentenceByKey(cameraOrientKey, displayParamsLine,
		                               sentencesPos_[cameraOrientKey].x,
		                               sentencesPos_[cameraOrientKey].y,
		                               1.0f, 1.0f, 1.0f);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't set the sentence with camera orientation params");
		return false;
	}

	return true;
} // SetCameraOrientation()


// print onto the screen the number of models that was actually rendered this frame
bool DebugTextClass::SetRenderCount(int renderCount)
{
	std::string renderCountLine{ "Render count: " };
	std::string renderCountKey{ "RenderCount" };
	bool result = false;

	renderCountLine += std::to_string(renderCount);
	result = pText_->SetSentenceByKey(renderCountKey, renderCountLine,
		                               sentencesPos_[renderCountKey].x,
		                               sentencesPos_[renderCountKey].y,
		                               1.0f, 1.0f, 1.0f);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't set the render count for printing onto the screen");
		return false;
	}

	return true;
} // SetRenderCount();





////////////////////////////////////////////////////////////////////
//
//                    PUBLIC FUNCTIONS
//
////////////////////////////////////////////////////////////////////
void DebugTextClass::SetSentencePosByKey(std::string key, int posX, int posY)
{
	sentencesPos_.insert({ key, POINT{posX, posY} });
}