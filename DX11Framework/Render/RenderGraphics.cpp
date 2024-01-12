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
	ID3D11DeviceContext* pDeviceContext,
	DataContainerForShaders* pDataContainerForShaders)
{
	Log::Debug(THIS_FUNC_EMPTY);

	assert(pGraphics != nullptr);
	assert(pSettings != nullptr);
	assert(pDevice != nullptr);
	assert(pDeviceContext != nullptr);
	assert(pDataContainerForShaders != nullptr);
	
	try
	{
		// init local copies of pointers
		pGraphics_ = pGraphics;
		pDevice_ = pDevice;
		pDeviceContext_ = pDeviceContext;
		pDataForShaders_ = pDataContainerForShaders;

		// the number of point light sources on the scene
		//numPointLights_ = pSettings->GetSettingIntByKey("NUM_POINT_LIGHTS");  
		windowWidth_    = pSettings->GetSettingIntByKey("WINDOW_WIDTH");
		windowHeight_   = pSettings->GetSettingIntByKey("WINDOW_HEIGHT");

		// get a ptr a plane object which will be an another render target to render to
		pPlane2DRenderTargetObj_ = pGraphics_->GetGameObjectsList()->GetGameObjectByID("plane(1)");
		pPlane3DRenderTargetObj_ = pGraphics_->GetGameObjectsList()->GetGameObjectByID("plane(2)");

		// setup rendering planes
		pPlane3DRenderTargetObj_->GetData()->SetPosition(0, 0, 0);
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
	// SETUP THE MAIN DIFFUSE LIGHT SOURCE (SUN)
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

	this->RenderReflectionPlane(renderCount);


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

	pDataForShaders_->world_main_matrix = pGraphics_->GetWorldMatrix();

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
	result = pGraphics->pUserInterface_->Render(pGraphics->pD3D_,
		pGraphics->GetWorldMatrix(), 
		pGraphics->GetOrthoMatrix());
	COM_ERROR_IF_FALSE(result, "can't render the user interface");


	////////////////////////////////////////////////

	// local timer							
	DWORD dwTimeCur = GetTickCount();
	static DWORD dwTimeStart = dwTimeCur;

	// update the local timer
	float t = (dwTimeCur - dwTimeStart) / 1000.0f;


	///////////////////////////////////////////////
	// RENDER A MODEL INTO THE TEXTURE
	///////////////////////////////////////////////

	// get a model and setup its position
	GameObject* pCurrentPickedGameObj = pGraphics_->pGameObjectsList_->GetGameObjectByID("cube(2)");
	pCurrentPickedGameObj->GetData()->SetWorldMatrix(DirectX::XMMatrixRotationY(t));

	// render picked model to the texture and show a plane with this texture on the screen
	this->RenderPickedGameObjToTexture(pCurrentPickedGameObj);

	////////////////////////////////////////////////

	// render 2D sprites onto the screen
	this->Render2DSprites(deltaTime);

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

void RenderGraphics::RenderReflectionPlane(int & renderCount)
{
	// local timer							
	DWORD dwTimeCur = GetTickCount();
	static DWORD dwTimeStart = dwTimeCur;

	// update the local timer
	float t = (dwTimeCur - dwTimeStart) / 1000.0f;

	// get some models and render it onto the texture
	GameObject* pSphere1 = pGraphics_->pGameObjectsList_->GetGameObjectByID("sphere(1)");
	GameObject* pSphere2 = pGraphics_->pGameObjectsList_->GetGameObjectByID("sphere(2)");
	GameObject* pCube1   = pGraphics_->pGameObjectsList_->GetGameObjectByID("cube(1)");

	// save previous cube position in the 3D world so we will be able to set it back
	DirectX::XMFLOAT3 prevCubePos = pCube1->GetData()->GetPosition();

	// setup game objects position before rendering to texture
	pSphere1->GetData()->SetPosition(0, 0, 0);
	pCube1->GetData()->SetPosition(0, 2, 0);
	pCube1->GetData()->SetRotationInRad(0, t, 0);

	std::vector<GameObject*> gameObjectsArr{ pSphere1, pSphere2, pCube1 };
	this->RenderSceneToTexture(gameObjectsArr);

	// since we've already rendered the cube set back its previous position
	pCube1->GetData()->SetPosition(prevCubePos);

	////////////////////////////////////////////////

	// prepare a rendering pipeline for rendering this plane model onto the screen
	Model* pPlaneRenderTargetModel = pPlane3DRenderTargetObj_->GetModel();
	pPlaneRenderTargetModel->GetMeshByIndex(0)->Draw();
	/*


	pPlaneRenderTargetObj_->GetData()->SetPosition(0, 0, 0);
	pPlaneRenderTargetObj_->GetData()->SetScale(0.1f, 0.1f, 0.1f);
	
	*/


	// setup data container before rendering of this model
	pDataForShaders_->indexCount = pPlaneRenderTargetModel->GetIndexCount();
	pDataForShaders_->world = pPlane3DRenderTargetObj_->GetData()->GetWorldMatrix();
	pDataForShaders_->view = pGraphics_->GetViewMatrix();
	pDataForShaders_->orthoOrProj = pGraphics_->GetProjectionMatrix();
	pDataForShaders_->texturesMap.insert_or_assign("diffuse", pGraphics_->pRenderToTexture_->GetShaderResourceViewAddress());


	// render a plane with the scene (or only a single game obj) on it
	ShaderClass* pShader = pGraphics_->pShadersContainer_->GetShaderByName("TextureShaderClass");
	TextureShaderClass* pTextureShader = static_cast<TextureShaderClass*>(pShader);

	pTextureShader->Render(pDeviceContext_, pDataForShaders_);

	// increase the renderCount since we've rendered a reflection plane
	renderCount++;

	return;
} // end RenderReflectionPlane

///////////////////////////////////////////////////////////

void RenderGraphics::RenderGameObjectsFromList(const std::map<std::string, GameObject*> gameObjRenderList,
	int & renderCount,  // the number of rendered polygons
	const float t)      // local timer
{
	// this function renders all the game objects from the input list (map) onto the screen;

	// control flags
	bool isRenderModel = false;              // according to this flag we define to render this model or not
	const bool enableModelMovement = false;  // do random movement/rotation of the models
	const float radius = 1.0f;               // a default radius of the model (it is used to check if a model is in the view frustum or not) 
	

	UINT modelIndex = 0;                     // the current index of the model
	GameObject* pGameObj = nullptr;

	try
	{
		// go through all the models and render only if they can be seen by the camera view
		for (const auto & elem : gameObjRenderList)
		{
			if (elem.first == "cube(2)")
			{
				continue;
			}

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
		const GameObject* pTerrainGameObj = pGraphics_->pZone_->GetTerrainGameObj();

		// if there is some terrain game object
		if (pTerrainGameObj != nullptr)
		{
			Model* pTerrainModel = pTerrainGameObj->GetModel();
			
			// if we already initialized a terrain we have to setup some data
			if (pTerrainModel != nullptr)
			{
				TerrainClass* pTerrain = static_cast<TerrainClass*>(pTerrainModel);

				pSystemState->renderCount = pTerrain->GetRenderCount();
				pSystemState->cellsDrawn  = pTerrain->GetCellsDrawn();
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

void RenderGraphics::Render2DSprites(const float deltaTime)
{
	// this function renders all the 2D sprites onto the screen
	
	// get a list with 2D sprites
	auto spritesList = pGraphics_->pGameObjectsList_->GetSpritesRenderingList();

	// check if we have any 2D sprites for rendering
	if (spritesList.empty())
	{
		Log::Error(THIS_FUNC, "NO SPRITES FOR RENDERING");
		return;
	}


	// turn off the Z buffer to begin 2D rendering
	pGraphics_->GetD3DClass()->TurnZBufferOff();
	

	
	for (const auto & elem : spritesList)
	{
		GameObject* pSpriteGameObj = elem.second;
		SpriteClass* pSpriteModel = static_cast<SpriteClass*>(pSpriteGameObj->GetModel());


		// according to the rendering shader name we define a dataset for rendering of this sprite
		if (pSpriteModel->GetRenderShaderName() == "SpriteShaderClass")
		{
			// (WVP = main_world_matrix * base_view_matrix * ortho_matrix);
			// because we have the same matrices for each sprite during the current frame we just
			// multiply them and pass the result into the shader for rendering;
			DirectX::XMMATRIX WVP = pDataForShaders_->world_main_matrix *
				                    pGraphics_->GetBaseViewMatrix() *
				                    pGraphics_->GetOrthoMatrix();

			// setup data container before rendering of all the 2D sprites
			this->pDataForShaders_->WVP = DirectX::XMMatrixTranspose(WVP);

		}
		else if (pSpriteModel->GetRenderShaderName() == "TextureShaderClass")
		{
			pDataForShaders_->world = pGraphics_->GetWorldMatrix();
			pDataForShaders_->view = pGraphics_->GetBaseViewMatrix();
			pDataForShaders_->orthoOrProj = pGraphics_->GetOrthoMatrix();
		}
		

		// if we want to render a crosshair we have to do it with some extra manipulations
		if (pSpriteGameObj->GetID() == "sprite_crosshair")
		{
			pGraphics_->GetD3DClass()->TurnOnAlphaBlending();
			pSpriteGameObj->RenderSprite();
			pGraphics_->GetD3DClass()->TurnOffAlphaBlending();

			continue;
		}
	

		// before rendering this sprite we have to update it using the frame delta time
		pSpriteModel->Update(deltaTime);
		pSpriteGameObj->RenderSprite();
	}

	// turn the Z buffer back on now that 2D rendering has completed
	pGraphics_->GetD3DClass()->TurnZBufferOn();

	
	
	return;

} // end Render2DSprites

///////////////////////////////////////////////////////////

void RenderGraphics::RenderPickedGameObjToTexture(GameObject* pGameObj)
{
	// if we picked some model (clicked on it) we render it to the texture and 
	// show this texture on the screen  

	// render the game object onto the texture
	RenderSceneToTexture({ pGameObj });

	// turn off the Z buffer to begin 2D rendering
	pGraphics_->GetD3DClass()->TurnZBufferOff();

	// prepare a rendering pipeline for rendering this plane model onto the screen
	Model* pPlaneRenderTargetModel = pPlane2DRenderTargetObj_->GetModel();
	pPlaneRenderTargetModel->GetMeshByIndex(0)->Draw();
	
	// setup data container before rendering of this model
	pDataForShaders_->indexCount  = pPlaneRenderTargetModel->GetIndexCount();
	pDataForShaders_->world       = pPlane2DRenderTargetObj_->GetData()->GetWorldMatrix();
	pDataForShaders_->view        = pGraphics_->GetBaseViewMatrix();
	pDataForShaders_->orthoOrProj = pGraphics_->GetOrthoMatrix();	
	pDataForShaders_->texturesMap.insert_or_assign("diffuse", pGraphics_->pRenderToTexture_->GetShaderResourceViewAddress());

	
	// render a plane with the scene (or only a single game obj) on it
	ShaderClass* pShader = pGraphics_->pShadersContainer_->GetShaderByName("TextureShaderClass");
	TextureShaderClass* pTextureShader = static_cast<TextureShaderClass*>(pShader);

	pTextureShader->Render(pDeviceContext_, pDataForShaders_);

	// turn the Z buffer back on now that 2D rendering has completed
	pGraphics_->GetD3DClass()->TurnZBufferOn();

	return;

} // end RenderPickedGameObjToTexture

///////////////////////////////////////////////////////////

bool RenderGraphics::RenderSceneToTexture(const std::vector<GameObject*> & gameObjectsArr)
{
	// if we have no game objects so just skip this function
	if (gameObjectsArr.empty() == true)
	{
		return true;
	}

	// the first part in this function is where we change the rendering output from the 
	// back buffer to our render texture object. We also clear the render texture to
	// some background colour here (for instance: light blue or black)
	pGraphics_->pRenderToTexture_->ChangeRenderTarget(this->pDeviceContext_);
	pGraphics_->pRenderToTexture_->ClearRenderTarget(this->pDeviceContext_, Color(0, 0, 0).GetFloat4());  

	////////////////////////////////////////////////////////

	// now we set our camera position here first before getting the resulting view matrix
	// from the camera. If we are using different cameras, we need to set it each frame
	// since the other rendering functions use different cameras from a different position
	pGraphics_->pCameraForRenderToTexture_->SetPosition(0.0f, 0.0f, -5.0f);

	// IMPORTANT: when we get our matrices, we have to get the projection matrix from 
	// the render texture as it has different dimensions than our regular screen projection
	// matrix. If your render texture ever look rendered incorrectly, it is usually because
	// you are using the wrong projection matrix
	pDataForShaders_->view = pGraphics_->pCameraForRenderToTexture_->GetViewMatrix();
	pGraphics_->pRenderToTexture_->GetProjectionMatrix(pDataForShaders_->orthoOrProj);

	// go through each game object in the array and render it into the texture
	for (GameObject* pGameObj : gameObjectsArr)
	{
		pGameObj->Render();
	}

	////////////////////////////////////////////////////////


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