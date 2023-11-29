#pragma once

// models stuff
#include "Model.h"
#include "GameObjectsListClass.h"
#include "../Model/ModelInitializerInterface.h"

// basic models
#include "CustomModel.h"
#include "Line3D.h"
#include "Triangle.h"
#include "Cube.h"
#include "Sphere.h"
#include "Plane.h"
#include "TreeModel.h"

// 2d sprite
#include "../2D/SpriteClass.h"

// terrain elements
#include "TerrainClass.h"
#include "SkyDomeClass.h"
#include "SkyPlaneClass.h"

// shaders stuff
#include "../ShaderClass/shaderclass.h"
#include "../ShaderClass/ModelToShaderMediatorInterface.h"



class GameObjectCreatorHelper;

class BasicGameObjectCreator
{
public:
	BasicGameObjectCreator(GameObjectsListClass* pGameObjectsList);
	~BasicGameObjectCreator() {};

	// returns an empty model object
	virtual Model* GetInstance(ModelInitializerInterface* pModelInitializer, 
		ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext) = 0;


	bool CreateDefaultGameObject(ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext,
		ModelInitializerInterface* pModelInitializer,
		ModelToShaderMediatorInterface* pModelToShaderMediator,
		const std::string & renderShaderName = "ColorShaderClass");

	GameObject* CreateNewGameObject(ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext,
		ModelInitializerInterface* pModelInitializer,
		ModelToShaderMediatorInterface* pModelToShaderMediator,
		const std::string & filePath,
		const std::string & renderShaderName = "ColorShaderClass",
		const bool isZoneElement = false);

	GameObject* CreateCopyOfGameObject(GameObject* pOriginGameObject);

private:
	GameObjectsListClass* pGameObjectsList_ = nullptr;
	GameObjectCreatorHelper* pCreatorHelper_ = nullptr;
};


////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////










///////////////////////////////////////////////////////////////////////////////////////////
//                     A HELPER CLASS FOR THE GAME OBJECT CREATOR CLASS
///////////////////////////////////////////////////////////////////////////////////////////


class GameObjectCreatorHelper
{
public:
	std::string TryToGetModelType_WhenException(Model* pModel)
	{
		// try to get a model type of some model where an exception happened

		COM_ERROR_IF_NULLPTR(pModel, "the input ptr to model == nullptr");

		if (pModel != nullptr)
		{
			return ": " + pModel->GetModelType();
		}

		return ": can't get a model type";
	}


	std::string GetPathToDataFile(Model* pModel)
	{
		// generate a path to a model's data file by its type

		COM_ERROR_IF_NULLPTR(pModel, "the input ptr to model == nullptr");

		const std::string defaultModelsDirPath{ Settings::Get()->GetSettingStrByKey("DEFAULT_MODELS_DIR_PATH") };
		const std::string defaultModelsExt{ ".obj" };

		// return the generated path
		return { defaultModelsDirPath + pModel->GetModelType() + defaultModelsExt };
	}
};


