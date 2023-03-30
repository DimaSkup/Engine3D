////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
// Revising: 14.10.22
////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"


// the class constructor
GraphicsClass::GraphicsClass(void) {};

// the class copy constructor
GraphicsClass::GraphicsClass(const GraphicsClass& copy) {};

// the class destructor
GraphicsClass::~GraphicsClass(void) {};




// ----------------------------------------------------------------------------------- //
//
//                             PUBLIC METHODS
//
// ----------------------------------------------------------------------------------- //

// Initializes all the main parts of graphics rendering module
bool GraphicsClass::Initialize(HWND hwnd)
{
	bool result = false;
	InitializeGraphics initGraphics_;

	// --------------------------------------------------------------------------- //
	//              INITIALIZE ALL THE PARTS OF GRAPHICS SYSTEM                    //
	// --------------------------------------------------------------------------- //

	settingsList = SETTINGS::GetSettings();

	Log::Debug("\n\n\n");
	Log::Print("------------- INITIALIZATION: GRAPHICS SYSTEM --------------");


	if (!initGraphics_.InitializeDirectX(this, hwnd,
			settingsList->WINDOW_WIDTH,
			settingsList->WINDOW_HEIGHT,
			settingsList->VSYNC_ENABLED,
			settingsList->FULL_SCREEN,
			settingsList->NEAR_Z,
			settingsList->FAR_Z))
		return false;

	if (!initGraphics_.InitializeTerrainZone(this, settingsList))
		return false;

	if (!initGraphics_.InitializeShaders(this, hwnd))
		return false;

	if (!initGraphics_.InitializeScene(this, hwnd, settingsList))
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
	_SHUTDOWN(pBitmap_);
	_DELETE(pLight_);
	_DELETE(pShadersContainer_);

	_DELETE(pZone_);
	//_SHUTDOWN(pModel_);
	_SHUTDOWN(pD3D_);

	Log::Debug(THIS_FUNC_EMPTY);

	return;
} // Shutdown()




// Executes rendering of each frame
bool GraphicsClass::RenderFrame(SystemState* systemState)
								//KeyboardEvent& kbe, 
								//MouseEvent& me,
								//float deltaTime)  // the time passed since the last frame
{
	bool result = false;

	// Clear all the buffers before frame rendering
	this->pD3D_->BeginScene();

	// update matrices
	pD3D_->GetWorldMatrix(worldMatrix_);
	projectionMatrix_ = pZone_->GetCamera()->GetProjectionMatrix();
	pD3D_->GetOrthoMatrix(orthoMatrix_);

	// get the view matrix based on the camera's position
	viewMatrix_ = pZone_->GetCamera()->GetViewMatrix();

	systemState->editorCameraPosition = pZone_->GetCamera()->GetPositionFloat3();
	systemState->editorCameraRotation = pZone_->GetCamera()->GetRotationFloat3();

	// before actual rendering we need to update data for shaders
	pDataForShaders_->Update(&viewMatrix_, &projectionMatrix_, &orthoMatrix_, pLight_);

	// turn on wire frame rendering of models if needed
	if (wireframeMode_)
	{
		pD3D_->EnableWireframe();
	}
	else // turn off wire frame rendering of the terrain if it was on
	{
		pD3D_->DisableWireframe();
	}

	RenderScene(systemState);  // render all the stuff on the screen

	// Show the rendered scene on the screen
	this->pD3D_->EndScene();

	return true;
}


// handle events from the keyboard
void GraphicsClass::HandleMovementInput(const KeyboardEvent& kbe, float deltaTime)
{
	this->pZone_->HandleMovementInput(kbe, deltaTime);
}

// handle events from the mouse
void GraphicsClass::HandleMovementInput(const MouseEvent& me, float deltaTime)
{
	this->pZone_->HandleMovementInput(me, deltaTime);
}

// matrices getters
const DirectX::XMMATRIX & GraphicsClass::GetWorldMatrix() const { return worldMatrix_; }
const DirectX::XMMATRIX & GraphicsClass::GetViewMatrix() const { return viewMatrix_; }
const DirectX::XMMATRIX & GraphicsClass::GetProjectionMatrix() const { return projectionMatrix_; }
const DirectX::XMMATRIX & GraphicsClass::GetOrthoMatrix() const { return orthoMatrix_; }


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
	try
	{
		RenderGraphics renderGraphics_;

		renderGraphics_.RenderModels(this, systemState->renderCount);
		renderGraphics_.RenderGUI(this, systemState);
	}
	catch (COMException& exception)
	{
		Log::Error(exception);
		return false;
	}

	return true;
}
