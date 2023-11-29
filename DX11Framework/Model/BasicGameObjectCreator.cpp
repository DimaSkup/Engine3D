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
		Log::Error(THIS_FUNC, e.what());
		COM_ERROR_IF_FALSE(false, "can't allocate memory for the game object creator members");
	}
}



////////////////////////////////////////////////////////////////////////////////////////////
//                             PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////





bool BasicGameObjectCreator::CreateDefaultGameObject(ID3D11Device* pDevice,
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

	GameObject* pGameObj = nullptr;
	Model* pModel = nullptr;

	try
	{
		// create a new empty instance of model
		pModel = this->GetInstance(pModelInitializer, pDevice, pDeviceContext);

		// get a path to the data file for this model
		std::string filePath{ this->pCreatorHelper_->GetPathToDataFile(pModel) };

		// make a relation between the model and some shader which will be used for
		// rendering this model (by default the rendering shader is a color shader)
		pModel->SetModelToShaderMediator(pModelToShaderMediator);
		pModel->SetRenderShaderName(renderingShaderName);

		// initialize the model according to its type loading data from the data file;
		// NOTE: some models types can initialize its data manually within its Initialize() function;
		bool result = pModel->Initialize(filePath);
		COM_ERROR_IF_FALSE(result, "can't initialize a default model object");

	
		///////////////////////////////////////////////

		// create a new game object and setup it
		pGameObj = new GameObject(pModel);

		// add this game object into the global game object list and set that this game object is default
		this->pGameObjectsList_->AddGameObject(pGameObj);
		this->pGameObjectsList_->SetGameObjectAsDefaultByID(pGameObj->GetID());

		std::string debugMsg{ "default game object: '" + pGameObj->GetID() + "' is created" };
		Log::Debug(THIS_FUNC, debugMsg.c_str());
	}
	catch (std::bad_alloc & e)
	{
		std::string exceptionMsg{ "can't allocate memory for some default model / game object" };
		exceptionMsg += pCreatorHelper_->TryToGetModelType_WhenException(pModel);

		// print error messages
		Log::Error(THIS_FUNC, e.what());
		Log::Error(THIS_FUNC, exceptionMsg.c_str());

		COM_ERROR_IF_FALSE(false, exceptionMsg);
	}
	catch (COMException & e)
	{
		std::string exceptionMsg{ "can't create and init some default model / game object" };
		exceptionMsg += pCreatorHelper_->TryToGetModelType_WhenException(pModel);

		// print error messages
		Log::Error(e, true);
		Log::Error(THIS_FUNC, exceptionMsg.c_str());

		return false;
	}

	return true;

} // end CreateDefaultGameObject

  ///////////////////////////////////////////////////////////

GameObject* BasicGameObjectCreator::CreateNewGameObject(ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,
	ModelInitializerInterface* pModelInitializer,
	ModelToShaderMediatorInterface* pModelToShaderMediator,
	const std::string & filePath,                               // path to model's data file which is used for importing this model   
	const std::string & renderShaderName,                       // name of a shader which will be used for rendering a model
	const bool isZoneElement)                                   // this flag defines if this game object is a zone element (terrain, sky_dome / sky_box, sky_plane, etc.)
{
	// check input params
	assert(pModelInitializer != nullptr);
	assert(pModelToShaderMediator != nullptr);
	assert(filePath.empty() != true);
	assert(renderShaderName.empty() != true);

	GameObject* pGameObj = nullptr;
	Model* pModel = nullptr;

	try
	{
		// create a new empty instance of model
		pModel = this->GetInstance(pModelInitializer, pDevice, pDeviceContext);

		///////////////////////////////////////////////

		// make a relation between the model and some shader which will be used for
		// rendering this model (by default the rendering shader is a color shader)
		pModel->SetModelToShaderMediator(pModelToShaderMediator);
		pModel->SetRenderShaderName(renderShaderName);

		// initialize the model loading its data from the data file by filePath;
		// NOTE: some models types can initialize its data manually within its Initialize() function so it doesn't use the filePath variable;
		bool result = pModel->Initialize(filePath);
		COM_ERROR_IF_FALSE(result, "can't initialize a model object");

		///////////////////////////////////////////////

		// create a new game object and setup it
		pGameObj = new GameObject(pModel);

		// add this game object into the GLOBAL list of all game objects and
		// into the rendering list as well
		this->pGameObjectsList_->AddGameObject(pGameObj);

		// if this game object is not a zone element we put it into the rendering list;
		// in another case we do nothing because we render zone elements in a separate way
		// withing the ZoneClass;
		if (!isZoneElement)
		{
			this->pGameObjectsList_->SetGameObjectForRenderingByID(pGameObj->GetID());
		}

		std::string debugMsg{ "game object: '" + pGameObj->GetID() + "' is created" };
		Log::Debug(THIS_FUNC, debugMsg.c_str());
	}
	catch (std::bad_alloc & e)
	{
		std::string exceptionMsg{ "can't allocate memory for a model / game object" };
		exceptionMsg += pCreatorHelper_->TryToGetModelType_WhenException(pModel);

		// print error messages
		Log::Error(THIS_FUNC, e.what());
		Log::Error(THIS_FUNC, exceptionMsg.c_str());

		COM_ERROR_IF_FALSE(false, exceptionMsg);
	}
	catch (COMException & e)
	{
		std::string exceptionMsg{ "can't create and init some model / game object" };
		exceptionMsg += pCreatorHelper_->TryToGetModelType_WhenException(pModel);

		// print error messages
		Log::Error(e, false);
		Log::Error(THIS_FUNC, exceptionMsg.c_str());

		COM_ERROR_IF_FALSE(false, exceptionMsg);
	}

	// return a pointer to the new game object 
	return pGameObj;

} // end CreateNewGameObject

  ///////////////////////////////////////////////////////////

GameObject* BasicGameObjectCreator::CreateCopyOfGameObject(GameObject* pOriginGameObj)
{
	// this function creates and initializes a copy of the input game object

	// check input params
	COM_ERROR_IF_NULLPTR(pOriginGameObj, "the input game object == nullptr");
	COM_ERROR_IF_NULLPTR(pOriginGameObj->GetModel(), "a model of the input game object == nullptr");

	GameObject* pGameObj = nullptr;
	Model* pModel = nullptr;

	try
	{
		// create a new empty instance of model
		pModel = this->GetInstance(pOriginGameObj->GetModel()->GetModelInitializer(),
			pOriginGameObj->GetModel()->GetDevice(),
			pOriginGameObj->GetModel()->GetDeviceContext());

		// copy data from the origin model into the new one
		// (copying of the vertex/index buffers, and other data as well)
		*pModel = *pOriginGameObj->GetModel();

		// create a new game object and setup it
		pGameObj = new GameObject(pModel);

		// add this game object into the GLOBAL list of all game objects and
		// into the rendering list as well
		this->pGameObjectsList_->AddGameObject(pGameObj);
		this->pGameObjectsList_->SetGameObjectForRenderingByID(pGameObj->GetID());

		std::string debugMsg{ " game object: '" + pGameObj->GetID() + "' is created" };
		Log::Debug(THIS_FUNC, debugMsg.c_str());
	}
	catch (std::bad_alloc & e)
	{
		// if smth went wrong we clean up memory
		_DELETE(pModel);
		_DELETE(pGameObj);

		std::string exceptionMsg{ "can't allocate memory for a model / game object" };
		exceptionMsg += pCreatorHelper_->TryToGetModelType_WhenException(pModel);

		// print error messages
		Log::Error(THIS_FUNC, e.what());
		Log::Error(THIS_FUNC, exceptionMsg.c_str());

		COM_ERROR_IF_FALSE(false, exceptionMsg);
	}
	catch (COMException & e)
	{
		std::string exceptionMsg{ "can't create copy of a game object" };
		exceptionMsg += pCreatorHelper_->TryToGetModelType_WhenException(pModel);

		// print error messages
		Log::Error(e, true);
		Log::Error(THIS_FUNC, exceptionMsg.c_str());

		COM_ERROR_IF_FALSE(false, exceptionMsg);
	}

	// return a pointer to the new game object (copy of the origin)
	return pGameObj;

} // end CreateCopyOfGameObject



