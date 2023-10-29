////////////////////////////////////////////////////////////////////
// Filename:     RenderGraphics.cpp
// Description:  there are functions for rendering graphics;
// Created:      01.01.23
// Revising:     01.01.23
////////////////////////////////////////////////////////////////////
#include "RenderGraphics.h"


RenderGraphics::RenderGraphics(GraphicsClass* pGraphics, Settings* pSettings)
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

	// local timer							
	DWORD dwTimeCur = GetTickCount();
	static DWORD dwTimeStart = dwTimeCur;

	// update the local timer
	float t = (dwTimeCur - dwTimeStart) / 1000.0f;

	Model* pModel = nullptr;    // a temporal pointer to a model
	renderCount = 0;            // set to zero as we haven't rendered models for this frame yet
	bool result = false;


	////////////////////////////////////////////////

	// setup the colour of the diffuse light on the scene
	pGraphics_->arrDiffuseLights_[0]->SetDiffuseColor(1.0f, 0.5f, 0.0f, 1.0f);

	// setup the diffuse light direction (sun direction)
	pGraphics->arrDiffuseLights_[0]->SetDirection(cos(t / 2), -0.5f, sin(t / 2));

	// renders models which are related to the terrain: the terrain, sky dome, trees, etc.
	pGraphics->pZone_->Render(renderCount,
		pGraphics->GetD3DClass(),
		deltaTime,
		pGraphics->arrDiffuseLights_,
		pGraphics->arrPointLights_);

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

	// for getting the terrain data
	TerrainClass* pTerrain = static_cast<TerrainClass*>(pGraphics->pModelList_->GetModelByID("terrain"));

	// update user interface
	result = pGraphics->pUserInterface_->Frame(pGraphics->pD3D_->GetDeviceContext(), 
		//pGraphics->pSettingsList_, 
		systemState, 
		pGraphics->GetCamera()->GetPositionFloat3(),
		pGraphics->GetCamera()->GetRotationFloat3InDegrees(),
		pTerrain->GetRenderCount(),   // the number of rendered models
		pTerrain->GetCellsDrawn(),    // the number of rendered terrain cells
		pTerrain->GetCellsCulled());  // the number of culled terrain cells
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
	this->RenderPickedModelToTexture(pGraphics->pD3D_->GetDeviceContext(), pCurrentPickedModel);

	////////////////////////////////////////////////

	// render 2D sprites onto the screen
	this->Render2DSprites(pGraphics->pD3D_->GetDeviceContext(), pGraphics, deltaTime);

	return true;

} // end RenderGUI()

///////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////////////////
//                             PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////

void RenderGraphics::RenderModelsObjects(ID3D11DeviceContext* pDeviceContext,
	int & renderCount)
{
	try
	{


	//
	// this function renders different models from the models rendering list 
	//

	DirectX::XMFLOAT3 modelPosition;   // contains some model's position
	DirectX::XMFLOAT4 modelColor;      // contains a colour of a model

	Model* pModel = nullptr;                            
	DataContainerForShaders* pDataContainer = nullptr;  // a ptr to data container for shaders
	int modelIndex = 0;                                 // the current index of the model 
	size_t modelCount = 0;                              // the number of models that will be rendered during this frame
	float radius = 1.0f;                                // a default radius of the model (it is used to check if a model is in the view frustum or not) 
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

	// get the number of models that will be rendered
	modelCount = pGraphics_->pModelList_->GetRenderedModelsCount();

	// get a list with all the models for rendering on the scene
	auto modelsList = pGraphics_->pModelList_->GetModelsRenderingList();


	// render the triangle
	/*
	
	pModel = modelsList["triangle(1)"];
	assert(pModel != nullptr);
	pModel->GetModelDataObj()->SetPosition({ 0.0f, 5.0f, 0.0f });
	pModel->Render(pDeviceContext);
	pGraphics_->GetShadersContainer()->GetColorShader()->Render(pDeviceContext,
		pModel->GetModelDataObj()->GetIndexCount(),
		pModel->GetModelDataObj()->GetWorldMatrix(),
		pGraphics_->GetViewMatrix(),
		pGraphics_->GetProjectionMatrix(),
		{ 1.0f, 1.0f, 1.0f, 1.0f });
	
	*/
	


	////////////////////////////////////////////////




	// go through all the models and render only if they can be seen by the camera view
	for (const auto & elem : modelsList)
	{
		// skip the terrain related stuff since it is already rendered particularly
		if (elem.first == "sphere(1)" ||
			elem.first == "sphere(2)" || 
			elem.first == "sphere(3)")
		{
			continue;
		}

		// check if the current element has a propper pointer to the model
		assert(elem.second != nullptr);

		// get a pointer to the model for easier using 
		pModel = elem.second;   

		if (elem.first == "triangle(1)")
			pModel->GetModelDataObj()->SetPosition({ 0.0f, 5.0f, 0.0f });


		if (pModel->GetModelDataObj()->GetID() == "line3D")
		{
			// setup data container before rendering of this model
			pDataContainer = pModel->GetDataContainerForShaders();
			pDataContainer->indexCount = pModel->GetModelDataObj()->GetIndexCount();
			pDataContainer->world = pModel->GetModelDataObj()->GetWorldMatrix();
			pDataContainer->view = pGraphics_->GetViewMatrix();
			pDataContainer->orthoOrProj = pGraphics_->GetProjectionMatrix();
			pDataContainer->modelColor = pModel->GetModelDataObj()->GetColor();

			// render the model onto the screen
			pModel->Render(pDeviceContext);
		}
	

		// get the position and colour of the model at this index
		pGraphics_->pModelList_->GetDataByID(pModel->GetModelDataObj()->GetID(), modelPosition, modelColor);

		// check if the sphere model is in the view frustum
		isRenderModel = pGraphics_->pFrustum_->CheckSphere(modelPosition.x, modelPosition.y, modelPosition.z, radius);

		// if it can be seen then render it, if not skip this model and check the next sphere
		if (isRenderModel)
		{
			if (enableModelMovement)
			{
				// modifications of the models' position/scale/rotation
				pModel->GetModelDataObj()->SetPosition(modelPosition.x, modelPosition.y, modelPosition.z);   // move the model to the location it should be rendered at
				pModel->GetModelDataObj()->SetScale(5.0f, 5.0f, 5.0f);
				pModel->GetModelDataObj()->SetRotation(t, 0.0f, 0.0f);

				if (modelIndex % 3 == 0)
				{
					pModel->GetModelDataObj()->SetRotation(t, 0.0f, 0.0f);
					pModel->GetModelDataObj()->SetPosition(modelPosition.x, t, modelPosition.z);
				}

				if (modelIndex % 2 == 0)
				{
					pModel->GetModelDataObj()->SetRotation(0.0f, t, 0.0f);
					pModel->GetModelDataObj()->SetScale(1.0f, 1.0f, 1.0f);
					pModel->GetModelDataObj()->SetPosition(t, modelPosition.y, modelPosition.z);
				}
			}

			// setup lighting for this model to make it colored with some color
			pGraphics_->arrDiffuseLights_[0]->SetDiffuseColor(modelColor.x, modelColor.y, modelColor.z, modelColor.w);

		
			

		


			// put the model vertex and index buffers on the graphics pipeline 
			// to prepare them for drawing

			
			
			//pModel->Render(pDeviceContext);
			
			/*
			
			pGraphics_->GetShadersContainer()->GetLightShader()->Render(pDeviceContext,
			pModel->GetModelDataObj()->GetIndexCount(),
			pModel->GetModelDataObj()->GetWorldMatrix(),
			pGraphics_->GetViewMatrix(),
			pGraphics_->GetProjectionMatrix(),
			pModel->GetTextureArray()->GetTextureResourcesArray(),
			pGraphics_->pCamera_->GetPositionFloat3(),
			*(pGraphics_->arrDiffuseLights_.data()));
			
			*/
			
			
			
			

			// since this model was rendered then increase the counts for this frame
			renderCount++;
			modelIndex++;
		} // if
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

void RenderGraphics::Render2DSprites(ID3D11DeviceContext* pDeviceContext,
	GraphicsClass* pGraphics,
	const float deltaTime)
{
	// this function renders all the 2D sprites onto the screen

	// turn off the Z buffer to begin 2D rendering
	pGraphics->GetD3DClass()->TurnZBufferOff();

	// get a list with 2D sprites
	auto spritesList = pGraphics->GetModelsList()->GetSpritesRenderingList();

	for (const auto & listElem : spritesList)
	{
		SpriteClass* pSprite = static_cast<SpriteClass*>(listElem.second);

		if (pSprite->GetModelDataObj()->GetID() == "sprite_crosshair")
		{
			pGraphics->GetD3DClass()->TurnOnAlphaBlending();
		}

		// before rendering this sprite we have to update it using the frame time
		pSprite->Update(deltaTime);

		//pSprite->GetModelDataObj()->SetPosition(0.0f, 4.0f, 0.0f);

		pSprite->Render(pDeviceContext);
		pGraphics->GetShadersContainer()->GetTextureShader()->Render(pDeviceContext,
			pSprite->GetModelDataObj()->GetIndexCount(),
			pGraphics->GetWorldMatrix(),
			pGraphics->GetBaseViewMatrix(),
			pGraphics->GetOrthoMatrix(),
			pSprite->GetTexture());


		if (pSprite->GetModelDataObj()->GetID() == "sprite_crosshair")
		{
			pGraphics->GetD3DClass()->TurnOffAlphaBlending();
		}
	}

	// turn the Z buffer back on now that 2D rendering has completed
	pGraphics->GetD3DClass()->TurnZBufferOn();

	
	
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
	Model* pPlane = pGraphics_->pModelList_->GetModelByID("plane(1)");
	pPlane->GetModelDataObj()->SetPosition(posX_OfTexture, posY_OfTexture, 0.0f);
	pPlane->GetModelDataObj()->SetRotationInDegrees(0.0f, 0.0f, 180.0f);
	pPlane->GetModelDataObj()->SetScale(100.0f, 100.0f, 1.0f);

	// setup data container before rendering of this model
	DataContainerForShaders* pDataContainer = pPlane->GetDataContainerForShaders();
	pDataContainer->indexCount = pPlane->GetModelDataObj()->GetIndexCount();
	pDataContainer->world = pPlane->GetModelDataObj()->GetWorldMatrix();
	pDataContainer->view = pGraphics_->GetBaseViewMatrix();
	pDataContainer->orthoOrProj = pGraphics_->GetOrthoMatrix();
	pDataContainer->ppTextures = pGraphics_->pRenderToTexture_->GetShaderResourceView();

	pPlane->Render(pDeviceContext);

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