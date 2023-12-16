////////////////////////////////////////////////////////////////////
// Filename:     RenderGraphics.cpp
// Description:  there are functions for rendering graphics;
// Created:      01.01.23
// Revising:     01.01.23
////////////////////////////////////////////////////////////////////
#include "RenderGraphics.h"


RenderGraphics::RenderGraphics(GraphicsClass* pGraphics, 
	Settings* pSettings,
	ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext)
{
	Log::Debug(THIS_FUNC_EMPTY);

	assert(pGraphics != nullptr);
	assert(pSettings != nullptr);
	assert(pDevice != nullptr);
	assert(pDeviceContext != nullptr);
	
	try
	{
		// init local copies of pointers
		pGraphics_ = pGraphics;
		pDevice_ = pDevice;
		pDeviceContext_ = pDeviceContext;

		// the number of point light sources on the scene
		//numPointLights_ = pSettings->GetSettingIntByKey("NUM_POINT_LIGHTS");  
		windowWidth_    = pSettings->GetSettingIntByKey("WINDOW_WIDTH");
		windowHeight_   = pSettings->GetSettingIntByKey("WINDOW_HEIGHT");

		// resize point light data arrays according to the number of point light sources
		//arrPointLightsPositions_.resize(numPointLights_);
		//arrPointLightsColors_.resize(numPointLights_);
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(THIS_FUNC, "can't create an instance of the RenderGraphics class");
	}
}


RenderGraphics::~RenderGraphics()
{
	Log::Debug(THIS_FUNC_EMPTY);
}




////////////////////////////////////////////////////////////////////////////////////////////
//                             PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////



bool RenderGraphics::RenderModels(GraphicsClass* pGraphics, 
	HWND hwnd,
	int & renderCount, 
	float deltaTime)
{    
	// this function prepares and renders all the models on the scene

	

	// setup data container for shader with some common data before rendering of the scene
	pGraphics->pModelsToShaderMediator_->GetDataContainerForShaders()->cameraPos = pGraphics->GetCamera()->GetPositionFloat3();

	// local timer							
	DWORD dwTimeCur = GetTickCount();
	static DWORD dwTimeStart = dwTimeCur;

	// update the local timer
	float t = (dwTimeCur - dwTimeStart) / 1000.0f;

	Model* pModel = nullptr;    // a temporal pointer to a model
	renderCount = 0;            // set to zero as we haven't rendered models for this frame yet
	bool result = false;


	////////////////////////////////////////////////
	// SETUP LIGHT SOURCES
	////////////////////////////////////////////////

	// setup the colour of the diffuse light on the scene
	pGraphics_->arrDiffuseLights_[0]->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);

	// setup the diffuse light direction (sun direction)
	pGraphics_->arrDiffuseLights_[0]->SetDirection(cos(t / 2), -0.5f, sin(t / 2));


	////////////////////////////////////////////////
	// SETUP THE ZONE / TERRAIN / SKYBOX / etc.
	////////////////////////////////////////////////

	// renders models which are related to the terrain: the terrain, sky dome, trees, etc.

	result = pGraphics->pZone_->Render(renderCount,
		pGraphics->GetD3DClass(),
		deltaTime,
		pGraphics->arrDiffuseLights_,
		pGraphics->arrPointLights_);
	COM_ERROR_IF_FALSE(result, "can't render the zone");

	
	////////////////////////////////////////////////
	// RENDER MODELS
	////////////////////////////////////////////////

	// render different models (from the models list) on the scene
	this->RenderModelsObjects(renderCount);


	return true;
} // RenderModels()

///////////////////////////////////////////////////////////


bool RenderGraphics::RenderGUI(GraphicsClass* pGraphics, 
	SystemState* systemState, 
	const float deltaTime)
{
	// ATTENTION: do 2D rendering only when all 3D rendering is finished;
	// this function renders the engine/game GUI

	bool result = false;

	// if some rendering state has been updated we have to update some data for the GUI
	this->UpdateGUIData(systemState);
	

	// update user interface
	result = pGraphics->pUserInterface_->Frame(pGraphics->pD3D_->GetDeviceContext(), 
		//pGraphics->pSettingsList_, 
		systemState, 
		pGraphics->GetCamera()->GetPositionFloat3(),
		pGraphics->GetCamera()->GetRotationFloat3InDegrees());  
	COM_ERROR_IF_FALSE(result, "can't do frame calculations for the user interface");

	// render the user interface
	result = pGraphics->pUserInterface_->Render(pGraphics->pD3D_, pGraphics->GetWorldMatrix(), pGraphics->GetOrthoMatrix());
	COM_ERROR_IF_FALSE(result, "can't render the user interface");


	////////////////////////////////////////////////

	// local timer							
	DWORD dwTimeCur = GetTickCount();
	static DWORD dwTimeStart = dwTimeCur;

	// update the local timer
	float t = (dwTimeCur - dwTimeStart) / 1000.0f;

	// render picked model to the texture and show a plane with this texture on the screen
	//this->RenderPickedModelToTexture(pGraphics->pD3D_->GetDeviceContext(), pCurrentPickedModel);

	////////////////////////////////////////////////

	// render 2D sprites onto the screen
	//this->Render2DSprites(pGraphics->pD3D_->GetDeviceContext(), pGraphics, deltaTime);

	return true;

} // end RenderGUI()






////////////////////////////////////////////////////////////////////////////////////////////
//                             PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////


void RenderGraphics::RenderModelsObjects(int & renderCount)
{
	//
	// this function renders different game objects from the game object rendering list 
	//

	try
	{
		// a ptr to the list of game objects for rendering onto the screen
		const std::map<std::string, GameObject*> & gameObjectsRenderList = pGraphics_->pGameObjectsList_->GetGameObjectsRenderingList();

		// a ptr to data container for shaders
		DataContainerForShaders* pDataContainer = gameObjectsRenderList.begin()->second->GetModel()->GetDataContainerForShaders();  

		// a temporal ptr to some game object
		GameObject* pGameObj = nullptr;                               

		// local timer							
		DWORD dwTimeCur = GetTickCount();
		static DWORD dwTimeStart = dwTimeCur;

		// update the local timer
		float t = (dwTimeCur - dwTimeStart) / 1000.0f;


		////////////////////////////////////////////////

		// construct the frustum for this frame
		pGraphics_->pFrustum_->ConstructFrustum(pGraphics_->projectionMatrix_, pGraphics_->viewMatrix_);

		// setup shaders' common data for rendering this frame
		pDataContainer->cameraPos = pGraphics_->GetCamera()->GetPositionFloat3();
		pDataContainer->view = pGraphics_->GetViewMatrix();
		pDataContainer->orthoOrProj = pGraphics_->GetProjectionMatrix();
		pDataContainer->ptrToDiffuseLightsArr = &(pGraphics_->arrDiffuseLights_);
	
		////////////////////////////////////////////////

		// render models from the rendering list
		RenderGameObjectsFromList(gameObjectsRenderList, renderCount, t);

	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(THIS_FUNC, "can't render some model");
		COM_ERROR_IF_FALSE(false, "can't render some model");
	}

	return;
}

///////////////////////////////////////////////////////////

void RenderGraphics::RenderGameObjectsFromList(const std::map<std::string, GameObject*> gameObjRenderList,
	int & renderCount,  // the number of rendered polygons
	const float t)      // local timer
{
	// this function renders all the game objects from the input list (map) onto the screen;

	// control flags
	bool isRenderModel = false;             // according to this flag we define to render this model or not
	const bool enableModelMovement = false;  // do random movement/rotation of the models
	const float radius = 1.0f;              // a default radius of the model (it is used to check if a model is in the view frustum or not) 
	

	UINT modelIndex = 0;                    // the current index of the model
	GameObject* pGameObj = nullptr;

	try
	{
		// go through all the models and render only if they can be seen by the camera view
		for (const auto & elem : gameObjRenderList)
		{
			// check if the current element has a propper pointer to the model
			COM_ERROR_IF_NULLPTR(elem.second, "ptr to elem == nullptr");

			// get a pointer to the game object for easier using 
			pGameObj = elem.second;

			if (enableModelMovement)
				MoveRotateScaleGameObjects(pGameObj, t, modelIndex);

			// check if the sphere model is in the view frustum
			isRenderModel = pGraphics_->pFrustum_->CheckSphere(pGameObj->GetData()->GetPosition(), radius);

			// if it can be seen then render it, if not skip this model and check the next sphere
			if (isRenderModel)
			{
				// setup lighting for this model to make it colored with some color
				//pGraphics_->arrDiffuseLights_[0]->SetDiffuseColor(pGameObj->GetData()->GetColor());

				pGameObj->Render();
				
				// since this model was rendered then increase the counts for this frame
				renderCount++;

			} // if

			modelIndex++;
		} // for
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		COM_ERROR_IF_FALSE(false, "can't render models from the rendering list");
	}

	return;
} // end RenderGameObjectsFromList

///////////////////////////////////////////////////////////

void RenderGraphics::UpdateGUIData(SystemState* pSystemState)
{
	// each frame some rendering data about the terrain are changing so we have 
	// to update some data for the GUI to render it onto the screen

	// try to update some data about the terrain rendering process to show it onto the screen;
	try
	{
		// for getting the terrain data
		GameObject* pTerrainGameObj = pGraphics_->pGameObjectsList_->GetGameObjectByID("terrain");

		// if there is some terrain game object
		if (pTerrainGameObj != nullptr)
		{
			Model* pTerrainModel = pTerrainGameObj->GetModel();
			
			// if we already initialized a terrain we have to setup some data
			if (pTerrainModel != nullptr)
			{
				TerrainClass* pTerrain = static_cast<TerrainClass*>(pTerrainModel);

				pSystemState->renderCount = pTerrain->GetRenderCount();
				pSystemState->cellsDrawn = pTerrain->GetCellsDrawn();
				pSystemState->cellsCulled = pTerrain->GetCellsCulled();
			} // if
		} // if

		return;
	}
	// don't worry; there are cases when we don't have the terrain game object so we can't
	// update the rendering data due to it we just catch exception about it and go out
	catch (COMException & e)    
	{
		Log::Error(e, false);
		Log::Error(THIS_FUNC, "no terrain");
		return;
	}
	
} // end UpdateGUIData

///////////////////////////////////////////////////////////

void RenderGraphics::Render2DSprites(ID3D11DeviceContext* pDeviceContext,
	const float deltaTime)
{
	// this function renders all the 2D sprites onto the screen

	// turn off the Z buffer to begin 2D rendering
	pGraphics_->GetD3DClass()->TurnZBufferOff();

	// get a list with 2D sprites
	auto spritesList = pGraphics_->pGameObjectsList_->GetSpritesRenderingList();

	for (const auto & elem : spritesList)
	{
		GameObject* pSpriteGameObj = elem.second;
		SpriteClass* pSpriteModel = static_cast<SpriteClass*>(pSpriteGameObj->GetModel());


		if (pSpriteGameObj->GetID() == "sprite_crosshair")
		{
			pGraphics_->GetD3DClass()->TurnOnAlphaBlending();
		}

		// before rendering this sprite we have to update it using the frame time
		pSpriteModel->Update(deltaTime);

		// setup data container before rendering of this 2D sprite
		DataContainerForShaders* pDataContainer = pSpriteGameObj->GetDataContainerForShaders();
		pDataContainer->world = pGraphics_->GetWorldMatrix();
		pDataContainer->view = pGraphics_->GetBaseViewMatrix();
		pDataContainer->orthoOrProj = pGraphics_->GetOrthoMatrix();

		pSpriteGameObj->Render();


		if (pSpriteGameObj->GetID() == "sprite_crosshair")
		{
			pGraphics_->GetD3DClass()->TurnOffAlphaBlending();
		}
	}

	// turn the Z buffer back on now that 2D rendering has completed
	pGraphics_->GetD3DClass()->TurnZBufferOn();

	
	
	return;

} // end Render2DSprites

///////////////////////////////////////////////////////////

void RenderGraphics::RenderPickedModelToTexture(Model* pModel)
{
	// if we picked some model (clicked on it) we render it to the texture and 
	// show this texture on the screen  

	// rotation value for rotating a model
	static float rotation = 0.0f;
	rotation -= 0.01745f * 0.25f;
	rotation += (rotation < 0.0f) ? 360.0f : 0.0f;

	RenderSceneToTexture(pModel, rotation);   // render the model onto the texture

	//
	// render the display plane using the render texture as its texture resource
	//

	// setup the plane for rendering to
	float renderToTextureWidth = 100.0f;
	float renderToTextureHeight = renderToTextureWidth;

	// setup the position of the texture on the screen
	float posX_OfTexture =  (windowWidth_ / 2.0f) - renderToTextureWidth;
	float posY_OfTexture = -(windowHeight_ / 2.0f) + renderToTextureHeight;


	//render the display plane using the texture shader and the render texture resource
	GameObject* pPlaneGameObj = pGraphics_->GetGameObjectsList()->GetGameObjectByID("plane(1)");
	GameObjectData* pPlaneGameObjData = pPlaneGameObj->GetData();
	pPlaneGameObjData->SetPosition(posX_OfTexture, posY_OfTexture, 0.0f);
	pPlaneGameObjData->SetRotationInDeg(0.0f, 0.0f, 180.0f);
	pPlaneGameObjData->SetScale(100.0f, 100.0f, 1.0f);

	// setup data container before rendering of this model
	DataContainerForShaders* pDataContainer = pPlaneGameObj->GetDataContainerForShaders();
	pDataContainer->world = pPlaneGameObjData->GetWorldMatrix();
	pDataContainer->view = pGraphics_->GetBaseViewMatrix();
	pDataContainer->orthoOrProj = pGraphics_->GetOrthoMatrix();
	pDataContainer->ppTextures = pGraphics_->pRenderToTexture_->GetShaderResourceView();
	pDataContainer->modelColor = pPlaneGameObjData->GetColor();

	pPlaneGameObj->Render();

	// turn off the Z buffer to begin 2D rendering
	pGraphics_->GetD3DClass()->TurnZBufferOff();

	// turn the Z buffer back on now that 2D rendering has completed
	pGraphics_->GetD3DClass()->TurnZBufferOn();

	return;

} // end RenderPickedModelToTexture

///////////////////////////////////////////////////////////

bool RenderGraphics::RenderSceneToTexture(Model* pModel,
	const float rotation)
{
	DirectX::XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	DirectX::XMFLOAT4 blueColor{ 0.0f, 0.5f, 1.0f, 1.0f };
	bool result = false;

	// the first part in this function is where we change the rendering output from the 
	// back buffer to our render texture object. We also clear the render texture to
	// light blue here
	pGraphics_->pRenderToTexture_->SetRenderTarget(this->pDeviceContext_);
	pGraphics_->pRenderToTexture_->ClearRenderTarget(this->pDeviceContext_, blueColor);

	// now we set our camera position here first before getting the resulting view matrix
	// from the camera. If we are using different cameras, we need to set it each frame
	// since the other rendering functions use different cameras from a different position
	pGraphics_->pCameraForRenderToTexture_->SetPosition(0.0f, 0.0f, -5.0f);

	// IMPORTANT: when we get our matrices, we have to get the projection matrix from 
	// the render texture as it has different dimensions than our regular screen projection
	// matrix. If your render texture ever look rendered incorrectly, it is usually because
	// you are using the wrong projection matrix
	pGraphics_->pD3D_->GetWorldMatrix(worldMatrix);
	viewMatrix = pGraphics_->pCameraForRenderToTexture_->GetViewMatrix();
	pGraphics_->pRenderToTexture_->GetProjectionMatrix(projectionMatrix);

	////////////////////////////////////////////////////////

	// now we render our regular spinning cube scene as normal, but the output is now going
	// to the render texture

	// rotate the world matrix by the rotation value so that the cube will spin
	worldMatrix = DirectX::XMMatrixRotationY(rotation);

	// render the model using the texture shader
	if (pModel != nullptr)
	{
	/*
	
		pModel->Render(pDeviceContext);

		result = pGraphics_->GetShadersContainer()->GetTextureShader()->Render(pDeviceContext,
			pModel->GetModelDataObj()->GetIndexCount(),
			worldMatrix,
			viewMatrix,
			projectionMatrix,
			pModel->GetTextureArray()->GetTextureResourcesArray());
		COM_ERROR_IF_FALSE(result, "can't render the cube");
	
	*/
	}

	// once we are done rendering, we need to switch the rendering back to the original
	// back buffer. We also need to switch the viewport back to the original since
	// the render texture's viewport may have different dimensions then the screen's viewport.
	pGraphics_->pD3D_->SetBackBufferRenderTarget();
	pGraphics_->pD3D_->ResetViewport();

	return true;

} // end RenderSceneToTexture

///////////////////////////////////////////////////////////

void RenderGraphics::MoveRotateScaleGameObjects(GameObject* pGameObj, 
	const float t,
	const UINT modelIndex)
{
	// a function for dynamic modification game objects' positions,
	// rotation, etc. during the rendering of the scene

	// check input params
	COM_ERROR_IF_NULLPTR(pGameObj, "the input game obj == nullptr");

	// current game object's position;
	const DirectX::XMFLOAT3 & curPos = pGameObj->GetData()->GetPosition();

	// move and rotate the game object
	pGameObj->GetData()->SetRotationInRad(t, 0.0f, 0.0f);

	if (modelIndex % 3 == 0)
	{
		pGameObj->GetData()->SetRotationInRad(t, 0.0f, 0.0f);
		pGameObj->GetData()->SetPosition(curPos.x, t, curPos.z);
	}

	if (modelIndex % 2 == 0)
	{
		pGameObj->GetData()->SetRotationInRad(0.0f, t, 0.0f);
		pGameObj->GetData()->SetPosition(t, curPos.y, curPos.z);
	}

	return;
}