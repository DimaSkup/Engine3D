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
#include "../Model/RenderableGameObject.h"

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
	void GenerateRandomDataForRenderableGameObjects();

	// release memory
	void Shutdown(void);



	///////////////////////////////   GETTERS   ///////////////////////////////

	//CameraGameObject* GetCameraGameObjByID(const std::string & cameraID);
	//LightSourceGameObject* GetLightSrcGameObjByID(const std::string & lightID);

	RenderableGameObject* GetRenderableGameObjByID(const std::string & gameObjID);
	RenderableGameObject* GetZoneGameObjectByID(const std::string & gameObjID);
	RenderableGameObject* GetSpriteByID(const std::string & gameObjID);
	RenderableGameObject* GetDefaultGameObjectByID(const std::string & gameObjID) const;


	// get a map of game objects for rendering onto the scene
	const std::map<std::string, std::unique_ptr<RenderableGameObject>> & GetGameObjectsRenderingList();

	// get a map of 2D sprites
	const std::map<std::string, std::unique_ptr<RenderableGameObject>> &  GetSpritesRenderingList();


	///////////////////////////   SETTERS / ADDERS   ////////////////////////////

	//std::string AddCameraGameObj(std::unique_ptr<CameraGameObject> gameObj);
	//std::string AddLightSourceGameObj(std::unique_ptr<LightSourceGameObject> gameObj);
	std::string AddRenderableGameObj(std::unique_ptr<RenderableGameObject> pGameObj);
	std::string AddRenderableZoneElementGameObj(std::unique_ptr<RenderableGameObject> pGameObj);
	std::string AddRenderableSpriteGameObj(std::unique_ptr<RenderableGameObject> pGameObj);
	std::string AddRenderableDefaultGameObj(std::unique_ptr<RenderableGameObject> pGameObj);

	// DELETERS
	void DeleteGameObjectByID(const std::string & gameObjID);     // delete a game object by id at all
	void DontRenderGameObjectByID(const std::string & gameObjID); // set that we don't want to render a game object by this id


private:  // restrict a copying of this class instance
	GameObjectsListClass(const GameObjectsListClass & obj);
	GameObjectsListClass & operator=(const GameObjectsListClass & obj);

private:
	bool SetRenderableGameObjectIntoList(
		std::unique_ptr<RenderableGameObject> gameObj,
		std::map<std::string, std::unique_ptr<RenderableGameObject>> & map);

	// generates a new key which is based on the passed one
	std::string GenerateNewKeyInGlobalListMap(const std::string & key);


private:
	//std::map<std::string, CameraGameObject*>      cameraGameObjectsList_;
	//std::map<std::string, LightSourceGameObject*> lightSourcesGameObjectsList_;
	std::map<std::string, std::unique_ptr<RenderableGameObject>> renderableGameObjectsList_;   // [game_obj_id => game_obj_ptr] pairs for rendering on the scene; this list has ownership about renderable_game_objects
	std::map<std::string, std::unique_ptr<RenderableGameObject>> zoneGameObjectsList_;        // [game_obj_id => game_obj_ptr] pairs of zone elements
	std::map<std::string, std::unique_ptr<RenderableGameObject>> defaultGameObjectsList_;     // contains a pointers to the default game objects (cubes, spheres, triangles, etc.)
	std::map<std::string, std::unique_ptr<RenderableGameObject>> spritesList_;

	static GameObjectsListClass* pInstance_;                        // a pointer to the current instance of the game objects list class
};
