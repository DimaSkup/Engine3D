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


#include "../GameObjects/MeshHelperTypes.h"
#include "../GameObjects/MeshStorage.h"
#include "../GameObjects/TextureHelperTypes.h"
#include "../GameObjects/RenderingShaderHelperTypes.h"

 // from the ECS module
#include "ECS_Common/ECS_Types.h"              
#include "ECS_Entity/EntityManager.h"


class EntityCreationWindow
{
public:
	// data structure to hold the state of the window between frames
	struct StatesOfWindowToCreateEntity
	{
		StatesOfWindowToCreateEntity() {}

		// common
		std::string entityName;
		std::string errorMsg;
		std::set<ComponentType> addedComponents;   // is used in the selectable menu for adding components

		// transform data (related to the Transform component)
		DirectX::XMFLOAT3 position{ 0,0,0 };
		DirectX::XMFLOAT3 direction{ 0,0,0 };  // in radians
		DirectX::XMFLOAT3 scale{ 1,1,1 };

		// movement data (related to the Movement component)
		DirectX::XMFLOAT3 scaleChange{ 1,1,1 };   // each frame the scale (from the Transform component) will be multiplyied by this value
		DirectX::XMFLOAT3 rotationAngles{ 0,0,0 };
		DirectX::XMFLOAT3 translation{ 0,0,0 };

		// mesh data (related to the Meshcomponent)
		std::set<MeshName> chosenMeshes;
		TextureID chosenTextureID;
		RENDERING_SHADERS selectedRenderingShader = RENDERING_SHADERS::COLOR_SHADER;

		void Reset()
		{
			entityName.clear();
			errorMsg.clear();	
			addedComponents.clear();

			ZeroMemory(&position, sizeof(position));
			ZeroMemory(&direction, sizeof(direction));
			ZeroMemory(&scale, sizeof(scale));

			ZeroMemory(&scaleChange, sizeof(scaleChange));
			ZeroMemory(&rotationAngles, sizeof(rotationAngles));
			ZeroMemory(&translation, sizeof(translation));

			chosenMeshes.clear();
			chosenTextureID.clear();
			selectedRenderingShader = RENDERING_SHADERS::COLOR_SHADER;
		}
	};

public:
	EntityCreationWindow() {}
	void Shutdown() { _DELETE(pWndStates_); }

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

	void ShowSelectableMenuOfMeshes(std::set<MeshName>& chosenMeshes);

	void ShowTexturesMenuForMesh(TextureID& chosenTexture);

	void ShowSelectableListOfRenderingShaders(RENDERING_SHADERS& selectedRenderingShader);

	// "Create/Close/Reset/etc." buttons
	void ShowButtonsOfWindow(EntityManager& entityMgr, bool* pOpen);

	void AddChosenComponentsToEntity(EntityManager& entityMgr, const EntityID enttID);
	
private:
	StatesOfWindowToCreateEntity* pWndStates_ = nullptr;
};