#include "EntityCreationWindow.h"
#include "../GameObjects/TextureManagerClass.h"

void EntityCreationWindow::ShowWindowToCreateEntity(
	bool* pOpen,
	EntityManager& entityMgr)
{
	// after choosing "Create->Entity" in the main menu bar we get here;
	// we show to the user a window for creation and setup of a new entity;


	// setup and show a modal window for entity creation
	ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
	ImGui::OpenPopup("CreateEntity");

	if (ImGui::BeginPopupModal("CreateEntity", NULL, ImGuiWindowFlags_MenuBar))
	{
		std::set<ComponentID> selectedComponentsIDs;

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

	if (addedComponents.contains(ComponentType::TransformComponent) &&
		ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_None))
	{
		ShowFieldsToSetupTransformParams(
			wndStates.position,
			wndStates.direction,
			wndStates.scale);
	}

	if (addedComponents.contains(ComponentType::MovementComponent) &&
		ImGui::CollapsingHeader("Movement", ImGuiTreeNodeFlags_None))
	{
		ShowFieldsToSetupMovementParams(
			wndStates.translation,
			wndStates.rotationQuat,
			wndStates.scaleChange);
	}

	if (addedComponents.contains(ComponentType::MeshComp) &&
		ImGui::CollapsingHeader("MeshComponent", ImGuiTreeNodeFlags_None))
	{
		ShowSelectableListOfMeshes(
			wndStates.chosenMeshes,
			MeshStorage::Get()->GetMeshesIDs());
	}

	if (addedComponents.contains(ComponentType::RenderedComponent) &&
		ImGui::CollapsingHeader("Rendered", ImGuiTreeNodeFlags_None))
	{
		ShowSelectableListOfRenderingShaders(wndStates.selectedRenderingShader);
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
	ImGui::DragFloat3("rotation", &inOutRotationQuat.x);
	ImGui::DragFloat3("scale change", &inOutScaleChange.x);
}

///////////////////////////////////////////////////////////

void EntityCreationWindow::ShowSelectableListOfMeshes(
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

			// if such mesh is chosen we store its ID
			if (isSelected)
				chosenMeshesIDs.insert(meshID);
			else
			{
				chosenMeshesIDs.erase(meshID);
			}
		}
	}

	if (ImGui::CollapsingHeader("Set texture for mesh", ImGuiTreeNodeFlags_None))
	{
		TextureManagerClass* pTextureManager = TextureManagerClass::Get();
		const auto mapOfTextures = pTextureManager->GetMapOfTexID_ToSRV();
		auto iter = mapOfTextures.begin();
		
		const int columnsCount = 3;
		const int rowsCount = mapOfTextures.size() / columnsCount;



		if (ImGui::BeginTable("table", columnsCount, ImGuiTableFlags_BordersV))
		{
			
			for (int row = 0; row < rowsCount; ++row)
			{
				ImGui::TableNextRow();
				for (int column = 0; column < columnsCount; ++column)
				{
					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
					ImGui::TableSetColumnIndex(column);

					if (pWndStates_->chosenTexture == iter->first)
					{
						ImGui::PopStyleVar();
						ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2.0f, 2.0f));
					}
					
					ImVec2 imgDimensions = ImGui::GetContentRegionAvail();
					if (ImGui::ImageButton((void*)iter->second, ImVec2(imgDimensions.x, imgDimensions.x)))
					{
						pWndStates_->chosenTexture = iter->first;
					}

					ImGui::PopStyleVar();

					
					++iter;
				}
			}
			
			ImGui::EndTable();
		}
		
	}
}

///////////////////////////////////////////////////////////

void EntityCreationWindow::ShowSelectableListOfRenderingShaders(
	Mesh::RENDERING_SHADERS& selectedRenderingShader)
{
	// show a selectable menu for choosing a rendering shader type

	std::map<Mesh::RENDERING_SHADERS, std::string> typeToName =
	{
		{ Mesh::RENDERING_SHADERS::COLOR_SHADER, "ColorShader" },
		{ Mesh::RENDERING_SHADERS::TEXTURE_SHADER, "TextureShader" },
		{ Mesh::RENDERING_SHADERS::LIGHT_SHADER, "LightShader" },
	};

	for (const auto& it : typeToName)
	{
		if (ImGui::Selectable(it.second.c_str(), selectedRenderingShader == it.first))
			selectedRenderingShader = it.first;
	}
}

///////////////////////////////////////////////////////////

void EntityCreationWindow::ShowButtonsOfWindow(
	EntityManager& entityMgr, 
	bool* pOpen)
{
	// show some buttons of the creation window;

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
				const std::string entityID = pWndStates_->entityID;
				const std::set<ComponentType> addedComponents = pWndStates_->addedComponents; // set of components added to this entity 

				if (addedComponents.contains(ComponentType::TransformComponent))
				{
					entityMgr.AddTransformComponent(
						entityID,
						pWndStates_->position,
						pWndStates_->direction,
						pWndStates_->scale);
				}

#if 0
				if (addedComponents.contains("Movement"))
				{
					const Transform::ComponentData& data = static_cast<const Transform::ComponentData&>(pWndStates_->GetData("Transform"));
					entityMgr.AddTransformComponent(entityID, data.position_, data.direction_, data.scale_);
				}
#endif


				if (addedComponents.contains(ComponentType::MeshComp))
				{
					std::vector<MeshID> meshesIDsArr(pWndStates_->chosenMeshes.begin(), pWndStates_->chosenMeshes.end());
					entityMgr.AddMeshComponents({ entityID }, meshesIDsArr);

					// if we add the Rendered component the entity will be immediatelly rendered after creation
					if (addedComponents.contains(ComponentType::RenderedComponent))
					{
						for (const MeshID& meshID : meshesIDsArr)
							MeshStorage::Get()->SetRenderingShaderForMeshByID(meshID, pWndStates_->selectedRenderingShader);

						entityMgr.AddRenderingComponents({ entityID });
					}
				}

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
