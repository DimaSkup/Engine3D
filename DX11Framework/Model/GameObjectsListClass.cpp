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
		Log::Debug(THIS_FUNC_EMPTY);
		pInstance_ = this;
	}
}

GameObjectsListClass::~GameObjectsListClass()
{
	Log::Print("------------------------------------------------------");
	Log::Print("               GAME OBJECTS DESTROYMENT:              ");
	Log::Print("------------------------------------------------------");
	Log::Debug(THIS_FUNC_EMPTY);

	this->Shutdown();
	pInstance_ = nullptr;
}






////////////////////////////////////////////////////////////////////////////////////////////
//
//                               PUBLIC FUNCTIONS
//
////////////////////////////////////////////////////////////////////////////////////////////



bool GameObjectsListClass::GenerateDataForGameObjects()
{
	// this function generates random color/position values 
	// for the game objects on the scene

	float red = 1.0f, green = 1.0f, blue = 1.0f, alpha = 1.0f;
	float posX = 0.0f, posY = 0.0f, posZ = 0.0f;
	float posMultiplier = 50.0f;
	float gameObjCoordsStride = 20.0f;


	// seed the random generator with the current time
	srand(static_cast<unsigned int>(time(NULL)));

	for (auto & elem : gameObjectsRenderingList_)
	{
		// generate a random colour for the game object
		red = static_cast<float>(rand())   / RAND_MAX;
		green = static_cast<float>(rand()) / RAND_MAX;
		blue = static_cast<float>(rand())  / RAND_MAX;
		alpha = 1.0f;

		// generate a random position in from of the viewer for the game object
		posX = (static_cast<float>(rand()) / RAND_MAX) * posMultiplier + gameObjCoordsStride;
		posY = (static_cast<float>(rand()) / RAND_MAX) * posMultiplier + 5.0f;
		posZ = (static_cast<float>(rand()) / RAND_MAX) * posMultiplier + gameObjCoordsStride;


		elem.second->GetData()->SetColor(red, green, blue, 1.0f);
		elem.second->GetData()->SetPosition(posX, posY, posZ);
	}


	return true;

} // end GenerateDataForGameObjects

///////////////////////////////////////////////////////////

void GameObjectsListClass::Shutdown(void)
{
	// this function releases memory from all the game objects

	Log::Debug(THIS_FUNC, "start of execution");


	//////////////////////////  CLEAR MEMORY FROM GAME OBJECTS  //////////////////////////

	// delete all the game objects
	if (!gameObjectsGlobalList_.empty())
	{
		for (auto & elem : gameObjectsGlobalList_)
		{
			gameObjectsGlobalList_.erase(elem.first);
			//_DELETE(elem.second); // delete data by this game object's pointer
		}

		gameObjectsGlobalList_.clear();

		Log::Debug(THIS_FUNC, "game objects global list is deleted");
	}

	

	//////////////////////////  CLEAN UP ALL THE OTHER LISTS  //////////////////////////

	// clear all the data from the game objects rendering list
	if (!gameObjectsRenderingList_.empty())
	{
		gameObjectsRenderingList_.clear();
		Log::Debug(THIS_FUNC, "game objects rendering list is cleared");
	}

	// clear all the data from the zone elements list
	if (!zoneGameObjectsList_.empty())
	{
		zoneGameObjectsList_.clear();
		Log::Debug(THIS_FUNC, "zone game objects list is cleared");
	}

	// clear all the data from the DEFAULT game objects list
	if (!defaultGameObjectsList_.empty())
	{
		defaultGameObjectsList_.clear();
		Log::Debug(THIS_FUNC, "default game objects list is cleared");
	}

	// clear all the data from the sprites rendering list
	if (!spritesRenderingList_.empty())
	{
		spritesRenderingList_.clear();
		Log::Debug(THIS_FUNC, "sprites game objects list is cleared");
	}


	Log::Debug(THIS_FUNC, "is shutted down successfully");

	return;

} // end Shutdown





////////////////////////////////////////////////////////////////////////////////////////////
//                                 PUBLIC GETTERS
////////////////////////////////////////////////////////////////////////////////////////////


GameObject* GameObjectsListClass::GetGameObjectByID(const std::string & gameObjID)
{
	// this function returns a pointer to the game object by its id

	assert(!gameObjID.empty());

	auto it = GetIteratorByID(gameObjectsGlobalList_, gameObjID);

	// if we found a game object by this ID we return a pointer to it or nullptr in another case;
	return (it != gameObjectsGlobalList_.end()) ? it->second : nullptr;

} // end GetGameObjectByID

///////////////////////////////////////////////////////////

GameObject* GameObjectsListClass::GetZoneGameObjectByID(const std::string & gameObjID)
{
	// this function returns a pointer to the zone's game object by its id

	assert(!gameObjID.empty());

	auto it = GetIteratorByID(zoneGameObjectsList_, gameObjID);

	// if we found a game object by this ID we return a pointer
	// to it or nullptr in another case;
	return (it != zoneGameObjectsList_.end()) ? it->second : nullptr;

} // end GetZoneGameObjectByID

///////////////////////////////////////////////////////////

GameObject* GameObjectsListClass::GetDefaultGameObjectByID(const std::string& gameObjID) const
{
	// this function returns a pointer to the DEFAULT game object by its id

	COM_ERROR_IF_FALSE(gameObjID.empty() == false, "the input gameObjID is empty");

	return defaultGameObjectsList_.at(gameObjID);
}

///////////////////////////////////////////////////////////

std::map<std::string, GameObject*> & GameObjectsListClass::GetDefaultGameObjectsList()
{
	// this function returns a reference to the map which contains 
	// pointers to all the DEFAULT game objects
	return this->defaultGameObjectsList_;
}


///////////////////////////////////////////////////////////

const std::map<std::string, GameObject*> & GameObjectsListClass::GetGameObjectsGlobalList() const
{
	// return a reference to the GLOBAL game objects list;
	// this list contains pointers to all the game objects on the scene;
	return gameObjectsGlobalList_;
}

///////////////////////////////////////////////////////////

const std::map<std::string, GameObject*> & GameObjectsListClass::GetGameObjectsRenderingList()
{
	// this function returns a reference to the map which contains
	// the game objects for rendering onto the scene
	return this->gameObjectsRenderingList_;
}

///////////////////////////////////////////////////////////

const std::map<std::string, GameObject*> & GameObjectsListClass::GetSpritesRenderingList()
{
	// this function returns a reference to the map which contains 
	// 2D sprite game objects for rendering
	return this->spritesRenderingList_;
}







////////////////////////////////////////////////////////////////////////////////////////////
//                              PUBLIC SETTERS/ADDERS
////////////////////////////////////////////////////////////////////////////////////////////

void GameObjectsListClass::AddGameObject(GameObject* pGameObj)
{
	// this function adds a new game object into the GLOBAL list;
	// if there is already a game object with the same ID as in the pGameObj
	// we generate a new ID for the input pGameObj;
	//
	// NOTE: if we remove game object from this list so we remove it from anywhere;

	COM_ERROR_IF_NULLPTR(pGameObj, "the input pGameObj == nullptr");
	COM_ERROR_IF_NULLPTR(pGameObj->GetModel(), "the input pGameObj has no model");


	// try to insert a game object pointer by such an id
	auto res = gameObjectsGlobalList_.insert({ pGameObj->GetID(), pGameObj });

	// if the game object wasn't inserted
	if (!res.second)
	{
		// we have a duplication by such a key so generate a new one (new ID for the game object)
		std::string newGameObjID = this->GenerateNewKeyInMap(gameObjectsGlobalList_, pGameObj->GetID());

		// insert a game object pointer by the new id
		pGameObj->SetID(newGameObjID);
		gameObjectsGlobalList_.insert({ newGameObjID, pGameObj });
	}

} // end AddGameObject

/////////////////////////////////////////////////

void GameObjectsListClass::AddZoneElement(GameObject* pGameObj)
{
	// this function adds [game_obj_id => game_obj_ptr] pairs of zone elements 
	// (game object, for instance: terrain, sky dome, clouds, etc.) into the list

	COM_ERROR_IF_NULLPTR(pGameObj, "the input pGameObj == nullptr");
	COM_ERROR_IF_NULLPTR(pGameObj->GetModel(), "the input pGameObj has no model");

	// first of all we add a game object into the game objects global list
	this->AddGameObject(pGameObj);

	// try to insert a game object pointer by such an id into the zone elements list
	auto res = zoneGameObjectsList_.insert({ pGameObj->GetID(), pGameObj });

	// if the game object wasn't inserted
	if (!res.second)
	{
		std::string errorMsg{ "there is a duplication of key: '" + pGameObj->GetID() + "' in the zoneGameObjectsList_" };
		COM_ERROR_IF_FALSE(false, errorMsg.c_str());
#if 0
		// we have a duplication by such a key so generate a new one (new ID for the game object)
		std::string newID = this->GenerateNewKeyInMap(zoneGameObjectsList_, pGameObj->GetID());

		// insert a game object pointer by the new id
		pGameObj->SetID(newID);
		zoneGameObjectsList_.insert({ newID, pGameObj });
#endif
	}


} // end AddZoneElement

/////////////////////////////////////////////////

void GameObjectsListClass::AddSprite(GameObject* pGameObj)
{

	// this function adds a new 2D sprite (plane) into the sprites list 
	// for rendering onto the screen;
	//
	// if there is already ID in the list which is the same as the input one we 
	// generate new ID for proper inserting into the sprites list;

	COM_ERROR_IF_NULLPTR(pGameObj, "the input pGameObj == nullptr");
	COM_ERROR_IF_NULLPTR(pGameObj->GetModel(), "the input pGameObj has no model");

	// first of all we add a game object into the game objects global list
	this->AddGameObject(pGameObj);

	// try to insert a sprite pointer by such an id
	auto res = spritesRenderingList_.insert({ pGameObj->GetID(), pGameObj });

	// if the game object wasn't inserted
	if (!res.second)
	{
		std::string errorMsg{ "there is a duplication of key: '" + pGameObj->GetID() + "' in the spritesRenderingList_" };
		COM_ERROR_IF_FALSE(false, errorMsg.c_str());
#if 0
		// we have a duplication by such a key so generate a new one (new ID for the sprite)
		std::string newID = this->GenerateNewKeyInMap(spritesRenderingList_, pGameObj->GetID());

		// insert a game object pointer by the new id
		pGameObj->SetID(newID);    // rewrite sprite's ID with the generated new one
		spritesRenderingList_.insert({ newID, pGameObj });
#endif
	}

} // add AddSprite

/////////////////////////////////////////////////

void GameObjectsListClass::SetGameObjectForRenderingByID(const std::string & gameObjID)
{
	// adds a game object ptr into the rendering list and asigns it with a gameObjID name;
	// (all these game objects from the list will be rendered on the scene);
	// it gets a pointer to the game object from the game objects GLOBAL list;

	COM_ERROR_IF_FALSE(!gameObjID.empty(), "the input ID is empty");

	// try to find this game object in the game objects GLOBAL list
	auto iterator = GetIteratorByID(gameObjectsGlobalList_, gameObjID);

	// if we got a correct iterator
	if (iterator != gameObjectsGlobalList_.end())
	{
		// add this game object into the rendering list
		auto res = gameObjectsRenderingList_.insert({ iterator->first, iterator->second });

		if (!res.second)   // if the game object wasn't inserted
		{
			std::string errorMsg{ "can't insert a game object (" + gameObjID + ") into the game objects RENDERING list" };
			COM_ERROR_IF_FALSE(false, errorMsg);
		}
	}

	return;

} // end SetGameObjectForRenderingByID

/////////////////////////////////////////////////

void GameObjectsListClass::SetGameObjectAsDefaultByID(const std::string & gameObjID)
{
	// set that a game object by this ID must be default;
	// it gets a pointer to the game object from the game objects GLOBAL list;

	COM_ERROR_IF_FALSE(!gameObjID.empty(), "the input ID is empty");

	try
	{
		// try to find this game object in the game objects GLOBAL list
		auto iterator = GetIteratorByID(gameObjectsGlobalList_, gameObjID);

		// if we got a correct iterator
		if (iterator != gameObjectsGlobalList_.end())
		{
			// add it into the default game objects list
			auto res = defaultGameObjectsList_.insert({ iterator->first, iterator->second });

			if (!res.second)   // if the game object wasn't inserted
			{
				std::string errorMsg{ "can't insert a game object (" + gameObjID + ") into the DEFAULT game objects list" };
				COM_ERROR_IF_FALSE(false, errorMsg);
			}
		}
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(THIS_FUNC, "can't set a game object as default");
	}

	return;

} // end SetGameObjectAsDefaultByID






////////////////////////////////////////////////////////////////////////////////////////////
//                            PUBLIC REMOVERS/DELETERS
////////////////////////////////////////////////////////////////////////////////////////////


void GameObjectsListClass::RemoveGameObjectByID(const std::string & gameObjID)
{
	// this function removes a game object by its id at all (deletes from the memory)

	COM_ERROR_IF_FALSE(!gameObjID.empty(), "the input ID is empty");

	// check if we have such an id in the game objects GLOBAL list
	auto iterator = GetIteratorByID(gameObjectsGlobalList_, gameObjID);

	// if we got a correct iterator
	if (iterator != gameObjectsGlobalList_.end())
	{
		gameObjectsGlobalList_.erase(iterator->first);

		// if we had this game object in the rendering list / default
		// game objects list we also remove it from there
		gameObjectsRenderingList_.erase(iterator->first);
		defaultGameObjectsList_.erase(iterator->first);

		// delete the game object object
		_DELETE(gameObjectsGlobalList_[gameObjID]);
	}
	

	return;

} // end RemoveGameObjectByID

/////////////////////////////////////////////////

void GameObjectsListClass::DontRenderGameObjectByID(const std::string & gameObjID)
{
	// if we have a game object by such gameObjID we delete it from the 
	// game objects rendering list
	// but if we can't find such a game object we throw an exception about it

	assert(!gameObjID.empty());

	// check if we have such an id in the game objects RENDERING list
	auto iterator = GetIteratorByID(gameObjectsRenderingList_, gameObjID);

	// if we got a correct iterator
	if (iterator != gameObjectsRenderingList_.end())
	{
		// and remove it from the rendering list
		gameObjectsRenderingList_.erase(iterator->first);  
	}

	return;

} // end DontRenderGameObjectByID







////////////////////////////////////////////////////////////////////////////////////////////
//
//                         PRIVATE FUNCTIONS (HELPERS)
//
////////////////////////////////////////////////////////////////////////////////////////////

std::string GameObjectsListClass::GenerateNewKeyInMap(
	const std::map<std::string, GameObject*> & map, 
	const std::string & key)
{
	// this function generates a new key (ID) for a game object in the map;
	//
	// INPUT: takes as input a map and an old key (which is already exists in this map
	// so that we have to generate a new one)
	//
	// RETURN: a new generated key 

	COM_ERROR_IF_FALSE(!key.empty(), "the input key is empty");

	// an index for concatenation it with the origin key ( for example: old_key(1) )
	size_t copyIndex = 1;

	// try to make a new key which is based on the original one
	std::string newKey{ key + '(' + std::to_string(copyIndex) + ')'};

	// while we have the same key in the map we will generate a new
	while (map.find(newKey) != map.end())
	{
		++copyIndex;
		newKey = { key + '(' + std::to_string(copyIndex) + ')' }; // generate a new key
	}

	return newKey;

} // end GenerateNewKeyInMap

/////////////////////////////////////////////////

std::_Tree_const_iterator<std::_Tree_val<std::_Tree_simple_types<std::pair<const std::string, GameObject*>>>>  
GameObjectsListClass::GetIteratorByID(const std::map<std::string,
	GameObject*> & map,
	const std::string & gameObjID) const
{
	// this function searches a game object in the map and returns an iterator to it;

	// try to find a game object by its ID in the map
	auto iterator = map.find(gameObjID);

	// if we didn't find any data by the key (ID)
	if (iterator == map.end())
	{
		std::string errorMsg{ "there is no game object with such an id: " + gameObjID };
		Log::Error(THIS_FUNC, errorMsg.c_str());
	}

	// return the iterator in any case
	return iterator;  

} // end GetIteratorByID