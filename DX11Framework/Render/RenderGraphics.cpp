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

		// setup planes which will be other render targets 
		//SetupRenderTargetPlanes();			
		//SetupGameObjectsForRenderingToTexture();
		
		// setup a scene which will be used for rendering a reflection/shadow demo
		//SetupRoom();
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

bool RenderGraphics::Render(HWND hwnd, SystemState* pSystemState, const float deltaTime)
{
	try
	{
		RenderModels(pSystemState->renderCount, deltaTime);
		RenderGUI(pSystemState, deltaTime);
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(THIS_FUNC, "can't render the scene onto the screen");
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////

bool RenderGraphics::RenderModels(int & renderCount, 
	const float deltaTime)
{    
	// this function prepares and renders all the models on the scene

	// set to zero as we haven't rendered models for this frame yet
	renderCount = 0;  

	// local timer							
	const DWORD dwTimeCur = GetTickCount();
	static DWORD dwTimeStart = dwTimeCur;
	localTimer_ = (dwTimeCur - dwTimeStart) * inv_thousand_;   // update the local timer

	bool result = false;


	////////////////////////////////////////////////
	//  SETUP COMMON STUFF FOR THIS FRAME
	////////////////////////////////////////////////

	// construct the frustum for this frame
	pGraphics_->pFrustum_->ConstructFrustum(pGraphics_->projectionMatrix_, pGraphics_->viewMatrix_);

	// setup shaders' common data for rendering this frame
	pDataForShaders_->cameraPos = pGraphics_->GetCamera()->GetPositionFloat3();
	pDataForShaders_->view = pGraphics_->GetViewMatrix();
	pDataForShaders_->orthoOrProj = pGraphics_->GetProjectionMatrix();
	pDataForShaders_->ptrToDiffuseLightsArr = &(pGraphics_->arrDiffuseLights_);


	////////////////////////////////////////////////
	// SETUP THE MAIN DIFFUSE LIGHT SOURCE (SUN)
	////////////////////////////////////////////////

	// setup the colour of the diffuse light on the scene
	pGraphics_->arrDiffuseLights_[0]->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);

	// setup the diffuse light direction (sun direction)
	pGraphics_->arrDiffuseLights_[0]->SetDirection(cos(localTimer_ / 2), -0.5f, sin(localTimer_ / 2));


	////////////////////////////////////////////////
	// RENDER THE ZONE / TERRAIN / SKYBOX / etc.
	////////////////////////////////////////////////

	// renders models which are related to the terrain: the terrain, sky dome, trees, etc.
	result = pGraphics_->pZone_->Render(renderCount,
		pGraphics_->GetD3DClass(),
		deltaTime,
		pGraphics_->arrDiffuseLights_,
		pGraphics_->arrPointLights_);
	COM_ERROR_IF_FALSE(result, "can't render the zone");


	
	////////////////////////////////////////////////
	// RENDER MODELS
	////////////////////////////////////////////////

	// render different models (from the models list) on the scene
	this->RenderModelsObjects(renderCount);
	//this->RenderReflectionPlane(renderCount);



	//GameObject* pPlane28 = pGraphics_->GetGameObjectsList()->GetGameObjectByID("plane(28)");
	GameObject* pPlane29 = pGraphics_->GetGameObjectsList()->GetGameObjectByID("plane(29)");
	GameObject* pPlane30 = pGraphics_->GetGameObjectsList()->GetGameObjectByID("plane(30)");

	
	pGraphics_->GetD3DClass()->TurnOnTransparentBS();

	//pPlane28->Render();
	pPlane29->Render();
	pPlane30->Render();

	pGraphics_->GetD3DClass()->TurnOffTransparentBS();

#if 0
	GameObject* pPlane25 = pGraphics_->GetGameObjectsList()->GetGameObjectByID("plane(25)");
	GameObject* pPlane26 = pGraphics_->GetGameObjectsList()->GetGameObjectByID("plane(26)");
	GameObject* pPlane27 = pGraphics_->GetGameObjectsList()->GetGameObjectByID("plane(27)");
	GameObject* pPlane28 = pGraphics_->GetGameObjectsList()->GetGameObjectByID("plane(28)");
	GameObject* pPlane29 = pGraphics_->GetGameObjectsList()->GetGameObjectByID("plane(29)");
	GameObject* pPlane30 = pGraphics_->GetGameObjectsList()->GetGameObjectByID("plane(30)");

	pPlane25->Render();
	pPlane28->Render();
	pPlane30->Render();


	pGraphics_->GetD3DClass()->TurnOnAddingBS();
	pPlane26->Render();
	pGraphics_->GetD3DClass()->TurnOffAddingBS();

	pGraphics_->GetD3DClass()->TurnOnSubtractingBS();
	pPlane27->Render();
	pGraphics_->GetD3DClass()->TurnOffSubtractingBS();

	pGraphics_->GetD3DClass()->TurnOnMultiplyingBS();
	pPlane29->Render();
	pGraphics_->GetD3DClass()->TurnOffMultiplyingBS();
#endif
	////////////////////////////////////////////////
	// RENDER A REFLECTION/MIRROR DEMO
	////////////////////////////////////////////////

#if 0
	// primary elements
	DrawRoom();            // wall/floor 
	DrawSphere();          // sphere for reflection

	MarkMirrorOnStencil();
	

	// reflected elements
	DrawFloorReflection();
	//DrawSphereReflection();
	
	DrawMirror();
	

	//DrawSphereShadowReflection();
	//DrawMirror();

	//pDeviceContext_->ClearDepthStencilView(pGraphics_->pD3D_->GetDepthStencilView(), )

	//DrawSphereShadow();
#endif

	return true;
} // RenderModels()

///////////////////////////////////////////////////////////


bool RenderGraphics::RenderGUI(SystemState* systemState, 
	const float deltaTime)
{
	// ATTENTION: do 2D rendering only when all 3D rendering is finished;
	// this function renders the engine/game GUI

	bool result = false;

	// we will use this matrix later during rendering of different 2D stuff
	pDataForShaders_->world_main_matrix = pGraphics_->GetWorldMatrix();

	// if some rendering state has been updated we have to update some data for the GUI
	this->UpdateGUIData(systemState);

	// update user interface for this frame (for the editor window)
	result = pGraphics_->pUserInterface_->Frame(pGraphics_->pD3D_->GetDeviceContext(), 
		systemState);  
	COM_ERROR_IF_FALSE(result, "can't do frame calculations for the user interface");

	// render the user interface
	result = pGraphics_->pUserInterface_->Render(pGraphics_->pD3D_,
		pGraphics_->GetWorldMatrix(),
		pGraphics_->GetOrthoMatrix());
	COM_ERROR_IF_FALSE(result, "can't render the user interface");


	///////////////////////////////////////////////
	// RENDER A PICKED GAME OBJECT INTO THE TEXTURE
	///////////////////////////////////////////////

	// get a model and setup its position
	GameObject* pCurrentPickedGameObj = pGraphics_->pGameObjectsList_->GetGameObjectByID("cube(2)");
	pCurrentPickedGameObj->GetData()->SetRotationInRad(0, localTimer_, 0);

	// render picked model to the texture and show a plane with this texture on the screen
	//this->RenderPickedGameObjToTexture(pCurrentPickedGameObj);

	////////////////////////////////////////////////

	// render 2D sprites onto the screen
	this->Render2DSprites(deltaTime);

	return true;

} // end RenderGUI()






////////////////////////////////////////////////////////////////////////////////////////////
//
//                             PRIVATE FUNCTIONS
//
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

		// render models from the rendering list
		RenderGameObjectsFromList(gameObjectsRenderList, renderCount);

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

void RenderGraphics::SetupGameObjectsForRenderingToTexture()
{
	// get some models and render it onto the texture
	GameObject* pSphere1 = pGraphics_->pGameObjectsList_->GetGameObjectByID("sphere(1)");
	GameObject* pSphere2 = pGraphics_->pGameObjectsList_->GetGameObjectByID("sphere(2)");
	GameObject* pCube1 = pGraphics_->pGameObjectsList_->GetGameObjectByID("cube(1)");

	// setup game objects position before rendering to texture
	pSphere1->GetData()->SetPosition(0, 0, 0);
	pSphere1->GetModel()->SetRenderShaderName("TextureShaderClass");

	pCube1->GetData()->SetPosition(0, 2, 0);
	
	
	// add these game objects for rendering into a texture
	renderToTextureGameObjArr_.push_back(pSphere1);
	renderToTextureGameObjArr_.push_back(pSphere2);
	renderToTextureGameObjArr_.push_back(pCube1);

	return;

} // end SetupGameObjectsForRenderingToTexture

///////////////////////////////////////////////////////////

void RenderGraphics::RenderReflectionPlane(int & renderCount)
{
	///////////////////////////////////////////////////////////////////////
	//  RENDER REFLECTED OBJECTS INTO A TEXTURE
	///////////////////////////////////////////////////////////////////////
	
	// render reflection of the game objects to a texture of the reflection plane (mirror)
	this->RenderReflectedSceneToTexture(renderToTextureGameObjArr_, pPlane3DRenderTargetObj_);


	///////////////////////////////////////////////////////////////////////
	//  RENDER THE REFLECTION PLANE AND REFLECTED OBJECTS ONTO THE SCREEN
	//////////////////////////////////////////////////////////////////////

	renderToTextureGameObjArr_[0]->GetData()->SetRotationInRad(0, localTimer_, 0); // sphere_1
	renderToTextureGameObjArr_[0]->GetData()->SetRotationInRad(0, -localTimer_, 0); // cube_1
#if 0
	// start by rendering the reflected objects as normal
	pDataForShaders_->view = pGraphics_->pCamera_->GetViewMatrix();  // use a normal view matrix

	for (GameObject* pGameObj : gameObjectsArr)
	{
		pGameObj->Render();
	}
#endif

	// now render the floor (reflection plane) using the reflection shader to blend the 
	// reflected render texture of the reflected objects into the floor model
	Model* pPlaneRenderTargetModel = pPlane3DRenderTargetObj_->GetModel();

	// setup data container before rendering of this model
	pDataForShaders_->indexCount = pPlaneRenderTargetModel->GetIndexCount();
	pDataForShaders_->world = pPlane3DRenderTargetObj_->GetData()->GetWorldMatrix();
	pDataForShaders_->view = pGraphics_->pCamera_->GetViewMatrix();  // use a normal view matrix
	pDataForShaders_->reflectionMatrix = pGraphics_->pCamera_->GetReflectionViewMatrix();
	pDataForShaders_->orthoOrProj = pGraphics_->GetProjectionMatrix();
	pDataForShaders_->texturesMap.insert_or_assign("diffuse", pPlane3DRenderTargetObj_->GetModel()->GetMeshByIndex(0)->GetTexturesArr()[0]->GetTextureResourceViewAddress());
	pDataForShaders_->texturesMap.insert_or_assign("reflection_texture", pGraphics_->pRenderToTexture_->GetShaderResourceViewAddress());

	pPlaneRenderTargetModel->GetMeshByIndex(0)->Draw();

	// render a plane with the scene (or only a single game obj) on it
	ShaderClass* pShader = pGraphics_->pShadersContainer_->GetShaderByName("ReflectionShaderClass");
	ReflectionShaderClass* pReflectionShader = static_cast<ReflectionShaderClass*>(pShader);

	pReflectionShader->Render(pDeviceContext_, pDataForShaders_);

	// increase the renderCount since we've rendered a reflection plane
	renderCount++;

	return;
} // end RenderReflectionPlane

///////////////////////////////////////////////////////////

void RenderGraphics::SetupRenderTargetPlanes()
{
	/////////////  SETUP PLANES WHICH WILL BE AN ANOTHER RENDER TARGET  /////////////

	// setup the plane for rendering to
	const float renderToTextureWidth = 100.0f;
	const float renderToTextureHeight = renderToTextureWidth;

	// setup the position of the texture on the screen
	const float posX_OfTexture = (windowWidth_ * 0.5f) - renderToTextureWidth;
	const float posY_OfTexture = -(windowHeight_ * 0.5f) + renderToTextureHeight;

	GameObject* pPlaneGameObj = pGraphics_->GetGameObjectsList()->GetGameObjectByID("plane(1)");
	GameObjectData* pPlaneGameObjData = pPlaneGameObj->GetData();

	pPlaneGameObjData->SetPosition(posX_OfTexture, posY_OfTexture, 0.0f);
	pPlaneGameObjData->SetRotationInDeg(0.0f, 0.0f, 0.0f);
	pPlaneGameObjData->SetScale(100.0f, 100.0f, 1.0f);

	// also change the rendering texture
	pPlaneGameObj->GetModel()->SetRenderShaderName("TextureShaderClass");

	///////////////////////////////////////////////////////////////////////////

	// get a ptr a plane object which will be an another render target to render to
	pPlane2DRenderTargetObj_ = pGraphics_->GetGameObjectsList()->GetGameObjectByID("plane(1)");
	pPlane3DRenderTargetObj_ = pGraphics_->GetGameObjectsList()->GetGameObjectByID("plane(2)");

	// setup rendering planes
	pPlane3DRenderTargetObj_->GetData()->SetPosition(0, -1.5f, 0);
	pPlane3DRenderTargetObj_->GetData()->SetScale(3, 3, 3);
	pPlane3DRenderTargetObj_->GetData()->SetRotationInDeg(0, 0, 90);  // the plane looks upward

	pPlane3DRenderTargetObj_->GetModel()->SetRenderShaderName("ReflectionShaderClass");
	pPlane3DRenderTargetObj_->GetModel()->GetMeshByIndex(0)->SetTextureByIndex(0, "data/textures/blue01.tga", aiTextureType::aiTextureType_DIFFUSE);

	return;
}

///////////////////////////////////////////////////////////

void RenderGraphics::RenderGameObjectsFromList(const std::map<std::string, GameObject*> gameObjRenderList,
	int & renderCount)  // the number of rendered polygons
{
	// this function renders all the game objects from the input list (map) onto the screen;

	// control flags
	bool isRenderModel = false;              // according to this flag we define to render this model or not
	const bool enableModelMovement = false;  // do random movement/rotation of the models
	const float radius = 1.0f;               // a default radius of the model (it is used to check if a model is in the view frustum or not) 
	

	UINT modelIndex = 0;                     // the current index of the model
	GameObject* pGameObj = nullptr;

	//const std::vector<std::string> arrSkipID{ "cube(1)", "cube(2)" };
	//const std::vector<std::string> arrSkipType{ "plane" };

	try
	{
		// go through all the models and render only if they can be seen by the camera view
		for (const auto & elem : gameObjRenderList)
		{
			//if (elem.second->GetID() == pMirrorPlane_->GetID())
			//	continue;

			if (elem.second->GetModel()->GetModelType() != "cube")
				continue;

			// check if the current element has a propper pointer to the model
			COM_ERROR_IF_NULLPTR(elem.second, "ptr to elem == nullptr");

			// get a pointer to the game object for easier using 
			pGameObj = elem.second;

			if (enableModelMovement)
				MoveRotateScaleGameObjects(pGameObj, localTimer_, modelIndex);

			// check if the sphere model is in the view frustum
			isRenderModel = pGraphics_->pFrustum_->CheckSphere(pGameObj->GetData()->GetPosition(), radius);

			// if it can be seen then render it, if not skip this model and check the next sphere
			if (isRenderModel)
			{
				// setup lighting for this model to make it colored with some color
				//pGraphics_->arrDiffuseLights_[0]->SetDiffuseColor(pGameObj->GetData()->GetColor());

				pGraphics_->pD3D_->SetRenderState(D3DClass::RASTER_PARAMS::CULL_MODE_NONE);

				pGameObj->Render();

				pGraphics_->pD3D_->SetRenderState(D3DClass::RASTER_PARAMS::CULL_MODE_BACK);
				
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
	//
	// this function renders all the 2D sprites onto the screen
	//

	// get a list with 2D sprites
	auto spritesList = pGraphics_->pGameObjectsList_->GetSpritesRenderingList();

	// check if we have any 2D sprites for rendering
	if (spritesList.empty())
	{
		Log::Error(THIS_FUNC, "NO SPRITES FOR RENDERING");
		return;
	}

	/////////////////////////////////////////////


	// (WVP = main_world_matrix * base_view_matrix * ortho_matrix);
	// because we have the same matrices for each sprite during the current frame we just
	// multiply them and pass the result into the shader for rendering;
	//
	// NOTE: this WVP matrix uses in the SpriteShaderClass
	DirectX::XMMATRIX WVP = pDataForShaders_->world_main_matrix *
		                    pGraphics_->GetBaseViewMatrix() *
		                    pGraphics_->GetOrthoMatrix();

	// setup data container before rendering of all the 2D sprites
	this->pDataForShaders_->WVP = DirectX::XMMatrixTranspose(WVP);

	// for cases when some 2D sprite don't use the SpriteShaderClass for rendering we
	// have to setup matrices in separate way (for instance: we use TextureShaderClass)
	pDataForShaders_->world = pGraphics_->GetWorldMatrix();
	pDataForShaders_->view = pGraphics_->GetBaseViewMatrix();
	pDataForShaders_->orthoOrProj = pGraphics_->GetOrthoMatrix();

	////////////////////////////////////////////////


	// turn off the Z buffer to begin 2D rendering
	pGraphics_->GetD3DClass()->TurnZBufferOff();
	
	for (const auto & elem : spritesList)
	{
		GameObject* pSpriteGameObj = elem.second;
		SpriteClass* pSpriteModel = static_cast<SpriteClass*>(pSpriteGameObj->GetModel());

		// if we want to render a crosshair we have to do it with some extra manipulations
		if (pSpriteGameObj->GetID() == "sprite_crosshair")
		{
			pGraphics_->GetD3DClass()->TurnOnAlphaBlending();
			pSpriteGameObj->RenderSprite();
			pGraphics_->GetD3DClass()->TurnOffAlphaBlending();

			continue;
		}
	
		// before rendering this sprite we have to update it (if it is an animation)
		// using the frame delta time
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

void RenderGraphics::RenderSceneToTexture(const std::vector<GameObject*> & gameObjArr)
{
	// if we have no game objects so just skip this function
	if (gameObjArr.empty() == true)
	{
		return;
	}

	// the first part in this function is where we change the rendering output from the 
	// back buffer to our render texture object. We also clear the render texture to
	// some background colour here (for instance: light blue or black)
	pGraphics_->pRenderToTexture_->ChangeRenderTarget(this->pDeviceContext_);
	pGraphics_->pRenderToTexture_->ClearRenderTarget(this->pDeviceContext_, Color(50, 100, 200).GetFloat4());  

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
	for (GameObject* pGameObj : gameObjArr)
	{
		pGameObj->Render();
	}

	////////////////////////////////////////////////////////


	// once we are done rendering, we need to switch the rendering back to the original
	// back buffer. We also need to switch the viewport back to the original since
	// the render texture's viewport may have different dimensions then the screen's viewport.
	pGraphics_->pD3D_->SetBackBufferRenderTarget();
	pGraphics_->pD3D_->ResetViewport();

} // end RenderSceneToTexture

///////////////////////////////////////////////////////////

void RenderGraphics::RenderReflectedSceneToTexture(const std::vector<GameObject*> & gameObjArr,
	const GameObject* pReflectionPlane)
{
	// this function renders objects to a reflection texture so we can
	// get an effect of planar reflection


	// if we have no game objects so just skip this function
	if (gameObjArr.empty() == true)
	{
		return;
	}

	// the first part in this function is where we change the rendering output from the 
	// back buffer to our render texture object. We also clear the render texture to
	// some background colour here (for instance: light blue or black)
	//pGraphics_->pRenderToTexture_->ChangeRenderTarget(this->pDeviceContext_);
	//pGraphics_->pRenderToTexture_->ClearRenderTarget(this->pDeviceContext_, Color(50, 100, 200).GetFloat4());

	////////////////////////////////////////////////////////


	// before render the scene to a texture, we need to create the reflection matrix using
	// the position of the floor/mirror (by Y axis) as the height variable
	pGraphics_->pCamera_->UpdateReflectionViewMatrix(pReflectionPlane->GetData()->GetPosition(), 
		pReflectionPlane->GetData()->GetRotation());

	// now render the scene as normal but use the reflection matrix instead of the normal view matrix.
	//pDataForShaders_->view = pGraphics_->pCamera_->GetReflectionViewMatrix();
	pDataForShaders_->orthoOrProj = pGraphics_->GetProjectionMatrix();

	
	DirectX::XMMATRIX reflectionMatrix = pGraphics_->pCamera_->GetReflectionViewMatrix();

	// go through each game object in the array and render it into the texture
	for (GameObject* pGameObj : gameObjArr)
	{
		DirectX::XMMATRIX prevWorldMatrix = pGameObj->GetData()->GetWorldMatrix();
		
		pGameObj->GetData()->SetWorldMatrix(prevWorldMatrix * reflectionMatrix);
		pGameObj->Render();

		pGameObj->GetData()->SetWorldMatrix(prevWorldMatrix);
	}


	////////////////////////////////////////////////////////

	// once we are done rendering, we need to switch the rendering back to the original
	// back buffer. We also need to switch the viewport back to the original since
	// the render texture's viewport may have different dimensions then the screen's viewport.
	//pGraphics_->pD3D_->SetBackBufferRenderTarget();
	//pGraphics_->pD3D_->ResetViewport();

} // end RenderReflectedSceneToTexture

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

///////////////////////////////////////////////////////////

void RenderGraphics::SetupRoom()
{
	UINT planeIndex = 0;

	// setup a room which will be used for reflection/shadow rendering demo
	planeIndex = SetupWall();
	planeIndex = SetupFloor(planeIndex);

	// setup mirror plane
	pMirrorPlane_ = pGraphics_->pGameObjectsList_->GetGameObjectByID("plane(" + std::to_string(planeIndex) + ")");
	pMirrorPlane_->GetData()->SetPosition(0, 0, 0);
	pMirrorPlane_->GetModel()->GetMeshByIndex(0)->SetTextureByIndex(0, "data/textures/blue01.tga", aiTextureType_DIFFUSE);
	//pMirrorPlane_->GetModel()->SetRenderShaderName("ReflectionShaderClass");

	// setup the sphere which we will use to make a relfection and shadow of it
	pSphereForReflection_ = pGraphics_->pGameObjectsList_->GetGameObjectByID("sphere(10)");
	pSphereForReflection_->GetData()->SetPosition(0, 0, -3);

	return;
}

///////////////////////////////////////////////////////////

UINT RenderGraphics::SetupWall()
{
	// setup a wall for the reflection/shadow demo

	const UINT wallWidth = 3;
	const UINT wallHeight = 3;
	const int wallBias = 2;
	UINT planeIndex = 0;
	std::string planeID{ "" };

	// get game objects for a wall
	for (UINT i = 1; i <= wallWidth; i++)
		for (UINT j = 0; j < wallHeight; j++)
		{
			planeIndex = i * wallWidth + j;
			if (planeIndex == 6) continue;
			planeID = "plane(" + std::to_string(planeIndex) + ")";

			// get a single plane and set texture for it
			GameObject* pWallGameObj = pGraphics_->pGameObjectsList_->GetGameObjectByID(planeID);
			pWallGameObj->GetModel()->GetMeshByIndex(0)->SetTextureByIndex(0, "data/textures/brick01.dds", aiTextureType::aiTextureType_DIFFUSE);

			// setup the wall plane's position
			pWallGameObj->GetData()->SetPosition((float)(i*2 - wallBias), float(j*2), 0);
			pWallGameObj->GetData()->AdjustPosition({ -2, 0, 0 });   // move by -2 by X-axis

			// add this plane into the array
			wallPlanesArr_.push_back(pWallGameObj);
		}

	// later we will use this index for initialization
	// of the floor in the SetupFloor() function
	return planeIndex;   
}

///////////////////////////////////////////////////////////

UINT RenderGraphics::SetupFloor(UINT startPlaneIndex)
{
	// setup a floor for the reflection/shadow demo

	UINT floorWidth = 3;
	UINT floorHeight = 3;
	UINT planeIndex = 0;
	std::string planeID{ "" };
	
	int planeWidth = 2;
	int floorPosX = 0;
	int floorPosZ = 0;

	startPlaneIndex += 3;   // because we're already using previous indices for the wall

	// get game objects for a floor
	for (UINT i = 0; i < floorWidth; i++)       
	{
		for (UINT j = 0; j < floorHeight; j++)
		{
			planeIndex = startPlaneIndex + ((i * floorWidth) + j);
			planeID = "plane(" + std::to_string(planeIndex) + ")";

			// find a plane game object and setup it
			GameObject* pFloorGameObj = pGraphics_->pGameObjectsList_->GetGameObjectByID(planeID);
			pFloorGameObj->GetModel()->GetMeshByIndex(0)->SetTextureByIndex(0, "data/textures/checkboard.dds", aiTextureType::aiTextureType_DIFFUSE);

			// setup floor plane's position/rotation
			pFloorGameObj->GetData()->SetPosition((float)floorPosX, -1, (float)floorPosZ - 1.0f);
			pFloorGameObj->GetData()->AdjustPosition({ -2, 0, 0 });   // move by -2 by X-axis
			pFloorGameObj->GetData()->SetRotationInDeg(0, 0, 90);

			// change the Y position for the next plane 
			floorPosZ -= planeWidth;

			// add this plane into the array
			floorPlanesArr_.push_back(pFloorGameObj);
		}

		// change the X and Y positions for the new row of floor planes
		floorPosX += planeWidth;
		floorPosZ = 0;
	}

	// later we will use this index for to get a plane game object which will be our mirror
	return ++planeIndex;
}

///////////////////////////////////////////////////////////

void RenderGraphics::DrawRoom()
{
	// render wall planes
	for (GameObject* pWallPlane : wallPlanesArr_)
	{
		pWallPlane->Render();
	}

	// render floor planes
	for (GameObject* pFloorPlane : floorPlanesArr_)
	{
		pFloorPlane->Render();
	}

	return;
}

///////////////////////////////////////////////////////////

void RenderGraphics::DrawSphere()
{
	// update the rotation of the sphere
	pSphereForReflection_->GetData()->SetRotationInRad(0, localTimer_, 0);

	// ... and render it onto the screen
	pSphereForReflection_->Render();

	return;
}

void RenderGraphics::MarkMirrorOnStencil()
{
	// draw mirror to stencil
	//pMirrorPlane_->GetModel()->GetMeshByIndex(0)->Draw();

	for (int p = 0; p < 2; p++)
	{
		// setup the blend state before the marking of the mirror
		pGraphics_->GetD3DClass()->TurnOnMarkMirrorOnStencil();

		pMirrorPlane_->Render();

		// restore the blend state after the marking of the mirror
		pGraphics_->GetD3DClass()->TurnOffMarkMirrorOnStencil();
	}
	

	return;
}

///////////////////////////////////////////////////////////

void RenderGraphics::DrawMirror()
{


	// now render the floor (reflection plane) using the reflection shader to blend the 
	// reflected render texture of the reflected objects into the floor model
	Model* pMirrorModel = pMirrorPlane_->GetModel();

	// setup data container before rendering of this model
	pDataForShaders_->indexCount = pMirrorModel->GetIndexCount();
	pDataForShaders_->world = pMirrorPlane_->GetData()->GetWorldMatrix();
	pDataForShaders_->view = pGraphics_->pCamera_->GetViewMatrix();  // use a normal view matrix
	pDataForShaders_->reflectionMatrix = pGraphics_->pCamera_->GetReflectionViewMatrix();
	pDataForShaders_->orthoOrProj = pGraphics_->GetProjectionMatrix();
	pDataForShaders_->texturesMap.insert_or_assign("diffuse", pMirrorModel->GetMeshByIndex(0)->GetTexturesArr()[0]->GetTextureResourceViewAddress());
	pDataForShaders_->texturesMap.insert_or_assign("reflection_texture", pGraphics_->pRenderToTexture_->GetShaderResourceViewAddress());

	pMirrorModel->GetMeshByIndex(0)->Draw();

	// render a plane with the scene (or only a single game obj) on it
	ShaderClass* pShader = pGraphics_->pShadersContainer_->GetShaderByName("ReflectionShaderClass");
	ReflectionShaderClass* pReflectionShader = static_cast<ReflectionShaderClass*>(pShader);

	pReflectionShader->Render(pDeviceContext_, pDataForShaders_);
}

///////////////////////////////////////////////////////////

void RenderGraphics::DrawFloorReflection()
{
	// render reflection of the game objects to a texture of the reflection plane (mirror)
	//this->RenderReflectedSceneToTexture(floorPlanesArr_, pMirrorPlane_);

	//return;

	// update the reflection matrix for this frame
	pGraphics_->pCamera_->UpdateReflectionViewMatrix(
		pMirrorPlane_->GetData()->GetPosition(),
		pMirrorPlane_->GetData()->GetRotation());

	DirectX::XMMATRIX reflectionMatrix = pGraphics_->pCamera_->GetReflectionViewMatrix();


	for (int p = 0; p < 2; p++)
	{
		// setup the blend state before the rendering of the mirror
		pGraphics_->GetD3DClass()->TurnOnMarkMirrorOnStencil();



		// now render the floor (reflection plane) using the reflection shader to blend the 
		// reflected render texture of the reflected objects into the floor model
		Model* pFloorPlaneModel = floorPlanesArr_[0]->GetModel();

		// setup data container before rendering of this model
		pDataForShaders_->indexCount = pFloorPlaneModel->GetIndexCount();
		pDataForShaders_->world = floorPlanesArr_[0]->GetData()->GetWorldMatrix() * reflectionMatrix;
		pDataForShaders_->view = pGraphics_->pCamera_->GetViewMatrix();  // use a normal view matrix
		//pDataForShaders_->reflectionMatrix = pGraphics_->pCamera_->GetReflectionViewMatrix();
		pDataForShaders_->orthoOrProj = pGraphics_->GetProjectionMatrix();
		pDataForShaders_->texturesMap.insert_or_assign("diffuse", pFloorPlaneModel->GetMeshByIndex(0)->GetTexturesArr()[0]->GetTextureResourceViewAddress());
		//pDataForShaders_->texturesMap.insert_or_assign("reflection_texture", pGraphics_->pRenderToTexture_->GetShaderResourceViewAddress());

		pFloorPlaneModel->GetMeshByIndex(0)->Draw();

		// render a plane with the scene (or only a single game obj) on it
		ShaderClass* pShader = pGraphics_->pShadersContainer_->GetShaderByName("TextureShaderClass");
		TextureShaderClass* pTextureShader = static_cast<TextureShaderClass*>(pShader);

		pTextureShader->Render(pDeviceContext_, pDataForShaders_);


		

		// restore the blend state after the rendering of the mirror
		pGraphics_->GetD3DClass()->TurnOffMarkMirrorOnStencil();
	}


	return;
}

///////////////////////////////////////////////////////////

void RenderGraphics::DrawSphereReflection()
{
	// render reflection of the game objects to a texture of the reflection plane (mirror)
	//this->RenderReflectedSceneToTexture({ pSphereForReflection_ }, pMirrorPlane_);

	return;
}