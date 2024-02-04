////////////////////////////////////////////////////////////////////////////////////////////
// Filename:     GameObjectsListClass.cpp
// Description:  contains implementation of functional of the GameObjectsListClass
//
////////////////////////////////////////////////////////////////////////////////////////////
#include "GameObjectsListClass.h"


// initialize static elements
GameObjectsListClass* GameObjectsListClass::pInstance_ = nullptr;



GameObjectsListClass::GameObjectsListClass()
{
	// we can have only one instance of this class
	if (GameObjectsListClass::pInstance_ != nullptr)   
	{
		COM_ERROR_IF_FALSE(false, "there is already exists one instance of the game objects list class");
	}
	else
	{
		Log::Debug(LOG_MACRO);
		pInstance_ = this;
	}
}

GameObjectsListClass::~GameObjectsListClass()
{
	Log::Print("------------------------------------------------------");
	Log::Print("               GAME OBJECTS DESTROYMENT:              ");
	Log::Print("------------------------------------------------------");
	Log::Debug(LOG_MACRO);

	this->Shutdown();
	pInstance_ = nullptr;
}






////////////////////////////////////////////////////////////////////////////////////////////
//
//                               PUBLIC FUNCTIONS
//
////////////////////////////////////////////////////////////////////////////////////////////



void GameObjectsListClass::GenerateRandomDataForRenderableGameObjects()
{
	// this function generates random color/position values 
	// for the game objects on the scene

	DirectX::XMFLOAT3 color{ 1, 1, 1 };   // white
	DirectX::XMFLOAT3 position{ 0, 0, 0 };
	const float posMultiplier = 50.0f;
	const float gameObjCoordsStride = 20.0f;


	// seed the random generator with the current time
	srand(static_cast<unsigned int>(time(NULL)));

	for (auto & elem : renderableGameObjectsList_)
	{
		// generate a random RGB colour for the game object
		color.x = static_cast<float>(rand()) / RAND_MAX;  
		color.y = static_cast<float>(rand()) / RAND_MAX;  
		color.z = static_cast<float>(rand()) / RAND_MAX;  

		// generate a random position in from of the viewer for the game object
		position.x = (static_cast<float>(rand()) / RAND_MAX) * posMultiplier + gameObjCoordsStride;
		position.y = (static_cast<float>(rand()) / RAND_MAX) * posMultiplier + 5.0f;
		position.z = (static_cast<float>(rand()) / RAND_MAX) * posMultiplier + gameObjCoordsStride;

		elem.second->SetColor(color);
		elem.second->SetPosition(position);
	}


	return;

} // end GenerateDataForGameObjects

///////////////////////////////////////////////////////////

void GameObjectsListClass::Shutdown(void)
{
	// this function releases memory from all the game objects

	Log::Debug(LOG_MACRO, "start of execution");


	//////////////////////////  CLEAR MEMORY FROM GAME OBJECTS  //////////////////////////

	// each game object is set in the game objects list (map) as a unique_ptr object
	// so we don't have to delete it manually
	

	//////////////////////////  CLEAN UP ALL THE OTHER LISTS  //////////////////////////

	// clear all the data from the game objects rendering list
	if (!renderableGameObjectsList_.empty())
	{
		renderableGameObjectsList_.clear();
		Log::Debug(LOG_MACRO, "game objects rendering list is cleared");
	}

	// clear all the data from the zone elements list
	if (!zoneGameObjectsList_.empty())
	{
		zoneGameObjectsList_.clear();
		Log::Debug(LOG_MACRO, "zone game objects list is cleared");
	}

	// clear all the data from the DEFAULT game objects list
	if (!defaultGameObjectsList_.empty())
	{
		defaultGameObjectsList_.clear();
		Log::Debug(LOG_MACRO, "default game objects list is cleared");
	}

	// clear all the data from the sprites rendering list
	if (!spritesList_.empty())
	{
		spritesList_.clear();
		Log::Debug(LOG_MACRO, "sprites game objects list is cleared");
	}


	Log::Debug(LOG_MACRO, "is shutted down successfully");

	return;

} // end Shutdown





////////////////////////////////////////////////////////////////////////////////////////////
//                                 PUBLIC GETTERS
////////////////////////////////////////////////////////////////////////////////////////////

RenderableGameObject* GameObjectsListClass::GetRenderableGameObjByID(const std::string & gameObjID)
{
	// get a ptr to the renderable game object from the map

	try
	{
		return this->renderableGameObjectsList_.at(gameObjID).get();
	}
	catch (const std::out_of_range & e)
	{
		Log::Error(LOG_MACRO, e.what());
		Log::Error(LOG_MACRO, std::string("can't find a renderable game object in the map by such an id: " + gameObjID));
		return nullptr;
	}
}

///////////////////////////////////////////////////////////

RenderableGameObject* GameObjectsListClass::GetZoneGameObjectByID(const std::string & gameObjID)
{
	// this function returns a pointer to the zone's game object by its id

	try
	{
		return this->zoneGameObjectsList_.at(gameObjID).get();
	}
	catch (const std::out_of_range & e)
	{
		Log::Error(LOG_MACRO, e.what());
		Log::Error(LOG_MACRO, "can't find a zone game object in the map by such an id: " + gameObjID);
		return nullptr;
	}

} // end GetZoneGameObjectByID

///////////////////////////////////////////////////////////

RenderableGameObject* GameObjectsListClass::GetSpriteByID(const std::string & gameObjID)
{
	// this function returns a pointer to the sprite game object by the input ID

	try
	{
		return this->spritesList_.at(gameObjID).get();
	}
	catch (const std::out_of_range & e)
	{
		Log::Error(LOG_MACRO, e.what());
		Log::Error(LOG_MACRO, "can't find a zone game object in the map by such an id: " + gameObjID);
		return nullptr;
	}
}

///////////////////////////////////////////////////////////

RenderableGameObject* GameObjectsListClass::GetDefaultGameObjectByID(const std::string& gameObjID) const
{
	// this function returns a pointer to the DEFAULT game object by its id

	try
	{
		return this->defaultGameObjectsList_.at(gameObjID).get();
	}
	catch (const std::out_of_range & e)
	{
		Log::Error(LOG_MACRO, e.what());
		Log::Error(LOG_MACRO, "can't find a default renderable game object in the map by such an id: " + gameObjID);
		return nullptr;
	}
}

///////////////////////////////////////////////////////////






const std::map<std::string, std::unique_ptr<RenderableGameObject>> & GameObjectsListClass::GetGameObjectsRenderingList()
{
	// this function returns a reference to the map which contains
	// the game objects for rendering onto the scene
	return this->renderableGameObjectsList_;
}

///////////////////////////////////////////////////////////

const std::map<std::string, std::unique_ptr<RenderableGameObject>> &  GameObjectsListClass::GetSpritesRenderingList()
{
	// this function returns a reference to the map which contains 
	// 2D sprite game objects for rendering
	return this->spritesList_;
}







////////////////////////////////////////////////////////////////////////////////////////////
//
//                              PUBLIC SETTERS/ADDERS
//
////////////////////////////////////////////////////////////////////////////////////////////

void GameObjectsListClass::AddRenderableGameObj(const std::string & gameObjID)
{
	// this function adds [game_obj_id => game_obj_ptr] pairs of zone elements 
	// (game object, for instance: terrain, sky dome, clouds, etc.) into the list/map

	RenderableGameObject* pGameObj = this->GetRenderableGameObjectFromGlobalListByID(gameObjID);
	this->SetRenderableGameObjectIntoList(pGameObj, zoneGameObjectsList_);

	return;

} // end SetGameObjAsZoneElementByID

/////////////////////////////////////////////////

void GameObjectsListClass::SetGameObjAsSpriteByID(const std::string & gameObjID)
{
	// this function adds a new 2D sprite (plane) into the sprites list 
	// for rendering onto the screen;

	RenderableGameObject* pGameObj = this->GetRenderableGameObjectFromGlobalListByID(gameObjID);
	this->SetRenderableGameObjectIntoList(pGameObj, spritesList_);

	return;

} // add SetGameObjAsSpriteByID

/////////////////////////////////////////////////

void GameObjectsListClass::SetGameObjectForRenderingByID(const std::string & gameObjID)
{
	// adds a game object ptr into the rendering list and asigns it with a gameObjID name;
	// (all these game objects from the list will be rendered on the scene);
	// it gets a pointer to the game object from the game objects GLOBAL list;

	RenderableGameObject* pGameObj = this->GetRenderableGameObjectFromGlobalListByID(gameObjID);
	this->SetRenderableGameObjectIntoList(pGameObj, renderableGameObjectsList_);

	return;

} // end SetGameObjectForRenderingByID

/////////////////////////////////////////////////

void GameObjectsListClass::SetGameObjectAsDefaultByID(const std::string & gameObjID)
{
	// set that a game object by this ID must be default (we use such game objects 
	// for faster initialization of other instances of renderable game objects);
	// it gets a pointer to the game object from the game objects GLOBAL list;

	RenderableGameObject* pGameObj = this->GetRenderableGameObjectFromGlobalListByID(gameObjID);
	this->SetRenderableGameObjectIntoList(pGameObj, defaultGameObjectsList_);

	return;

} // end SetGameObjectAsDefaultByID






////////////////////////////////////////////////////////////////////////////////////////////
//
//                            PUBLIC REMOVERS/DELETERS
//
////////////////////////////////////////////////////////////////////////////////////////////


void GameObjectsListClass::DeleteGameObjectByID(const std::string & gameObjID)
{
	// this function removes a game object by its ID at all (deletes from the memory)

	try
	{
		// try to find a game object by such an ID
		GameObject* pGameObj = gameObjectsGlobalList_.at(gameObjID).get();

		// remove this game object from the GLOBAL list
		gameObjectsGlobalList_.erase(gameObjID);

		// if we had this game object in the rendering list or default
		// game objects list we also remove it from there
		renderableGameObjectsList_.erase(gameObjID);
		defaultGameObjectsList_.erase(gameObjID);

		// delete the game object from the memory
		_DELETE(pGameObj);
	}
	catch (const std::out_of_range & e)
	{
		Log::Error(LOG_MACRO, e.what());
		Log::Error(LOG_MACRO, "there is no game objects by such an id: " + gameObjID);
		return;
	}

	return;

} // end DeleteGameObjectByID

/////////////////////////////////////////////////

void GameObjectsListClass::DontRenderGameObjectByID(const std::string & gameObjID)
{
	// if we have a game object by such gameObjID we delete it from the 
	// game objects rendering list
	// but if we can't find such a game object we throw an exception about it

	COM_ERROR_IF_ZERO(gameObjID.size(), "the input ID is empty");
	renderableGameObjectsList_.erase(gameObjID);

	return;

} // end DontRenderGameObjectByID







////////////////////////////////////////////////////////////////////////////////////////////
//
//                         PRIVATE FUNCTIONS (HELPERS)
//
////////////////////////////////////////////////////////////////////////////////////////////



bool GameObjectsListClass::SetRenderableGameObjectIntoList(
	std::unique_ptr<RenderableGameObject> pGameObj,
	std::map<std::string, std::unique_ptr<RenderableGameObject>> & map)
{
	// THIS FUNCTION inserts a pair ['game_obj_id' => 'ptr_to_game_obj'] into the input map

	// check input params
	COM_ERROR_IF_FALSE(pGameObj, "the input ptr to the game object == nullptr");

	// try to insert a game object pointer by such an id into the map
	auto res = map.insert({ pGameObj->GetID(), std::move(pGameObj) });

	// if the game object wasn't inserted
	if (!res.second)
	{
		// check if it is the same game object in this map if so we just print a log
		// message about it and go out from the function
		if (map.at(pRenderableGameObj->GetID()) == pRenderableGameObj)
		{
			Log::Debug(LOG_MACRO, "you want to set a game object into the map but it has already been done earlier");
			return;     // go out
		}

		// well... we want to set some another game object by the duplicated id -- throw an exception
		COM_ERROR_IF_FALSE(false, "there is a duplication of key: '" + pRenderableGameObj->GetID() + "' in the list/map");
	}
}

///////////////////////////////////////////////////////////

std::string GameObjectsListClass::GenerateNewKeyInGlobalListMap(const std::string & key)
{
	// this function generates a new key (ID) for a game object in the map;
	//
	// INPUT: takes as input a map and an old key (which is already exists in this map
	// so that we have to generate a new one)
	//
	// RETURN: a new generated key 

	COM_ERROR_IF_FALSE(key.empty() == false, "the input key is empty");

	// an index for concatenation it with the origin key ( for example: old_key(1) )
	size_t copyIndex = 1;

	// try to make a new key which is based on the original one
	std::string newKey{ key + '(' + std::to_string(copyIndex) + ')'};

	auto iteratorOfEnd = gameObjectsGlobalList_.end();

	// while we have the same key in the map we will generate a new
	while (gameObjectsGlobalList_.find(newKey) != iteratorOfEnd)
	{
		++copyIndex;
		newKey = { key + '(' + std::to_string(copyIndex) + ')' }; // generate a new key
	}

	return newKey;

} // end GenerateNewKeyInMap

/////////////////////////////////////////////////