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

	
	//_SHUTDOWN(pModelCharacter2D_);
	_SHUTDOWN(pBitmap_);

	_DELETE(pLight_);

	// release shaders
	if (!shadersMap_.empty())
	{
		for (auto& elem : shadersMap_)  // delete each shader object from the memory
		{
			_DELETE(elem.second); 
		}

		shadersMap_.clear();
	}
	
	/*
	_DELETE(pTextureShader_);
	_DELETE(pLightShader_);
	_DELETE(pMultiTextureShader_);
	_DELETE(pLightMapShader_);
	_DELETE(pAlphaMapShader_);
	_DELETE(pBumpMapShader_);
	*/

	_SHUTDOWN(pModel_);
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

	RenderScene(systemState);  // render all the stuff on the screen

	// Show the rendered scene on the screen
	this->pD3D_->EndScene();

	return true;
}


// handle events from the keyboard and mouse
void GraphicsClass::HandleMovementInput(const KeyboardEvent& kbe, const MouseEvent& me, float deltaTime)
{
	this->pZone_->HandleMovementInput(kbe, me, deltaTime);
}


// adds a new shader into the shader map
void GraphicsClass::AddShader(std::string shaderName, ShaderClass* pShader)
{
	// check if there is already the shader with such a name
	for (const auto& elem : shadersMap_)
	{
		if (elem.first == shaderName) // if we already have the same shader name
		{
			COM_ERROR_IF_FALSE(false, "there is already the shader with such a name");
		}
	}

	shadersMap_.insert({ shaderName, pShader });
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
