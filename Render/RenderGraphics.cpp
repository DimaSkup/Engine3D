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
	DirectX::XMVECTOR modelColor;           // contains a colour of a model

	bool result = false;
	int modelCount = 0;                // the number of models that will be rendered
	bool renderModel = false;          // a flag which defines if we render a model or not
	float radius = 0.0f;               // a default radius of the model
	renderCount = 0;                   // set to zero as we haven't rendered models yet

	// temporal pointers for easier using
	ID3D11Device*        pDevice = pGraphics->pD3D_->GetDevice();
	ID3D11DeviceContext* pDevCon = pGraphics->pD3D_->GetDeviceContext();

	// timer							 
	static float t = 0.0f;
	static DWORD dwTimeStart = 0;
	DWORD dwTimeCur = GetTickCount();

	if (dwTimeStart == 0)
		dwTimeStart = dwTimeCur;

	t = (dwTimeCur - dwTimeStart) / 1000.0f;



	// render the squares
	if (true)
	{
		bool result = false;

		static TextureClass* textureCat = nullptr;
		if (textureCat == nullptr)
		{
			textureCat = new TextureClass();
			COM_ERROR_IF_FALSE(textureCat, "can't create a new TextureClass object for the cat texture");

			result = textureCat->Initialize(pDevice, L"data/textures/cat.dds");
			COM_ERROR_IF_FALSE(result, "can't init the cat texture");
		}


		static TextureClass* textureGigachad = nullptr;
		if (textureGigachad == nullptr)
		{
			textureGigachad = new TextureClass();
			COM_ERROR_IF_FALSE(textureGigachad, "can't create a new TextureClass object for the gigachad texture");

			result = textureGigachad->Initialize(pDevice, L"data/textures/gigachad.dds");
			COM_ERROR_IF_FALSE(result, "can't init the gigachad texture");
		}


		pGraphics->pD3D_->TurnOnAlphaBlending();
		pGraphics->pCatSquare_->Render(pDevCon);

		//
		// cat
		//
		pGraphics->pCatSquare_->SetScale(1.0f, 1.0f, 1.0f);
		pGraphics->pCatSquare_->SetPosition(0.0f, 5.0f, 1.0f);

		// render the cat square
		result = pGraphics->pTextureShader_->Render(pDevCon, pGraphics->pCatSquare_->GetIndexCount(), pGraphics->pCatSquare_->GetWorldMatrix(), pGraphics->viewMatrix_, pGraphics->projectionMatrix_, textureCat->GetTexture(), 1.0f);
		COM_ERROR_IF_FALSE(result, "can't render the cat square");


		//
		// bateman
		//
		pGraphics->pCatSquare_->SetPosition(0.0f, 5.0f, 0.0f);
		pGraphics->pCatSquare_->SetScale(2.0f, 2.0f, 2.0f);

		// render the bateman square
		result = pGraphics->pTextureShader_->Render(pDevCon, pGraphics->pCatSquare_->GetIndexCount(), pGraphics->pCatSquare_->GetWorldMatrix(), pGraphics->viewMatrix_, pGraphics->projectionMatrix_, pGraphics->pCatSquare_->GetTexture(), 1.0f);
		COM_ERROR_IF_FALSE(result, "can't render the bateman square");


		//
		// gigachad
		//
		pGraphics->pCatSquare_->SetScale(0.2f, 0.2f, 0.2f);
		pGraphics->pCatSquare_->SetPosition(0.0f, 5.0f, -1.0f);

		// render the gigachad texture
		result = pGraphics->pTextureShader_->Render(pDevCon, pGraphics->pCatSquare_->GetIndexCount(), pGraphics->pCatSquare_->GetWorldMatrix(), pGraphics->viewMatrix_, pGraphics->projectionMatrix_, textureGigachad->GetTexture(), 0.5f);
		COM_ERROR_IF_FALSE(result, "can't render the gigachad square");

		pGraphics->pD3D_->TurnOffAlphaBlending();
		//_SHUTDOWN(textureGigachad);
	}


	// render the yellow square
	if (true)
	{
		pGraphics->pYellowSquare_->Render(pDevCon);

		result = pGraphics->pColorShader_->Render(pDevCon, pGraphics->pYellowSquare_->GetIndexCount(), pGraphics->pYellowSquare_->GetWorldMatrix(), pGraphics->viewMatrix_, pGraphics->projectionMatrix_);
		COM_ERROR_IF_FALSE(result, "can't render the red triangle using the colour shader");
	}




	// render the red triangle
	if (true)
	{
		pGraphics->pTriangleRed_->Render(pDevCon);

		result = pGraphics->pColorShader_->Render(pDevCon, pGraphics->pTriangleRed_->GetIndexCount(), pGraphics->pTriangleRed_->GetWorldMatrix(), pGraphics->viewMatrix_, pGraphics->projectionMatrix_);
		COM_ERROR_IF_FALSE(result, "can't render the red triangle using the colour shader");
	}

	// render the green triangle
	if (true)
	{
		pGraphics->pTriangleGreen_->Render(pDevCon);
		pGraphics->pTriangleGreen_->SetScale(0.3f, 0.3f, 0.3f);

		result = pGraphics->pColorShader_->Render(pDevCon, pGraphics->pTriangleGreen_->GetIndexCount(), pGraphics->pTriangleGreen_->GetWorldMatrix(), pGraphics->viewMatrix_, pGraphics->projectionMatrix_);
		COM_ERROR_IF_FALSE(result, "can't render the green triangle using the colour shader");
	}




	// construct the frustum
	pGraphics->pFrustum_->ConstructFrustum(pGraphics->settingsList->FAR_Z, pGraphics->projectionMatrix_, pGraphics->viewMatrix_);

	// get the number of models that will be rendered
	modelCount = pGraphics->pModelList_->GetModelCount();

	if (true)
	{
		// go through all the models and render only if they can be seen by the camera view
		for (size_t index = 0; index < modelCount; index++)
		{
			// get the position and colour of the sphere model at this index
			pGraphics->pModelList_->GetData(static_cast<int>(index), modelPosition, modelColor);

			// set the radius of the sphere to 1.0 since this is already known
			radius = 1.0f;

			// check if the sphere model is in the view frustum
			renderModel = pGraphics->pFrustum_->CheckSphere(modelPosition.x, modelPosition.y, modelPosition.z, radius);

			// if it can be seen then render it, if not skip this model and check the next sphere
			if (renderModel)
			{
				// put the model vertex and index buffers on the graphics pipeline 
				// to prepare them for drawing
				pGraphics->pModel_->Render(pDevCon);
				pGraphics->pModel_->SetPosition(modelPosition.x, modelPosition.y, modelPosition.z);   // move the model to the location it should be rendered at
																						   //pModel_->SetScale(2.0f, 1.0f, 1.0f);
				pGraphics->pModel_->SetRotation(t, 0.0f);

				// render the model using the light shader
				result = pGraphics->pLightShader_->Render(pDevCon,
					pGraphics->pModel_->GetIndexCount(),
					pGraphics->pModel_->GetWorldMatrix(), 
					pGraphics->viewMatrix_, 
					pGraphics->projectionMatrix_,
					pGraphics->pModel_->GetTexture(),
					//m_Light->GetDiffuseColor(),
					{
						DirectX::XMVectorGetX(modelColor),
						DirectX::XMVectorGetY(modelColor),
						DirectX::XMVectorGetZ(modelColor),
						1.0f
					},

					pGraphics->pLight_->GetDirection(),
					pGraphics->pLight_->GetAmbientColor(),

					pGraphics->editorCamera_.GetPositionFloat3(),
					pGraphics->pLight_->GetSpecularColor(),
					pGraphics->pLight_->GetSpecularPower());

				COM_ERROR_IF_FALSE(result, "can't render the model using the colour shader");

				// since this model was rendered then increase the count for this frame
				renderCount++;
			} // if
		} // for

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