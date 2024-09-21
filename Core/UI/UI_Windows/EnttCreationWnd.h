// ********************************************************************************
// Filename:     EnttCreationWnd.h
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
#include "Common/Types.h"              
#include "Entity/EntityManager.h"


class EnttCreationWnd
{
public:
	using EntityManager = ECS::EntityManager;

public:

	// data structure to hold the state of the window between frames
	struct WndStates
	{
		WndStates() {}

		// common
		std::string entityName;
		std::string errorMsg;
		std::set<ECS::ComponentType> addedComponents;   // is used in the selectable menu for adding components

		ECS::TransformRawData transform;
		ECS::MovementRawData movement;

		MeshName chosenMesh;
		TexName chosenTexName;
		::RENDERING_SHADERS selectedRenderingShader = ::RENDERING_SHADERS::COLOR_SHADER;

		void Reset()
		{
			entityName.clear();
			errorMsg.clear();	
			addedComponents.clear();

			ZeroMemory(&transform, sizeof(transform));
			transform.uniScale = 1.0f;

			ZeroMemory(&movement, sizeof(movement));
			movement.uniScale= 1.0f;

			chosenMesh.clear();
			chosenTexName.clear();
			selectedRenderingShader = ::RENDERING_SHADERS::COLOR_SHADER;
		}
	};

public:
	EnttCreationWnd(ID3D11Device* pDevice) 
	{
		Assert::NotNullptr(pDevice, "ptr to device == nullptr");
		pDevice_ = pDevice;
	}

	void Shutdown() { SafeDelete(pWndStates_); }

	void ShowWndToCreateEntt(bool* pOpen, EntityManager& mgr);
	void ShowSelectableMenuToAddComponents(EntityManager& mgr);
	void ShowSetupFieldsOfAddedComponents();

	// components setup elements
	void ShowFieldsToSetupTransformParams(ECS::TransformRawData& data);
	void ShowFieldsToSetupMovementParams(ECS::MovementRawData& data);
	void ShowSelectableMenuOfMeshes(MeshName& chosenMesh);
	void ShowTexturesMenuForMesh(TexName& chosenTex);
	void ShowSelectableListOfRenderingShaders(::RENDERING_SHADERS& selectedRenderingShader);

	// "Create/Close/Reset/etc." buttons
	void ShowButtonsOfWnd(EntityManager& mgr, bool* pOpen);

	void AddChosenComponentsToEntity(EntityManager& mgr, const EntityID id);
	
private:
	WndStates* pWndStates_ = nullptr;
	ID3D11Device* pDevice_ = nullptr;
};