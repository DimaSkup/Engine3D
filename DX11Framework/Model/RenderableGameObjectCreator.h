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
	Model* RenderableGameObjectCreator<T>::InitializeModelForRenderableGameObj(
		const std::string & renderShaderName,                     // name of a shader which will be used for rendering a model)
		const std::string & filePath)                             // path to model's data file which is used for importing this model   


	bool CreateDefaultRenderableGameObject(const std::string & renderShaderName);

	RenderableGameObject* CreateNewRenderableGameObject(const std::string & filePath,
		const std::string & renderShaderName,
		const GameObject::GameObjectType type,
		const std::string & gameObjID = "");     // make such an ID for this game object inside the game objects list                                  

	std::unique_ptr<RenderableGameObject> MakeCopyOfRenderableGameObj(RenderableGameObject* pOriginGameObj);

	void SetupRenderableGameObjByType(RenderableGameObject* pGameObj,
		                              const GameObject::GameObjectType type);



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
	//
	// this function creates, initializes, and setups a new model object by the filePath
	//

	Model* pModel = nullptr;
	bool result = false;

	try
	{
		// create a new empty instance of model
		pModel = new T(pDevice, pDeviceContext);

		// make a relation between the model and some shader which will be used for
		// rendering this model (by default the rendering shader is a color shader)
		pModel->SetModelToShaderMediator(pModelToShaderMediator_);
		pModel->SetRenderShaderName(renderShaderName);

		if (filePath.empty())
		{
			// if the input file path is empty it means that we want to create
			// a default renderable game object (for instance: cube, sphere, etc.)
			// so we have to create a path to data file manually according to the model's type

			// get a path to the data file for this model
			std::string dataFilePath{ this->pCreatorHelper_->GetPathToDataFile(pModel) };

			// initialize the model loading its data from the data file by filePath;
			result = pModel->Initialize(dataFilePath, pModelInitializer_);
		}
		else
		{
			// the input path to data file is correct so just initialize a model
			result = pModel->Initialize(filePath, pModelInitializer_);
		}

		COM_ERROR_IF_FALSE(result, "can't initialize a model from file: " + filePath);

		////////////////////////////////////////////////////////////////


		// return a ptr to the create model object
		return pModel;
	}
	catch (const std::bad_alloc & e)
	{
		Log::Error(LOG_MACRO, e.what());
		COM_ERROR_IF_FALSE(false, "can't allocate memory for the model object");
	}
}

///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////

template<class T>
bool RenderableGameObjectCreator<T>::CreateDefaultRenderableGameObject(
	const std::string & renderingShaderName)
{
	// INPUT: a name of the shader which will be used for rendering this game obj
	//
	// creates a default (cube, sphere, etc.) game object which will be 
	// used for creating other game objects of this type (for instance: we won't need to 
	// read model data from its data file each time when we create a game object, 
	// so we just copy model data from this default game object for the sake of speed);
	//
	// NOTE: this default model won't be rendered after creation;

	// check input params
	COM_ERROR_IF_ZERO(renderingShaderName.size(), "the input shader name is empty");

	try
	{
		// create a model object
		Model* pModel = this->InitializeModelForRenderableGameObj(renderingShaderName);

		// create a new game object and setup it with the model
		std::unique_ptr<RenderableGameObject> pGameObj = std::make_unique<RenderableGameObject>(pModel);

		// add this game object into the global game object list and set that this game object is default
		GameObject* rawPtrToGameObj = this->pGameObjectsList_->AddGameObject(std::move(pGameObj));

		// set this game object as default (add it into the list of defaults)
		this->pGameObjectsList_->SetGameObjectAsDefaultByID(rawPtrToGameObj->GetID());

		Log::Debug(LOG_MACRO, "a default renderable game object: '" + rawPtrToGameObj->GetID() + "' was created");
	}
	/////////////////////////////////////////////
	catch (std::bad_alloc & e)
	{
		// print error messages
		Log::Error(LOG_MACRO, e.what());
		COM_ERROR_IF_FALSE(false, "can't allocate memory for some default renderable game object");
	}
	/////////////////////////////////////////////
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(LOG_MACRO, "can't create and init some default renderable game object");

		return false;
	}

	// return true since we've successfully create this default renderable game object
	return true;

} // end CreateDefaultRenderableGameObject

///////////////////////////////////////////////////////////

