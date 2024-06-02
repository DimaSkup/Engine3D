////////////////////////////////////////////////////////////////////////////////////////////
// Filename:     UserInterfaceclass.cpp
// Description:  a functional for initialization, 
//               updating and rendering of the UI
// 
// Created:      25.05.23
////////////////////////////////////////////////////////////////////////////////////////////
#include "UserInterfaceClass.h"
#include "../GameObjects/MeshStorage.h"

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

struct StatesOfWindowToCreateEntity
{
	StatesOfWindowToCreateEntity()
	{
		componentsData.insert({ "Transform",     std::make_unique<Transform::ComponentData>() });
		componentsData.insert({ "Movement",      std::make_unique<Movement::ComponentData>() });
		componentsData.insert({ "MeshComponent", std::make_unique<MeshComponent::ComponentData>() });
		componentsData.insert({ "Rendered",      std::make_unique<Rendered::ComponentData>() });

		// init selection params of the selectatable field for adding components
		componentsSelection.resize(componentsData.size(), false);
	}

	std::string errorMsg;
	std::string entityID;

	std::map<ComponentID, std::unique_ptr<BaseComponent::BaseComponentData>> componentsData;
	std::vector<bool> componentsSelection;   // is used in the selectable menu for adding components
	std::set<MeshID> chosenMeshesIDs;
	Mesh::RENDERING_SHADERS renderingShaderType = Mesh::RENDERING_SHADERS::COLOR_SHADER;

	BaseComponent::BaseComponentData& GetData(const ComponentID& componentID)
	{
		try
		{
			return *componentsData.at(componentID).get();
		}
		catch (const std::out_of_range& e)
		{
			Log::Error(LOG_MACRO, e.what());
			THROW_ERROR("there is no data of component by ID: " + componentID);
		}
		
	}

	void Reset()
	{
		errorMsg.clear();
		entityID.clear();
		componentsData.clear();
		componentsSelection.clear();
		//ZeroMemory(&transformData, sizeof(transformData));
		//ZeroMemory(&movementData, sizeof(movementData));
	}
};

void ShowFieldsToSetupTransformParams(Transform::ComponentData& data)
{
	// show input/grag fields for setup the transform data

	ImGui::DragFloat3("position", &data.position_.x);
	ImGui::DragFloat3("direction", &data.direction_.x);
	ImGui::DragFloat3("scale", &data.scale_.x);
}

void ShowFieldsToSetupMovementParams(Movement::ComponentData& data)
{
	// show input/grag fields for setup the movement data

	ImGui::DragFloat3("translation", &data.translation_.x);
	ImGui::DragFloat3("rotation", &data.rotationQuat_.x);
	ImGui::DragFloat3("scale change", &data.scaleChange_.x);
}

void ShowMeshesListToSetupMeshComponentParams(
	std::set<MeshID>& chosenMeshesIDs,
	const std::vector<MeshID>& meshesIDs)
{
	// show a selectable menu for adding meshes to the entity

	if (ImGui::CollapsingHeader("Add mesh", ImGuiTreeNodeFlags_None))
	{
		for (const MeshID& meshID : meshesIDs)
		{
			bool isSelected = chosenMeshesIDs.contains(meshID);
			ImGui::Selectable(meshID.c_str(), &isSelected);

			// if such component is chosen we store its ID
			if (isSelected)
				chosenMeshesIDs.insert(meshID);
			else
			{
				chosenMeshesIDs.erase(meshID);
			}
		}
	}
}

void ShowListOfRenderingShaderTypes(Mesh::RENDERING_SHADERS& renderingShaderType)
{
	// show a selectable menu for choosing a rendering shader type
	static Mesh::RENDERING_SHADERS selected = Mesh::RENDERING_SHADERS::COLOR_SHADER;

	std::map<Mesh::RENDERING_SHADERS, std::string> typeToName =
	{
		{ Mesh::RENDERING_SHADERS::COLOR_SHADER, "ColorShader" },
		{ Mesh::RENDERING_SHADERS::TEXTURE_SHADER, "TextureShader" },
		{ Mesh::RENDERING_SHADERS::LIGHT_SHADER, "LightShader" },
	};

	for (const auto& it : typeToName)
	{
		if (ImGui::Selectable(it.second.c_str(), selected == it.first))
			selected = it.first;
	}

	renderingShaderType = selected;
}



void SetupAddedComponents(
	const std::set<ComponentID>& selectedComponents, 
	StatesOfWindowToCreateEntity& wndStates)
{
	// for each added component we show setup fields

	if (selectedComponents.size() == 0) return;

	
	ImGui::SeparatorText("Added Components Setup");
	const auto& iterEnd = selectedComponents.end();

	if (selectedComponents.find("Transform") != iterEnd)
	{
		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_None))
		{
			Transform::ComponentData& data = static_cast<Transform::ComponentData&>(wndStates.GetData("Transform"));
			ShowFieldsToSetupTransformParams(data);
		}
	}

	if (selectedComponents.find("Movement") != iterEnd)
	{
		if (ImGui::CollapsingHeader("Movement", ImGuiTreeNodeFlags_None))
		{
			Movement::ComponentData& data = static_cast<Movement::ComponentData&>(wndStates.GetData("Transform"));
			ShowFieldsToSetupMovementParams(data);
		}
	}

	if (selectedComponents.find("MeshComponent") != iterEnd)
	{
		if (ImGui::CollapsingHeader("MeshComponent", ImGuiTreeNodeFlags_None))
		{
			//MeshComponent::ComponentData& data = static_cast<MeshComponent::ComponentData&>(*wndStates.GetData("MeshComponent"));
			ShowMeshesListToSetupMeshComponentParams(
				wndStates.chosenMeshesIDs, 
				MeshStorage::Get()->GetMeshesIDs());
		}
	}

	if (selectedComponents.find("Rendered") != iterEnd)
	{
		if (ImGui::CollapsingHeader("Rendered", ImGuiTreeNodeFlags_None))
		{
			ShowListOfRenderingShaderTypes(wndStates.renderingShaderType);
		}
	}
}

///////////////////////////////////////////////////////////

std::set<ComponentID> ShowAddComponentsSelectableMenu(
	EntityManager& entityMgr,
	StatesOfWindowToCreateEntity& wndStates)
{
	// show a selectable menu for adding components to the entity
	// return: IDs set of added components

	std::set<ComponentID> componentsIDs = entityMgr.GetAllComponentsIDs();
	std::set<ComponentID> selectedComponentsIDs;
	std::vector<bool> componentSelection = wndStates.componentsSelection;
	
	// show a selectable menu for adding components to the entity
	if (ImGui::CollapsingHeader("Add component", ImGuiTreeNodeFlags_None))
	{
		UINT data_idx = 0;
		for (const ComponentID& componentID : componentsIDs)
		{
			bool isSelected = wndStates.componentsSelection[data_idx];
			ImGui::Selectable(componentID.c_str(), &isSelected);
			wndStates.componentsSelection[data_idx] = isSelected;
			++data_idx;
			
			// if such component is chosen we store its ID
			if (isSelected)
				selectedComponentsIDs.insert(componentID);
		}

		//ImGui::TreePop();
	}

	return selectedComponentsIDs;
}

void ShowButtonsForWindowToCreateEntity(
	EntityManager& entityMgr,
	const std::set<ComponentID>& addedComponents,  // set of components added to this entity 
	StatesOfWindowToCreateEntity** ppWndStates,    // current states of the window's fields
	bool* pOpen)                                   // keep window open
{
	StatesOfWindowToCreateEntity* pWndStates = *ppWndStates;

	if (ImGui::Button("Create"))
	{
		if (!pWndStates->entityID.empty())
		{
			// if we didn't manage to create an entity so print a message about it
			if (!entityMgr.CreateEntity(pWndStates->entityID))
			{
				pWndStates->errorMsg = { "there is already entity with such ID: " + pWndStates->entityID };
			}

			// the entity was created so setup it and reset+close the creation window
			else
			{
				const std::string entityID = pWndStates->entityID;

				if (addedComponents.contains("Transform"))
				{
					const Transform::ComponentData& data = static_cast<const Transform::ComponentData&>(pWndStates->GetData("Transform"));
					entityMgr.AddTransformComponent(entityID, data.position_, data.direction_, data.scale_);
				}

#if 0
				if (addedComponents.contains("Movement"))
				{
					const Transform::ComponentData& data = static_cast<const Transform::ComponentData&>(pWndStates->GetData("Transform"));
					entityMgr.AddTransformComponent(entityID, data.position_, data.direction_, data.scale_);
				}
#endif

				if (addedComponents.contains("MeshComponent"))
				{
					std::vector<MeshID> meshesIDsArr(pWndStates->chosenMeshesIDs.begin(), pWndStates->chosenMeshesIDs.end());
					entityMgr.AddMeshComponents({ entityID }, meshesIDsArr);


					if (addedComponents.contains("Rendered"))
					{
						for (const MeshID& meshID : meshesIDsArr)
							MeshStorage::Get()->SetRenderingShaderForMeshByID(meshID, pWndStates->renderingShaderType);

						entityMgr.AddRenderingComponents({ entityID });
					}
				}

				


				pWndStates->Reset();
				_DELETE(*ppWndStates);
				ImGui::CloseCurrentPopup();
				*pOpen = false;
			}
		}
	}

	if (ImGui::Button("Close"))
	{
		ImGui::CloseCurrentPopup();
		*pOpen = false;
	}

	if (ImGui::Button("Reset"))
	{
		pWndStates->Reset();
		_DELETE(*ppWndStates);
	}
}

///////////////////////////////////////////////////////////


void ShowWindowToCreateEntity(bool* pOpen, EntityManager& entityMgr)
{
	// after choosing "Create->Entity" in the main menu bar we get here;
	// we show to the user a window for creation and setup of a new entity;


	// setup and show a modal window for entity creation
	ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
	ImGui::OpenPopup("CreateEntity");

	if (ImGui::BeginPopupModal("CreateEntity", NULL, ImGuiWindowFlags_MenuBar))
	{
		static StatesOfWindowToCreateEntity* pWndStates = nullptr;  // stores all the fields states
		std::set<ComponentID> selectedComponentsIDs;
		
		// if we open the window for the first time or reset it
		// we allocate memory for the window states params
		try
		{
			if (pWndStates == nullptr)
				pWndStates = new StatesOfWindowToCreateEntity();
		}
		catch (const std::bad_alloc& e)
		{
			Log::Error(LOG_MACRO, e.what());
			THROW_ERROR("can't allocate memory for the window states container obj");
		}

		// ------------------------------------------------ //

		ImGui::Text("Here we create and setup a new entity");   

	

		// show an error msg
		if (!pWndStates->errorMsg.empty()) ImGui::TextColored(ImVec4(1, 0, 0, 1), pWndStates->errorMsg.c_str());

		// input field for entity ID
		if (ImGui::InputText("entity ID", &(pWndStates->entityID)))
			if (!pWndStates->entityID.empty()) 
				pWndStates->errorMsg = "";
		
		if (pWndStates->entityID.empty()) 
			pWndStates->errorMsg = "entity ID cannot be empty";
		

		// show menu for adding components and get a set of IDs of chosen components
		selectedComponentsIDs = ShowAddComponentsSelectableMenu(entityMgr, *pWndStates);

		// setup data of each chosen components
		SetupAddedComponents(selectedComponentsIDs, *pWndStates);  	       

		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();

		ShowButtonsForWindowToCreateEntity(entityMgr, selectedComponentsIDs, &pWndStates, pOpen);

		ImGui::EndPopup();
	}

}


void UserInterfaceClass::RenderMainMenuBar(EntityManager& entityMgr)
{

	static bool show_app_create_entity = false;

	if (show_app_create_entity) ShowWindowToCreateEntity(&show_app_create_entity, entityMgr);

	// create a window called "My first Tool" with a menu bar
	static bool my_tool_active = true;
	ImGui::SetNextWindowPos(ImVec2(1300, 0), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 600), ImGuiCond_FirstUseEver);
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
		samples[n] = sinf(n * 0.2f + (float)ImGui::GetTime() * 1.5f);
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
	const std::vector<std::string> debugText =
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

	ASSERT_TRUE(textIDsToUpdate.size() == debugText.size(), "not equal count of keys and debug strings");

	debugStrings_.Update(
		pDeviceContext,
		font1_,
		textIDsToUpdate,
		debugText);

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

	debugStrings_.Render(pDeviceContext,
			&fontShader_,
			font1_.GetTextureResourceViewAddress(),   // ppFontText
			WVO, 
			textColor);
}