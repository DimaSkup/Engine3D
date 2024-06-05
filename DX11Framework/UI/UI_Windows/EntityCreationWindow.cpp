#include "EntityCreationWindow.h"
#include "../GameObjects/TextureManagerClass.h"


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

		// show an error msg (if it is)
		if (!pWndStates_->errorMsg.empty()) ImGui::TextColored(ImVec4(1, 0, 0, 1), pWndStates_->errorMsg.c_str());

		// input field for entity ID
		if (ImGui::InputText("entity ID", &(pWndStates_->entityID)))
			if (!pWndStates_->entityID.empty())
				pWndStates_->errorMsg = "";

		if (pWndStates_->entityID.empty())
			pWndStates_->errorMsg = "entity ID cannot be empty";

		ShowSelectableMenuToAddComponents(entityMgr);
		ShowSetupFieldsOfAddedComponents();

		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();

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


	const std::set<ComponentType>& addedComponents = pWndStates_->addedComponents;

	// if we currently haven't chosen any component just go out
	if (addedComponents.size() == 0) return;

	StatesOfWindowToCreateEntity wndStates = *pWndStates_;
	bool statesWereUpdates = false;

	ImGui::SeparatorText("Added Components Setup");

	for (const ComponentType compType : addedComponents)
	{
		switch (compType)
		{
			case ComponentType::TransformComponent:
			{
				if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_None))
				{
					ShowFieldsToSetupTransformParams(
						wndStates.position,
						wndStates.direction,
						wndStates.scale);
				}

				break;
			}
			case ComponentType::MoveComponent:
			{
				if (ImGui::CollapsingHeader("Movement", ImGuiTreeNodeFlags_None))
				{
					ShowFieldsToSetupMovementParams(
						wndStates.translation,
						wndStates.rotationAngles,
						wndStates.scaleChange);
				}
				break;
			}
			case ComponentType::MeshComp:
			{
				if (ImGui::CollapsingHeader("MeshComponent", ImGuiTreeNodeFlags_None))
				{
					ShowSelectableMenuOfMeshes(
						wndStates.chosenMeshes,
						MeshStorage::Get()->GetMeshesIDs());

					ShowTexturesMenuForMesh(wndStates.chosenTextureID);

				}

				break;
			}
			case ComponentType::RenderedComponent:
			{
				if (ImGui::CollapsingHeader("Rendered", ImGuiTreeNodeFlags_None))
					ShowSelectableListOfRenderingShaders(wndStates.selectedRenderingShader);
	
				break;
			}
			default:
			{
				THROW_ERROR("Unknown component type: " + std::to_string(compType));
			}
		}
	}
	
	// if some setup field was updates we write back updated data
	//if (statesWereUpdates)
	*pWndStates_ = wndStates;
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

void EntityCreationWindow::ShowSelectableMenuOfMeshes(
	std::set<MeshID>& chosenMeshesIDs,
	const std::vector<MeshID>& meshesIDs)
{
	// show a selectable menu for adding meshes to the entity
	// in-out: IDs set of the chosen meshes

	if (ImGui::CollapsingHeader("Add mesh", ImGuiTreeNodeFlags_None))
	{
		for (const MeshID& meshID : meshesIDs)
		{
			bool isSelected = chosenMeshesIDs.contains(meshID);
			ImGui::Selectable(meshID.c_str(), &isSelected);

			// if such mesh is chosen we store its ID
			if (isSelected)
				chosenMeshesIDs.insert(meshID);
			else
			{
				chosenMeshesIDs.erase(meshID);
			}
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
	// pOpen - defines either the window must be opened for the next frame or not

	if (ImGui::Button("Create"))
	{
		if (!pWndStates_->entityID.empty())
		{
			// if we didn't manage to create an entity so print a message about it
			if (!entityMgr.CreateEntity(pWndStates_->entityID))
			{
				pWndStates_->errorMsg = { "there is already entity with such ID: " + pWndStates_->entityID };
			}

			// the entity was created so setup it and reset+close the creation window
			else
			{
				AddChosenComponentsToEntity(entityMgr);

				// after successful creation of the entity
				//we reset the window and close it
				pWndStates_->Reset();
				_DELETE(pWndStates_);
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
		pWndStates_->Reset();
	}
}

///////////////////////////////////////////////////////////

void EntityCreationWindow::AddChosenComponentsToEntity(EntityManager& entityMgr)
{
	const std::string entityID = pWndStates_->entityID;
	const std::set<ComponentType> addedComponents = pWndStates_->addedComponents; // set of components added to this entity 

	// go through chosen components types and add a responsible component to the entity
	for (const ComponentType compType : addedComponents)
	{
		switch (compType)
		{
			case ComponentType::TransformComponent:
			{
				entityMgr.AddTransformComponent(
					entityID,
					pWndStates_->position,
					pWndStates_->direction,
					pWndStates_->scale);
				
				break;
			}
			case ComponentType::MoveComponent:
			{
				// compute a rotation quaternion by chosen rotation params
				const DirectX::XMFLOAT3 angles = pWndStates_->rotationAngles;
				DirectX::XMFLOAT4 rotQuat;
				DirectX::XMStoreFloat4(&rotQuat, DirectX::XMQuaternionRotationRollPitchYaw(angles.x, angles.y, angles.z));

				entityMgr.AddMoveComponent(
					entityID,
					pWndStates_->translation,
					rotQuat,
					pWndStates_->scaleChange);

				break;
			}
			case ComponentType::MeshComp:
			{
				std::vector<MeshID> meshesIDsArr(pWndStates_->chosenMeshes.begin(), pWndStates_->chosenMeshes.end());
				entityMgr.AddMeshComponents({ entityID }, meshesIDsArr);

				// setup meshes according to chosen params
				for (const MeshID& meshID : meshesIDsArr)
				{
					// set rendering shader for the mesh
					MeshStorage* pMeshStorage = MeshStorage::Get();
					pMeshStorage->SetRenderingShaderForMeshByID(meshID, pWndStates_->selectedRenderingShader);

					// setup texture for the mesh
					const TextureID textureID = pWndStates_->chosenTextureID;
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
				// we can add the Rendered component only if the Mesh component were added to the entity
				if (addedComponents.contains(ComponentType::MeshComp))
					entityMgr.AddRenderingComponents({ entityID });

				break;
			}
		}
	}
}