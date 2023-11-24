////////////////////////////////////////////////////////////////////
// Filename:     GameObjectsListClass.h
// Description:  this class has a couple of containers for GameObject elements;
//               these containers are used for different purposes:
//
//               1. global list of game objects (using this list we delete game objects from the memory)
//               2. list of default game objects
//               3. list of game objects for rendering on the scene
//               4. list of 2D sprites
//
//
// Created:     22.07.23
////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include "../Engine/Log.h"
#include "../Engine/macros.h"
#include "../Model/GameObject.h"

#include <DirectXMath.h>
#include <cstdlib>
#include <ctime>  // is needed for random generator
#include <map>




//////////////////////////////////
// Class name: GameObjectsListClass
//////////////////////////////////
class GameObjectsListClass final
{

public:
	GameObjectsListClass(void);
	~GameObjectsListClass(void);

	// get instance of this class
	static GameObjectsListClass* Get() { return pInstance_; }

	// using this function we generate random positions/colors/etc. for the game objects
	// on the scene;
	void GenerateRandomDataForGameObjects();

	// release memory
	void Shutdown(void);



	///////////////////////////////   GETTERS   ///////////////////////////////

	GameObject* GetGameObjectByID(const std::string & gameObjID);
	GameObject* GetZoneGameObjectByID(const std::string & gameObjID);
	GameObject* GetDefaultGameObjectByID(const std::string & gameObjID) const;

	// get a map of all the game objects on the scene 
	const std::map<std::string, GameObject*> & GetGameObjectsGlobalList() const;

	// get a map of game objects for rendering onto the scene
	const std::map<std::string, GameObject*> & GetGameObjectsRenderingList();  

	// get a map of 2D sprites
	const std::map<std::string, GameObject*> & GetSpritesRenderingList();

	// get a map of all the default game objects
	std::map<std::string, GameObject*> & GameObjectsListClass::GetDefaultGameObjectsList();


	///////////////////////////   SETTERS / ADDERS   ////////////////////////////

	void AddGameObject(GameObject* pGameObj);
	void AddZoneElement(GameObject* pGameObj);
	void AddSprite(GameObject* pGameObj);

	void SetGameObjectForRenderingByID(const std::string& gameObjID);
	void SetGameObjectAsDefaultByID(const std::string& gameObjID);


	// DELETERS
	void RemoveGameObjectByID(const std::string& gameObjID);     // delete a game object by id at all
	void DontRenderGameObjectByID(const std::string& gameObjID); // set that we don't want to render a game object by this id


private:  // restrict a copying of this class instance
	GameObjectsListClass(const GameObjectsListClass & obj);
	GameObjectsListClass & operator=(const GameObjectsListClass & obj);

private:
	// generates a new key which is based on the passed one
	std::string GenerateNewKeyInMap(const std::map<std::string, GameObject*> & map, const std::string & key);  

	// searches a GameObject in the map and returns an iterator to it;
	std::_Tree_const_iterator<std::_Tree_val<std::_Tree_simple_types<std::pair<const std::string, GameObject*>>>> GetIteratorByID(const std::map<std::string, GameObject*> & map, const std::string & gameObjID) const;

private:
	std::map<std::string, GameObject*> gameObjectsGlobalList_;      // all the loaded game objects of the project
	std::map<std::string, GameObject*> gameObjectsRenderingList_;   // [game_obj_id => game_obj_ptr] pairs for rendering on the scene
	std::map<std::string, GameObject*> zoneGameObjectsList_;        // [game_obj_id => game_obj_ptr] pairs of zone elements
	std::map<std::string, GameObject*> defaultGameObjectsList_;     // contains a pointers to the default game objects (cubes, spheres, triangles, etc.)
	std::map<std::string, GameObject*> spritesRenderingList_;

	static GameObjectsListClass* pInstance_;                        // a pointer to the current instance of the game objects list class
};
