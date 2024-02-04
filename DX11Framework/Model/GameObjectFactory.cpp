#include "GameObjectFactory.h"






////////////////////////////////////////////////////////////////////////////////////////////
//                             PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////





  ///////////////////////////////////////////////////////////

RenderableGameObject* GameObjectFactory::CreateNewRenderableGameObject(
	const std::string & filePath,                               // path to model's data file which is used for importing this model   
	const std::string & renderShaderName,                       // name of a shader which will be used for rendering a model
	const GameObject::GameObjectType type,
	const std::string & gameObjID)                              // make such a key for this game object inside the game objects list                                  
{
	// check input params
	
	
} // end CreateNewRenderableGameObject

  ///////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////

std::unique_ptr<RenderableGameObject> GameObjectFactory::MakeCopyOfRenderableGameObj(RenderableGameObject* pOriginGameObj)
{
	// this function makes a copy of the input renderable game object

	const std::string debugMsg{ "copy a renderable game object: " + pOriginGameObj->GetID() };
	Log::Debug(LOG_MACRO, debugMsg.c_str());

	Model* pModel = nullptr;
	std::unique_ptr<RenderableGameObject> pGameObj;

	try
	{
		// create a copy of the origin model
		// (copying of the vertex/index buffers, and other data as well)
		pModel = new Model(*pOriginGameObj->GetModel());

		// create a new game object and setup it with our copy of the origin model
		pGameObj = std::make_unique<RenderableGameObject>(pModel);
	}
	catch (const std::bad_alloc & e)
	{
		std::string exceptionMsg{ "can't allocate memory for a model / game object" };
		exceptionMsg += pCreatorHelper_->TryToGetModelType_WhenException(pModel);

		// print error messages
		Log::Error(LOG_MACRO, e.what());
		Log::Error(LOG_MACRO, exceptionMsg.c_str());

		// if we can't create a model/game object we have to clean up the memory
		_DELETE(pModel);

		COM_ERROR_IF_FALSE(false, exceptionMsg);
	}

	// return a ptr to the copy of the input renderable game object
	return std::move(pGameObj);
}

///////////////////////////////////////////////////////////



void GameObjectFactory::SetupRenderableGameObjByType(RenderableGameObject* pGameObj,
	const GameObject::GameObjectType type)
{
	//
	// setup this game object according to its type
	//

	switch (type)
	{
		case GameObject::ZONE_ELEMENT_GAME_OBJ:
		{
			// if this game object is a zone element we put it into the zone elements list;
			// so later we can get this elements withing the ZoneClass;
			this->pGameObjectsList_->SetGameObjAsZoneElementByID(pGameObj);
			break;
		}
		case GameObject::RENDERABLE_GAME_OBJ:
		{
			// if we created a usual game object we just put it into the rendering list
			this->pGameObjectsList_->SetGameObjectForRenderingByID(pGameObj->GetID());
			break;
		}
		case GameObject::SPRITE_GAME_OBJ:
		{
			// if we created a 2D sprite we put it into the sprites rendering list
			// because we have to render it in a particular way (as 2D UI elements or something like it)
			this->pGameObjectsList_->SetGameObjAsSpriteByID(pGameObj);
			break;
		}
		default:
		{
			// this model isn't any king of renderable game objects
			// (for instance: camera) so we can't set it for rendering or something like that; 
		}
	} // switch

	return;
}