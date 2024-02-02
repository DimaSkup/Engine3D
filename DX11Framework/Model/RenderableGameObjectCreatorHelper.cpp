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

	GameObject* rawPtrToGameObj = nullptr;

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
		rawPtrToGameObj = pGameObjList->AddGameObject(std::move(pGameObj));

		// set this game object as default (add it into the list of defaults)
		pGameObjList->SetGameObjectAsDefaultByID(rawPtrToGameObj->GetID());

		Log::Debug(LOG_MACRO, "a default renderable game object: '" + rawPtrToGameObj->GetID() + "' was created");

	}
	/////////////////////////////////////////////
	catch (std::bad_alloc & e)
	{
		// print error messages
		Log::Error(LOG_MACRO, e.what());
		Log::Error(LOG_MACRO, "can't allocate memory for a default renderable game object; its ID: "
			                   + ((pGameObjForDefault) ? pGameObjForDefault->GetID() : "")); // concatenate an ID to the string if we can
	}
	/////////////////////////////////////////////
	catch (COMException & e)
	{
		// we have to try both pointers because if we catch this exception one of these
		// pointers is already incorrect
		const std::string ID { ((pGameObjForDefault) ? pGameObjForDefault->GetID() : "") + // we can get ID from here ...
			                   ((rawPtrToGameObj) ? rawPtrToGameObj->GetID() : "") };      // ... or from here

		Log::Error(e, true);
		Log::Error(LOG_MACRO, "can't init a default renderable game object; its ID: " + ID);

		// return false because we can't initialize this game object
		return false;
	}

	// return true since we've successfully create this default renderable game object
	return true;
}

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

	GameObject* rawPtrToGameObj = nullptr;

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
		rawPtrToGameObj = pGameObjList->AddGameObject(std::move(pGameObj));
		RenderableGameObject* rawPtrToRenderableGameObj = dynamic_cast<RenderableGameObject*>(rawPtrToGameObj);

		// setup this game object according to its type
		this->SetupRenderableGameObjByType((RenderableGameObject*)rawPtrToGameObj, rawPtrToGameObj->GetType());

		return (RenderableGameObject*)rawPtrToGameObj;
	}
	catch (const std::bad_cast & e)
	{
		Log::Error(LOG_MACRO, e.what());
		Log::Error(LOG_MACRO, "can't cast a game object to the RenderableGameObject type; its ID: " 
			                  + ((rawPtrToGameObj) ? rawPtrToGameObj->GetID() : ""));
		return false;
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(LOG_MACRO, e.what());
		Log::Error(LOG_MACRO, "can't allocate memory for a game object; its ID: "
			                  + ((rawPtrToGameObj) ? rawPtrToGameObj->GetID() : "");

		return false;
	}
	catch (COMException & e)
	{
		std::string exceptionMsg{  };

		// print error messages
		Log::Error(e, false);
		Log::Error(LOG_MACRO, "can't init a game object; its ID: ");

		return false;
	}
}

std::unique_ptr<RenderableGameObject> RenderableGameObjectCreatorHelper::MakeCopyOfRenderableGameObj(
	RenderableGameObject* pOriginGameObj);

void RenderableGameObjectCreatorHelper::SetupRenderableGameObjByType(
	const RenderableGameObject* pGameObj,
	const GameObject::GameObjectType type);