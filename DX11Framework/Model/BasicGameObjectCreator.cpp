#include "BasicGameObjectCreator.h"



BasicGameObjectCreator::BasicGameObjectCreator(GameObjectsListClass* pGameObjectsList)
{
	// check input params
	COM_ERROR_IF_NULLPTR(pGameObjectsList, "the input ptr to the game objects list == nullptr");

	try
	{
		this->pGameObjectsList_ = pGameObjectsList;
		this->pCreatorHelper_ = new GameObjectCreatorHelper;
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(LOG_MACRO, e.what());
		COM_ERROR_IF_FALSE(false, "can't allocate memory for the game object creator members");
	}
}



////////////////////////////////////////////////////////////////////////////////////////////
//                             PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////





bool BasicGameObjectCreator::CreateDefaultRenderableGameObject(ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,
	ModelInitializerInterface * pModelInitializer,
	ModelToShaderMediatorInterface * pModelToShaderMediator,
	const std::string & renderingShaderName)        // name of a shader which will be used for rendering a model
{
	// creates a default (cube, sphere, etc.) game object which will be 
	// used for creating other game objects of this type (for instance: we won't need to 
	// read model data from its data file each time when we create a game object, 
	// so we just copy model data from this default game object for the sake of speed);
	//
	// NOTE: this default model won't be rendered after creation;

	// check input params
	COM_ERROR_IF_NULLPTR(pModelInitializer, "the input ptr to model initializer == nullptr");
	COM_ERROR_IF_NULLPTR(pModelToShaderMediator, "the input ptr to mediator == nullptr");
	COM_ERROR_IF_FALSE(!renderingShaderName.empty(), "the input shader name is empty");


	try
	{
		// create a model object
		Model* pModel = this->InitializeModelForRenderableGameObj(pDevice, pDeviceContext,
			pModelInitializer,
			pModelToShaderMediator,
			renderingShaderName);

		// create a new game object and setup it with the model
		std::unique_ptr<RenderableGameObject> pGameObj = std::make_unique<RenderableGameObject>(pModel);

		pGameObj->SetType(GameObject::RENDERABLE_GAME_OBJ);

		// add this game object into the global game object list and set that this game object is default
		GameObject* rawPtrToGameObj = this->pGameObjectsList_->AddGameObject(std::move(pGameObj));
		this->pGameObjectsList_->SetGameObjectAsDefaultByID(rawPtrToGameObj->GetID());

		

		Log::Debug(LOG_MACRO, "a default renderable game object: '" + rawPtrToGameObj->GetID() + "' is created");
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

	return true;

} // end CreateDefaultRenderableGameObject

  ///////////////////////////////////////////////////////////

RenderableGameObject* BasicGameObjectCreator::CreateNewRenderableGameObject(ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,
	ModelInitializerInterface* pModelInitializer,
	ModelToShaderMediatorInterface* pModelToShaderMediator,
	const std::string & filePath,                               // path to model's data file which is used for importing this model   
	const std::string & renderShaderName,                       // name of a shader which will be used for rendering a model
	const GameObject::GameObjectType type,
	const std::string & gameObjID)                              // make such a key for this game object inside the game objects list                                  
{
	// check input params
	assert(pModelInitializer != nullptr);
	assert(pModelToShaderMediator != nullptr);
	assert(filePath.empty() != true);
	assert(renderShaderName.empty() != true);

	std::unique_ptr<RenderableGameObject> pGameObj;

	try
	{
		Model* pModel = this->InitializeModelForRenderableGameObj(pDevice, pDeviceContext,
			pModelInitializer,
			pModelToShaderMediator,
			renderShaderName,
			filePath);

		///////////////////////////////////////////////

		// create a new renderable game object and setup it
		pGameObj = std::make_unique<RenderableGameObject>(pModel);

		// if we want to have some particular ID for this game obj;
		// in another case we use the default ID according to the game object's type (look at the constructor of game object)
		if (!gameObjID.empty())	pGameObj->SetID(gameObjID);
		
		// add this game object into the GLOBAL list of all game objects
		//
		// NOTE: the list takes ownership
		RenderableGameObject* rawPtrToGameObj = static_cast<RenderableGameObject*>(this->pGameObjectsList_->AddGameObject(std::move(pGameObj)));

		// setup this game object according to its type
		this->SetupRenderableGameObjByType(rawPtrToGameObj, type);

		return rawPtrToGameObj;
		
	}
	catch (std::bad_alloc & e)
	{
		std::string exceptionMsg{ "can't allocate memory for a game object" };

		// print error messages
		Log::Error(LOG_MACRO, e.what());
		Log::Error(LOG_MACRO, exceptionMsg.c_str());

		COM_ERROR_IF_FALSE(false, exceptionMsg);
	}
	catch (COMException & e)
	{
		std::string exceptionMsg{ "can't create and init some game object" };

		// print error messages
		Log::Error(e, false);
		Log::Error(LOG_MACRO, exceptionMsg.c_str());

		COM_ERROR_IF_FALSE(false, exceptionMsg);
	}
} // end CreateNewRenderableGameObject

  ///////////////////////////////////////////////////////////

GameObject* BasicGameObjectCreator::CreateCopyOfGameObject(GameObject* pOriginGameObj)
{
	// this function creates and initializes a copy of the input game object

	// check input params
	COM_ERROR_IF_NULLPTR(pOriginGameObj, "the input game object == nullptr");

	// a ptr to the created copy of the input game object
	std::unique_ptr<GameObject> pGameObj;

	try
	{
		// try to cast the game object to the RenderableGameObject type:
		// if it is a RenderableGameObject (example: Cube, Sphere) type we copy
		// this game object in some particular way, 
		// not as usual game object (example of usual: CameraClass)
		RenderableGameObject* pOriginRenderableGameObj = dynamic_cast<RenderableGameObject*>(pOriginGameObj);

		pGameObj = this->MakeCopyOfRenderableGameObj(pOriginRenderableGameObj);	

		// add this game object into the GLOBAL list of all game objects and
		// into the rendering list as well
		//
		// NOTE: the list takes ownership
		GameObject* rawPtrToGameObj = this->pGameObjectsList_->AddGameObject(std::move(pGameObj)); 

		// setup this game object according to its type
		this->SetupRenderableGameObjByType(static_cast<RenderableGameObject*>(rawPtrToGameObj), pOriginGameObj->GetType());

		// return a raw ptr to the game object so we can use it for further setup
		return rawPtrToGameObj;
	}
	catch (const std::bad_cast & e)
	{
		// if we got this exception type it means that we want to create a copy of 
		// some basic game object (for instance: camera)
		// but not a copy of the renderable game object

		Log::Debug(LOG_MACRO, e.what());
		Log::Debug(LOG_MACRO, "copy of a not renderable game object: " + pOriginGameObj->GetID());

		pGameObj = std::make_unique<GameObject>(*pOriginGameObj);

		// add this game object into the GLOBAL list of all game objects 
		GameObject* rawPtrToGameObj = this->pGameObjectsList_->AddGameObject(std::move(pGameObj));

		// return a raw ptr to the game object so we can use it for further setup
		return rawPtrToGameObj;
	}

	////////////////////////////////////////////////

	catch (std::bad_alloc & e)
	{
		// print error messages
		Log::Error(LOG_MACRO, e.what());

		COM_ERROR_IF_FALSE(false, "can't allocate memory for a copy of the game object");
	}

	////////////////////////////////////////////////

	catch (COMException & e)
	{
		std::string exceptionMsg{ "can't create copy of a game object:" };
		exceptionMsg += pOriginGameObj->GetID();

		// print error messages
		Log::Error(e, true);
		Log::Error(LOG_MACRO, exceptionMsg.c_str());

		COM_ERROR_IF_FALSE(false, exceptionMsg);
	}

} // end CreateCopyOfGameObject

///////////////////////////////////////////////////////////

std::unique_ptr<RenderableGameObject> BasicGameObjectCreator::MakeCopyOfRenderableGameObj(RenderableGameObject* pOriginGameObj)
{
	// this function makes a copy of the input renderable game object

	const std::string debugMsg{ "copy a renderable game object: " + pOriginGameObj->GetID() };
	Log::Debug(LOG_MACRO, debugMsg.c_str());

	Model* pModel = nullptr;
	std::unique_ptr<RenderableGameObject> pGameObj;

	try
	{
		// create a new empty instance of model
		pModel = this->GetInstance(pOriginGameObj->GetModel()->GetModelInitializer(),
			pOriginGameObj->GetModel()->GetDevice(),
			pOriginGameObj->GetModel()->GetDeviceContext());

		// copy data from the origin model into the new one
		// (copying of the vertex/index buffers, and other data as well)
		*pModel = *pOriginGameObj->GetModel();

		// create a new game object and setup it with a model
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

Model* BasicGameObjectCreator::InitializeModelForRenderableGameObj(ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,
	ModelInitializerInterface* pModelInitializer,
	ModelToShaderMediatorInterface* pModelToShaderMediator,
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
		pModel = this->GetInstance(pModelInitializer, pDevice, pDeviceContext);

		if (filePath.empty())
		{
			// if the input file path is empty it means that we want to create
			// a default renderable game object (for instance: cube, sphere, etc.)
			// so we have to create a path to data file manually according to the model's type

			// get a path to the data file for this model
			std::string dataFilePath{ this->pCreatorHelper_->GetPathToDataFile(pModel) };

			// initialize the model loading its data from the data file by filePath;
			result = pModel->Initialize(dataFilePath);
		}
		else
		{
			// the input path to data file is correct so just initialize a model
			result = pModel->Initialize(filePath);
		}

		COM_ERROR_IF_FALSE(result, "can't initialize a model object");

		///////////////////////////////////////////////

		// make a relation between the model and some shader which will be used for
		// rendering this model (by default the rendering shader is a color shader)
		pModel->SetModelToShaderMediator(pModelToShaderMediator);
		pModel->SetRenderShaderName(renderShaderName);


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

void BasicGameObjectCreator::SetupRenderableGameObjByType(RenderableGameObject* pGameObj,
	const GameObject::GameObjectType type)
{
	//
	// setup this game object according to its type
	//

	switch (type)
	{
		case GameObject::ZONE_ELEMENT_GAME_OBJ:
		{
			// if this game object is not a zone element we put it into the rendering list;
			// in another case we do nothing because we render zone elements in a separate way
			// withing the ZoneClass;
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
			this->pGameObjectsList_->SetGameObjAsSprite(pGameObj);
			break;
		}
		default:
		{
			// this model isn't any king of renderable game objects
			// (for instance: camera) so we can't set it for rendering 
		}
	} // switch

	return;
}