////////////////////////////////////////////////////////////////////
// Filename:     RenderGraphics.cpp
// Description:  there are functions for rendering graphics;
// Created:      01.01.23
// Revising:     01.01.23
////////////////////////////////////////////////////////////////////
#include "RenderGraphics.h"


RenderGraphics::RenderGraphics()
{
	Log::Debug(THIS_FUNC_EMPTY);
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
	
	DirectX::XMFLOAT3 modelPosition;   // contains some model's position
	DirectX::XMFLOAT4 modelColor;      // contains a colour of a model
	static Model* pModel = nullptr;
	int modelIndex = 0;
	bool enableModelMoving = false;


	bool result = false;
	size_t modelCount = 0;             // the number of models that will be rendered
	bool renderModel = false;          // a flag which defines if we render a model or not
	float radius = 0.0f;               // a default radius of the model
	renderCount = 0;                   // set to zero as we haven't rendered models yet

	// temporal pointers for easier using
	static ID3D11Device*        pDevice = pGraphics->pD3D_->GetDevice();
	static ID3D11DeviceContext* pDevCon = pGraphics->pD3D_->GetDeviceContext();

	// timer							 
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
	pGraphics->pLight_->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);

	// renders models which are related to the terrain: the terrain, sky dome, trees, etc.
	pGraphics->pZone_->Render(renderCount, pGraphics->GetD3DClass(), deltaTime);

	if (true)
	{
		// go through all the models and render only if they can be seen by the camera view
		for (const auto& elem : modelsList)
		{
			// we render the terrain related models separately (because we don't want to move it or do something else)
			if (elem.first == "terrain"  ||
				elem.first == "sky_dome" ||
				elem.first == "sky_plane")
			{
				continue;
			}
	
			// setup the diffuse light direction
			pGraphics->pLight_->SetDirection(cos(t / 2), -0.5f, sin(t / 2));

			pModel = elem.second;   // get a pointer to the model for easier using 

			// get the position and colour of the model at this index
			pGraphics->pModelList_->GetDataByID(pModel->GetModelDataObj()->GetID(), modelPosition, modelColor);

			// set the radius of the sphere to 1.0 since this is already known
			radius = 10.0f;

			// check if the sphere model is in the view frustum
			renderModel = pGraphics->pFrustum_->CheckCube(modelPosition.x, modelPosition.y, modelPosition.z, radius);

			// if it can be seen then render it, if not skip this model and check the next sphere
			if (renderModel)
			{
				if (false)
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

			
				// put the model vertex and index buffers on the graphics pipeline 
				// to prepare them for drawing
				pGraphics->pLight_->SetSpecularPower(32.0f);
				pGraphics->pLight_->SetDiffuseColor(modelColor.x, modelColor.y, modelColor.z, modelColor.w);


				pModel->Render(pDevCon);

				// since this model was rendered then increase the count for this frame
				renderCount++;
				modelIndex++;
			} // if
		} // for
	}



	// --- RENDER 2D SPRITES --- //

	// turn off the Z buffer to begin all 2D rendering
	pGraphics->pD3D_->TurnZBufferOff();

	// get a list with 2D sprites
	auto spritesList = pGraphics->pModelList_->GetSpritesRenderingList();

	for (const auto & listElem : spritesList)
	{
		SpriteClass* pSprite = static_cast<SpriteClass*>(listElem.second);
		ShaderClass* pShader = pGraphics->GetShadersContainer()->GetShaderByName("TextureShaderClass");
		TextureShaderClass* pTextureShader = static_cast<TextureShaderClass*>(pShader);

		// before rendering this sprite we have to update it using the frame time
		pSprite->Update(deltaTime);

		//pSprite->GetModelDataObj()->SetPosition(0.0f, 4.0f, 0.0f);

		pSprite->Render(pDevCon);
		pTextureShader->Render(pDevCon,
			pSprite->GetModelDataObj()->GetIndexCount(),
			pGraphics->GetWorldMatrix(),
			pGraphics->GetBaseViewMatrix(),
			pGraphics->GetOrthoMatrix(),
			pSprite->GetTexture());
	}

	// turn the Z buffer back on now that all 2D rendering has completed
	pGraphics->pD3D_->TurnZBufferOn();



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
		pGraphics->pZone_->GetCamera()->GetPositionFloat3(),
		pGraphics->pZone_->GetCamera()->GetRotationFloat3InDegrees(),
		pTerrain->GetRenderCount(),   // the number of rendered models
		pTerrain->GetCellsDrawn(),    // the number of rendered terrain cells
		pTerrain->GetCellsCulled());  // the number of culled terrain cells
	COM_ERROR_IF_FALSE(result, "can't do frame calculations for the user interface");



	// render the user interface
	result = pGraphics->pUserInterface_->Render(pGraphics->pD3D_, pGraphics->GetWorldMatrix(), pGraphics->GetOrthoMatrix());
	COM_ERROR_IF_FALSE(result, "can't render the user interface");

	return true;
} // RenderGUI()