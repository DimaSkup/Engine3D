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



void GameObjectsListClass::GenerateRandomDataForGameObjects()
{
	// this function generates random color/position values 
	// for the game objects on the scene

	DirectX::XMFLOAT3 color{ 1, 1, 1 };   // white
	DirectX::XMFLOAT3 position{ 0, 0, 0 };
	const float posMultiplier = 50.0f;
	const float gameObjCoordsStride = 20.0f;


	// seed the random generator with the current time
	srand(static_cast<unsigned int>(time(NULL)));

	for (auto & elem : gameObjectsRenderingList_)
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
	if (!gameObjectsRenderingList_.empty())
	{
		gameObjectsRenderingList_.clear();
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
	if (!spritesRenderingList_.empty())
	{
		spritesRenderingList_.clear();
		Log::Debug(LOG_MACRO, "sprites game objects list is cleared");
	}


	Log::Debug(LOG_MACRO, "is shutted down successfully");

	return;

} // end Shutdown





////////////////////////////////////////////////////////////////////////////////////////////
//                                 PUBLIC GETTERS
////////////////////////////////////////////////////////////////////////////////////////////


GameObject* GameObjectsListClass::GetGameObjectByID(const std::string & gameObjID)
{
	// this function returns a pointer to the game object by its id

	try
	{
		return this->gameObjectsGlobalList_.at(gameObjID).get();
	}
	catch (const std::out_of_range & e)
	{
		Log::Error(LOG_MACRO, e.what());
		Log::Error(LOG_MACRO, "can't find a game object in the map by such an id: " + gameObjID);
		return nullptr;
	}

} // end GetGameObjectByID

///////////////////////////////////////////////////////////

RenderableGameObject* GameObjectsListClass::GetRenderableGameObjByID(const std::string & gameObjID)
{
	// get a ptr to the renderable game object from the map

	try
	{
		return this->gameObjectsRenderingList_.at(gameObjID);
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
		return this->zoneGameObjectsList_.at(gameObjID);
	}
	catch (const std::out_of_range & e)
	{
		Log::Error(LOG_MACRO, e.what());
		Log::Error(LOG_MACRO, "can't find a zone game object in the map by such an id: " + gameObjID);
		return nullptr;
	}

} // end GetZoneGameObjectByID

///////////////////////////////////////////////////////////

RenderableGameObject* GameObjectsListClass::GetDefaultGameObjectByID(const std::string& gameObjID) const
{
	// this function returns a pointer to the DEFAULT game object by its id

	try
	{
		return this->defaultGameObjectsList_.at(gameObjID);
	}
	catch (const std::out_of_range & e)
	{
		Log::Error(LOG_MACRO, e.what());
		Log::Error(LOG_MACRO, "can't find a default renderable game object in the map by such an id: " + gameObjID);
		return nullptr;
	}
}

///////////////////////////////////////////////////////////

std::map<std::string, RenderableGameObject*> & GameObjectsListClass::GetDefaultGameObjectsList()
{
	// this function returns a reference to the map which contains 
	// pointers to all the DEFAULT game objects
	return this->defaultGameObjectsList_;
}

///////////////////////////////////////////////////////////

const std::map<std::string, RenderableGameObject*> & GameObjectsListClass::GetGameObjectsRenderingList()
{
	// this function returns a reference to the map which contains
	// the game objects for rendering onto the scene
	return this->gameObjectsRenderingList_;
}

///////////////////////////////////////////////////////////

const std::map<std::string, RenderableGameObject*> & GameObjectsListClass::GetSpritesRenderingList()
{
	// this function returns a reference to the map which contains 
	// 2D sprite game objects for rendering
	return this->spritesRenderingList_;
}







////////////////////////////////////////////////////////////////////////////////////////////
//
//                              PUBLIC SETTERS/ADDERS
//
////////////////////////////////////////////////////////////////////////////////////////////

GameObject* GameObjectsListClass::AddGameObject(std::unique_ptr<GameObject> pGameObj)
{
	// this function adds a new game object into the GLOBAL list;
	// if there is already a game object with the same ID as in the pGameObj
	// we generate a new ID for the input pGameObj;
	//
	// NOTE: if we remove game object from this list so we remove it from anywhere;

	COM_ERROR_IF_NULLPTR(pGameObj, "the input pGameObj == nullptr");

	// first of all we have to check if there is already such an ID in the global list
	if (gameObjectsGlobalList_.find(pGameObj->GetID()) == gameObjectsGlobalList_.end())
	{
		// try to insert a game object pointer by such an id
		auto res = gameObjectsGlobalList_.insert({ pGameObj->GetID(), std::move(pGameObj) });

		// return a raw pointer to the game object
		return res.first->second.get();
	}
	else
	{
		// if we have a duplication of the ID we have to generate a new one for this game obj
		// we have a duplication by such a key so generate a new one (new ID for the game object)
		const std::string newGameObjID{ this->GenerateNewKeyInGlobalListMap(pGameObj->GetID()) };

		// insert a game object pointer by the new id
		pGameObj->SetID(newGameObjID);
		auto insertRes = gameObjectsGlobalList_.insert({ newGameObjID, std::move(pGameObj) });

		// return a raw pointer to the game object
		return insertRes.first->second.get();
	}


} // end AddGameObject

/////////////////////////////////////////////////

void GameObjectsListClass::SetGameObjAsZoneElement(RenderableGameObject* pGameObj)
{
	// this function adds [game_obj_id => game_obj_ptr] pairs of zone elements 
	// (game object, for instance: terrain, sky dome, clouds, etc.) into the list

	COM_ERROR_IF_NULLPTR(pGameObj, "the input pGameObj == nullptr");

	// try to insert a game object pointer by such an id into the zone elements list
	auto res = zoneGameObjectsList_.insert({ pGameObj->GetID(), pGameObj });

	// if the game object wasn't inserted
	if (!res.second)
	{
		COM_ERROR_IF_FALSE(false, "there is a duplication of key: '" + pGameObj->GetID() + "' in the zoneGameObjectsList_");
	}


} // end SetGameObjAsZoneElement

/////////////////////////////////////////////////

void GameObjectsListClass::SetGameObjAsSprite(RenderableGameObject* pGameObj)
{

	// this function adds a new 2D sprite (plane) into the sprites list 
	// for rendering onto the screen;
	//
	// if there is already a record in the list which has the same ID as the input one we 
	// check if it is the same game object;
	// in case if want to add a new game object by duplicated ID -- we throw an exception

	COM_ERROR_IF_NULLPTR(pGameObj, "the input pGameObj == nullptr");
	COM_ERROR_IF_NULLPTR(pGameObj->GetModel(), "the input pGameObj has no model");

	const std::string gameObjID{ pGameObj->GetID() };

	try
	{
		// if there is no game objects (sprites) with such an ID in
		const bool result = (this->gameObjectsGlobalList_.find(gameObjID) != gameObjectsGlobalList_.end());
		COM_ERROR_IF_FALSE(result, "there is no game object with such an id: " + gameObjID);
		

		// try to insert a pointer to the sprite by such an id
		auto res = spritesRenderingList_.insert({ gameObjID, pGameObj });

		// if the game object wasn't inserted
		if (!res.second)
		{
			// check if it is the same game object if so we just print a log
			// message about it and go out from the function
			if (spritesRenderingList_.at(gameObjID) == pGameObj)
			{
				Log::Debug(LOG_MACRO, "you want to set a game object as sprite but it has already been done earlier");
				return;     // go out
			}

			// well... we want to set some another game object by the duplicated id -- throw an exception
			COM_ERROR_IF_FALSE(false, "there is a duplication of key: '" + gameObjID + "' in the spritesRenderingList_");
		}
	}
	catch (const std::out_of_range & e)
	{
		// there is no game object with such an id
		Log::Error(LOG_MACRO, e.what());
		COM_ERROR_IF_FALSE(false, "there is no game objects with such an id: " + gameObjID);
	}

} // add SetGameObjAsSprite

/////////////////////////////////////////////////

void GameObjectsListClass::SetGameObjectForRenderingByID(const std::string & gameObjID)
{
	// adds a game object ptr into the rendering list and asigns it with a gameObjID name;
	// (all these game objects from the list will be rendered on the scene);
	// it gets a pointer to the game object from the game objects GLOBAL list;

	try
	{
		// try to find a game object by the input ID
		GameObject* pGameObj = gameObjectsGlobalList_.at(gameObjID).get();

		// try to cast this game object to the RenderableGameObject type to check if this game object can be rendered (for instance: if it is a camera it isn't possible to render it as a visible object)
		RenderableGameObject* pRenderableGameObj = dynamic_cast<RenderableGameObject*>(pGameObj);
		
		// add this game object into the rendering list (pair: ['id' => 'ptr_to_game_obj'])
		auto res = gameObjectsRenderingList_.insert({ pRenderableGameObj->GetID(), pRenderableGameObj });

		// if the game object wasn't inserted successfully
		if (!res.second)   
		{
			Log::Error(LOG_MACRO, "can't insert a game object (" + gameObjID + ") into the game objects RENDERING list");
		}
	}
	catch (const std::out_of_range & e)
	{
		Log::Error(LOG_MACRO, e.what());
		Log::Error(LOG_MACRO, "there is no game objects by such an id: " + gameObjID);
		return;
	}
	catch (const std::bad_cast & e)
	{
		Log::Error(LOG_MACRO, e.what());
		Log::Error(LOG_MACRO, "can't set a game object by id: " + gameObjID + " for rendering because it is not a renderable game object");
		return;
	}

	return;

} // end SetGameObjectForRenderingByID

/////////////////////////////////////////////////

void GameObjectsListClass::SetGameObjectAsDefaultByID(const std::string & gameObjID)
{
	// set that a game object by this ID must be default;
	// it gets a pointer to the game object from the game objects GLOBAL list;

	COM_ERROR_IF_FALSE(gameObjID.empty() == false, "the input ID is empty");

	try
	{
		// try to find a game object by the input ID
		GameObject* pGameObj = gameObjectsGlobalList_.at(gameObjID).get();

		// try to cast this game object to the RenderableGameObject type to check if this game object can be rendered (for instance: if it is a camera it isn't possible to render it as a visible object)
		RenderableGameObject* pRenderableGameObj = dynamic_cast<RenderableGameObject*>(pGameObj);

		// add this game object into the rendering list (pair: ['id' => 'ptr_to_game_obj'])
		auto res = defaultGameObjectsList_.insert({ pRenderableGameObj->GetID(), pRenderableGameObj });

		// if the game object wasn't inserted successfully
		if (!res.second)
		{
			Log::Error(LOG_MACRO, "can't set a game object (" + gameObjID + ") as default");
		}
	}
	catch (const std::out_of_range & e)
	{
		Log::Error(LOG_MACRO, e.what());
		Log::Error(LOG_MACRO, "there is no game objects by such an id: " + gameObjID);
		return;
	}
	catch (const std::bad_cast & e)
	{
		Log::Error(LOG_MACRO, e.what());
		Log::Error(LOG_MACRO, "can't set a game object (" + gameObjID + ") as default because it is not a renderable game object");
		return;
	}

	return;

} // end SetGameObjectAsDefaultByID






////////////////////////////////////////////////////////////////////////////////////////////
//                            PUBLIC REMOVERS/DELETERS
////////////////////////////////////////////////////////////////////////////////////////////


void GameObjectsListClass::RemoveGameObjectByID(const std::string & gameObjID)
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
		gameObjectsRenderingList_.erase(gameObjID);
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

} // end RemoveGameObjectByID

/////////////////////////////////////////////////

void GameObjectsListClass::DontRenderGameObjectByID(const std::string & gameObjID)
{
	// if we have a game object by such gameObjID we delete it from the 
	// game objects rendering list
	// but if we can't find such a game object we throw an exception about it

	gameObjectsRenderingList_.erase(gameObjID);

	return;

} // end DontRenderGameObjectByID







////////////////////////////////////////////////////////////////////////////////////////////
//
//                         PRIVATE FUNCTIONS (HELPERS)
//
////////////////////////////////////////////////////////////////////////////////////////////

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