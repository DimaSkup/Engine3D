#pragma once

// models stuff
#include "Model.h"
#include "GameObjectsListClass.h"
#include "../Model/ModelInitializerInterface.h"

// shaders stuff
#include "../ShaderClass/shaderclass.h"
#include "../ShaderClass/ModelToShaderMediatorInterface.h"




class GameObjectCreator
{
public:
	GameObjectCreator(GameObjectsListClass* pGameObjectsList);
	virtual ~GameObjectCreator() {};

	// get an instance of a model
	virtual Model* GetInstance(ModelInitializerInterface* pModelInitializer) = 0; 

	// define if this model is a basic model (cube, sphere, plane, etc.) 
	// in another case it is a Zone element (terrain, sky dome, sky plane, etc.)
	virtual bool IsBasicModel() const = 0;

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
		const std::string & renderShaderName = "ColorShaderClass");  // name of a shader which will be used for rendering a model

	GameObject* CreateCopyOfGameObject(GameObject* pOriginGameObject);

private:
	std::string TryToGetModelType_WhenException(Model* pModel);

private:
	GameObjectsListClass* pGameObjectsList_ = nullptr;
};
