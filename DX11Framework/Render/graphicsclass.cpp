////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
// Revising: 14.10.22
////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"

#include <random>

// the class constructor
GraphicsClass::GraphicsClass() 
{
	Log::Debug(LOG_MACRO);

	try
	{
		// get a refference to the settings container
		Settings & settings = engineSettings_;

	
		// get a pointer to the engine settings class
		pFrustum_ = new FrustumClass();
		pRenderToTexture_ = new RenderToTextureClass();
		pIntersectionWithGameObjects_ = new IntersectionWithGameObjects();             // execution of picking of some model
	}
	catch (std::bad_alloc & e)
	{
		this->Shutdown();
		Log::Error(LOG_MACRO, e.what());
		COM_ERROR_IF_FALSE(false, "can't allocate memory for the member of the GraphicsClass");
	}
}

// the class copy constructor
GraphicsClass::GraphicsClass(const GraphicsClass& copy) 
{
}

// the class destructor
GraphicsClass::~GraphicsClass() 
{
	Log::Debug(LOG_MACRO);
	this->Shutdown();
}




// ----------------------------------------------------------------------------------- //
//
//                             PUBLIC METHODS
//
// ----------------------------------------------------------------------------------- //


// Initializes all the main parts of graphics rendering module
bool GraphicsClass::Initialize(HWND hwnd, const SystemState & systemState)
{
	try
	{
		bool result = false;

		// --------------------------------------------------------------------------- //
		//              INITIALIZE ALL THE PARTS OF GRAPHICS SYSTEM                    //
		// --------------------------------------------------------------------------- //

		Log::Print("------------------------------------------------------------");
		Log::Print("              INITIALIZATION: GRAPHICS SYSTEM               ");
		Log::Print("------------------------------------------------------------");



		Settings & settings = engineSettings_;

		// prepare some common params for graphics initialization
		const bool vsyncEnabled = settings.GetSettingBoolByKey("VSYNC_ENABLED");
		const bool isFullScreenMode = settings.GetSettingBoolByKey("FULL_SCREEN");
		const bool enable4xMSAA = settings.GetSettingBoolByKey("ENABLE_4X_MSAA");
		const UINT windowWidth = settings.GetSettingIntByKey("WINDOW_WIDTH");
		const UINT windowHeight = settings.GetSettingIntByKey("WINDOW_HEIGHT");

		const float screenNear = settings.GetSettingFloatByKey("NEAR_Z");
		const float screenDepth = settings.GetSettingFloatByKey("FAR_Z");       // how far we can see
		const float fovDegrees = settings.GetSettingFloatByKey("FOV_DEGREES");  // field of view in degrees

		// get default configurations for cameras
		const float cameraSpeed = settings.GetSettingFloatByKey("CAMERA_SPEED");;
		const float cameraSensitivity = settings.GetSettingFloatByKey("CAMERA_SENSITIVITY");



		InitializeGraphics initGraphics(this);

		result = initGraphics.InitializeDirectX(hwnd,
			windowWidth,
			windowHeight,
			screenNear,
			screenDepth,
			vsyncEnabled,
			isFullScreenMode,
			enable4xMSAA);
		COM_ERROR_IF_FALSE(result, "can't initialize D3DClass");

		// after initialization of the DirectX we can use pointers to the device and device context
		ID3D11Device* pDevice = nullptr;
		ID3D11DeviceContext* pDeviceContext = nullptr;
		this->d3d_.GetDeviceAndDeviceContext(pDevice, pDeviceContext);


		// initialize all the shader classes
		result = initGraphics.InitializeShaders(pDevice, pDeviceContext, shaders_);
		COM_ERROR_IF_FALSE(result, "can't initialize shaders");

		// initialize models: cubes, spheres, trees, etc.
		result = initGraphics.InitializeScene(pDevice,
			pDeviceContext,
			hwnd,
			models_,
			settings,
			windowWidth,
			windowHeight,
			screenNear,
			screenDepth,
			fovDegrees,
			cameraSpeed,
			cameraSensitivity);
		COM_ERROR_IF_FALSE(result, "can't initialize the scene elements (models, etc.)");


		// initialize the GUI of the game/engine (interface elements, text, etc.)
		result = initGraphics.InitializeGUI(d3d_,
			settings,
			pDevice,
			pDeviceContext,
			windowWidth,
			windowHeight);
		COM_ERROR_IF_FALSE(result, "can't initialize the GUI");


		// initialize terrain and sky elements; 
		// (ATTENTION: initialize the terrain zone only after the shader & models initialization)
		result = initGraphics.InitializeTerrainZone(settings, screenDepth);
		COM_ERROR_IF_FALSE(result, "can't initialize the scene elements (models, etc.)");

		// initialize 2D sprites
		//result = pInitGraphics_->InitializeSprites();
		//COM_ERROR_IF_FALSE(result, "can't create and initialize 2D sprites");


		// set the value of main_world and ortho matrices;
		// as they aren't supposed to change we do it only once and only here;
		d3d_.GetWorldMatrix(worldMatrix_);
		d3d_.GetOrthoMatrix(orthoMatrix_);

		// compute the WVO matrix which will be used for 2D rendering (UI, etc.)
		this->WVO_ = worldMatrix_ * baseViewMatrix_ * orthoMatrix_;

		// after all the initialization create an instance of RenderGraphics class which will
		// be used for rendering onto the screen
		renderGraphics_.Initialize(pDevice, pDeviceContext, settings);
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		return false;
	}
	

	Log::Print(LOG_MACRO, " is successfully initialized");
	return true;
}

//////////////////////////////////////////////////

// Shutdowns all the graphics rendering parts, releases the memory
void GraphicsClass::Shutdown()
{
	Log::Debug(LOG_MACRO);
	
	_DELETE(pFrustum_);

	_DELETE(pZone_);
	_DELETE(pRenderToTexture_);


	d3d_.Shutdown();

	return;
} // Shutdown()

//////////////////////////////////////////////////

void GraphicsClass::RenderFrame(SystemState & systemState,
	const float deltaTime,
	const float totalGameTime)
{
	//
	// Executes rendering of each frame
	//


	// render all the stuff on the screen
	try
	{

		// Clear all the buffers before frame rendering
		this->d3d_.BeginScene();

		EditorCamera & editorCamera = GetEditorCamera();

		// update world/view/proj/ortho matrices
		editorCamera.UpdateViewMatrix();             // rebuild the view matrix for this frame
		
		const DirectX::XMMATRIX viewMatrix = editorCamera.GetViewMatrix();  // update the view matrix for this frame
		const DirectX::XMMATRIX projectionMatrix = editorCamera.GetProjectionMatrix(); // update the projection matrix
		const DirectX::XMMATRIX viewProj = viewMatrix * projectionMatrix;

		DirectX::XMFLOAT3 cameraPos;
		editorCamera.GetPositionFloat3(cameraPos);

		systemState.editorCameraPosition = editorCamera.GetPosition();
		systemState.editorCameraRotation = editorCamera.GetRotation();

		ID3D11Device* pDevice = nullptr;
		ID3D11DeviceContext* pDeviceContext = nullptr;

		d3d_.GetDeviceAndDeviceContext(pDevice, pDeviceContext);

		renderGraphics_.Render(
			shaders_.colorShader_,
			shaders_.textureShader_,
			shaders_.lightShader_,
			shaders_.pointLightShader_,

		
			pDevice,
			pDeviceContext,
			d3d_,
			systemState,
			models_,
			lightsStore_,
			userInterface_,
			WVO_,               // main_world * basic_view_matrix * ortho_matrix
			viewProj,           // view_matrix * projection_matrix
			cameraPos,
			deltaTime,
			totalGameTime);

		// Show the rendered scene on the screen
		this->d3d_.EndScene();
	}
	catch (COMException& exception)
	{
		Log::Error(exception);
		COM_ERROR_IF_FALSE(false, "can't render scene");
	}

	

	return;
}

//////////////////////////////////////////////////

void GraphicsClass::HandleKeyboardInput(const KeyboardEvent& kbe, const float deltaTime)
{
	// handle input from the keyboard to modify some rendering params


	static bool keyN_WasActive = false;
	static bool keyF_WasActive = false;
	static bool keyF2_WasActive = false;

	///////////////////////////////////////////////////////
	//  HANDLE PRESSING OF SOME KEYS
	///////////////////////////////////////////////////////
	if (kbe.IsPress())
	{
		UCHAR keyCode = kbe.GetKeyCode();

		// if F2 we change the rendering fill mode
		if (keyCode == VK_F2 && !keyF2_WasActive)
		{
			keyF2_WasActive = true;
			ChangeModelFillMode();
			Log::Debug(LOG_MACRO, "F2 key is pressed");
		}

		// if we press N we enable/disable using normals (vectors) values as color of pixel
		if (keyCode == KEY_N && !keyN_WasActive)
		{
			keyN_WasActive = true;

			Log::Debug(LOG_MACRO, "N key is pressed");
			return;
		}

		if (keyCode == KEY_F && !keyF_WasActive)
		{
			keyF_WasActive = true;

			Log::Debug(LOG_MACRO, "F key is pressed");
			return;
		}
	}


	///////////////////////////////////////////////////////
	//  HANDLE RELEASING OF SOME KEYS
	///////////////////////////////////////////////////////
	if (kbe.IsRelease())
	{
		UCHAR keyCode = kbe.GetKeyCode();


		switch (keyCode)
		{
			case VK_F2:
			{
				keyF2_WasActive = false;
				break;
			}
			case KEY_N:
			{
				keyN_WasActive = false;
				break;
			}
			case KEY_F:
			{
				keyF_WasActive = false;
				break;
			}
		}
	}
	
	
	
	///////////////////////////////////////////////////////

	// handle other possible inputs from the keyboard and update the zone according to it
	this->pZone_->HandleMovementInput(kbe, deltaTime);

	return;

} // end HandleKeyboardInput

//////////////////////////////////////////////////

void GraphicsClass::HandleMouseInput(const MouseEvent& me, 
	const MouseEvent::EventType eventType,
	const POINT & windowDimensions,
	const float deltaTime)
{
	// this function handles the input events from the mouse

	// handle mouse movement events
	if (eventType == MouseEvent::EventType::Move ||
		eventType == MouseEvent::EventType::RAW_MOVE)
	{
		// get the delta values of x and y
		const MousePoint mPoint = me.GetPos();

		// if we are having any mouse movement
		if ((mPoint.x != 0) || (mPoint.y != 0))
		{
			// update the camera rotation
			this->pZone_->HandleMovementInput(me, mPoint.x, mPoint.y, deltaTime);
		}
	}
		


	// in the input handling that is called each frame we now check to see if the user
	// has pressed or released the LMB. If the LMB was pressed then we perform the 
	// intersection check with particular shape (the sphere, the rectangle, etc.) using
	// the current 2D mouse coordinates

	// check if the left mouse button has been pressed
	if (eventType == MouseEvent::EventType::LPress)
	{
		/*
		
		// if the used has clicked on the screen with the mouse then perform an intersection test
		if (isBeginCheck_ == false)
		{
			isBeginCheck_ = true;
			MousePoint mPoint = me.GetPos();

			Model* pIntersectedModel = nullptr;

			// execute an intersection test and define if we clicked on some model 
			// if so we have a pointer to this model as the result of the function;
			pIntersectedModel = pIntersectionWithModels_->TestIntersectionWithModel(
				mPoint.x,
				mPoint.y, 
				windowDimensions,
				pModelList_->GetGameObjectsRenderingList(),
				editorCamera_,
				pD3D_->GetWorldMatrix());



			// render the picked model on the bottom right plane
			renderGraphics_->SetCurrentlyPickedModel(pIntersectedModel);

			

			pInitGraphics_->CreateLine3D(editorCamera_->GetPositionFloat3(),
				pIntersectionWithModels_->GetIntersectionPoint());



			// we remove the picked model from the scene
			if (pIntersectedModel != nullptr)
			{
			
				//pModelList_->RemoveFromRenderingListModelByID(pIntersectedModel->GetModelDataObj()->GetID());
			} // if
		} // if 
		*/
	} // if

	// check if the left mouse button has been released
	if (eventType == MouseEvent::EventType::LRelease)
	{
	}

	return;
} // end HandleMouseInput

///////////////////////////////////////////////////////////

// toggling on and toggling off the wireframe fill mode for the models
void GraphicsClass::ChangeModelFillMode()
{
	wireframeMode_ = !wireframeMode_;


	// turn on wire frame rendering of models if needed
	if (!wireframeMode_)
	{
		d3d_.SetRenderState(D3DClass::RASTER_PARAMS::FILL_MODE_SOLID);
	}
	else // turn off wire frame rendering of the terrain if it was on
	{
		d3d_.SetRenderState(D3DClass::RASTER_PARAMS::FILL_MODE_WIREFRAME);
	}
};

///////////////////////////////////////////////////////////

// returns a pointer to the D3DClass instance
D3DClass & GraphicsClass::GetD3DClass() { return d3d_; }

// returns a pointer to the camera object
EditorCamera & GraphicsClass::GetEditorCamera() { return editorCamera_; }
CameraClass & GraphicsClass::GetCameraForRenderToTexture() { return cameraForRenderToTexture_; }

// get a refference to the storage of all the light sources
const LightStore & GraphicsClass::GetLightStore() { return lightsStore_; }


///////////////////////////////////////////////////////////

// matrices getters
const DirectX::XMMATRIX & GraphicsClass::GetWorldMatrix()      const { return worldMatrix_; }
const DirectX::XMMATRIX & GraphicsClass::GetBaseViewMatrix()   const { return baseViewMatrix_; }
const DirectX::XMMATRIX & GraphicsClass::GetOrthoMatrix()      const { return orthoMatrix_; }

///////////////////////////////////////////////////////////

// memory allocation and releasing
void* GraphicsClass::operator new(size_t i)
{
	if (void* ptr = _aligned_malloc(i, 16))
		return ptr;

	Log::Error(LOG_MACRO, "can't allocate memory for this object");
	throw std::bad_alloc{};
}

///////////////////////////////////////////////////////////

void GraphicsClass::operator delete(void* ptr)
{
	_aligned_free(ptr);
}







