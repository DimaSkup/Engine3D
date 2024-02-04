#include "RenderableGameObjectCreatorHelper.h"

#include "../Engine/Settings.h"





void RenderableGameObjectCreatorHelper::InitializeRenderableGameObject(
	RenderableGameObject* pGameObjForInitialization,
	ModelToShaderMediatorInterface* pModelToShaderMediator,
	const std::string & renderShaderName,                     // name of a shader which will be used for rendering a model)
	const std::string & filePath)                             // path to model's data file which is used for importing this model   
{
	//
	// this function creates, initializes, and setups a new model object by the filePath
	//

	bool result = false;

	try
	{
		Model* pModel = static_cast<Model*>(pGameObjForInitialization);

		// make a relation between the model and some shader which will be used for
		// rendering this model (by default the rendering shader is a color shader)
		pModel->SetModelToShaderMediator(pModelToShaderMediator);
		pModel->SetRenderShaderName(renderShaderName);

		if (filePath.empty())
		{
			// if the input file path is empty it means that we want to create
			// a default renderable game object (for instance: cube, sphere, etc.)
			// so we have to create a path to data file manually according to the model's type

			// generate a path to the data file for this model by its model type
			const std::string defaultModelsDirPath{ Settings::Get()->GetSettingStrByKey("DEFAULT_MODELS_DIR_PATH") };
			const std::string defaultModelsExt{ ".obj" };

			// return the generated path
			const std::string dataFilePath { defaultModelsDirPath + pGameObjForInitialization->GetModelType() + defaultModelsExt };

			// initialize the game object loading its data from the data file by filePath;
			result = pGameObjForInitialization->Initialize(dataFilePath);
		}
		else
		{
			// the input path to data file is correct so just initialize a model
			result = pGameObjForInitialization->Initialize(filePath);
		}

		// check if we successfully initialized the model
		COM_ERROR_IF_FALSE(result, "can't initialize a game object from file: " + filePath);

		////////////////////////////////////////////////////////////////

	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		COM_ERROR_IF_FALSE(result, "can't initialize the input game object");
	}
}

///////////////////////////////////////////////////////////

bool RenderableGameObjectCreatorHelper::CreateDefaultRenderableGameObject(
	RenderableGameObject* pGameObjForDefault,
	ModelToShaderMediatorInterface* pModelToShaderMediator,
	const std::string & renderingShaderName)
{
	// INPUT: 1. A ptr to the game object (model)
	//        2. A name of the shader which will be used for rendering this game obj
	//         
	//
	// creates a default (cube, sphere, etc.) game object which will be 
	// used for creating other game objects of this type (for instance: we won't need to 
	// read model data from its data file each time when we create a game object, 
	// so we just copy model data from this default game object for the sake of speed);
	//
	// NOTE: this default model won't be rendered after creation;

	// check input params
	COM_ERROR_IF_NULLPTR(pGameObjForDefault, "the input ptr to the game object == nullptr");
	COM_ERROR_IF_NULLPTR(pModelToShaderMediator, "the input ptr to the model_to_shader_mediator == nullptr");
	COM_ERROR_IF_ZERO(renderingShaderName.size(), "the input name of a shader is empty");

	// will contain an ID of the game object if it has already been added to the game object list
	std::string gameObjID{ "" };

	try
	{
		// first of all we have to initialize the renderable game obj (model)
		this->InitializeRenderableGameObject(pGameObjForDefault, pModelToShaderMediator,renderingShaderName, "");

		// create a unique_ptr to the input game object
		std::unique_ptr<RenderableGameObject> pGameObj = std::make_unique<RenderableGameObject>(pGameObjForDefault);

		// get a ptr to the game objects list so we can add a new game object into it
		GameObjectsListClass* pGameObjList = GameObjectsListClass::Get();

		// add this game object into the global game object;
		//
		// NOTE: the list takes an ownership about this game object
		gameObjID = pGameObjList->AddGameObject(std::move(pGameObj));

		// set this game object as default (add it into the list of defaults)
		pGameObjList->SetGameObjectAsDefaultByID(gameObjID);

		Log::Debug(LOG_MACRO, "a default renderable game object: '" + gameObjID + "' was created");

	}
	/////////////////////////////////////////////
	catch (std::bad_alloc & e)
	{
		this->HandleBadAllocException(e, pGameObjForDefault, gameObjID);
		return false;
	}
	catch (COMException & e)
	{
		this->HandleCOMException(e, pGameObjForDefault, gameObjID);
		return false;
	}

	// return true since we've successfully create this default renderable game object
	return true;
}

///////////////////////////////////////////////////////////

RenderableGameObject* RenderableGameObjectCreatorHelper::CreateNewRenderableGameObject(
	RenderableGameObject* pNewRenderableGameObj,
	ModelToShaderMediatorInterface* pModelToShaderMediator,
	const std::string & renderingShaderName,
	const std::string & filePath,
	const std::string & gameObjID = "")    // make such an ID for this game object inside the game objects list                                  
{

	// check input params
	COM_ERROR_IF_NULLPTR(pNewRenderableGameObj, "the input ptr to the game object == nullptr");
	COM_ERROR_IF_NULLPTR(pModelToShaderMediator, "the input ptr to the model_to_shader_mediator == nullptr");
	COM_ERROR_IF_ZERO(renderingShaderName.size(), "the input name of a shader is empty");
	COM_ERROR_IF_ZERO(filePath.size(), "the input filePath is empty");

	// will contain an ID of the game object if it has already been added to the game object list
	std::string gameObjID{ "" };

	try
	{
		// first of all we have to initialize the renderable game obj (model)
		this->InitializeRenderableGameObject(pNewRenderableGameObj, pModelToShaderMediator, renderingShaderName, "");

		///////////////////////////////////////////////

		// create a unique_ptr to the input game object
		std::unique_ptr<RenderableGameObject> pGameObj = std::make_unique<RenderableGameObject>(pNewRenderableGameObj);

		// if we want to have some particular ID for this game obj;
		// in another case we use the default ID according to the game object's type (look at the constructor of game object)
		if (!gameObjID.empty())	pGameObj->SetID(gameObjID);

		// get a ptr to the game objects list so we can add a new game object into it
		GameObjectsListClass* pGameObjList = GameObjectsListClass::Get();

		// add this game object into the GLOBAL list of all game objects
		//
		// NOTE: the list takes an ownership about this game object
		gameObjID = pGameObjList->AddGameObject(std::move(pGameObj));
		
		// setup this game object according to its type
		this->SetupRenderableGameObjByType(gameObjID);

		// return a RenderableGameObject ptr to this game object
		return pGameObjList->GetRenderableGameObjByID(gameObjID);
	}
	catch (std::bad_alloc & e)
	{
		this->HandleBadAllocException(e, pNewRenderableGameObj, gameObjID);
		return false;
	}
	catch (COMException & e)
	{
		this->HandleCOMException(e, pNewRenderableGameObj, gameObjID);
		return false;
	}
}

///////////////////////////////////////////////////////////

RenderableGameObject* RenderableGameObjectCreatorHelper::MakeCopyOfRenderableGameObj(
	RenderableGameObject* pOriginGameObj)
{
	// THIS FUNCTION creates and initializes a copy of the input game object

	// check input params
	COM_ERROR_IF_NULLPTR(pOriginGameObj, "the input game object == nullptr");

	// a ptr to the created copy of the input game object
	std::unique_ptr<GameObject> pCopiedGameObj;

	try
	{
		pCopiedGameObj = this->MakeCopyOfRenderableGameObj(pOriginRenderableGameObj);

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
}

///////////////////////////////////////////////////////////

void RenderableGameObjectCreatorHelper::SetupRenderableGameObjByType(
	const std::string & renderableGameObjID)
{
	return;
}


///////////////////////////////////////////////////////////////////////////////////////////
//
//                                 PRIVATE FUNCTIONS
//
///////////////////////////////////////////////////////////////////////////////////////////


void RenderableGameObjectCreatorHelper::HandleBadAllocException(
	const std::bad_alloc & e,
	RenderableGameObject* pGameObj,             // contains a ptr to the game object if it hasn't been added to the game object list
	const std::string & gameObjID)              // contains an ID of the game object if it has already been added to the game object list
{
	std::string errorMsg{ "can't allocate memory for a game object; its ID: " };
	errorMsg += ((pGameObj) ? pGameObj->GetID() : "");   
	errorMsg += gameObjID;

	// print error messages
	Log::Error(LOG_MACRO, e.what());
	Log::Error(LOG_MACRO, errorMsg);

	return;
}

///////////////////////////////////////////////////////////

void RenderableGameObjectCreatorHelper::HandleCOMException(
	COMException & e,
	RenderableGameObject* pGameObj,             // contains a ptr to the game object if it hasn't been added to the game object list
	const std::string & gameObjID)              // contains an ID of the game object if it has already been added to the game object list
{
	std::string errorMsg{ "can't initializer a game object; its ID: " };
	errorMsg += ((pGameObj) ? pGameObj->GetID() : "");
	errorMsg += gameObjID;

	// print error messages
	Log::Error(e, false);
	Log::Error(LOG_MACRO, errorMsg);

	return;
}