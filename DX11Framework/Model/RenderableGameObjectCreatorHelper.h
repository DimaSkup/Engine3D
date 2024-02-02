#pragma once

#include "../Model/Model.h"
#include "../Engine/COMException.h"
#include "../Model/GameObjectsListClass.h"

class RenderableGameObjectCreatorHelper final
{
public:
	void InitializeRenderableGameObject(
		RenderableGameObject* pGameObjForInitialization,
		ModelToShaderMediatorInterface* pModelToShaderMediator,
		const std::string & renderingShaderName,                     // name of a shader which will be used for rendering a model)
		const std::string & filePath);                            // path to model's data file which is used for importing this model   

	bool CreateDefaultRenderableGameObject(
		RenderableGameObject* pGameObjForDefault,
		ModelToShaderMediatorInterface* pModelToShaderMediator,
		const std::string & renderingShaderName);

	RenderableGameObject* CreateNewRenderableGameObject(
		RenderableGameObject* pNewRenderableGameObj,
		ModelToShaderMediatorInterface* pModelToShaderMediator,
		const std::string & renderingShaderName,
		const std::string & filePath,
		const std::string & gameObjID = "");     // make such an ID for this game object inside the game objects list                                  

	std::unique_ptr<RenderableGameObject> MakeCopyOfRenderableGameObj(
		RenderableGameObject* pOriginGameObj);

	void SetupRenderableGameObjByType(
		const RenderableGameObject* pGameObj,
		const GameObject::GameObjectType type);
};
