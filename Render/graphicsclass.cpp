////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
// Revising: 14.10.22
////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"

GraphicsClass::GraphicsClass(void)
{

}

// We don't use the copy constructor and destructor in this class
GraphicsClass::GraphicsClass(const GraphicsClass& another) {}
GraphicsClass::~GraphicsClass(void) {}



// ----------------------------------------------------------------------------------- //
//
//                             PUBLIC METHODS
//
// ----------------------------------------------------------------------------------- //

// Initializes all the main parts of graphics rendering module
bool GraphicsClass::Initialize(HWND hwnd)
{
	Log::Debug(THIS_FUNC_EMPTY);

	bool result = false;

	// --------------------------------------------------------------------------- //
	//              INITIALIZE ALL THE PARTS OF GRAPHICS SYSTEM                    //
	// --------------------------------------------------------------------------- //

	settingsList = SETTINGS::GetSettings();


	if (!InitializeDirectX(this, hwnd,
			settingsList->WINDOW_WIDTH,
			settingsList->WINDOW_HEIGHT,
			settingsList->VSYNC_ENABLED,
			settingsList->FULL_SCREEN,
			settingsList->NEAR_Z,
			settingsList->FAR_Z))
		return false;

	if (!InitializeShaders(this, hwnd))
		return false;

	if (!InitializeScene(this, hwnd, settingsList))
		return false;



	Log::Print(THIS_FUNC, " is successfully initialized");
	return true;
}

// Shutdowns all the graphics rendering parts, releases the memory
void GraphicsClass::Shutdown()
{
	_SHUTDOWN(pModelList_);
	_DELETE(pFrustum_);
	_SHUTDOWN(pDebugText_);

	
	//_SHUTDOWN(pCharacter2D_);
	_SHUTDOWN(pBitmap_);

	_DELETE(pLight_);

	// shaders
	_SHUTDOWN(pColorShader_);

	_SHUTDOWN(pModel_);
	_SHUTDOWN(pD3D_);

	Log::Get()->Debug(THIS_FUNC_EMPTY);

	return;
} // Shutdown()




// Executes rendering of each frame
bool GraphicsClass::RenderFrame(SystemState* systemState, 
								KeyboardEvent& kbe, 
								MouseEvent& me,
								MouseClass& mouse,
								float deltaTime)  // the time passed since the last frame
{
	bool result = false;

	// Clear all the buffers before frame rendering
	this->pD3D_->BeginScene();

	// update matrices
	pD3D_->GetWorldMatrix(worldMatrix_);
	projectionMatrix_ = editorCamera_.GetProjectionMatrix();
	pD3D_->GetOrthoMatrix(orthoMatrix_);

	// get the view matrix based on the camera's position
	viewMatrix_ = editorCamera_.GetViewMatrix();

	// during each frame the position class object is updated with the 
	// frame time for calculation the updated position
	editorCamera_.SetFrameTime(deltaTime);

	// after the frame time update the position class movement functions can be updated
	// with the current state of the input devices. The movement function will update
	// the position of the camera to the location for this frame
	editorCamera_.HandleMovement(kbe, me, mouse);
	

	systemState->editorCameraPosition = editorCamera_.GetPositionFloat3();
	systemState->editorCameraRotation = editorCamera_.GetRotationFloat3();

	RenderScene(systemState);  // render all the stuff on the screen

	// Show the rendered scene on the screen
	this->pD3D_->EndScene();

	return true;
}


bool GraphicsClass::ResizeBuffers()
{
	IDXGISwapChain* pSwapChain = pD3D_->GetSwapChain();
	int width = SETTINGS::GetSettings()->WINDOW_WIDTH;
	int height = SETTINGS::GetSettings()->WINDOW_HEIGHT;
	//HRESULT hr = pSwapChain->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
	/*
	if (FAILED(hr))
	{
		Log::Error(THIS_FUNC, "can't resize the swap chain's buffers");
		return false;
	}
	*/

	return true;
}




// memory allocation and releasing
void* GraphicsClass::operator new(size_t i)
{
	void* ptr = _aligned_malloc(i, 16);

	if (!ptr)
	{
		Log::Get()->Error(THIS_FUNC, "can't allocate memory for this object");
		return nullptr;
	}

	return ptr;
}


void GraphicsClass::operator delete(void* ptr)
{
	_aligned_free(ptr);
}











// ----------------------------------------------------------------------------------- //
// 
//                             PRIVATE METHODS 
//
// ----------------------------------------------------------------------------------- //


// renders all the stuff on the engine screen
bool GraphicsClass::RenderScene(SystemState* systemState)
{
	if (!RenderModels(systemState->renderCount))
		return false;

	if (!RenderGUI(systemState))
		return false;

	return true;
}


// prepares and renders all the models on the scene
bool GraphicsClass::RenderModels(int& renderCount)
{
	DirectX::XMFLOAT3 modelPosition;   // contains a position for particular model
	DirectX::XMVECTOR modelColor;           // contains a colour of a model
	//DirectX::XMMATRIX modelWorld;      // write here some model's world matrix
	//DirectX::XMMATRIX WVP;             // result of world * view * projection

	bool result = false;
	int modelCount = 0;                // the number of models that will be rendered
	bool renderModel = false;          // a flag which defines if we render a model or not
	float radius = 0.0f;               // a default radius of the model
	renderCount = 0;                   // set to zero as we haven't rendered models yet

	ID3D11DeviceContext* pDevCon = pD3D_->GetDeviceContext(); // a temporal pointer for easier using

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
		static TextureClass* textureCat = nullptr;
		if (textureCat == nullptr)
		{
			textureCat = new TextureClass();
			if (!textureCat->Initialize(pD3D_->GetDevice(), L"data/textures/cat.dds"))
			{
				Log::Error(THIS_FUNC, "can't init the cat texture");
				return false;
			}
		}


		static TextureClass* textureGigachad = nullptr;
		if (textureGigachad == nullptr)
		{
			textureGigachad = new TextureClass();
			if (!textureGigachad->Initialize(pD3D_->GetDevice(), L"data/textures/gigachad.dds"))
			{
				Log::Error(THIS_FUNC, "can't init the gigachad texture");
				return false;
			}
		}
	

		// rendering process
		this->pD3D_->TurnOnAlphaBlending();

		pCatSquare_->Render(pDevCon);



		// cat
		pCatSquare_->SetScale(1.0f, 1.0f, 1.0f);
		pCatSquare_->SetPosition(0.0f, 5.0f, 1.0f);
		result = pTextureShader_->Render(pDevCon, pCatSquare_->GetIndexCount(), pCatSquare_->GetWorldMatrix(), viewMatrix_, projectionMatrix_, textureCat->GetTexture(), 1.5f);
		if (!result)
		{
			Log::Error(THIS_FUNC, "can't render the cat square");
			return false;
		}


		// bateman
		pCatSquare_->SetPosition(0.0f, 5.0f, 0.0f);
		pCatSquare_->SetScale(2.0f, 2.0f, 2.0f);

		result = pTextureShader_->Render(pDevCon, pCatSquare_->GetIndexCount(), pCatSquare_->GetWorldMatrix(), viewMatrix_, projectionMatrix_, pCatSquare_->GetTexture(), 1.0f);
		if (!result)
		{
			Log::Error(THIS_FUNC, "can't render the bateman square");
			return false;
		}


		// gigachad
		pCatSquare_->SetScale(0.2f, 0.2f, 0.2f);
		pCatSquare_->SetPosition(0.0f, 5.0f, -1.0f);
		result = pTextureShader_->Render(pDevCon, pCatSquare_->GetIndexCount(), pCatSquare_->GetWorldMatrix(), viewMatrix_, projectionMatrix_, textureGigachad->GetTexture(), 0.5f);
		if (!result)
		{
			Log::Error(THIS_FUNC, "can't render the gigachad square");
			return false;
		}


		this->pD3D_->TurnOffAlphaBlending();
		//_SHUTDOWN(textureGigachad);
	}


	// render the yellow square
	if (true)
	{
		pYellowSquare_->Render(pDevCon);
		//pYellowSquare_->SetPosition(0.0f, 0.0f, 0.0f);


		result = pColorShader_->Render(pDevCon, pYellowSquare_->GetIndexCount(), pYellowSquare_->GetWorldMatrix(), viewMatrix_, projectionMatrix_);
		if (!result)
		{
			Log::Error(THIS_FUNC, "can't render the red triangle using the colour shader");
			return false;
		}
	}




	// render the red triangle
	if (true)
	{
		pTriangleRed_->Render(pDevCon);
		//pTriangleRed_->SetPosition(0.0f, 0.0f, 0.0f);


		result = pColorShader_->Render(pDevCon, pTriangleRed_->GetIndexCount(), pTriangleRed_->GetWorldMatrix(), viewMatrix_, projectionMatrix_);
		if (!result)
		{
			Log::Error(THIS_FUNC, "can't render the red triangle using the colour shader");
			return false;
		}
	}

	// render the green triangle
	if (true)
	{
		pTriangleGreen_->Render(pDevCon);
		//pTriangleGreen_->SetPosition(0.0f, 0.0f, 0.0f);
		pTriangleGreen_->SetScale(0.3f, 0.3f, 0.3f);


		result = pColorShader_->Render(pDevCon, pTriangleGreen_->GetIndexCount(), pTriangleGreen_->GetWorldMatrix(), viewMatrix_, projectionMatrix_);
		if (!result)
		{
			Log::Error(THIS_FUNC, "can't render the green triangle using the colour shader");
			return false;
		}
	}









	// construct the frustum
	pFrustum_->ConstructFrustum(settingsList->FAR_Z, projectionMatrix_, viewMatrix_);


	// get the number of models that will be rendered
	modelCount = pModelList_->GetModelCount();

	if (true)
	{
		// go through all the models and render only if they can be seen by the camera view
		for (size_t index = 0; index < modelCount; index++)
		{
			// get the position and colour of the sphere model at this index
			pModelList_->GetData(static_cast<int>(index), modelPosition, modelColor);

			// set the radius of the sphere to 1.0 since this is already known
			radius = 1.0f;

			// check if the sphere model is in the view frustum
			renderModel = pFrustum_->CheckSphere(modelPosition.x, modelPosition.y, modelPosition.z, radius);

			// if it can be seen then render it, if not skip this model and check the next sphere
			if (renderModel)
			{
				// put the model vertex and index buffers on the graphics pipeline 
				// to prepare them for drawing
				pModel_->Render(pDevCon);
				pModel_->SetPosition(modelPosition.x, modelPosition.y, modelPosition.z);   // move the model to the location it should be rendered at
				//pModel_->SetScale(2.0f, 1.0f, 1.0f);
				pModel_->SetRotation(t, 0.0f);
				
				// render the model using the light shader
				result = pLightShader_->Render(pDevCon,
					pModel_->GetIndexCount(),
					pModel_->GetWorldMatrix(), viewMatrix_, projectionMatrix_,
					pModel_->GetTexture(),
					//m_Light->GetDiffuseColor(),
					{
						DirectX::XMVectorGetX(modelColor),
						DirectX::XMVectorGetY(modelColor),
						DirectX::XMVectorGetZ(modelColor),
						1.0f
					},

					pLight_->GetDirection(),
					pLight_->GetAmbientColor(),

					editorCamera_.GetPositionFloat3(),
					pLight_->GetSpecularColor(),
					pLight_->GetSpecularPower());

				if (!result)
				{
					Log::Debug(THIS_FUNC, "can't render the model using the colour shader");
					return false;
				}

				// since this model was rendered then increase the count for this frame
				renderCount++;
			} // if
		} // for

	}
	

	return true;
} // RenderModels()

// renders the engine/game GUI
bool GraphicsClass::RenderGUI(SystemState* systemState)
{
	bool result = false;


	// ATTENTION: do 2D rendering only when all 3D rendering is finished
	// turn off the Z buffer to begin all 2D rendering
	pD3D_->TurnZBufferOff();

	// turn on the alpha blending before rendering the text
	pD3D_->TurnOnAlphaBlending();


	if (!RenderGUIDebugText(systemState))
	{
		Log::Error(THIS_FUNC, "can't render debut text");
		return false;
	}



	// turn off alpha blending after rendering the text
	pD3D_->TurnOffAlphaBlending();

	// turn the Z buffer on now that all 2D rendering has completed
	pD3D_->TurnZBufferOn();



	return true;
}


// render the debug data onto the screen in the upper-left corner
bool GraphicsClass::RenderGUIDebugText(SystemState* systemState)
{
	bool result = false;
	DirectX::XMFLOAT2 mousePos { 0.0f, 0.0f };  // pInput->GetMousePos()
	int cpu = 0;
	

	// set up the debug text data
	result = pDebugText_->SetDebugParams(mousePos,
		SETTINGS::GetSettings()->WINDOW_WIDTH, 
		SETTINGS::GetSettings()->WINDOW_HEIGHT,
		systemState->fps, systemState->cpu,
		systemState->editorCameraPosition, 
		systemState->editorCameraRotation,
		systemState->renderCount);


	// render the debug text onto the screen
	result = pDebugText_->Render(pD3D_->GetDeviceContext(), worldMatrix_, orthoMatrix_);
	if (!result)
	{
		Log::Error(THIS_FUNC, "can't render the debug info onto the screen");
		return false;
	}

	return true;
} // RenderGUIDebugText()


/*
  // the method for initialization all the 2D stuff (text, 2D-background, 2D-characters, etc.)
bool GraphicsClass::Initialize2D(HWND hwnd, DirectX::XMMATRIX baseViewMatrix)
{
	bool result = false;

	Log::Get()->Debug(THIS_FUNC_EMPTY);

	// ------------------------------ BACKGROUND --------------------------------- //
	// Create the bitmap with the background
	m_Bitmap = new BitmapClass();
	if (!m_Bitmap)
	{
		Log::Get()->Error(THIS_FUNC, "can't create the BitmapClass object");
		return false;
	}

	// Initialize the bitmap object
	result = m_Bitmap->Initialize(m_D3D->GetDevice(), m_screenWidth, m_screenHeight,
		L"grass.dds", m_screenWidth, m_screenHeight);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't initialize the bitmapClass object");
		return false;
	}

	// ------------------------------ CHARACTER 2D -------------------------------- //
	// Create the Character2D object
	m_Character2D = new(std::nothrow) Character2D();
	if (!m_Character2D)
	{
		Log::Get()->Error(THIS_FUNC, "can't create the Character2D object");
		return false;
	}

	// Initialize the Character2D object
	result = m_Character2D->Initialize(m_D3D->GetDevice(), m_screenWidth, m_screenHeight,
		L"pot.dds", 100, 100);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't initialize the Character2D object");
		return false;
	}

	m_Character2D->SetCharacterPos(100, 100);

} // Initialize2D()




   // --------------------------------------------------------------------------- //
   //                         2D RENDERING METHOD                                 //
   // --------------------------------------------------------------------------- //

   // prepares and renders all the 2D-stuff onto the screen
bool GraphicsClass::Render2D(InputClass* pInput,
	int fps, int cpu, int renderCount)
{
	bool result = false;



	/*
	result = m_Bitmap->Render(m_D3D->GetDeviceContext(), 0, 0, 0.0f, 0.0f, 1.0f, 1.0f, 3);
	if (!result)
	{
	Log::Get()->Error(THIS_FUNC, "can't render the BitmapClass object");
	return false;
	}

	// render the bitmap with the textureGigachad shader
	result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_Bitmap->GetIndexCount(),
	m_worldMatrix, m_viewMatrix, m_orthoMatrix, m_Bitmap->GetTexture());
	if (!result)
	{
	Log::Get()->Error(THIS_FUNC, "can't render the bitmap using textureGigachad shader");
	return false;
	}


	// put the bitmap vertex and index buffers on the graphics pipeline to prepare them for drawing
	result = m_Character2D->Render(m_D3D->GetDeviceContext());
	if (!result)
	{
	Log::Get()->Error(THIS_FUNC, "can't render the 2D model");
	return false;
	}

	// render the character2D with the textureGigachad shader
	result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_Character2D->GetIndexCount(),
	m_worldMatrix, m_viewMatrix, m_orthoMatrix,
	m_Character2D->GetTexture());
	if (!result)
	{
	Log::Get()->Error(THIS_FUNC, "can't render the 2D model using textureGigachad shader");
	return false;
	}
}  // Render2D()






*/