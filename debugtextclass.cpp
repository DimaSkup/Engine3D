#include "debugtextclass.h"

DebugTextClass::DebugTextClass(void)
{
	sentencesPos.insert({ "Debug_MouseX", POINT{ 10, 10 } });
	sentencesPos.insert({ "Debug_MouseY", POINT{ 10, 27 } });
	sentencesPos.insert({ "Debug_Fps", POINT{ 10, 44 } });
	sentencesPos.insert({ "Debug_Cpu", POINT{ 10, 61 } });
	sentencesPos.insert({ "Debug_DisplayWH", POINT{ 10, 78 } });
	sentencesPos.insert({ "Debug_CameraOrientation", POINT{ 10, 95 } });
}




// Takes the fps integer value given to it and then converts it to a string. Once the fps
// count is in a string format it gets concatenated to another string so it has a prefix
// indicating that it is the fps speed. After that it is stored in the sentence structure
// for rendering. The SetFps() function also sets the colour of the fps string to green 
// if above 60 fps, yellow if below 60 fps, and red if below 30 fps
bool TextClass::SetFps(int fps)
{
	char tempString[16];
	char fpsString[16];
	float red = 0.0f, green = 0.0f, blue = 0.0f;
	bool result = false;


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



	result = SetSentenceByKey("fps_productivity", fpsString, m_fpsLinePos.x, m_fpsLinePos.y, red, green, blue);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't update the sentence with FPS data");
		return false;
	}


	return true;
} // SetFps()



  // this function is similar to the SetFps() function. It takes the cpu value and converts
  // it to a string which is the store in the sentence structure and rendered
bool TextClass::SetCpu(int cpu)
{
	char tempString[16];
	char cpuString[16];
	bool result = false;


	// convert the cpu integer to string format
	_itoa_s(cpu, tempString, 10);

	// setup the cpu string
	strcpy_s(cpuString, "Cpu: ");
	strcat_s(cpuString, tempString);
	strcat_s(cpuString, "%");

	// set the sentence with CPU data for output it on the screen
	if (!m_cpuLineIndex)  // if there is no sentence with CPU data yet
	{
		m_cpuLineIndex = this->AddSentence(cpuString, m_cpuLinePos.x, m_cpuLinePos.y, 0.0f, 1.0f, 0.0f);
	}
	else
	{
		result = UpdateSentence(m_sentencesVector[m_cpuLineIndex], cpuString,
			m_cpuLinePos.x, m_cpuLinePos.y, 1.0f, 1.0f, 1.0f);
		if (!result)
		{
			Log::Get()->Error(THIS_FUNC, "can't update the sentence with CPU data");
			return false;
		}
	}

	return true;
}



// takes a POINT with the current mouse coorinates and prints it on the screen
bool TextClass::SetMousePosition(DirectX::XMFLOAT2 pos)
{
	bool result = false;

	POINT mousePos{ static_cast<int>(pos.x), static_cast<int>(pos.y) };


	// HACK
	if (pos.x < -5000)
	{
		pos = { 0, 0 };
	}

	std::string strMouse;

	// output mouse X coord data
	strMouse = "Mouse X: " + std::to_string(mousePos.x);
	result = CreateOrUpdateSentenceByKey("mouseXPos", strMouse,
		m_mouseXLinePos.x, m_mouseXLinePos.y, 1.0f, 1.0f, 1.0f);

	// output mouse Y coord data
	strMouse = "Mouse Y: " + std::to_string(mousePos.y);
	result = CreateOrUpdateSentenceByKey("mouseYPos", strMouse,
		m_mouseYLinePos.x, m_mouseYLinePos.y, 1.0f, 1.0f, 1.0f);

	return true;
} // SetMousePosition()

  // takes display width and height and sets it for output on the screen
bool TextClass::SetDisplayParams(int width, int height)
{
	bool result = false;
	std::string displayParamsLine{ "Display: " };
	displayParamsLine += std::to_string(width) + "x" + std::to_string(height);

	result = CreateOrUpdateSentenceByKey("displayWHParams", displayParamsLine,
		m_displayWHParamsLinePos.x, m_displayWHParamsLinePos.y,
		1.0f, 1.0f, 1.0f);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't create or update the sentence with display params");
		return false;
	}

	return true;
} // SetDisplayParams()

bool TextClass::SetCameraOrientation(DirectX::XMFLOAT2 orientation)
{
	bool result = false;
	std::string text{ "" };

	text = "yaw: " + std::to_string(orientation.x) + "; pitch: " + std::to_string(orientation.y);
	result = CreateOrUpdateSentenceByKey("cameraOrientation", text, m_cameraOrientationLinePos.x, m_cameraOrientationLinePos.y, 1.0f, 1.0f, 1.0f);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't create/update the sentence with the camera orientation data");
		return false;
	}

	return true;
}