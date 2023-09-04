////////////////////////////////////////////////////////////////////
// Filename:     RenderGraphics.cpp
// Description:  there are functions for rendering graphics;
// Created:      01.01.23
// Revising:     01.01.23
////////////////////////////////////////////////////////////////////
#include "RenderGraphics.h"


RenderGraphics::RenderGraphics(Settings* pSettings)
{
	Log::Debug(THIS_FUNC_EMPTY);

	
	try
	{
		// the number of point light sources on the scene
		numPointLights_ = pSettings->GetSettingIntByKey("NUM_POINT_LIGHTS");  

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



// prepares and renders all the models on the scene
bool RenderGraphics::RenderModels(GraphicsClass* pGraphics, 
	int & renderCount, 
	float deltaTime)
{    
	Model* pModel = nullptr;   // a temporal pointer to some model
	bool result = false;

	// temporal pointers for handy using
	ID3D11Device*        pDevice = pGraphics->pD3D_->GetDevice();
	ID3D11DeviceContext* pDeviceContext = pGraphics->pD3D_->GetDeviceContext();

	// get pointers to shaders using which will be used to render the models
	ShaderClass* pShader = pGraphics->GetShadersContainer()->GetShaderByName("TextureShaderClass");
	TextureShaderClass* pTextureShader = static_cast<TextureShaderClass*>(pShader);
	pShader = pGraphics->pShadersContainer_->GetShaderByName("PointLightShaderClass");
	PointLightShaderClass* pPointLightShader = static_cast<PointLightShaderClass*>(pShader);
	pShader = pGraphics->pShadersContainer_->GetShaderByName("ColorShaderClass");
	ColorShaderClass* pColorShader = static_cast<ColorShaderClass*>(pShader);



	renderCount = 0;  // reset to zero as we haven't rendered models for this frame yet


	
	// renders models which are related to the terrain: the terrain, sky dome, trees, etc.
	pGraphics->pZone_->Render(renderCount, 
		pGraphics->GetD3DClass(),
		deltaTime, 
		pGraphics->pLights_,
		pGraphics->pPointLights_);


	/*

		
	// --- RENDER POINT LIGHTED MODELS --- //

	// setup the two arrays (color and position) from the point lights. 
	for (UINT i = 0; i < numPointLights_; i++)
	{
		arrPointLightsPositions_[i] = pGraphics->pLights_[i + 1].GetPosition();      // create the diffuse color array from the light colors
		arrPointLightsColors_[i] = pGraphics->pLights_[i + 1].GetDiffuseColor();     // create the light position array from the light positions
	}

	// render spheres as like they are point light sources
	for (UINT i = 0; i < numPointLights_; i++)
	{
		std::string sphereID{ "sphere(" + std::to_string(i + 1) + ")" };
		pModel = pGraphics->pModelList_->GetModelByID(sphereID);

		// setup spheres positions and colors to be the same as a point light source by this index 
		pModel->GetModelDataObj()->SetColor(arrPointLightsColors_[i]);
		pModel->GetModelDataObj()->SetPosition(arrPointLightsPositions_[i]);
		pModel->GetModelDataObj()->SetScale(0.2f, 0.2f, 0.2f);

		// render the sphere
		pModel->Render(pDeviceContext);

		result = pColorShader->Render(pDeviceContext,
			pModel->GetModelDataObj()->GetIndexCount(),
			pModel->GetModelDataObj()->GetWorldMatrix(),
			pGraphics->GetViewMatrix(),
			pGraphics->GetProjectionMatrix(),
			pModel->GetModelDataObj()->GetColor());
		COM_ERROR_IF_FALSE(result, "can't render the sphere using the color shader");
	}

	// setup the plane which will be illuminated by point light sources
	pModel = pGraphics->pModelList_->GetModelByID("plane(1)");

	pModel->GetModelDataObj()->SetPosition(0.0f, 0.5f, 0.0f);
	pModel->GetModelDataObj()->SetRotationInDegrees(0.0f, -90.0f, 0.0f);
	//pModel->GetModelDataObj()->SetScale(5.0f, 0.1f, 5.0f);

	pModel->Render(pDeviceContext);    // put the model's buffers into the rendering pipeline


	result = pPointLightShader->Render(pGraphics->pD3D_->GetDeviceContext(),
		pModel->GetModelDataObj()->GetIndexCount(),
		pModel->GetModelDataObj()->GetWorldMatrix(),
		pGraphics->GetViewMatrix(),
		pGraphics->GetProjectionMatrix(),
		pModel->GetTextureArray()->GetTextureResourcesArray(),
		arrPointLightsColors_.data(),
		arrPointLightsPositions_.data());
	COM_ERROR_IF_FALSE(result, "can't render the plane using the point light shader");


	*/




	/*
	
	
	
	DirectX::XMFLOAT3 modelPosition;   // contains some model's position
	DirectX::XMFLOAT4 modelColor;      // contains a colour of a model
	Model* pModel = nullptr;    // a temporal pointer to a model

	int modelIndex = 0;                // the current index of the model 
	bool result = false;
	size_t modelCount = 0;             // the number of models that will be rendered during this frame
	float radius = 1.0f;               // a default radius of the model (it is used to check if a model is in the view frustum or not) 
	
	bool enableModelRendering = true;  // defines if we enable models rendering at all
	bool enableModelMovement = false;  // defines if the rendered model must move in some way
	bool isRenderModel = false;         // if the model inside the frustum we render it
	bool isRenderTerrain = true;       // defines if we render terrain, sky dome, etc. (for debugging)
	bool isRender2DSprites = false;    // defines if we render 2D sprites onto the screen
	
	
	



	// local timer							 
	static float t = 0.0f;
	static DWORD dwTimeStart = 0;
	DWORD dwTimeCur = GetTickCount();

	if (dwTimeStart == 0)
		dwTimeStart = dwTimeCur;
	t = (dwTimeCur - dwTimeStart) / 1000.0f;






	// construct the frustum
	pGraphics->pFrustum_->ConstructFrustum(pGraphics->projectionMatrix_, pGraphics->viewMatrix_);

	// get the number of models that will be rendered
	modelCount = pGraphics->pModelList_->GetRenderedModelsCount();

	// get a list with all the models for rendering on the scene
	auto modelsList = pGraphics->pModelList_->GetModelsRenderingList();

	// setup the colour of the diffuse light on the scene
	pGraphics->pLights_->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);

	// setup the diffuse light direction
	pGraphics->pLights_->SetDirection(cos(t / 2), -0.5f, sin(t / 2));
	

	if (enableModelRendering)
	{

		// go through all the models and render only if they can be seen by the camera view
		for (const auto& elem : modelsList)
		{
			// skip the terrain related stuff since it is already rendered particularly
			if (elem.first == "terrain"  ||
				elem.first == "sky_dome" ||
				elem.first == "sky_plane")
			{
				continue;
			}

			pModel = elem.second;   // get a pointer to the model for easier using 

			// get the position and colour of the model at this index
			pGraphics->pModelList_->GetDataByID(pModel->GetModelDataObj()->GetID(), modelPosition, modelColor);

			// set the radius of the sphere/cube to 1.0 since this is already known
			// radius = 1.0f;

			// check if the sphere model is in the view frustum
			isRenderModel = pGraphics->pFrustum_->CheckCube(modelPosition.x, modelPosition.y, modelPosition.z, radius);

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
				pGraphics->pLights_->SetDiffuseColor(modelColor.x, modelColor.y, modelColor.z, modelColor.w);
			
				// put the model vertex and index buffers on the graphics pipeline 
				// to prepare them for drawing
				pModel->Render(pDeviceContext);
				pTextureShader->Render(pDeviceContext,
					pModel->GetModelDataObj()->GetIndexCount(),
					pModel->GetModelDataObj()->GetWorldMatrix(),
					pGraphics->GetBaseViewMatrix(),
					pGraphics->GetOrthoMatrix(),
					pModel->GetTextureArray()->GetTextureResourcesArray());

				// since this model was rendered then increase the counts for this frame
				renderCount++;
				modelIndex++;
			} // if
		} // for
	} // if (isRenderModel)


	
	//
	//     RENDER 2D SPRITES     //
	//
	if (isRender2DSprites)
	{
		// turn off the Z buffer to begin all 2D rendering
		pGraphics->pD3D_->TurnZBufferOff();

		// get a list with 2D sprites
		auto spritesList = pGraphics->pModelList_->GetSpritesRenderingList();

		for (const auto & listElem : spritesList)
		{
			SpriteClass* pSprite = static_cast<SpriteClass*>(listElem.second);
			
			// before rendering this sprite we have to update it using the frame time
			pSprite->Update(deltaTime);

			//pSprite->GetModelDataObj()->SetPosition(0.0f, 4.0f, 0.0f);

			pSprite->Render(pDeviceContext);
			pTextureShader->Render(pDeviceContext,
				pSprite->GetModelDataObj()->GetIndexCount(),
				pGraphics->GetWorldMatrix(),
				pGraphics->GetBaseViewMatrix(),
				pGraphics->GetOrthoMatrix(),
				pSprite->GetTexture());
		}

		// turn the Z buffer back on now that all 2D rendering has completed
		pGraphics->pD3D_->TurnZBufferOn();
	}

	
	
	*/



	return true;
} // RenderModels()


// ATTENTION: do 2D rendering only when all 3D rendering is finished;
// renders the engine/game GUI
bool RenderGraphics::RenderGUI(GraphicsClass* pGraphics, SystemState* systemState)
{
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

	return true;
} // RenderGUI()