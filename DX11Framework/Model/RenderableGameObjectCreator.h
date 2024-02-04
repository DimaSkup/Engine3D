#pragma once

#include "GameObjectFactory.h"



template<class T>
class RenderableGameObjectCreator : public GameObjectFactory
{
public:
	RenderableGameObjectCreator(GameObjectsListClass* pGameObjList,
		ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext,
		ModelInitializerInterface* pModelInitializer,
		ModelToShaderMediatorInterface* pModelToShaderMediator);

	virtual GameObject* Create() = 0;                                 // create a new game object
	virtual GameObject* CreateCopyOf(GameObject* pOriginGameObj) = 0; // create a copy of the origin game obj


private:
	



	///////////////////////////////////////////////////////
	void InitializeAllDefaultRenderableGameObjects();

	// create usual renderable game object
	RenderableGameObject* CreateLine3D(const DirectX::XMFLOAT3 & startPos, const DirectX::XMFLOAT3 & endPos);
	RenderableGameObject* CreateTriangle();
	RenderableGameObject* CreateCube(RenderableGameObject* pOriginCube = nullptr);
	RenderableGameObject* CreateSphere(RenderableGameObject* pOriginSphere = nullptr);
	RenderableGameObject* CreatePlane();
	RenderableGameObject* CreateTree();
	RenderableGameObject* Create2DSprite(const std::string & setupFilename, const std::string & spriteID, const POINT & renderAtPos, const UINT screenWidth, const UINT screenHeight);
	RenderableGameObject* CreateGameObjectFromFile(const std::string & modelFilename, const std::string & gameObjID = "");

	// create the zone's elements
	RenderableGameObject* CreateTerrain();
	RenderableGameObject* CreateSkyDome();
	RenderableGameObject* CreateSkyPlane();



private:
	ModelInitializerInterface*      pModelInitializer_ = nullptr;
	ModelToShaderMediatorInterface* pModelToShaderMediator_ = nullptr;
};






////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                   PRIVATE FUNCTIONS
//
////////////////////////////////////////////////////////////////////////////////////////////////


template<class T>
Model* RenderableGameObjectCreator<T>::InitializeModelForRenderableGameObj(
	const std::string & renderShaderName,                     // name of a shader which will be used for rendering a model)
	const std::string & filePath)                             // path to model's data file which is used for importing this model   
{

}

///////////////////////////////////////////////////////////

