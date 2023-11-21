////////////////////////////////////////////////////////////////////
// Filename:     RenderGraphics.cpp
// Description:  there are functions for rendering graphics;
// Created:      01.01.23
// Revising:     01.01.23
////////////////////////////////////////////////////////////////////
#include "RenderGraphics.h"


RenderGraphics::RenderGraphics(GraphicsClass* pGraphics, 
	Settings* pSettings)
	: gameObjectsList(pGraphics->GetGameObjectsList()->GetGameObjectsRenderingList())
{
	Log::Debug(THIS_FUNC_EMPTY);

	assert(pGraphics != nullptr);
	assert(pSettings != nullptr);
	
	try
	{
		pGraphics_ = pGraphics;

		// the number of point light sources on the scene
		numPointLights_ = pSettings->GetSettingIntByKey("NUM_POINT_LIGHTS");  
		windowWidth_ = pSettings->GetSettingIntByKey("WINDOW_WIDTH");
		windowHeight_ = pSettings->GetSettingIntByKey("WINDOW_HEIGHT");

		// resize point light data arrays according to the number of point light sources
		arrPointLightsPositions_.resize(numPointLights_);
		arrPointLightsColors_.resize(numPointLights_);
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

	// temporal pointers for easier using
	ID3D11Device*        pDevice = pGraphics->pD3D_->GetDevice();
	ID3D11DeviceContext* pDeviceContext = pGraphics->pD3D_->GetDeviceContext();

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
	pGraphics_->arrDiffuseLights_[0]->SetDiffuseColor(1.0f, 0.5f, 0.0f, 1.0f);

	// setup the diffuse light direction (sun direction)
	pGraphics->arrDiffuseLights_[0]->SetDirection(cos(t / 2), -0.5f, sin(t / 2));


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
	this->RenderModelsObjects(pDeviceContext, renderCount);


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

///////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////////////////
//                             PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////

void RenderGraphics::RenderModelsObjects(ID3D11DeviceContext* pDeviceContext,
	int & renderCount)
{
	//
	// this function renders different game objects from the game object rendering list 
	//

	try
	{

	DirectX::XMFLOAT3 modelPosition;   // contains some model's position
	//DirectX::XMFLOAT4 modelColor;      // contains a colour of a model                          
	
	UINT modelIndex = 0;                                // the current index of the model 
	float radius = 1.0f;                                // a default radius of the model (it is used to check if a model is in the view frustum or not) 
	DataContainerForShaders* pDataContainer = nullptr;  // a ptr to data container for shaders
	GameObject* pGameObj = nullptr;

	// control flags
	bool isRenderModel = false;
	bool enableModelMovement = false;
	bool result = false;

	// local timer							
	DWORD dwTimeCur = GetTickCount();
	static DWORD dwTimeStart = dwTimeCur;

	// update the local timer
	float t = (dwTimeCur - dwTimeStart) / 1000.0f;


	////////////////////////////////////////////////

	// construct the frustum
	pGraphics_->pFrustum_->ConstructFrustum(pGraphics_->projectionMatrix_, pGraphics_->viewMatrix_);

	////////////////////////////////////////////////


	// go through all the models and render only if they can be seen by the camera view
	for (const auto & elem : gameObjectsList)
	{
		// check if the current element has a propper pointer to the model
		COM_ERROR_IF_NULLPTR(elem.second, "ptr to elem == nullptr");

		// get a pointer to the game object for easier using 
		pGameObj = elem.second;

		// check if the sphere model is in the view frustum
		isRenderModel = pGraphics_->pFrustum_->CheckSphere(modelPosition.x, modelPosition.y, modelPosition.z, radius);

		// if it can be seen then render it, if not skip this model and check the next sphere
		if (isRenderModel)
		{
			if (enableModelMovement)
				MoveRotateScaleGameObjects(pGameObj, modelPosition, t, modelIndex);

			// setup lighting for this model to make it colored with some color
			//pGraphics_->arrDiffuseLights_[0]->SetDiffuseColor(modelColor.x, modelColor.y, modelColor.z, modelColor.w);


			// setup data container before rendering of this game object
			DataContainerForShaders* pDataContainer = pGameObj->GetModel()->GetDataContainerForShaders();
			pDataContainer->world = pGameObj->GetData()->GetWorldMatrix();
			pDataContainer->view = pGraphics_->GetViewMatrix();
			pDataContainer->orthoOrProj = pGraphics_->GetProjectionMatrix();
			pDataContainer->modelColor = pGameObj->GetData()->GetColor();
			
			pGameObj->Render();
		
			// since this model was rendered then increase the counts for this frame
			renderCount++;
			
		} // if

		modelIndex++;
	} // for

	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(THIS_FUNC, "can't render some model");
		COM_ERROR_IF_FALSE(false, "can't render some model");
	}
}

///////////////////////////////////////////////////////////

void RenderGraphics::UpdateGUIData(SystemState* pSystemState)
{
	// for getting the terrain data
	Model* pTerrainModel = pGraphics_->pGameObjectsList_->GetGameObjectByID("terrain")->GetModel();
	TerrainClass* pTerrain = static_cast<TerrainClass*>(pTerrainModel);

	// if we already initialized a terrain we have to setup some data
	if (pTerrain != nullptr)
	{
		pSystemState->renderCount = pTerrain->GetRenderCount();
		pSystemState->cellsDrawn = pTerrain->GetCellsDrawn();
		pSystemState->cellsCulled = pTerrain->GetCellsCulled(); 
	}
}

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

void RenderGraphics::RenderPickedModelToTexture(ID3D11DeviceContext* pDeviceContext, Model* pModel)
{
	// if we picked some model (clicked on it) we render it to the texture and 
	// show this texture on the screen  

	// rotation value for rotating a model
	static float rotation = 0.0f;
	rotation -= 0.01745f * 0.25f;
	rotation += (rotation < 0.0f) ? 360.0f : 0.0f;

	RenderSceneToTexture(pDeviceContext, pModel, rotation);   // render the model onto the texture

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
	pPlaneGameObjData->SetRotationInDegrees(0.0f, 0.0f, 180.0f);
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

bool RenderGraphics::RenderSceneToTexture(ID3D11DeviceContext* pDeviceContext,
	Model* pModel,
	const float rotation)
{
	DirectX::XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	DirectX::XMFLOAT4 blueColor{ 0.0f, 0.5f, 1.0f, 1.0f };
	bool result = false;

	// the first part in this function is where we change the rendering output from the 
	// back buffer to our render texture object. We also clear the render texture to
	// light blue here
	pGraphics_->pRenderToTexture_->SetRenderTarget(pDeviceContext);
	pGraphics_->pRenderToTexture_->ClearRenderTarget(pDeviceContext, blueColor);

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
	const DirectX::XMFLOAT3 & modelPosition,
	const float t,
	const UINT modelIndex)
{
	// a function for dynamic modification game objects' positions,
	// rotation, etc. during the rendering of the scene

	// move and rotate the game object
	pGameObj->GetData()->SetPosition(modelPosition.x, modelPosition.y, modelPosition.z);  
	pGameObj->GetData()->SetRotation(t, 0.0f, 0.0f);

	if (modelIndex % 3 == 0)
	{
		pGameObj->GetData()->SetRotation(t, 0.0f, 0.0f);
		pGameObj->GetData()->SetPosition(modelPosition.x, t, modelPosition.z);
	}

	if (modelIndex % 2 == 0)
	{
		pGameObj->GetData()->SetRotation(0.0f, t, 0.0f);
		pGameObj->GetData()->SetPosition(t, modelPosition.y, modelPosition.z);
	}

	return;
}