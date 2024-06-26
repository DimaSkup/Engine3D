#include "EntityCreationWindow.h"
#include "../GameObjects/TextureManagerClass.h"
#include "../GameObjects/RenderingShaderHelperTypes.h"

// IMGUI STUFF
#include "imgui.h"
#include "imgui_stdlib.h"

void EntityCreationWindow::ShowWindowToCreateEntity(
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
				pWndStates_ = new StatesOfWindowToCreateEntity();
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
		ShowButtonsOfWindow(entityMgr, pOpen);

		ImGui::EndPopup();
	}
}

///////////////////////////////////////////////////////////

void EntityCreationWindow::ShowSelectableMenuToAddComponents(
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

void EntityCreationWindow::ShowSetupFieldsOfAddedComponents()
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
					ShowFieldsToSetupTransformParams(
						pWndStates_->position,
						pWndStates_->direction,
						pWndStates_->scale);
				}

				break;
			}
			case ComponentType::MoveComponent:
			{
				if (ImGui::CollapsingHeader("Movement", ImGuiTreeNodeFlags_None))
				{
					ShowFieldsToSetupMovementParams(
						pWndStates_->translation,
						pWndStates_->rotationAngles,
						pWndStates_->scaleChange);
				}
				break;
			}
			case ComponentType::MeshComp:
			{
				if (ImGui::CollapsingHeader("MeshComponent", ImGuiTreeNodeFlags_None))
				{
					ShowSelectableMenuOfMeshes(pWndStates_->chosenMeshes);
					ShowTexturesMenuForMesh(pWndStates_->chosenTextureID);
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

void EntityCreationWindow::ShowFieldsToSetupTransformParams(
	DirectX::XMFLOAT3& inOutPosition,
	DirectX::XMFLOAT3& inOutDirection,
	DirectX::XMFLOAT3& inOutScale)
{
	// show input/grag fields for setup the transform data

	ImGui::DragFloat3("position", &inOutPosition.x);
	ImGui::DragFloat3("direction", &inOutDirection.x);
	ImGui::DragFloat3("scale", &inOutScale.x);
}

///////////////////////////////////////////////////////////

void EntityCreationWindow::ShowFieldsToSetupMovementParams(
	DirectX::XMFLOAT3& inOutTranslation,
	DirectX::XMFLOAT3& inOutRotationQuat,
	DirectX::XMFLOAT3& inOutScaleChange)
{
	// show input/grag fields for setup the movement data

	ImGui::DragFloat3("translation", &inOutTranslation.x);
	ImGui::SameLine();
	ImGui::Button("?");
	ImGui::SetItemTooltip("translation respectively by x,y,z");

	ImGui::DragFloat3("rotation", &inOutRotationQuat.x);
	ImGui::SameLine();
	ImGui::Button("?");
	ImGui::SetItemTooltip("rotation angles in radians: pitch, yaw, roll (around X,Y,Z-axis respectively)");

	ImGui::DragFloat3("scale change", &inOutScaleChange.x);
	ImGui::SameLine();
	ImGui::Button("?");
	ImGui::SetItemTooltip("each update cycle a scale of entity will be multiplied by these values");
}

///////////////////////////////////////////////////////////

void EntityCreationWindow::ShowSelectableMenuOfMeshes(std::set<MeshName>& chosenMeshes)
{
	// show a selectable menu for adding meshes to the entity
	// in-out: IDs set of the chosen meshes

	if (ImGui::CollapsingHeader("Add mesh", ImGuiTreeNodeFlags_None))
	{
		// get IDs of all the basic meshes + IDs of all the currently imported meshes
		std::set<MeshName> meshesNames;

		for (const auto& it : Mesh::basicTypeToName)
			meshesNames.emplace(it.second);

		for (const MeshName& meshID : MeshStorage::Get()->GetAllMeshesNames())
			meshesNames.emplace(meshID);

		for (const MeshName& meshName : meshesNames)
		{
			bool isSelected = chosenMeshes.contains(meshName);
			ImGui::Selectable(meshName.c_str(), &isSelected);

			// if such mesh is chosen we store its name
			if (isSelected)
				chosenMeshes.emplace(meshName);
			else
				chosenMeshes.erase(meshName);
		}
	}
}

///////////////////////////////////////////////////////////

void EntityCreationWindow::ShowTexturesMenuForMesh(TextureID& chosenTexture)
{
	// show a menu with textures images to choose some texture for a mesh;
	// if we choose some texture we make a border around its image;
	// 
	// in-out: an ID of the chosen texture;

	TextureManagerClass* pTextureManager = TextureManagerClass::Get();
	std::vector<TextureID> texturesIDs;
	std::vector<ID3D11ShaderResourceView*> texturesSRVs;

	pTextureManager->GetAllTexturesIDs(texturesIDs);
	pTextureManager->GetAllTexturesSRVs(texturesSRVs);
	ASSERT_TRUE(std::ssize(texturesIDs) == std::ssize(texturesSRVs), "count of textures IDs and SRVs must be equal");

	if (ImGui::CollapsingHeader("Set texture for mesh", ImGuiTreeNodeFlags_None))
	{
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
			
					const TextureID& textureID = texturesIDs[data_idx];
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
	}
}

///////////////////////////////////////////////////////////

void EntityCreationWindow::ShowSelectableListOfRenderingShaders(
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

void EntityCreationWindow::ShowButtonsOfWindow(EntityManager& entityMgr, bool* pOpen)
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

void EntityCreationWindow::AddChosenComponentsToEntity(
	EntityManager& entityMgr,
	const EntityID enttID)
{
	// go through chosen components types and add a responsible component to the entity

	for (const ComponentType compType : pWndStates_->addedComponents)
	{
		switch (compType)
		{
			case ComponentType::TransformComponent:
			{
				entityMgr.AddTransformComponent(
					enttID,
					pWndStates_->position,
					pWndStates_->direction,
					pWndStates_->scale);
				
				break;
			}
			case ComponentType::MoveComponent:
			{
				// compute a rotation quaternion by chosen rotation angles
				const DirectX::XMFLOAT3 angles = pWndStates_->rotationAngles;
				DirectX::XMFLOAT4 rotQuat;
				DirectX::XMStoreFloat4(&rotQuat, DirectX::XMQuaternionRotationRollPitchYaw(angles.x, angles.y, angles.z));

				entityMgr.AddMoveComponent(
					enttID,
					pWndStates_->translation,
					rotQuat,
					pWndStates_->scaleChange);

				break;
			}
			case ComponentType::MeshComp:
			{
				const std::vector<MeshName> chosenMeshesArr{ pWndStates_->chosenMeshes.begin(), pWndStates_->chosenMeshes.end() };
				MeshStorage* pMeshStorage = MeshStorage::Get();
				std::vector<MeshID> meshesIDsArr;
	
				pMeshStorage->GetMeshesIDsByNames(chosenMeshesArr, meshesIDsArr);
				entityMgr.AddMeshComponent({ enttID }, meshesIDsArr);

				// setup meshes according to chosen params
				for (const MeshID& meshID : meshesIDsArr)
				{
					
					const TextureID textureID = pWndStates_->chosenTextureID;

					// setup texture for the mesh
					if (!textureID.empty())
					{
						TextureClass* pTexture = TextureManagerClass::Get()->GetTextureByKey(textureID);
						pMeshStorage->SetTextureForMeshByID(meshID, aiTextureType_DIFFUSE, pTexture);
					}
				}

				break;
			}
			case ComponentType::RenderedComponent:
			{
				entityMgr.AddRenderingComponent(
					enttID, 
					static_cast<ECS::RENDERING_SHADERS>(pWndStates_->selectedRenderingShader),
					D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

				break;
			}
		}
	}
}
