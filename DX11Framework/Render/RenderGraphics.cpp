////////////////////////////////////////////////////////////////////
// Filename:     RenderGraphics.cpp
// Description:  there are functions for rendering graphics;
// Created:      01.01.23
// Revising:     01.01.23
////////////////////////////////////////////////////////////////////
#include "RenderGraphics.h"


RenderGraphics::RenderGraphics()
{
	Log::Debug(LOG_MACRO);
}


RenderGraphics::~RenderGraphics()
{
	Log::Debug(LOG_MACRO);
}




////////////////////////////////////////////////////////////////////////////////////////////
//                             PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////



void RenderGraphics::Initialize(ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,
	const Settings & settings)      
{
	Log::Debug(LOG_MACRO);

	try
	{

		// the number of point light sources on the scene
		//numPointLights_ = pSettings->GetSettingIntByKey("NUM_POINT_LIGHTS");  
		//windowWidth_    = pSettings->GetSettingIntByKey("WINDOW_WIDTH");
		//windowHeight_   = pSettings->GetSettingIntByKey("WINDOW_HEIGHT");

#if 0
		// setup the common params for all the shaders
		pDataForShaders_->fogEnabled = pSettings->GetSettingBoolByKey("FOG_ENABLED");
		pDataForShaders_->fogStart = pSettings->GetSettingFloatByKey("FOG_START");
		pDataForShaders_->fogRange = pSettings->GetSettingFloatByKey("FOG_RANGE");
		pDataForShaders_->fogRange_inv = 1.0f / pDataForShaders_->fogRange;

		pDataForShaders_->useAlphaClip = pSettings->GetSettingBoolByKey("USE_ALPHA_CLIP");
#endif

		// setup planes which will be other render targets 
		//SetupRenderTargetPlanes();			
		//SetupGameObjectsForRenderingToTexture();

		// setup a scene which will be used for rendering a reflection/shadow demo
		//SetupRoom();
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(LOG_MACRO, "can't create an instance of the RenderGraphics class");
	}
}

///////////////////////////////////////////////////////////

bool RenderGraphics::Render(
	ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,

	SystemState & systemState,
	D3DClass & d3d,
	ModelsStore & modelsStore,
	LightStore & lightsStore,
	UserInterfaceClass & UI,
	FrustumClass & editorFrustum,
	
	ColorShaderClass & colorShader,
	TextureShaderClass & textureShader,
	Parallel_LightShaderClass & lightShader,
	PointLightShaderClass & pointLightShader,

	const DirectX::XMMATRIX & WVO,           // world * basic_view * ortho
	const DirectX::XMMATRIX & viewProj,      // view * projection
	const DirectX::XMFLOAT3 & cameraPos,
	const float deltaTime,                   // time passed since the previous frame
	const float totalGameTime,               // time passed since the start of the application
	const float cameraDepth)                 // how far the camera can see
{
	try
	{
		//Log::Debug(LOG_MACRO, "sizeof(XMFLOAT3): " + std::to_string(sizeof(DirectX::XMFLOAT3)));
		//Log::Debug(LOG_MACRO, "sizeof(XMVECTOR): " + std::to_string(sizeof(DirectX::XMVECTOR)));
		//exit(-1);
		RenderModels(
			editorFrustum,
			colorShader,
			textureShader,
			lightShader,
			pointLightShader,

			pDevice,
			pDeviceContext,
			systemState,
			modelsStore,
			lightsStore,
			viewProj,
			cameraPos,
			deltaTime,
			totalGameTime,
			cameraDepth);

		RenderGUI(pDeviceContext,
			d3d,
			systemState, 
			UI,
			WVO, 
			deltaTime,
			(int)totalGameTime);



	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(LOG_MACRO, "can't render the scene onto the screen");
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////

bool RenderGraphics::RenderModels(
	FrustumClass & editorFrustum,
	ColorShaderClass & colorShader,
	TextureShaderClass & textureShader,
	Parallel_LightShaderClass & lightShader,
	PointLightShaderClass & pointLightShader,

	ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,
	SystemState & systemState,
	ModelsStore & modelsStore,
	LightStore & lightsStore,
	const DirectX::XMMATRIX & viewProj,   // view * projection
	const DirectX::XMFLOAT3 & cameraPos,
	const float deltaTime,
	const float totalGameTime,
	const float cameraDepth)
{    
	// this function prepares and renders all the models on the scene

	// set to zero as we haven't rendered models for this frame yet
	systemState.renderedModelsCount = 0;
	systemState.renderedVerticesCount = 0;

	
	

	bool result = false;


	////////////////////////////////////////////////
	//  SETUP COMMON STUFF FOR THIS FRAME
	////////////////////////////////////////////////

	// construct the frustum for this frame
	//pGraphics_->editorFrustum_->ConstructFrustum(pGraphics_->GetProjectionMatrix(), pGraphics_->GetViewMatrix());


	////////////////////////////////////////////////
	// SETUP THE MAIN DIFFUSE LIGHT SOURCE (SUN)
	////////////////////////////////////////////////

	// setup the diffuse light direction (sun direction) for this frame
	const float slower = 1.5f;
	const float diffuseLightHeight = sin(totalGameTime * slower);
	//const DirectX::XMVECTOR newDiffuseLightDir{ sin(totalGameTime * slower), -1, cos(totalGameTime * slower) };
	const DirectX::XMVECTOR newDiffuseLightDir{ -0.5f, -diffuseLightHeight, cos(totalGameTime * slower) };

	lightsStore.SetDirectionForDiffuseLightByIndex(0, newDiffuseLightDir);
	//lightsStore.SetPowerForDiffuseLightByIndex(0, diffuseLightHeight);


	////////////////////////////////////////////////
	// RENDER THE ZONE / TERRAIN / SKYBOX / etc.
	////////////////////////////////////////////////

	// renders models which are related to the terrain: the terrain, sky dome, trees, etc.
	//result = pGraphics_->pZone_->Render(pGraphics_->GetD3DClass(),
	//	deltaTime,
	//	localTimer_);
	//COM_ERROR_IF_FALSE(result, "can't render the zone");

	//pGraphics_->lightsStore_.UpdatePointLights(deltaTime);
	
	//modelsStore.UpdateModels(deltaTime);

	modelsStore.RenderModels(pDeviceContext,
		editorFrustum,
		colorShader,
		textureShader,
		lightShader,
		pointLightShader,
		lightsStore,
		viewProj,
		cameraPos,
		systemState.renderedModelsCount,
		systemState.renderedVerticesCount,
		cameraDepth,
		totalGameTime);

	


	
	////////////////////////////////////////////////
	// RENDER MODELS
	////////////////////////////////////////////////

	// render different models (from the models list) on the scene
	//this->RenderRenderableGameObjects();
	//this->RenderReflectionPlane();


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
#endif

	//DrawSphereShadowReflection();
	//DrawMirror();

	//pImmediateContext_->ClearDepthStencilView(pGraphics_->pD3D_->GetDepthStencilView(), )

	//DrawSphereShadow();


	return true;
} // RenderModels()

///////////////////////////////////////////////////////////

bool RenderGraphics::RenderGUI(
	ID3D11DeviceContext* pDeviceContext, 
	D3DClass & d3d,
	SystemState & systemState,
	UserInterfaceClass & UI,
	const DirectX::XMMATRIX & WVO,    // world * basic_view * ortho
	const float deltaTime,
	const int gameCycles)
{
	// ATTENTION: do 2D rendering only when all 3D rendering is finished;
	// this function renders the engine/game GUI

	// turn off the Z buffer and enable alpha blending to begin 2D rendering
	d3d.TurnZBufferOff();
	d3d.TurnOnAlphaBlending();

	// if some rendering state has been updated we have to update some data for the GUI
	//this->UpdateGUIData(systemState);

	//const int isUpdateGUI = int(60.0f / deltaTime);

	// every 60 frames we update the UI
	//if (gameCycles % isUpdateGUI == 0)
	//{
		// update user interface for this frame (for the editor window)
		UI.Update(pDeviceContext, systemState);
	//}
	

	// render the user interface
	UI.Render(pDeviceContext, WVO);


	d3d.TurnOffAlphaBlending();  // turn off alpha blending now that the text has been rendered
	d3d.TurnZBufferOn();         // turn the Z buffer back on now that the 2D rendering has completed


	///////////////////////////////////////////////
	// RENDER A PICKED GAME OBJECT INTO THE TEXTURE
	///////////////////////////////////////////////

	// get a model and setup its position
	//GameObject* pCurrentPickedGameObj = pGraphics_->pGameObjectsList_->GetGameObjectByID("cube(2)");
	//pCurrentPickedGameObj->SetRotationInRad(0, localTimer_, 0);

	// render picked model to the texture and show a plane with this texture on the screen
	//this->RenderPickedGameObjToTexture(pCurrentPickedGameObj);

	////////////////////////////////////////////////

	// render 2D sprites onto the screen
	//this->Render2DSprites(deltaTime);

	return true;

} // end RenderGUI()


#if 0






////////////////////////////////////////////////////////////////////////////////////////////
//
//                             PRIVATE FUNCTIONS
//
////////////////////////////////////////////////////////////////////////////////////////////


void RenderGraphics::RenderRenderableGameObjects()
{
	//
	// this function renders different game objects from the game object rendering list 
	//

	// a ptr to the list of game objects for rendering onto the screen
	const std::map<std::string, RenderableGameObject*> & gameObjRenderList = graphics_->pGameObjectsList_->GetGameObjectsRenderingList();

	// control flags
	bool isRenderModel = false;              // according to this flag we define to render this model or not
	const bool enableModelMovement = false;  // do random movement/rotation of the models
	const float radius = 1.0f;               // a default radius of the model (it is used to check if a model is in the view frustum or not) 


	UINT modelIndex = 0;                     // the current index of the model
	RenderableGameObject* pGameObj = nullptr;


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
				MoveRotateScaleGameObjects(pGameObj, localTimer_, modelIndex);

			// check if the sphere model is in the view frustum
			isRenderModel = graphics_->editorFrustum_->CheckCube(pGameObj->GetPosition(), radius);

			// if it can be seen then render it, if not skip this model and check the next sphere
			if (isRenderModel)
			{
				// setup lighting for this model to make it colored with some color
				//pGraphics_->arrDiffuseLights_[0]->SetDiffuseColor(pGameObj->GetData()->GetColor());

				graphics_->pD3D_->SetRenderState(D3DClass::RASTER_PARAMS::CULL_MODE_NONE);

				pGameObj->Render();

				graphics_->pD3D_->SetRenderState(D3DClass::RASTER_PARAMS::CULL_MODE_BACK);

			} // if

			modelIndex++;
		} // for

	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(LOG_MACRO, "can't render some model");
		COM_ERROR_IF_FALSE(false, "can't render some model");
	}

	return;
}

///////////////////////////////////////////////////////////

void RenderGraphics::SetupGameObjectsForRenderingToTexture()
{
	// get some models and render it onto the texture
	RenderableGameObject* pSphere1 = graphics_->pGameObjectsList_->GetRenderableGameObjByID("sphere(1)");
	RenderableGameObject* pSphere2 = graphics_->pGameObjectsList_->GetRenderableGameObjByID("sphere(2)");
	RenderableGameObject* pCube1 = graphics_->pGameObjectsList_->GetRenderableGameObjByID("cube(1)");

	// setup game objects position before rendering to texture
	pSphere1->SetPosition(0, 0, 0);
	pSphere1->GetModel()->SetRenderShaderName("TextureShaderClass");

	pCube1->SetPosition(0, 2, 0);
	
	
	// add these game objects for rendering into a texture
	renderToTextureGameObjArr_.push_back(pSphere1);
	renderToTextureGameObjArr_.push_back(pSphere2);
	renderToTextureGameObjArr_.push_back(pCube1);

	return;

} // end SetupGameObjectsForRenderingToTexture

///////////////////////////////////////////////////////////

void RenderGraphics::RenderReflectionPlane()
{
	///////////////////////////////////////////////////////////////////////
	//  RENDER REFLECTED OBJECTS INTO A TEXTURE
	///////////////////////////////////////////////////////////////////////
	
	// render reflection of the game objects to a texture of the reflection plane (mirror)
	this->RenderReflectedSceneToTexture(renderToTextureGameObjArr_, pPlane3DRenderTargetObj_);


	///////////////////////////////////////////////////////////////////////
	//  RENDER THE REFLECTION PLANE AND REFLECTED OBJECTS ONTO THE SCREEN
	//////////////////////////////////////////////////////////////////////

	renderToTextureGameObjArr_[0]->SetRotationInRad(0, localTimer_, 0); // sphere_1
	renderToTextureGameObjArr_[0]->SetRotationInRad(0, -localTimer_, 0); // cube_1

	// start by rendering the reflected objects as normal
	pDataForShaders_->view = graphics_->editorCamera_->GetViewMatrix();  // use a normal view matrix

	for (GameObject* pGameObj : gameObjectsArr)
	{
		pGameObj->Render();
	}


	// now render the floor (reflection plane) using the reflection shader to blend the 
	// reflected render texture of the reflected objects into the floor model
	Model* pPlaneRenderTargetModel = pPlane3DRenderTargetObj_->GetModel();

	// setup data container before rendering of this model
	pDataForShaders_->indexCount = pPlaneRenderTargetModel->GetIndexCount();
	pDataForShaders_->world = pPlane3DRenderTargetObj_->GetWorldMatrix();
	pDataForShaders_->view = graphics_->editorCamera_->GetViewMatrix();  // use a normal view matrix
	pDataForShaders_->reflectionMatrix = graphics_->editorCamera_->GetReflectionViewMatrix();
	pDataForShaders_->projection = graphics_->GetProjectionMatrix();
	pDataForShaders_->texturesMap.insert_or_assign("diffuse", pPlane3DRenderTargetObj_->GetModel()->GetMeshByIndex(0)->GetTexturesArr()[0]->GetTextureResourceViewAddress());
	pDataForShaders_->texturesMap.insert_or_assign("reflection_texture", graphics_->renderToTexture_->GetShaderResourceViewAddress());

	pPlaneRenderTargetModel->GetMeshByIndex(0)->Draw();

	// render a plane with the scene (or only a single game obj) on it
	ShaderClass* pShader = graphics_->pShadersContainer_->GetShaderByName("ReflectionShaderClass");

	pShader->Render(pImmediateContext_, pDataForShaders_);

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

	RenderableGameObject* pPlaneGameObj = graphics_->GetGameObjectsList()->GetRenderableGameObjByID("plane(1)");

	pPlaneGameObj->SetPosition(posX_OfTexture, posY_OfTexture, 0.0f);
	pPlaneGameObj->SetRotationInDeg(0.0f, 0.0f, 0.0f);
	pPlaneGameObj->SetScale(100.0f, 100.0f, 1.0f);

	// also change the rendering texture
	pPlaneGameObj->GetModel()->SetRenderShaderName("TextureShaderClass");

	///////////////////////////////////////////////////////////////////////////

	// get a ptr a plane object which will be an another render target to render to
	pPlane2DRenderTargetObj_ = graphics_->GetGameObjectsList()->GetRenderableGameObjByID("plane(1)");
	pPlane3DRenderTargetObj_ = graphics_->GetGameObjectsList()->GetRenderableGameObjByID("plane(2)");

	// setup rendering planes
	pPlane3DRenderTargetObj_->SetPosition(0, -1.5f, 0);
	pPlane3DRenderTargetObj_->SetScale(3, 3, 3);
	pPlane3DRenderTargetObj_->SetRotationInDeg(0, 0, 90);  // the plane looks upward

	pPlane3DRenderTargetObj_->GetModel()->SetRenderShaderName("ReflectionShaderClass");
	pPlane3DRenderTargetObj_->GetModel()->GetMeshByIndex(0)->SetTextureByIndex(0, "data/textures/blue01.tga", aiTextureType::aiTextureType_DIFFUSE);

	return;
}

///////////////////////////////////////////////////////////

void RenderGraphics::UpdateGUIData(SystemState* pSystemState)
{
	// each frame some rendering data about the terrain are changing so we have 
	// to update some data for the GUI to render it onto the screen

	// try to update some data about the terrain rendering process to show it onto the screen;
	try
	{
		const RenderableGameObject* pTerrainGameObj = graphics_->pZone_->GetTerrainGameObj();

		// if there is some terrain game object
		if (pTerrainGameObj != nullptr)
		{
			Model* pTerrainModel = pTerrainGameObj->GetModel();
			
			// if we already initialized a terrain we have to setup some data
			if (pTerrainModel != nullptr)
			{
				TerrainClass* pTerrain = static_cast<TerrainClass*>(pTerrainModel);

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
		Log::Error(LOG_MACRO, "no terrain");
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
	auto spritesList = graphics_->pGameObjectsList_->GetSpritesRenderingList();

	// check if we have any 2D sprites for rendering
	if (spritesList.empty())
	{
		Log::Error(LOG_MACRO, "NO SPRITES FOR RENDERING");
		return;
	}


	// SETUP FOR 2D RENDERING: we will use these matrices later during rendering of different 2D stuff
	pDataForShaders_->world_main_matrix = graphics_->GetWorldMatrix();
	pDataForShaders_->baseView = graphics_->GetBaseViewMatrix();
	pDataForShaders_->ortho = graphics_->GetOrthoMatrix();

	// (WVO = main_world_matrix * base_view_matrix * ortho_matrix);
	// because we have the same matrices for each sprite during the current frame we just
	// multiply them and pass the result into the shader for rendering;
	//
	// NOTE: this WVP matrix uses in the SpriteShaderClass
	DirectX::XMMATRIX WVO = pDataForShaders_->world_main_matrix *
		pDataForShaders_->baseView *
		pDataForShaders_->ortho;

	// setup data container before rendering of all the 2D sprites
	this->pDataForShaders_->WVO = DirectX::XMMatrixTranspose(WVO);

	/////////////////////////////////////////////

	// turn off the Z buffer to begin 2D rendering
	graphics_->GetD3DClass()->TurnZBufferOff();
	
	for (const auto & elem : spritesList)
	{
		RenderableGameObject* pSpriteGameObj = elem.second;
		SpriteClass* pSpriteModel = static_cast<SpriteClass*>(pSpriteGameObj->GetModel());

		// if we want to render a crosshair we have to do it with some extra manipulations
		if (pSpriteGameObj->GetID() == "sprite_crosshair")
		{
			graphics_->GetD3DClass()->TurnOnAlphaBlending();
			pSpriteGameObj->RenderSprite();
			graphics_->GetD3DClass()->TurnOffAlphaBlending();

			continue;
		}
	
		// before rendering this sprite we have to update it (if it is an animation)
		// using the frame delta time
		pSpriteModel->Update(deltaTime);
		pSpriteGameObj->RenderSprite();
	}

	// turn the Z buffer back on now that 2D rendering has completed
	graphics_->GetD3DClass()->TurnZBufferOn();
	
	return;

} // end Render2DSprites

///////////////////////////////////////////////////////////

void RenderGraphics::RenderPickedGameObjToTexture(RenderableGameObject* pGameObj)
{
	// if we picked some model (clicked on it) we render it to the texture and 
	// show this texture on the screen  

	// render the game object onto the texture
	RenderSceneToTexture({ pGameObj });

	// turn off the Z buffer to begin 2D rendering
	graphics_->GetD3DClass()->TurnZBufferOff();

	// prepare a rendering pipeline for rendering this plane model onto the screen
	Model* pPlaneRenderTargetModel = pPlane2DRenderTargetObj_->GetModel();
	pPlaneRenderTargetModel->GetMeshByIndex(0)->Draw();
	
	// setup data container before rendering of this model
	pDataForShaders_->indexCount  = pPlaneRenderTargetModel->GetIndexCount();
	pDataForShaders_->world       = pPlane2DRenderTargetObj_->GetWorldMatrix();
	pDataForShaders_->view        = graphics_->GetBaseViewMatrix();
	pDataForShaders_->projection = graphics_->GetOrthoMatrix();	
	pDataForShaders_->texturesMap.insert_or_assign("diffuse", graphics_->renderToTexture_->GetShaderResourceViewAddress());

	
	// render a plane with the scene (or only a single game obj) on it
	ShaderClass* pShader = graphics_->pShadersContainer_->GetShaderByName("TextureShaderClass");
	TextureShaderClass* pTextureShader = static_cast<TextureShaderClass*>(pShader);

	pTextureShader->Render(pImmediateContext_, pDataForShaders_);

	// turn the Z buffer back on now that 2D rendering has completed
	graphics_->GetD3DClass()->TurnZBufferOn();

	return;

} // end RenderPickedGameObjToTexture

///////////////////////////////////////////////////////////

void RenderGraphics::RenderSceneToTexture(const std::vector<RenderableGameObject*> & gameObjArr)
{
	// if we have no game objects so just skip this function
	if (gameObjArr.empty() == true)
	{
		return;
	}

	// the first part in this function is where we change the rendering output from the 
	// back buffer to our render texture object. We also clear the render texture to
	// some background colour here (for instance: light blue or black)
	graphics_->renderToTexture_->ChangeRenderTarget(this->pImmediateContext_);
	graphics_->renderToTexture_->ClearRenderTarget(this->pImmediateContext_, Color(50, 100, 200).GetFloat4());  

	////////////////////////////////////////////////////////

	// now we set our camera position here first before getting the resulting view matrix
	// from the camera. If we are using different cameras, we need to set it each frame
	// since the other rendering functions use different cameras from a different position
	graphics_->cameraForRenderToTexture_->SetPosition(0.0f, 0.0f, -5.0f);

	// IMPORTANT: when we get our matrices, we have to get the projection matrix from 
	// the render texture as it has different dimensions than our regular screen projection
	// matrix. If your render texture ever look rendered incorrectly, it is usually because
	// you are using the wrong projection matrix
	pDataForShaders_->view = graphics_->cameraForRenderToTexture_->GetViewMatrix();
	graphics_->renderToTexture_->GetProjectionMatrix(pDataForShaders_->projection);


	// go through each game object in the array and render it into the texture
	for (RenderableGameObject* pGameObj : gameObjArr)
	{
		pGameObj->Render();
	}

	////////////////////////////////////////////////////////


	// once we are done rendering, we need to switch the rendering back to the original
	// back buffer. We also need to switch the viewport back to the original since
	// the render texture's viewport may have different dimensions then the screen's viewport.
	graphics_->pD3D_->SetBackBufferRenderTarget();
	graphics_->pD3D_->ResetViewport();

} // end RenderSceneToTexture

///////////////////////////////////////////////////////////

void RenderGraphics::RenderReflectedSceneToTexture(const std::vector<RenderableGameObject*> & gameObjArr,
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
	//pGraphics_->renderToTexture_->ChangeRenderTarget(this->pImmediateContext_);
	//pGraphics_->renderToTexture_->ClearRenderTarget(this->pImmediateContext_, Color(50, 100, 200).GetFloat4());

	////////////////////////////////////////////////////////


	// before render the scene to a texture, we need to create the reflection matrix using
	// the position of the floor/mirror (by Y axis) as the height variable
	graphics_->editorCamera_->UpdateReflectionViewMatrix(pReflectionPlane->GetPosition(), 
		pReflectionPlane->GetRotation());

	// now render the scene as normal but use the reflection matrix instead of the normal view matrix.
	//pDataForShaders_->view = pGraphics_->editorCamera_->GetReflectionViewMatrix();
	pDataForShaders_->projection = graphics_->GetProjectionMatrix();

	
	const DirectX::XMMATRIX reflectionMatrix = graphics_->editorCamera_->GetReflectionViewMatrix();

	// go through each game object in the array and render it into the texture
	for (RenderableGameObject* pGameObj : gameObjArr)
	{
		const DirectX::XMMATRIX prevWorldMatrix = pGameObj->GetWorldMatrix();
		
		pGameObj->SetWorldMatrix(prevWorldMatrix * reflectionMatrix);
		pGameObj->Render();
		pGameObj->SetWorldMatrix(prevWorldMatrix);
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

	
	// move and rotate the game object
	pGameObj->SetRotationInRad(t, 0.0f, 0.0f);

	if (modelIndex % 3 == 0)
	{
		// current game object's position;
		const DirectX::XMFLOAT3 & curPos = pGameObj->GetPosition();

		pGameObj->SetRotationInRad(t, 0.0f, 0.0f);
		pGameObj->SetPosition(curPos.x, t, curPos.z);
	}

	if (modelIndex % 2 == 0)
	{
		// current game object's position;
		const DirectX::XMFLOAT3 & curPos = pGameObj->GetPosition();

		pGameObj->SetRotationInRad(0.0f, t, 0.0f);
		pGameObj->SetPosition(t, curPos.y, curPos.z);
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
	pMirrorPlane_ = graphics_->pGameObjectsList_->GetRenderableGameObjByID("plane(" + std::to_string(planeIndex) + ")");
	pMirrorPlane_->SetPosition(0, 0, 0);
	pMirrorPlane_->GetModel()->GetMeshByIndex(0)->SetTextureByIndex(0, "data/textures/blue01.tga", aiTextureType_DIFFUSE);
	//pMirrorPlane_->GetModel()->SetRenderShaderName("ReflectionShaderClass");

	// setup the sphere which we will use to make a relfection and shadow of it
	pSphereForReflection_ = graphics_->pGameObjectsList_->GetRenderableGameObjByID("sphere(10)");
	pSphereForReflection_->SetPosition(0, 0, -3);

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
			RenderableGameObject* pWallGameObj = graphics_->pGameObjectsList_->GetRenderableGameObjByID(planeID);
			pWallGameObj->GetModel()->GetMeshByIndex(0)->SetTextureByIndex(0, "data/textures/brick01.dds", aiTextureType::aiTextureType_DIFFUSE);

			// setup the wall plane's position
			pWallGameObj->SetPosition((float)(i*2 - wallBias), float(j*2), 0);
			pWallGameObj->AdjustPosition(-2, 0, 0);   // move by -2 by X-axis

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
			RenderableGameObject* pFloorGameObj = graphics_->pGameObjectsList_->GetRenderableGameObjByID(planeID);
			pFloorGameObj->GetModel()->GetMeshByIndex(0)->SetTextureByIndex(0, "data/textures/checkboard.dds", aiTextureType::aiTextureType_DIFFUSE);

			// setup floor plane's position/rotation
			pFloorGameObj->SetPosition((float)floorPosX, -1, (float)floorPosZ - 1.0f);
			pFloorGameObj->AdjustPosition(-2, 0, 0);   // move by -2 by X-axis
			pFloorGameObj->SetRotationInDeg(0, 0, 90);

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
	for (RenderableGameObject* pWallPlane : wallPlanesArr_)
	{
		pWallPlane->Render();
	}

	// render floor planes
	for (RenderableGameObject* pFloorPlane : floorPlanesArr_)
	{
		pFloorPlane->Render();
	}

	return;
}

///////////////////////////////////////////////////////////

void RenderGraphics::DrawSphere()
{
	// update the rotation of the sphere
	pSphereForReflection_->SetRotationInRad(0, localTimer_, 0);

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
		graphics_->GetD3DClass()->TurnOnMarkMirrorOnStencil();

		pMirrorPlane_->Render();

		// restore the blend state after the marking of the mirror
		graphics_->GetD3DClass()->TurnOffMarkMirrorOnStencil();
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
	pDataForShaders_->world = pMirrorPlane_->GetWorldMatrix();
	pDataForShaders_->view = graphics_->editorCamera_->GetViewMatrix();  // use a normal view matrix
	pDataForShaders_->reflectionMatrix = graphics_->editorCamera_->GetReflectionViewMatrix();
	pDataForShaders_->projection = graphics_->GetProjectionMatrix();
	pDataForShaders_->texturesMap.insert_or_assign("diffuse", pMirrorModel->GetMeshByIndex(0)->GetTexturesArr()[0]->GetTextureResourceViewAddress());
	pDataForShaders_->texturesMap.insert_or_assign("reflection_texture", graphics_->renderToTexture_->GetShaderResourceViewAddress());

	pMirrorModel->GetMeshByIndex(0)->Draw();

	// render a plane with the scene (or only a single game obj) on it
	ShaderClass* pShader = graphics_->pShadersContainer_->GetShaderByName("ReflectionShaderClass");
	//ReflectionShaderClass* pReflectionShader = static_cast<ReflectionShaderClass*>(pShader);

	pShader->Render(pImmediateContext_, pDataForShaders_);
}

///////////////////////////////////////////////////////////

void RenderGraphics::DrawFloorReflection()
{
	// render reflection of the game objects to a texture of the reflection plane (mirror)
	//this->RenderReflectedSceneToTexture(floorPlanesArr_, pMirrorPlane_);

	//return;

	// update the reflection matrix for this frame
	graphics_->editorCamera_->UpdateReflectionViewMatrix(
		pMirrorPlane_->GetPosition(),
		pMirrorPlane_->GetRotation());

	const DirectX::XMMATRIX reflectionMatrix = graphics_->editorCamera_->GetReflectionViewMatrix();


	for (int p = 0; p < 2; p++)
	{
		// setup the blend state before the rendering of the mirror
		graphics_->GetD3DClass()->TurnOnMarkMirrorOnStencil();



		// now render the floor (reflection plane) using the reflection shader to blend the 
		// reflected render texture of the reflected objects into the floor model
		Model* pFloorPlaneModel = floorPlanesArr_[0]->GetModel();

		// setup data container before rendering of this model
		pDataForShaders_->indexCount = pFloorPlaneModel->GetIndexCount();
		pDataForShaders_->world = floorPlanesArr_[0]->GetWorldMatrix() * reflectionMatrix;
		pDataForShaders_->view = graphics_->editorCamera_->GetViewMatrix();  // use a normal view matrix
		//pDataForShaders_->reflectionMatrix = pGraphics_->editorCamera_->GetReflectionViewMatrix();
		pDataForShaders_->projection = graphics_->GetProjectionMatrix();
		pDataForShaders_->texturesMap.insert_or_assign("diffuse", pFloorPlaneModel->GetMeshByIndex(0)->GetTexturesArr()[0]->GetTextureResourceViewAddress());
		//pDataForShaders_->texturesMap.insert_or_assign("reflection_texture", pGraphics_->renderToTexture_->GetShaderResourceViewAddress());

		pFloorPlaneModel->GetMeshByIndex(0)->Draw();

		// render a plane with the scene (or only a single game obj) on it
		ShaderClass* pShader = graphics_->pShadersContainer_->GetShaderByName("TextureShaderClass");
		TextureShaderClass* pTextureShader = static_cast<TextureShaderClass*>(pShader);

		pTextureShader->Render(pImmediateContext_, pDataForShaders_);


		

		// restore the blend state after the rendering of the mirror
		graphics_->GetD3DClass()->TurnOffMarkMirrorOnStencil();
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

#endif