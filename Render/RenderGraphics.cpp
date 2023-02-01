////////////////////////////////////////////////////////////////////
// Filename:     RenderGraphics.cpp
// Description:  there are functions for rendering graphics;
// Created:      01.01.23
// Revising:     01.01.23
////////////////////////////////////////////////////////////////////
#include "RenderGraphics.h"


// prepares and renders all the models on the scene
bool RenderModels(GraphicsClass* pGraphics, int& renderCount)
{
	DirectX::XMFLOAT3 modelPosition;   // contains a position for particular model
	DirectX::XMFLOAT4 modelColor;      // contains a colour of a model
	static ModelClass* pModel = nullptr;
	int modelIndex = 0;


	bool result = false;
	size_t modelCount = 0;                // the number of models that will be rendered
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
	pGraphics->pFrustum_->ConstructFrustum(pGraphics->settingsList->FAR_Z, pGraphics->projectionMatrix_, pGraphics->viewMatrix_);

	// get the number of models that will be rendered
	modelCount = pGraphics->pModelList_->GetModelCount();



	auto modelsList = pGraphics->pModelList_->GetModelsList();


	if (true)
	{
		// go through all the models and render only if they can be seen by the camera view
		for (const auto& elem : modelsList)
		{
			if (elem.first == "terrain")
				continue;

			pModel = elem.second;   // get a pointer to the model for easier using 


			// get the position and colour of the sphere model at this index
			pGraphics->pModelList_->GetDataByID(pModel->GetID(), modelPosition, modelColor);

			// set the radius of the sphere to 1.0 since this is already known
			radius = 2.0f;

			// check if the sphere model is in the view frustum
			renderModel = pGraphics->pFrustum_->CheckCube(modelPosition.x, modelPosition.y, modelPosition.z, radius);

			// if it can be seen then render it, if not skip this model and check the next sphere
			if (true)
			{
				pModel->SetPosition(modelPosition.x, modelPosition.y, modelPosition.z);   // move the model to the location it should be rendered at
				pModel->SetScale(3.0f, 3.0f, 3.0f);
				pModel->SetRotation(t, 0.0f);
			
				if (modelIndex % 3 == 0)
				{
					pModel->SetRotation(t, 0.0f);
					pModel->SetPosition(modelPosition.x, t, modelPosition.z);
				}

				if (modelIndex % 3 == 0)
				{
					pModel->SetRotation(0.0f, t);
					pModel->SetPosition(t, modelPosition.y, modelPosition.z);
				}

			
			

				// put the model vertex and index buffers on the graphics pipeline 
				// to prepare them for drawing
				pModel->Render(pDevCon);

				//const DirectX::XMFLOAT4 defaultDiffuseColor = pGraphics->pLight_->GetDiffuseColor();

				pGraphics->pLight_->SetDiffuseColor(modelColor.x, modelColor.y, modelColor.z, modelColor.w);

				// render the model using the light shader
				result = pGraphics->pCombinedShader_->Render(pDevCon,
					pModel,
					pGraphics->viewMatrix_, 
					pGraphics->projectionMatrix_,
					pGraphics->editorCamera_.GetPositionFloat3(),
					pGraphics->pLight_);


				// set the diffuse colour to the default state
				//pGraphics->pLight_->SetDiffuseColor(defaultDiffuseColor.x, defaultDiffuseColor.y, defaultDiffuseColor.z, defaultDiffuseColor.w);



				COM_ERROR_IF_FALSE(result, "can't render the model using the colour shader");

				// since this model was rendered then increase the count for this frame
				renderCount++;
				modelIndex++;
			} // if
		} // for
	}



	// RENDER THE TERRAIN
	if (true)
	{
		pModel = pGraphics->pModelList_->GetModelByID("terrain");   // get a pointer to the terrain model object
		
		/*put the model vertex and index buffers on the graphics pipeline
		to prepare them for drawing */
		pModel->Render(pDevCon);

		// render the model using the light shader
		result = pGraphics->pTextureShader_->Render(pDevCon,
			pModel->GetIndexCount(),
			pModel->GetWorldMatrix(),
			pGraphics->viewMatrix_,
			pGraphics->projectionMatrix_,
			pModel->GetTextureArray()[0],
			1.0f);

		COM_ERROR_IF_FALSE(result, "can't render the TERRAIN");
	}


	return true;
} // RenderModels()


// ATTENTION: do 2D rendering only when all 3D rendering is finished;
// renders the engine/game GUI
bool RenderGUI(GraphicsClass* pGraphics,SystemState* systemState)
{
	bool result = false;

	pGraphics->pD3D_->TurnZBufferOff();       // turn off the Z buffer to begin all 2D rendering
	pGraphics->pD3D_->TurnOnAlphaBlending();  // turn on the alpha blending before rendering the text

	// render the text 
	result = RenderGUIDebugText(pGraphics, systemState);
	COM_ERROR_IF_FALSE(result, "can't render the GUI debug text");


	pGraphics->pD3D_->TurnOffAlphaBlending();   // turn off alpha blending after rendering the text
	pGraphics->pD3D_->TurnZBufferOn();          // turn the Z buffer on now that all 2D rendering has completed


	return true;
} // RenderGUI()


// render the debug data onto the screen in the upper-left corner
bool RenderGUIDebugText(GraphicsClass* pGraphics, SystemState* systemState)
{
	bool result = false;
	DirectX::XMFLOAT2 mousePos{ 0.0f, 0.0f };  // pInput->GetMousePos()
	int cpu = 0;

	// set up the debug text data
	result = pGraphics->pDebugText_->SetDebugParams(mousePos,
		SETTINGS::GetSettings()->WINDOW_WIDTH,
		SETTINGS::GetSettings()->WINDOW_HEIGHT,
		systemState->fps, systemState->cpu,
		systemState->editorCameraPosition,
		systemState->editorCameraRotation,
		systemState->renderCount);
	COM_ERROR_IF_FALSE(result, "can't update the debug params for output onto the screen");


	// render the debug text onto the screen
	result = pGraphics->pDebugText_->Render(pGraphics->pD3D_->GetDeviceContext(), pGraphics->worldMatrix_, pGraphics->orthoMatrix_);
	COM_ERROR_IF_FALSE(result, "can't render the debug info onto the screen");

	return true;
} // RenderGUIDebugText()