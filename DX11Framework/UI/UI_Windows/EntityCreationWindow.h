// ********************************************************************************
// Filename:     EntityCreationWindow.h
// Description:  a UI window for creation and setup an ECS Entity
// 
// Created:      04.06.24
// ********************************************************************************
#pragma once

#include <vector>
#include <string>
#include <set>
#include <DirectXMath.h>

#include "../ECS_Entity/ECS_Types.h"
#include "../ECS_Entity/EntityManager.h"
#include "../GameObjects/MeshHelperTypes.h"
#include "../GameObjects/MeshStorage.h"

// IMGUI STUFF
#include "imgui.h"
#include "imgui_stdlib.h"


class EntityCreationWindow
{
public:
	// data structure to hold the state of the window between frames
	struct StatesOfWindowToCreateEntity
	{
		StatesOfWindowToCreateEntity() {}

		// common
		std::string entityID;
		std::string errorMsg;
		std::set<ComponentType> addedComponents;   // is used in the selectable menu for adding components

		// transform data (related to the Transform component)
		DirectX::XMFLOAT3 position{ 0,0,0 };
		DirectX::XMFLOAT3 direction{ 0,0,0 };  // in radians
		DirectX::XMFLOAT3 scale{ 1,1,1 };

		// movement data (related to the Movement component)
		DirectX::XMFLOAT3 translation{ 0,0,0 };
		DirectX::XMFLOAT3 rotationQuat{ 0,0,0 };
		DirectX::XMFLOAT3 scaleChange{ 0,0,0 };

		// mesh data (related to the Meshcomponent)
		std::set<MeshID> chosenMeshes;
		std::string chosenTexture;
		Mesh::RENDERING_SHADERS selectedRenderingShader = Mesh::RENDERING_SHADERS::COLOR_SHADER;

		void Reset()
		{
			entityID.clear();
			errorMsg.clear();	
			addedComponents.clear();
			chosenMeshes.clear();

			//ZeroMemory(&transformData, sizeof(transformData));
			//ZeroMemory(&movementData, sizeof(movementData));
		}
	};

public:
	void ShowWindowToCreateEntity(bool* pOpen, EntityManager& entityMgr);

	void ShowSelectableMenuToAddComponents(EntityManager& entityMgr);
	void ShowSetupFieldsOfAddedComponents();

	// components setup elements
	void ShowFieldsToSetupTransformParams(
		DirectX::XMFLOAT3& inOutPosition,
		DirectX::XMFLOAT3& inOutDirection,
		DirectX::XMFLOAT3& inOutScale);

	void ShowFieldsToSetupMovementParams(
		DirectX::XMFLOAT3& inOutTranslation,
		DirectX::XMFLOAT3& inOutRotationQuat,
		DirectX::XMFLOAT3& inOutScaleChange);

	void ShowSelectableListOfMeshes(
		std::set<MeshID>& chosenMeshesIDs,
		const std::vector<MeshID>& meshesIDs);

	void ShowSelectableListOfRenderingShaders(Mesh::RENDERING_SHADERS& selectedRenderingShader);

	// "Create/Close/Reset/etc." buttons
	void ShowButtonsOfWindow(EntityManager& entityMgr, bool* pOpen);

private:
	StatesOfWindowToCreateEntity* pWndStates_ = nullptr;
};