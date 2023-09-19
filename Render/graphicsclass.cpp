////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
// Revising: 14.10.22
////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"


// the class constructor
GraphicsClass::GraphicsClass() 
{
	try
	{
		// get default configurations for the editor's camera
		float cameraSpeed = Settings::Get()->GetSettingFloatByKey("CAMERA_SPEED");;
		float cameraSensitivity = Settings::Get()->GetSettingFloatByKey("CAMERA_SENSITIVITY");

		// get a pointer to the engine settings class
		pEngineSettings_ = Settings::Get();

		pInitGraphics_ = new InitializeGraphics(this);
		pRenderGraphics_ = new RenderGraphics(pEngineSettings_);
		pFrustum_ = new FrustumClass();
		pUserInterface_ = new UserInterfaceClass();
		pTextureManager_ = new TextureManagerClass();
		pCamera_ = new EditorCamera(cameraSpeed, cameraSensitivity);    // create the editor camera object
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(THIS_FUNC, e.what());
		COM_ERROR_IF_FALSE(false, "can't allocate memory for the member of the GraphicsClass");
	}
}

// the class copy constructor
GraphicsClass::GraphicsClass(const GraphicsClass& copy) 
{}

// the class destructor
GraphicsClass::~GraphicsClass() 
{
	Log::Debug(THIS_FUNC_EMPTY);
	this->Shutdown();
}




// ----------------------------------------------------------------------------------- //
//
//                             PUBLIC METHODS
//
// ----------------------------------------------------------------------------------- //

// Initializes all the main parts of graphics rendering module
bool GraphicsClass::Initialize(HWND hwnd)
{
	bool result = false;


	// --------------------------------------------------------------------------- //
	//              INITIALIZE ALL THE PARTS OF GRAPHICS SYSTEM                    //
	// --------------------------------------------------------------------------- //

	Log::Debug("\n\n\n");
	Log::Print("------------- INITIALIZATION: GRAPHICS SYSTEM --------------");

	if (!pInitGraphics_->InitializeDirectX(this, hwnd))
		return false;

	if (!pInitGraphics_->InitializeShaders(this, hwnd))
		return false;

	// initialize models: cubes, spheres, trees, etc.
	if (!pInitGraphics_->InitializeScene(this, hwnd))
		return false;

	if (!pInitGraphics_->InitializeGUI(this, hwnd, this->baseViewMatrix_)) // initialize the GUI of the game/engine (interface elements, text, etc.)
		return false;

	// initialize terrain and sky elements; 
	// (ATTENTION: initialize the terrain zone only after the shader & models initialization)
	if (!pInitGraphics_->InitializeTerrainZone(this))
		return false;

	

	// initialize 2D sprites
	if (!pInitGraphics_->InitializeSprites())
		return false;


	Log::Print(THIS_FUNC, " is successfully initialized");
	return true;
}

// Shutdowns all the graphics rendering parts, releases the memory
void GraphicsClass::Shutdown()
{
	Log::Debug(THIS_FUNC_EMPTY);

	_DELETE(pUserInterface_);


	// release all the light sources
	if (!arrDiffuseLights_.empty())
	{
		for (auto & pDiffuseLightSrc : arrDiffuseLights_)
		{
			_DELETE(pDiffuseLightSrc);
		}
		arrDiffuseLights_.clear();
	}

	if (!arrPointLights_.empty())
	{
		for (auto & pPointLightSrc : arrPointLights_)
		{
			_DELETE(pPointLightSrc);
		}
		arrPointLights_.clear();
	}
	
	

	_DELETE(pTextureManager_);
	_DELETE(pFrustum_);
	_DELETE(pModelList_);


	_DELETE(pCamera_);
	_DELETE(pZone_);
	_DELETE(pRenderGraphics_);

	_DELETE(pShadersContainer_);

	_SHUTDOWN(pD3D_);
	_DELETE(pInitGraphics_);

	return;
} // Shutdown()




// Executes rendering of each frame
bool GraphicsClass::RenderFrame(SystemState* systemState, HWND hwnd, float deltaTime)
								//KeyboardEvent& kbe, 
								//MouseEvent& me,
								//float deltaTime)  // the time passed since the last frame
{
	bool result = false;

	// update the delta time (the time between frames)
	SetDeltaTime(deltaTime);

	// Clear all the buffers before frame rendering
	this->pD3D_->BeginScene();

	// update matrices
	pD3D_->GetWorldMatrix(worldMatrix_);
	projectionMatrix_ = GetCamera()->GetProjectionMatrix();
	pD3D_->GetOrthoMatrix(orthoMatrix_);

	// get the view matrix based on the camera's position
	viewMatrix_ = GetCamera()->GetViewMatrix();

	systemState->editorCameraPosition = GetCamera()->GetPositionFloat3();
	systemState->editorCameraRotation = GetCamera()->GetRotationFloat3();

	RenderScene(systemState, hwnd);  // render all the stuff on the screen

	// Show the rendered scene on the screen
	this->pD3D_->EndScene();

	return true;
}


// handle input from the keyboard to modify some rendering params
void GraphicsClass::HandleKeyboardInput(const KeyboardEvent& kbe, float deltaTime)
{
	static bool keyN_WasActive = false;

	// if we press R we delete a file with cube's data
	if (kbe.IsPress() && kbe.GetKeyCode() == KEY_R)
	{
		if (std::remove("data/models/default/cube.txt") != 0)
			Log::Error(THIS_FUNC, "can't delete the output file");
	}

	// if we press N we create a new cube
	if (kbe.IsPress() && kbe.GetKeyCode() == KEY_N && !keyN_WasActive)
	{
		keyN_WasActive = true;   
		float posX = 0.0f, posY = 0.0f, posZ = 0.0f;

		posX = (static_cast<float>(rand()) / RAND_MAX) * 20.0f;
		posY = (static_cast<float>(rand()) / RAND_MAX) * 20.0f + 5.0f;
		posZ = (static_cast<float>(rand()) / RAND_MAX) * 20.0f;


		Model* pCube = pInitGraphics_->CreateCube(pD3D_->GetDevice());

		pCube->GetModelDataObj()->SetColor(1.0f, 1.0f, 1.0f, 1.0f);
		pCube->GetModelDataObj()->SetPosition(posX, posY, posZ);
		Log::Debug(THIS_FUNC, "N key is pressed");
		return;
	}
	else if (kbe.IsRelease() && kbe.GetKeyCode() == KEY_N)
	{
		keyN_WasActive = false;
	}

	// handle other inputs from the keyboard
	this->pZone_->HandleMovementInput(kbe, deltaTime);
}

// handle input from the mouse
void GraphicsClass::HandleMouseInput(const MouseEvent& me, float deltaTime)
{
	this->pZone_->HandleMovementInput(me, deltaTime);
}


// toggling on and toggling off the wireframe fill mode for the models
void GraphicsClass::ChangeModelFillMode()
{
	wireframeMode_ = !wireframeMode_;


	// turn on wire frame rendering of models if needed
	if (!wireframeMode_)
	{
		pD3D_->SetRenderState(D3DClass::RASTER_PARAMS::FILL_MODE_SOLID);
	}
	else // turn off wire frame rendering of the terrain if it was on
	{
		pD3D_->SetRenderState(D3DClass::RASTER_PARAMS::FILL_MODE_WIREFRAME);
	}
};


// returns a pointer to the D3DClass instance
D3DClass* GraphicsClass::GetD3DClass() const { return pD3D_; }

// returns a pointer to the camera object
EditorCamera* GraphicsClass::GetCamera() const _NOEXCEPT { return pCamera_; };

// returns a pointer to the shader container instance
ShadersContainer* GraphicsClass::GetShadersContainer() const { return pShadersContainer_; }

// matrices getters
const DirectX::XMMATRIX & GraphicsClass::GetWorldMatrix()      const { return worldMatrix_; }
const DirectX::XMMATRIX & GraphicsClass::GetViewMatrix()       const { return viewMatrix_; }
const DirectX::XMMATRIX & GraphicsClass::GetBaseViewMatrix()   const { return baseViewMatrix_; }
const DirectX::XMMATRIX & GraphicsClass::GetProjectionMatrix() const { return projectionMatrix_; }
const DirectX::XMMATRIX & GraphicsClass::GetOrthoMatrix()      const { return orthoMatrix_; }



// memory allocation and releasing
void* GraphicsClass::operator new(size_t i)
{
	if (void* ptr = _aligned_malloc(i, 16))
	{
		return ptr;
	}

	Log::Get()->Error(THIS_FUNC, "can't allocate memory for this object");
	throw std::bad_alloc{};
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
bool GraphicsClass::RenderScene(SystemState* systemState, HWND hwnd)
{
	try
	{
		pRenderGraphics_->RenderModels(this, hwnd, systemState->renderCount, deltaTime_);
		pRenderGraphics_->RenderGUI(this, systemState);
	}
	catch (COMException& exception)
	{
		Log::Error(exception);
		return false;
	}

	return true;
}
