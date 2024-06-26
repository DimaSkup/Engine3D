////////////////////////////////////////////////////////////////////////////////////////////
// Filename:     UserInterfaceclass.cpp
// Description:  a functional for initialization, 
//               updating and rendering of the UI
// 
// Created:      25.05.23
////////////////////////////////////////////////////////////////////////////////////////////
#include "UserInterfaceClass.h"

#include "UI_Windows/EntityCreationWindow.h"

// IMGUI STUFF
#include "imgui.h"
#include "imgui_stdlib.h"

#include <map>

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
		assert(windowWidth > 0);
		assert(windowHeight > 0);
		assert(videoCardMemory > 0);
		assert(!fontDataFilePath.empty());
		assert(!fontTextureFilePath.empty());
		assert(!videoCardName.empty());

		const UINT gapBetweenStringsInPixels = 2;
		const InitParamsForDebugStrings initParams;
		FontClass & font = font1_;

		// initialize the window dimensions members for internal using
		windowWidth_ = windowWidth;
		windowHeight_ = windowHeight;
		
		// data arrays for initialization
		std::vector<std::string> textContents;
		std::vector<std::string> textIDs;           // text IDs for associative navigation
		std::vector<POINT>       drawAtPositions;
		

		/////////////////////////////////////

		// initialize a font shader class which will be used 
		// for rendering all the text data onto the screen;
		fontShader_.Initialize(pDevice, pDeviceContext);

		// initialize the first font object
		font.Initialize(pDevice, fontDataFilePath, fontTextureFilePath);

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
	catch (EngineException & e)
	{
		Log::Error(e, false);
		Log::Error(LOG_MACRO, "can't initialize the UserInterfaceClass");
		return;
	}


	return;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//                             PUBLIC MODIFICATION API
///////////////////////////////////////////////////////////////////////////////////////////////
void UserInterfaceClass::SetStringByKey(
	ID3D11Device* pDevice,
	const std::string & key,
	const std::string & str,
	const POINT & drawAt)
{
	// add/change a string(str) by key and set its position to be drawAt

	// compute the starting position on the screen
	const LONG drawAt_x = (LONG)(-0.5f * (float)windowWidth_) + drawAt.x;
	const LONG drawAt_y = (LONG)(0.5f * (float)windowHeight_) - drawAt.y;

	debugStrings_.CreateSentence(pDevice, font1_, (UINT)str.length(), str, key, { drawAt_x, drawAt_y });
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
	catch (EngineException & e)
	{
		Log::Error(e, false);
		Log::Error(LOG_MACRO, "can't update some text string");
		return;
	}

	return;
}

///////////////////////////////////////////////////////////

void UserInterfaceClass::Render(
	ID3D11DeviceContext* pDeviceContext,
	EntityManager& entityMgr,
	const DirectX::XMMATRIX & WVO)
{
	//
	// this functions renders all the UI elements onto the screen
	//
	// ATTENTION: do 2D rendering only when all 3D rendering is finished;
	// this function renders the engine/game GUI

	// render the debug text data onto the screen
	RenderDebugText(pDeviceContext,
		WVO,                               // world * base_view * ortho
		{ 1, 1, 1 });                      // text color: white

	RenderMainMenuBar(entityMgr);

	
	return;
}






// *********************************************************************************
//
//                                PRIVATE FUNCTIONS
//
// *********************************************************************************


void UserInterfaceClass::RenderMainMenuBar(EntityManager& entityMgr)
{

	static bool show_app_create_entity = false;
	
	if (show_app_create_entity)
	{
		static EntityCreationWindow wnd;
		wnd.ShowWindowToCreateEntity(&show_app_create_entity, entityMgr);
	}

	// create a window called "Main menu bar" with a main menu 
	static bool my_tool_active = true;
	ImGui::SetNextWindowPos(ImVec2(1300, 0), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 600), ImGuiCond_FirstUseEver);

	ImGui::Begin("Main menu bar", &my_tool_active, ImGuiWindowFlags_MenuBar);

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Open..", "Ctrl+O")) 
			{
				entityMgr.Deserialize("entity_mgr_serialized_data.bin");
			}
			else if (ImGui::MenuItem("Save", "Ctrl+S")) 
			{ 
				entityMgr.Serialize("entity_mgr_serialized_data.bin");
			}
			else if (ImGui::MenuItem("Close", "Ctrl+W")) 
			{ 
				my_tool_active = false; 
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Create"))
		{
			ImGui::MenuItem("Entity", NULL, &show_app_create_entity);

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	ImGui::End();
	ImGui::Render();
}



void UserInterfaceClass::PrepareTextForDebugStringsToInit(
	const UINT videoCardMemory,
	const std::string& videoCardName,
	_Inout_ std::vector<std::string>& initStrArr)
{
	ASSERT_NOT_ZERO(videoCardName.size(), "the input str with video card name is empty");
	ASSERT_NOT_ZERO(videoCardMemory, "the input value of the video card memory == 0");

	// prepare initial data for debug text strings
	initStrArr =
	{
		// setup the video card info string and video card memory string
		"Video Card: " + videoCardName,
		"Video Memory: " + std::to_string(videoCardMemory) + "MB",

		"Fps: 0",
		"Frame time: 0 (ms)",
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
		"frame_time",
		"x_pos", "y_pos", "z_pos",  // position
		"x_rot", "y_rot", "z_rot",  // rotation
		"models_drawn",
		"cells_drawn",              // the number of terrain cells which were rendered
		"cells_culled",
		"vertices_drawn",
		"triangles_drawn",
	};

	// check if we prepared proper number of text IDs
	assert((numOfStrings == textIDs.size()) && "you have to prepare the proper number of text IDs");
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
	// prepare an array with positions data for the text strings placing onto the screen

	assert(gapBetweenStrings > 0);
	assert(fontHeight > 0);
	assert(windowWidth > 0);
	assert(windowHeight > 0);
	assert(positionsCount > 0);

	const UINT strideY = (fontHeight / 2) + gapBetweenStrings;
	POINT drawAt;

	drawAtPositionsArr.resize(positionsCount);

	// compute the starting position on the screen
	drawAt.x = (windowWidth / -2) + startDrawAt.x;
	drawAt.y = (windowHeight / 2) - startDrawAt.y;

	for (POINT& drawAtPos : drawAtPositionsArr)
	{
		drawAtPos = drawAt;
		drawAt.y -= strideY;    // the following string will be rendered by strideY pixels below
	}
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
}

///////////////////////////////////////////////////////////

void UserInterfaceClass::UpdateDebugStrings(
	ID3D11DeviceContext* pDeviceContext,
	const SystemState & systemState)
{
	//
	// update the debug strings to render it onto the screen
	//

	// prepare text IDs (by these ids we fill find strings and update it with new data)
	const std::vector<std::string> textIDsToUpdate =
	{
		"fps",
		"frame_time",
		"x_pos", "y_pos", "z_pos",  // position
		"x_rot", "y_rot", "z_rot",  // rotation
		"models_drawn",             // the number of rendered models onto the screen
		"cells_drawn",              // the number of terrain cells which were rendered
		"cells_culled",
		"vertices_drawn",
		"triangles_drawn",
	};

	// prepare debug text content
	const std::vector<std::string> debugTextArr =
	{
		// fps / frame time data
		{ "Fps: " + std::to_string(systemState.fps) },
		{ "Frame time: " + std::to_string(systemState.frameTime) + " (ms)" },
		
		// text about current position
		{ "X: " + std::to_string(systemState.editorCameraPos.x) },
		{ "Y: " + std::to_string(systemState.editorCameraPos.y) },
		{ "Z: " + std::to_string(systemState.editorCameraPos.z) },

		// text about current rotation
		{ "rX (pich): " + std::to_string(systemState.editorCameraDir.x) },
		{ "rY (yaw): " + std::to_string(systemState.editorCameraDir.y) },
		{ "rZ (roll): " + std::to_string(systemState.editorCameraDir.z) },

		// render counts data
		{ "Models drawn: " + std::to_string(systemState.renderedModelsCount) },
		{ "Cells drawn: " + std::to_string(systemState.cellsDrawn) },
		{ "Cells culled: " + std::to_string(systemState.cellsCulled) },
		{ "Vertices drawn: " + std::to_string(systemState.renderedVerticesCount) },
		{ "Triangles drawn: " + std::to_string(systemState.renderedVerticesCount / 3) },
	};

	ASSERT_TRUE(textIDsToUpdate.size() == debugTextArr.size(), "not equal count of keys and debug strings");

	debugStrings_.Update(
		pDeviceContext,
		font1_,
		textIDsToUpdate,
		debugTextArr);

	return;
}



///////////////////////////////////////////////////////////////////////////////////////////////
//                              PRIVATE RENDERING API
///////////////////////////////////////////////////////////////////////////////////////////////

void UserInterfaceClass::RenderDebugText(ID3D11DeviceContext* pDeviceContext, 
	const DirectX::XMMATRIX & WVO,
	const DirectX::XMFLOAT3 & textColor)
{
	// THIS FUNCTION renders all the debug text strings onto the screen

	debugStrings_.Render(pDeviceContext,
			&fontShader_,
			font1_.GetTextureResourceViewAddress(),   // ppFontText
			WVO, 
			textColor);
}