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

	// --------------------------------------------------------------------------- //
	//              INITIALIZE ALL THE PARTS OF GRAPHICS SYSTEM                    //
	// --------------------------------------------------------------------------- //

	settingsList = SETTINGS::GetSettings();


	Log::Debug("\n\n\n");
	Log::Print("------------- INITIALIZATION: GRAPHICS SYSTEM --------------");

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

	
	//_SHUTDOWN(pModelCharacter2D_);
	_SHUTDOWN(pBitmap_);

	_DELETE(pLight_);

	// shaders
	_DELETE(pTextureShader_);
	_DELETE(pLightShader_);
	_DELETE(pMultiTextureShader_);
	_DELETE(pLightMapShader_);
	_DELETE(pAlphaMapShader_);
	_DELETE(pBumpMapShader_);

	_SHUTDOWN(pModel_);
	_SHUTDOWN(pD3D_);

	Log::Debug(THIS_FUNC_EMPTY);

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
		RenderModels(this, systemState->renderCount);
		RenderGUI(this, systemState);
	}
	catch (COMException& exception)
	{
		Log::Error(exception);
		return false;
	}

	return true;
}
