#include "RenderableGameObjectCreator.h"


RenderableGameObjectCreator::RenderableGameObjectCreator(GameObjectsListClass* pGameObjList,
	ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,
	ModelInitializerInterface* pModelInitializer,
	ModelToShaderMediatorInterface* pModelToShaderMediator)
{
	// check input params
	COM_ERROR_IF_NULLPTR(pGameObjList, "the input ptr to the game objects list == nullptr");
	COM_ERROR_IF_NULLPTR(pDevice, "the input ptr to the device == nullptr");
	COM_ERROR_IF_NULLPTR(pDeviceContext, "the input ptr to the device context == nullptr");
	COM_ERROR_IF_NULLPTR(pModelInitializer, "the input ptr to model initializer == nullptr");
	COM_ERROR_IF_NULLPTR(pModelToShaderMediator, "the input ptr to mediator == nullptr");

	
	this->pGameObjectsList_ = pGameObjList;
	this->pDevice_ = pDevice;
	this->pDeviceContext_ = pDeviceContext;
	this->pModelInitializer_ = pModelInitializer;
	this->pModelToShaderMediator_ = pModelToShaderMediator;
}




////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                PUBLIC FUNCTIONS
//
////////////////////////////////////////////////////////////////////////////////////////////////

#if 0
void RenderableGameObjectCreator::InitializeAllDefaultRenderableGameObjects()
{
	// initialization of the default models which will be used for 
	// creation other basic models; 
	// for default models we use a color shader

	Log::Debug(LOG_MACRO);

	bool result = false;
	GameObject* pGameObj = nullptr;
	const std::string renderingShaderName{ "ColorShaderClass" };  // by default

																  
	try
	{
		// try to create and initialize internal default models

		// the default cube model
		result = this->CreateDefaultRenderableGameObject(
			new Model(pDevice_, pDeviceContext_),
			renderingShaderName);
		COM_ERROR_IF_FALSE(result, "can't initialize a default cube model");

		// the default sphere
		Log::Debug(LOG_MACRO, "creation of a default sphere model");
		result = this->CreateSphere();
		COM_ERROR_IF_FALSE(result, "can't initialize a default sphere model");


		/*

		// the default tree
		Log::Debug(LOG_MACRO, "creation of a default tree model");
		pTreeCreator_->CreateAndInitDefaultModel(pDevice_, pDeviceContext_, pGraphics_->pModelInitializer_);

		*/
	}
	catch (const std::bad_alloc & e)
	{
		Log::Error(LOG_MACRO, e.what());
		COM_ERROR_IF_FALSE(false, "can't allocate memory for some default model");
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		COM_ERROR_IF_FALSE(false, "can't initialize the default models");
	}

	Log::Debug("-------------------------------------------");
	return;

} // end CreateDefaultGameObjects
#endif 













////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                   PRIVATE FUNCTIONS
//
////////////////////////////////////////////////////////////////////////////////////////////////

template<class T>
bool RenderableGameObjectCreator::CreateDefaultRenderableGameObject(
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
	COM_ERROR_IF_FALSE(!renderingShaderName.empty(), "the input shader name is empty");

	RenderableGameObject* pDefaultGameObj = nullptr;

	try
	{
		// create a model object
		Model* pModel = this->InitializeModelForRenderableGameObj(renderingShaderName);

		// create a new game object and setup it with the model
		std::unique_ptr<RenderableGameObject> pGameObj = std::make_unique<RenderableGameObject>(pModel);

		// add this game object into the global game object list and set that this game object is default
		GameObject* rawPtrToGameObj = this->pGameObjectsList_->AddGameObject(std::move(pGameObj));

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

	return true;

} // end CreateDefaultRenderableGameObject


  ///////////////////////////////////////////////////////////

RenderableGameObject* RenderableGameObjectCreator::CreateLine3D(
	const DirectX::XMFLOAT3 & startPos,
	const DirectX::XMFLOAT3 & endPos)
{
	// create and initialize a line3D model

	RenderableGameObject* pGameObj = nullptr;

#if 0


	try
	{
		/////////////////////////  CREATE AND INIT A GAME OBJECT  /////////////////////////

		// create an instance of Line3D and get a pointer to the models list object
		Model* pModel = new Line3D(pGraphics_->pModelInitializer_,
			this->pDevice_,
			this->pDeviceContext_);

		Line3D* pLine = static_cast<Line3D*>(pModel);
		pLine->SetStartPoint(startPos);
		pLine->SetEndPoint(endPos);

		// initialize a model of the line
		bool result = pModel->Initialize("no_path");
		COM_ERROR_IF_FALSE(result, "can't initialize a Line3D object");

		// create a new game object and add a model into it
		pGameObj = new RenderableGameObject(pLine);


		///////////////////////// SETUP THE GAME OBJECT  /////////////////////////

		// setup the game object properties
		pGameObj->SetColor(1, 1, 1, 1);

		pGraphics_->pGameObjectsList_->AddGameObject(pGameObj);
		pGraphics_->pGameObjectsList_->SetGameObjectForRenderingByID(pGameObj->GetID());

		// make a relation between the model and some shader which will be used for
		// rendering this model (by default the rendering shader is a color shader)
		pModel->SetModelToShaderMediator(pGraphics_->pModelsToShaderMediator_);
		pModel->SetRenderShaderName("ColorShaderClass");

		// print a message about successful creation
		std::string msg{ pGameObj->GetID() + " is created" };
		Log::Debug(THIS_FUNC, msg.c_str());
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(THIS_FUNC, e.what());
		COM_ERROR_IF_FALSE(false, "can't allocate memory for the instance of Line3D");
	}
	catch (COMException & e)
	{
		std::string errorMsg{ "can't create the cube" };

		// try to get an ID of the failed game object
		if (pGameObj != nullptr)
		{
			errorMsg += ": " + pGameObj->GetID();
		}

		Log::Error(e, false);
		COM_ERROR_IF_FALSE(false, errorMsg);
	}
#endif
	return pGameObj;   // return a pointer to the created game object (line3D)

} // end CreateLine3D

  ///////////////////////////////////////////////////////////

RenderableGameObject* RenderableGameObjectCreator::CreateCube(RenderableGameObject* pOriginCube)
{
	// this function creates and initializes a new cube game object;
	// there is 2 cases:
	//   1. we want to create a new empty cube so we will create a copy of the default cube
	//   2. we want to create a copy of some another cube


	RenderableGameObject* pGameObj = nullptr;

	try
	{
		// if we didn't pass any cube model into the function 
		// we create a copy of the default cube
		if (pOriginCube == nullptr)
		{
			pOriginCube = this->pGameObjectsList_->GetDefaultGameObjectByID("cube");
		}

		
		// check if this game object has a correct model
		COM_ERROR_IF_NULLPTR(pOriginCube->GetModel(), "ptr to model == nullptr");

		// check if this game object has exactly a cube model
		bool result = (pOriginCube->GetModel()->GetModelType() == "cube");
		COM_ERROR_IF_FALSE(result, "the input game object hasn't a cube model");

	

		///////////////////////////////////////////////////

		// create a new cube game object
		pGameObj = dynamic_cast<RenderableGameObject*>(this->CreateCopyOfGameObject(pOriginCube));

		///////////////////////////////////////////////////

		// print message about success
		std::string debugMsg{ "cube '" + pGameObj->GetID() + "' is created" };
		Log::Debug(LOG_MACRO, debugMsg.c_str());

	}
	catch (COMException & e)
	{
		Log::Error(e, false);

		// if we wanted to create a copy of game object print a message about the origin game object
		Log::Error(LOG_MACRO, "can't create a copy of cube: " + pOriginCube->GetID());

		// try to get an ID of the failed game object
		COM_ERROR_IF_FALSE(false, "can't create the cube: " + pGameObj->GetID());
	}

	return pGameObj;   // return a pointer to the created game object

} // end CreateCube

///////////////////////////////////////////////////////////

RenderableGameObject* RenderableGameObjectCreator::CreateSphere(RenderableGameObject* pOriginSphere)
{
	// this function creates and initializes a new sphere game object;
	// there is 2 cases:
	//   1. we want to create a new empty sphere so we will create a copy of the default sphere
	//   2. we want to create a copy of some another sphere



	RenderableGameObject* pGameObj = nullptr;
	bool isCopyOfSphere = false;         // in case of error we will use this flag to get an ID of the origin game object

	try
	{
		// check input model's type (it must have a sphere type)
		if (pOriginSphere != nullptr)
		{
			// check if this game object has correct model
			COM_ERROR_IF_NULLPTR(pOriginSphere->GetModel(), "ptr to model == nullptr");

			bool result = (pOriginSphere->GetModel()->GetModelType() == "sphere");
			COM_ERROR_IF_FALSE(result, "the input model is not a sphere model");

			isCopyOfSphere = true;
		}

		// if we didn't pass any sphere model into the function 
		// we create a copy of the default sphere
		if (pOriginSphere == nullptr)
		{
			pOriginSphere = this->pGameObjectsList_->GetDefaultGameObjectByID("sphere");
		}

		///////////////////////////////////////////////////

		// create a new sphere game object
		pGameObj = dynamic_cast<RenderableGameObject*>(this->CreateCopyOfGameObject(pOriginSphere));

		///////////////////////////////////////////////////

		// print message about success
		std::string debugMsg{ "sphere '" + pGameObj->GetID() + "' is created" };
		Log::Debug(LOG_MACRO, debugMsg.c_str());

	}
	catch (COMException & e)
	{
		Log::Error(e, true);

		// if we wanted to create a copy of game object print a message about the origin game object
		if (isCopyOfSphere)
		{
			Log::Error(LOG_MACRO, "can't create a copy of sphere: " + pOriginSphere->GetID());
		}

		// try to get an ID of the failed model
		COM_ERROR_IF_FALSE(false, "can't create the sphere: " + pGameObj->GetID());
	}

	return pGameObj;   // return a pointer to the created model
} // end CreateSphere

#if 0
GameObject* RenderableGameObjectCreator::CreateTriangle()
{
	Log::Debug(THIS_FUNC_EMPTY);

	GameObject* pGameObj = nullptr;

	try
	{
		/////////////////////////  CREATE AND INIT A GAME OBJECT  /////////////////////////

		// create and init a model for a game object
		Model* pModel = pTriangleCreator_->CreateAndInitModel(pDevice_,
			pDeviceContext_,
			pGraphics_->pModelInitializer_,
			pGraphics_->pModelsToShaderMediator_,
			"no_path",
			"ColorShaderClass");

		// create a new game object and add a model into it
		pGameObj = new GameObject();
		pGameObj->SetModel(pModel);


		///////////////////////// SETUP THE GAME OBJECT  /////////////////////////

		// make a relation between the model and some shader which will be used for
		// rendering this model (by default the rendering shader is a color shader)
		pModel->SetModelToShaderMediator(pGraphics_->pModelsToShaderMediator_);
		pModel->SetRenderShaderName("ColorShaderClass");

		// print a message about successful creation
		std::string msg{ pGameObj->GetID() + " is created" };
		Log::Debug(THIS_FUNC, msg.c_str());
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(THIS_FUNC, e.what());
		COM_ERROR_IF_FALSE(false, "can't allocate memory for the instance of triangle");
	}
	catch (COMException & e)
	{
		std::string errorMsg{ "can't create the triangle" };

		// try to get an ID of the failed game object
		if (pGameObj != nullptr)
		{
			errorMsg += ": " + pGameObj->GetID();
		}

		Log::Error(e, false);
		COM_ERROR_IF_FALSE(false, errorMsg);
	}

	return pModel;   // return a pointer to the created model

} // end CreateTriangle

  /////////////////////////////////////////////////
#endif

RenderableGameObject* RenderableGameObjectCreator::CreatePlane()
{
	Log::Debug(LOG_MACRO);

	RenderableGameObject* pGameObj = nullptr;

	try
	{
		pGameObj = this->CreateNewRenderableGameObject(
			"no_path",              // the plane class creates data by itself (vertices/indices) so we don't need a path to the data file here
			"TextureShaderClass",   // the name of the shader class which will be used for rendering of this plane
			GameObject::RENDERABLE_GAME_OBJ);
	}
	catch (COMException & e)
	{
		Log::Error(e, false);

		//  try to get an ID of the failed game object
		const std::string gameObjID = (pGameObj != nullptr) ? pGameObj->GetID() : "";
		COM_ERROR_IF_FALSE(false, "can't create the plane: " + gameObjID);
	}


	return pGameObj;   // return a pointer to the created game object (plane)

} // end CreatePlane

///////////////////////////////////////////////////////////
#if 0
GameObject* RenderableGameObjectCreator::CreateTree()
{
	Log::Debug(THIS_FUNC_EMPTY);

	Model* pModel = nullptr;

	try
	{
		// as this model type (TreeModel) is default we have to get a path to the 
		// default models directory to get a data file
		std::string defaultModelsDirPath{ pEngineSettings_->GetSettingStrByKey("DEFAULT_MODELS_DIR_PATH") };

		// generate a path to the data file
		std::string filePath{ defaultModelsDirPath + modelType_ };

		pModel = pTreeCreator_->CreateAndInitModel(pDevice_,
			pDeviceContext_,
			pGraphics_->pModelInitializer_,
			pGraphics_->pModelsToShaderMediator_,
			filePath,
			"ColorShaderClass");
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		COM_ERROR_IF_FALSE(false, "can't create the tree: " + pModel->GetModelDataObj()->GetID());   //  try to get an ID of the failed model
	}

	return pModel;   // return a pointer to the created model

} // end CreateTree

  /////////////////////////////////////////////////
#endif

RenderableGameObject* RenderableGameObjectCreator::Create2DSprite(const std::string & setupFilename,
	const std::string & spriteID,
	const POINT & renderAtPos,
	const UINT screenWidth,
	const UINT screenHeight)
{
	Log::Debug(LOG_MACRO);

	RenderableGameObject* pGameObj = nullptr;

	// try to create and initialize a 2D sprite
	try
	{
		pGameObj = this->CreateNewRenderableGameObject(
			"no_path",                             // the 2D sprite class creates data by itself (vertices/indices) so we don't need a path to the data file here
			"SpriteShaderClass",                   // the name of the shader class which will be used (by default) for rendering all the 2D sprites 
			GameObject::SPRITE_GAME_OBJ,
			spriteID);

		// setupping of the sprite
		SpriteClass* pSprite = static_cast<SpriteClass*>(pGameObj->GetModel());
		pSprite->SetupSprite(renderAtPos, screenWidth, screenHeight, setupFilename);

	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(LOG_MACRO, "can't create a 2D sprite");
		COM_ERROR_IF_FALSE(false, "can't create a 2D sprite");
	}

	return pGameObj;

} // end Create2DSprite

  ///////////////////////////////////////////////////////////

RenderableGameObject* RenderableGameObjectCreator::CreateGameObjectFromFile(const std::string & filePath,
	const std::string & gameObjID)   // expected ID for this game object
{
	// this function IMPORTS some model from the outer model data file (by modelFilename)
	// and initializes a new internal model using this data

	Log::Debug(LOG_MACRO);

	// check input params
	assert(filePath.empty() != true);

	RenderableGameObject* pGameObj = nullptr;

	try
	{
		// create a model for the game object
		pGameObj = this->CreateNewRenderableGameObject(
			filePath,                                 // a path to the data file
			"TextureShaderClass",
			GameObject::RENDERABLE_GAME_OBJ,
			gameObjID);
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(LOG_MACRO, "can't import and create a new game obj with custom model");
		COM_ERROR_IF_FALSE(false, "can't import and create a new game obj with custom model");
	}

	return pGameObj;

} // CreateGameObjectFromFile

  ///////////////////////////////////////////////////////////


RenderableGameObject* RenderableGameObjectCreator::CreateTerrain()
{
	// this function creates and initializes a new terrain game object

	RenderableGameObject* pTerrainGameObj = nullptr;
	bool isZoneElement = true;

	try
	{
		// create a new terrain game object
		pTerrainGameObj = this->CreateNewRenderableGameObject(
			"no_path",
			"TerrainShaderClass",
			GameObject::ZONE_ELEMENT_GAME_OBJ);
#if 0
		// get a pointer to the terrain to setup its position, etc.
		TerrainClass* pTerrain = static_cast<TerrainClass*>(pTerrainGameObj->GetModel());

		// setup the terrain game object
		float terrainX_Pos = -pTerrain->GetWidth();
		float terrainY_Pos = -10.0f;                  // height in the world
		float terrainZ_Pos = -pTerrain->GetHeight();
#endif

		// move the terrain to the location it should be rendered at
		//pTerrainGameObj->GetData()->SetPosition(terrainX_Pos, terrainY_Pos, terrainZ_Pos);
		pTerrainGameObj->SetPosition(0, 0, 0);
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		COM_ERROR_IF_FALSE(false, "can't create the terrain");
	}

	return pTerrainGameObj;

} // end CreateTerrain

  ///////////////////////////////////////////////////////////

RenderableGameObject* RenderableGameObjectCreator::CreateSkyDome()
{
	RenderableGameObject* pSkyDomeGameObj = nullptr;
	bool isZoneElement = true;

	try
	{

		// as this model type (sky dome) is default we have to get a path to the 
		// default models directory to get a data file
		const std::string filePath{ "data/models/default/sky_dome.obj" };

		// create and initialize a sky dome model
		pSkyDomeGameObj = this->CreateNewRenderableGameObject(filePath,
			"SkyDomeShaderClass",
			GameObject::ZONE_ELEMENT_GAME_OBJ);
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		COM_ERROR_IF_FALSE(false, "can't create the sky dome");
	}

	return pSkyDomeGameObj;

} // end CreateSkyDome

  ///////////////////////////////////////////////////////////

RenderableGameObject* RenderableGameObjectCreator::CreateSkyPlane()
{
	RenderableGameObject* pSkyPlaneGameObj = nullptr;

	const std::string cloudTexture1{ "data/textures/cloud001.dds" };
	const std::string cloudTexture2{ "data/textures/cloud002.dds" };

	try
	{
		// create and initialize a sky plane model
		pSkyPlaneGameObj = this->CreateNewRenderableGameObject(
			"no_path",
			"SkyPlaneShaderClass",
			GameObject::ZONE_ELEMENT_GAME_OBJ);

		SkyPlaneClass* pSkyPlaneModel = static_cast<SkyPlaneClass*>(pSkyPlaneGameObj->GetModel());

		// after initialization we have to add cloud textures to the sky plane model
		bool result = pSkyPlaneModel->LoadCloudTextures(pDevice_, cloudTexture1, cloudTexture2);
		COM_ERROR_IF_FALSE(result, "can't load cloud textures for the sky plane model");
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		COM_ERROR_IF_FALSE(false, "can't create the sky plane model");
	}

	return pSkyPlaneGameObj;

} // end CreateSkyPlane

  ///////////////////////////////////////////////////////////