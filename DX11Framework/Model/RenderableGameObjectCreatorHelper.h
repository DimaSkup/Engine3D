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

	RenderableGameObject* MakeCopyOfRenderableGameObj(
		RenderableGameObject* pOriginGameObj);

	void SetupRenderableGameObjByType(
		const std::string & renderableGameObjID);

private:
	void HandleBadAllocException(
		const std::bad_alloc & e,
		RenderableGameObject* pGameObj,             // contains a ptr to the game object if it hasn't been added to the game object list
		const std::string & gameObjID);             // contains an ID of the game object if it has already been added to the game object list

	void HandleCOMException(
		COMException & e,
		RenderableGameObject* pGameObj,             // contains a ptr to the game object if it hasn't been added to the game object list
		const std::string & gameObjID);             // contains an ID of the game object if it has already been added to the game object list
};
