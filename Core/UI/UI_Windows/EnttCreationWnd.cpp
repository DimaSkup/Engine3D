#include "EnttCreationWnd.h"
#include "../GameObjects/TextureManager.h"
#include "../GameObjects/RenderingShaderHelperTypes.h"
#include "../GameObjects/ModelsCreator.h"

// IMGUI STUFF
#include "imgui.h"
#include "imgui_stdlib.h"

void EnttCreationWnd::ShowWndToCreateEntt(
	bool* pOpen,
	EntityManager& entityMgr)
{
	// after choosing "Create->Entity" in the main menu bar we get here;
	// we show to the user a window for creation and setup of a new entity;
	// 
	// param pOpen - defines either the window must be opened for the next frame or not

	// setup and show a modal window for entity creation
	ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
	ImGui::OpenPopup("CreateEntity");

	if (ImGui::BeginPopupModal("CreateEntity", NULL, ImGuiWindowFlags_MenuBar))
	{
		// if we open the window for the first time or reset it
		// we allocate memory for the window states params
		try
		{
			if (pWndStates_ == nullptr)
				pWndStates_ = new WndStates();
		}
		catch (const std::bad_alloc& e)
		{
			Log::Error(LOG_MACRO, e.what());
			THROW_ERROR("can't allocate memory for the window states container obj");
		}

		// ------------------------------------------------ //

		ImGui::Text("Here we create and setup a new entity");

		// show an error msg (if there is some)
		if (!pWndStates_->errorMsg.empty()) 
			ImGui::TextColored(ImVec4(1, 0, 0, 1), pWndStates_->errorMsg.c_str());

		// input field for entity name
		if (ImGui::InputText("entity name", &(pWndStates_->entityName)))
			if (!pWndStates_->entityName.empty())
				pWndStates_->errorMsg = "";

#if 0
		if (pWndStates_->entityName.empty())
			pWndStates_->errorMsg = "entity name cannot be empty";
#endif

		// show menu for components selection and its setup fields
		ShowSelectableMenuToAddComponents(entityMgr);
		ShowSetupFieldsOfAddedComponents();

		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();

		// show "Create", "Close", "Reset", etc. buttons
		ShowButtonsOfWnd(entityMgr, pOpen);

		ImGui::EndPopup();
	}
}

///////////////////////////////////////////////////////////

void EnttCreationWnd::ShowSelectableMenuToAddComponents(
	EntityManager& entityMgr)
{
	// show a selectable menu for adding components to the entity
	// return: IDs set of added components

	const auto& compTypeToName = entityMgr.GetPairsOfComponentTypeToName();

	// show a selectable menu for adding components to the entity
	if (ImGui::CollapsingHeader("Add component", ImGuiTreeNodeFlags_None))
	{
		for (const auto& it : compTypeToName)
		{
			const ComponentType type = it.first;
			const ComponentID& componentID = it.second;
			bool isSelected = pWndStates_->addedComponents.contains(type);

			ImGui::Selectable(componentID.c_str(), &isSelected);

			// if such component is chosen we store its type
			if (isSelected)
				pWndStates_->addedComponents.insert(type);
			else
				pWndStates_->addedComponents.erase(type);
		}
	}
}

///////////////////////////////////////////////////////////

void EnttCreationWnd::ShowSetupFieldsOfAddedComponents()
{
	// for each added component we show responsible setup fields

	ImGui::SeparatorText("Added Components Setup");

	for (const ComponentType compType : pWndStates_->addedComponents)
	{
		switch (compType)
		{
			case ComponentType::TransformComponent:
			{
				if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_None))
				{
					ShowFieldsToSetupTransformParams(pWndStates_->transform);
				}

				break;
			}
			case ComponentType::MoveComponent:
			{
				if (ImGui::CollapsingHeader("Movement", ImGuiTreeNodeFlags_None))
				{
					ShowFieldsToSetupMovementParams(pWndStates_->movement);
				}
				break;
			}
			case ComponentType::MeshComp:
			{
				if (ImGui::CollapsingHeader("MeshComponent", ImGuiTreeNodeFlags_None))
				{
					ShowSelectableMenuOfMeshes(pWndStates_->chosenMesh);
					ShowTexturesMenuForMesh(pWndStates_->chosenTexName);
				}

				break;
			}
			case ComponentType::RenderedComponent:
			{
				if (ImGui::CollapsingHeader("Rendered", ImGuiTreeNodeFlags_None))
					ShowSelectableListOfRenderingShaders(pWndStates_->selectedRenderingShader);
	
				break;
			}
			default:
			{
				THROW_ERROR("Unknown component type: " + std::to_string(compType));
			}
		}
	}
}

///////////////////////////////////////////////////////////

void EnttCreationWnd::ShowFieldsToSetupTransformParams(
	ECS::TransformRawData& data)
{
	// show input/grag fields for setup the transform data

	ImGui::DragFloat3("position",  &data.pos.x);
	ImGui::DragFloat3("direction", &data.dir.x);
	ImGui::DragFloat ("scale",     &data.uniScale);
}

///////////////////////////////////////////////////////////

void EnttCreationWnd::ShowFieldsToSetupMovementParams(ECS::MovementRawData& data)
{
	// show input/grag fields for setup the movement data

	ImGui::DragFloat3("translation", &data.trans.x);
	ImGui::SameLine();
	ImGui::Button("?");
	ImGui::SetItemTooltip("translation respectively by x,y,z");

	ImGui::DragFloat3("rotation", &data.rot.x);
	ImGui::SameLine();
	ImGui::Button("?");
	ImGui::SetItemTooltip("rotation angles in radians: pitch, yaw, roll (around X,Y,Z-axis respectively)");

	ImGui::DragFloat("scale change", &data.uniScale);
	ImGui::SameLine();
	ImGui::Button("?");
	ImGui::SetItemTooltip("each update cycle a scale of entity will be multiplied by this uniform scale value");
}

///////////////////////////////////////////////////////////

void EnttCreationWnd::ShowSelectableMenuOfMeshes(MeshName& chosenMesh)
{
	// show a selectable menu for adding meshes to the entity
	// in-out: IDs set of the chosen meshes

	if (ImGui::CollapsingHeader("Add mesh", ImGuiTreeNodeFlags_None))
	{
		// get IDs of all the basic meshes + IDs of all the currently imported meshes
		std::set<MeshName> meshesNames;

		// names of basic meshes
		for (const auto& it : Mesh::basicTypeToName)
			meshesNames.emplace(it.second);

		// names of meshes which are in the mesh storage
		for (const MeshName& meshID : MeshStorage::Get()->GetAllMeshesNames())
			meshesNames.emplace(meshID);

		for (const MeshName& meshName : meshesNames)
		{
			bool isSelected = (chosenMesh == meshName);
			ImGui::Selectable(meshName.c_str(), &isSelected);

			// if such mesh is chosen we store its name
			if (isSelected)
				chosenMesh = meshName;
		}
	}
}

///////////////////////////////////////////////////////////

void EnttCreationWnd::ShowTexturesMenuForMesh(TexName& chosenTex)
{
	// show a menu with textures images to choose some texture for a mesh;
	// if we choose some texture we make a border around its image;
	// 
	// in-out: an ID of the chosen texture;


	if (ImGui::CollapsingHeader("Set texture for mesh", ImGuiTreeNodeFlags_None))
	{
		TextureManager* pTextureManager = TextureManager::Get();
		std::vector<std::string> texturesPaths;

		pTextureManager->GetAllTexturesPathsWithinDirectory("data/textures/", texturesPaths);

		for (const TexName& path : texturesPaths)
		{
			bool isSelected = (pWndStates_->chosenTexName == path);
			ImGui::Selectable(path.c_str(), &isSelected);

			// if such mesh is chosen we store its name
			if (isSelected)
				pWndStates_->chosenTexName = path;
		}

#if 0
		std::vector<TexID> texturesIDs;
		std::vector<ID3D11ShaderResourceView*> texturesSRVs;

		pTextureManager->GetAllTexturesIDs(texturesIDs);
		pTextureManager->GetAllTexturesSRVs(texturesSRVs);
		ASSERT_TRUE(std::ssize(texturesIDs) == std::ssize(texturesSRVs), "count of textures IDs and SRVs must be equal");


		// setup the table params
		const size_t columnsCount = 3;
		const size_t rowsCount = texturesIDs.size() / columnsCount;
		ImVec2 imgDimensions{ 200, 200 };

		if (ImGui::BeginTable("show textures for mesh", columnsCount, ImGuiTableFlags_RowBg))
		{
			UINT data_idx = 0;
			for (size_t row = 0; row < rowsCount; ++row)
			{

				ImGui::TableNextRow();
				for (size_t column = 0; column < columnsCount; ++column)
				{
					size_t id = row * columnsCount + column;

					ImGui::TableSetColumnIndex((int)column);
					//ImVec2 imgDimensions = ImGui::GetContentRegionAvail();
			
					const TexID& textureID = texturesIDs[data_idx];
					const ID3D11ShaderResourceView* pTextureSRV = texturesSRVs[data_idx];

					// by default we remove the border around each texture image quad
					ImGui::PushID((int)id);
					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));

					//if (ImGui::InvisibleButton(textureID.c_str(), imgDimensions,))
					//if (ImGui::ImageButton((void*)pTextureSRV, imgDimensions))
					if (ImGui::Button(textureID.c_str()))
						chosenTexture = textureID;

					// make a border around chosen texture
					if (chosenTexture == textureID)
					{
						ImU32 chosenCellBgColor = ImGui::GetColorU32(ImVec4(1, 1, 0, 1));
						ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, chosenCellBgColor);
					}

					ImGui::PopStyleVar();
					ImGui::PopID();
					++data_idx;
				}
			}

			ImGui::EndTable();
		}
#endif
	}
}

///////////////////////////////////////////////////////////

void EnttCreationWnd::ShowSelectableListOfRenderingShaders(
	RENDERING_SHADERS& selectedRenderingShader)
{
	// show a selectable menu for choosing a rendering shader type
	// in-out: a type of the selected rendering shader

	for (const auto& it : g_ShaderTypeToShaderName)
	{
		if (ImGui::Selectable(it.second.c_str(), selectedRenderingShader == it.first))
			selectedRenderingShader = it.first;
	}
}

///////////////////////////////////////////////////////////

void EnttCreationWnd::ShowButtonsOfWnd(EntityManager& entityMgr, bool* pOpen)
{
	// show some buttons of the creation window;
	// input: pOpen - defines either the window must be opened for the next frame or not

	// create new entity and setup it with chosen params
	if (ImGui::Button("Create"))
	{
		const EntityID enttID = entityMgr.CreateEntity();
		AddChosenComponentsToEntity(entityMgr, enttID);

		// reset+close the creation window
		pWndStates_->Reset();
		_DELETE(pWndStates_);
		ImGui::CloseCurrentPopup();
		*pOpen = false;
	}

	// close the creation window (its state are saved between calls)
	if (ImGui::Button("Close"))
	{
		ImGui::CloseCurrentPopup();
		*pOpen = false;
	}

	// reset the fields state of the creation window
	if (ImGui::Button("Reset"))
	{
		pWndStates_->Reset();
	}
}

///////////////////////////////////////////////////////////

void EnttCreationWnd::AddChosenComponentsToEntity(
	EntityManager& mgr,
	const EntityID id)
{
	// go through chosen components types and add a responsible component to the entity

	WndStates& wndStates = *pWndStates_;

	for (const ComponentType compType : pWndStates_->addedComponents)
	{
		switch (compType)
		{
			case ComponentType::TransformComponent:
			{
				const DirectX::XMFLOAT3& angles = pWndStates_->transform.dir;
				const DirectX::XMVECTOR directionQuat = DirectX::XMQuaternionRotationRollPitchYaw(angles.x, angles.y, angles.z);

				mgr.AddTransformComponent(
					id,
					pWndStates_->transform.pos,
					directionQuat,
					pWndStates_->transform.uniScale);
				
				break;
			}
			case ComponentType::MoveComponent:
			{
				// compute a rotation quaternion by chosen rotation angles
				const DirectX::XMFLOAT3& angles = pWndStates_->movement.rot;
				const DirectX::XMVECTOR rotQuat = DirectX::XMQuaternionRotationRollPitchYaw(angles.x, angles.y, angles.z);

				mgr.AddMoveComponent(
					id,
					pWndStates_->movement.trans,
					rotQuat,
					pWndStates_->movement.uniScale);

				break;
			}
			case ComponentType::MeshComp:
			{
				MeshStorage* pMeshStorage = MeshStorage::Get();
				std::vector<MeshID> meshesIDsArr;

				// try to get meshes IDs by its names
				const bool foundMesh = pMeshStorage->GetMeshesIDsByNames(
					{ wndStates.chosenMesh },
					meshesIDsArr);

				if (!foundMesh)
				{
					// if we got here we want to add one BASIC mesh which 
					// isn't stored in the MeshStorage yet; so firstly create it

					Mesh::MeshType type;

					// define a type of the chosen basic mesh
					for (const auto& it : Mesh::basicTypeToName)
					{
						if (it.second == wndStates.chosenMesh)
						{
							type = it.first;
							break;               // go out from the for-loop
						}
					}

					ModelsCreator modelCreator;

					// create a mesh and store its ID
					meshesIDsArr.clear();
					meshesIDsArr.push_back(modelCreator.Create(pDevice_, type));
				}
				
				TextureManager* pTexMgr = TextureManager::Get();

				// setup meshes according to the chosen params
				for (const MeshID& meshID : meshesIDsArr)
				{
					TexName chosenTex = pWndStates_->chosenTexName;
					TextureClass* pTexture = nullptr;

					// if we didn't choose any texture we get a default one
					if (chosenTex.empty())
						chosenTex = "unloaded";

					pMeshStorage->SetTextureForMeshByID(
						meshID, 
						aiTextureType_DIFFUSE,               // texture type
						pTexMgr->GetIDByName(chosenTex));    // ID of texture
				}

				// after all we add a Mesh component to the entity
				mgr.AddMeshComponent({ id }, meshesIDsArr);

				break;
			}
			case ComponentType::RenderedComponent:
			{
				mgr.AddRenderingComponent(
					{ id },
					static_cast<ECS::RENDERING_SHADERS>(pWndStates_->selectedRenderingShader),
					D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

				break;
			}
		}
	}
}
