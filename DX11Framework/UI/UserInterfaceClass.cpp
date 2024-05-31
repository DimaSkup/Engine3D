////////////////////////////////////////////////////////////////////////////////////////////
// Filename:     UserInterfaceclass.cpp
// Description:  a functional for initialization, 
//               updating and rendering of the UI
// 
// Created:      25.05.23
////////////////////////////////////////////////////////////////////////////////////////////
#include "UserInterfaceClass.h"

#include "imgui.h"

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






////////////////////////////////////////////////////////////////////////////////////////////
//
//                                PRIVATE FUNCTIONS
//
////////////////////////////////////////////////////////////////////////////////////////////

void SetupTransformParamsOfEntity(
	const EntityID& entityID,
	EntityManager& entityMgr)
{
	bool valueChanged = false;
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 direction;
	DirectX::XMFLOAT3 scale;
	Transform* pTransformComponent = static_cast<Transform*>(entityMgr.GetComponent("Transform"));
	pTransformComponent->GetDataOfEntity(entityID, position, direction, scale);

	// setup values
	valueChanged |= ImGui::DragFloat3("position", &position.x);
	valueChanged |= ImGui::DragFloat3("direction", &direction.x);
	valueChanged |= ImGui::DragFloat3("scale", &scale.x);

	// if position/direction/scale has changed we write updated values back into the component
	if (valueChanged)
		entityMgr.GetTransformSystem().SetWorld("temp_entity", position, direction, scale, *pTransformComponent);
}



void SetupAddedComponents(
	const std::set<ComponentID>& selectedComponents, 
	EntityManager& entityMgr)
{
	if (selectedComponents.size() == 0) return;

	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::SeparatorText("Added Components Setup");
	if (selectedComponents.find("Transform") != selectedComponents.end())
	{
		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_None))
		{
			// if the entity doesn't have a component yet we add it
			if (!entityMgr.CheckEntityHasComponent("temp_entity", "Transform"))
				entityMgr.AddTransformComponent("temp_entity");
			
			SetupTransformParamsOfEntity("temp_entity", entityMgr);
		}
	}
}

std::set<ComponentID> ShowAddComponentsSelectableMenu(EntityManager& entityMgr)
{
	// show a selectable menu for adding components to the entity
	// return: IDs set of added components

	// create a selectable menu for adding components to the entity
	std::set<ComponentID> componentsIDs = entityMgr.GetAllComponentsIDs();
	static std::vector<bool> componentSelection(componentsIDs.size(), false);

	// show a selectable menu for adding a component to the entity
	if (ImGui::TreeNode("Add component"))
	{
		UINT data_idx = 0;
		// show each component ID as selectable option
		for (const ComponentID& componentID : componentsIDs)
		{
			bool isSelected = componentSelection[data_idx];
			ImGui::Selectable(componentID.c_str(), &isSelected);
			componentSelection[data_idx] = isSelected;
			++data_idx;
		}
		ImGui::TreePop();
	}

	std::set<ComponentID> selectedComponentsIDs;
	UINT data_idx = 0;
	// show each component ID as selectable option
	for (const ComponentID& componentID : componentsIDs)
	{
		if (componentSelection[data_idx++])
			selectedComponentsIDs.insert(componentID);
	}

	return selectedComponentsIDs;
}


void ShowWindowCreateEntity(bool* pOpen, EntityManager& entityMgr)
{
	// after choosing "Create->Entity" in the main menu bar we get there;
	// here we show to user a window for creation and setup of new entity;


	// setup and show a modal window for entity creation
	ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
	ImGui::OpenPopup("CreateEntity");

	if (ImGui::BeginPopupModal("CreateEntity", NULL, ImGuiWindowFlags_MenuBar))
	{
		ImGui::Text("Here we create and setup a new entity");  // description text

		// create a temporal entity so we will be able to setup entity during creation;
		// if we cancel creation -- this temporal entity will be deleted from the entity manager;
		if (!entityMgr.CheckEntityExist("temp_entity"))
			entityMgr.CreateEntities({ "temp_entity" });

		// input field for entity ID
		static char entityID[256]{ "\0" };
		ImGui::InputText("entity ID", entityID, IM_ARRAYSIZE(entityID));

		// show menu for adding components and get a set of IDs of chosen components
		std::set<ComponentID> selectedComponentsIDs = ShowAddComponentsSelectableMenu(entityMgr);
		
		SetupAddedComponents(selectedComponentsIDs, entityMgr);

		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();

		// if we pressed the "Create" button we create a new entity with ID from the input field
		if (ImGui::Button("Create"))
		{
			entityMgr.CreateEntities({ entityID });



			//for (UINT idx = 0; idx < componentSelection.size(); ++idx)
			//	componentSelection[idx] = false;

			memset(entityID, '\0', IM_ARRAYSIZE(entityID));   // reset ID string
			ImGui::CloseCurrentPopup();
			*pOpen = false;
		}

		if (ImGui::Button("Close"))
		{
			//for (UINT idx = 0; idx < componentSelection.size(); ++idx)
			//	componentSelection[idx] = false;

			memset(entityID, '\0', IM_ARRAYSIZE(entityID));   // reset ID string
			ImGui::CloseCurrentPopup();
			*pOpen = false;
		}
		ImGui::EndPopup();
	}

}


void UserInterfaceClass::RenderMainMenuBar(EntityManager& entityMgr)
{

	static bool show_app_create_entity = false;

	if (show_app_create_entity) ShowWindowCreateEntity(&show_app_create_entity, entityMgr);

	//if (show_app_create_entity) ShowAppCreateEntity(&show_app_create_entity);


	// create a window called "My first Tool" with a menu bar
	static bool my_tool_active = true;
	ImGui::Begin("My First Tool", &my_tool_active, ImGuiWindowFlags_MenuBar);
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Open..", "Ctrl+O")) { Log::Print(LOG_MACRO, "OPEN"); }
			if (ImGui::MenuItem("Save", "Ctrl+S")) { Log::Print(LOG_MACRO, "Save"); }
			if (ImGui::MenuItem("Close", "Ctrl+W")) { my_tool_active = false; }

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Create"))
		{
			ImGui::MenuItem("Entity", NULL, &show_app_create_entity);

			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	// Edit a color stored as 4 floats
	static float my_color[4];
	ImGui::ColorEdit4("Color", my_color);

	// generate samples and plot them
	float samples[100];
	for (int n = 0; n < 100; ++n)
		samples[n] = sinf(n * 0.2f + ImGui::GetTime() * 1.5f);
	ImGui::PlotLines("Samples", samples, 100);

	// display contect in a scrolling region
	ImGui::TextColored(ImVec4(1, 1, 0, 1), "Important Stuff");
	ImGui::BeginChild("Scrolling");

	for (int n = 0; n < 50; ++n)
		ImGui::Text("%04d: Some text", n);
	ImGui::EndChild();
	ImGui::End();

	// Rendering ImGui
	ImGui::Render();
}



void UserInterfaceClass::PrepareTextForDebugStringsToInit(
	const UINT videoCardMemory,
	const std::string & videoCardName,
	_Inout_ std::vector<std::string> & initStrArr)
{
	ASSERT_NOT_ZERO(videoCardName.size(), "the input str with video card name is empty");
	ASSERT_NOT_ZERO(videoCardMemory, "the input value of the video card memory == 0");

	// setup the video card info string and video card memory string
	const std::string videoStringData{ "Video Card: " + videoCardName };
	const std::string memoryStringData{ "Video Memory: " + std::to_string(videoCardMemory) + "MB" };
	
	// prepare initial data for debug text strings
	initStrArr =
	{
		videoStringData,
		memoryStringData,
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

	const UINT strideY = (fontHeight >> 1) + gapBetweenStrings;
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

	

	return;

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
	const SystemState & systemState)
{
	//
	// update the debug string to render it onto the screen
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

	// prepare text prefixes to update
	const std::vector<std::string> prefixes =
	{
		"Fps: ",
		"Frame time: ",
		"X: ", "Y: ", "Z: ",                         // position
		"rX (pich): ", "rY (yaw): ", "rZ (roll): ",  // rotation
		"Models drawn: ",
		"Cells drawn: ",
		"Cells culled: ",
		"Vertices drawn: ",
		"Triangles drawn: ",
	};

	// convert into the string format all the data to update 
	const std::vector<std::string> dataToUpdate =
	{
		// fps / frame time data
		std::to_string(systemState.fps),
		std::to_string(systemState.frameTime) + " (ms)",

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


	const size_t numOfStrToUpdate = textIDsToUpdate.size();

	assert(prefixes.size() == numOfStrToUpdate);
	assert(dataToUpdate.size() == numOfStrToUpdate);


	// prepare new text content
	std::vector<std::string> textContentsToUpdate(numOfStrToUpdate);
	
	for (size_t idx = 0; idx < numOfStrToUpdate; ++idx)
	{
		textContentsToUpdate[idx] = prefixes[idx] + dataToUpdate[idx];
	}

	debugStrings_.Update(pDeviceContext,
		font1_,
		textIDsToUpdate,
		textContentsToUpdate);

	return;
}



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