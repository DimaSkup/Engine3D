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
		pIntersectionWithGameObjects_ = new IntersectionWithGameObjects();             // execution of picking of some model
	}
	catch (std::bad_alloc & e)
	{
		this->Shutdown();
		Log::Error(LOG_MACRO, e.what());
		ASSERT_TRUE(false, "can't allocate memory for the member of the GraphicsClass");
	}
}

// the class destructor
GraphicsClass::~GraphicsClass() 
{
	Log::Debug(LOG_MACRO);
	this->Shutdown();
}




// ---------------------------------------------------------------------------------
//
//                             PUBLIC METHODS
//
// ---------------------------------------------------------------------------------

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


		

		// create an initializer object which will be used for initialization of all the graphics
		InitializeGraphics initGraphics;

		result = initGraphics.InitializeDirectX(
			d3d_,
			hwnd,
			windowWidth,
			windowHeight,
			screenNear,
			screenDepth,
			vsyncEnabled,
			isFullScreenMode,
			enable4xMSAA);
		ASSERT_TRUE(result, "can't initialize D3DClass");

		// after initialization of the DirectX we can use pointers to the device and device context
		ID3D11Device* pDevice = nullptr;
		ID3D11DeviceContext* pDeviceContext = nullptr;
		this->d3d_.GetDeviceAndDeviceContext(pDevice, pDeviceContext);

		//entityMgr_.Init();


		// initialize all the shader classes
		result = initGraphics.InitializeShaders(pDevice, pDeviceContext, shaders_);
		ASSERT_TRUE(result, "can't initialize shaders");

		///////////////////////////////////////////////////
		//  SETUP CAMERAS AND VIEW MATRICES
		///////////////////////////////////////////////////

		// initialize all the cameras on the scene
		initGraphics.InitializeCameras(
			editorCamera_,
			cameraForRenderToTexture_,
			baseViewMatrix_,           // init the base view matrix which is used for 2D rendering
			windowWidth,
			windowHeight,
			screenNear,
			screenDepth,
			fovDegrees,
			cameraSpeed,
			cameraSensitivity);

		
		textureManager_.Initialize(pDevice);

		// initialize models: cubes, spheres, trees, etc.
		result = initGraphics.InitializeScene(
			d3d_,
			entityMgr_,
			meshStorage_,
			lightsStore_,
			settings,
			editorFrustum_,
			renderToTexture_,
			pDevice,
			pDeviceContext,
			hwnd,
			screenNear,
			screenDepth);
		ASSERT_TRUE(result, "can't initialize the scene elements (models, etc.)");


		// initialize the GUI of the game/engine (interface elements, text, etc.)
		result = initGraphics.InitializeGUI(
			d3d_,
			userInterface_,
			settings,
			pDevice,
			pDeviceContext,
			windowWidth,
			windowHeight);
		ASSERT_TRUE(result, "can't initialize the GUI");

		// initialize 2D sprites
		//result = pInitGraphics_->InitializeSprites();
		//ASSERT_TRUE(result, "can't create and initialize 2D sprites");


		// set the value of main_world and ortho matrices;
		// as they aren't supposed to change we do it only once and only here;
		d3d_.GetWorldMatrix(worldMatrix_);
		d3d_.GetOrthoMatrix(orthoMatrix_);

		// compute the WVO matrix which will be used for 2D rendering (UI, etc.)
		this->WVO_ = worldMatrix_ * baseViewMatrix_ * orthoMatrix_;

		// after all the initialization create an instance of RenderGraphics class which will
		// be used for rendering onto the screen
		renderGraphics_.Initialize(pDevice, pDeviceContext, settings);

		// initialize local copies of pointers to the device and device context
		pDevice_ = pDevice;
		pDeviceContext_ = pDeviceContext;
	}
	catch (EngineException & e)
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
	
	d3d_.Shutdown();

	return;
} // Shutdown()

//////////////////////////////////////////////////

void GraphicsClass::RenderFrame(
	SystemState & systemState,
	const float deltaTime,
	const float totalGameTime)
{
	//
	// Executes rendering of each frame
	//

	try
	{
		

		// Clear all the buffers before frame rendering
		this->d3d_.BeginScene();

		// ----------------------------------------------- //

		EditorCamera & editorCamera = GetEditorCamera();

		// update world/view/proj/ortho matrices
		editorCamera.UpdateViewMatrix();             // rebuild the view matrix for this frame
		
		const DirectX::XMMATRIX viewMatrix = editorCamera.GetViewMatrix();  // update the view matrix for this frame
		const DirectX::XMMATRIX projectionMatrix = editorCamera.GetProjectionMatrix(); // update the projection matrix
		const DirectX::XMMATRIX viewProj = viewMatrix * projectionMatrix;

		const DirectX::XMVECTOR camPos = editorCamera.GetPosition();
		const DirectX::XMVECTOR camRot = editorCamera.GetRotation();

		DirectX::XMFLOAT3 cameraPos;
		DirectX::XMFLOAT3 cameraDir;

		// store camera params as XMFLOAT3
		DirectX::XMStoreFloat3(&cameraPos, camPos);
		DirectX::XMStoreFloat3(&cameraDir, DirectX::XMVector3Normalize(editorCamera.GetLookAt() - camPos));

		// update the info about the camera (it will be printed onto the screen)
		DirectX::XMStoreFloat3(&systemState.editorCameraPos, camPos);
		DirectX::XMStoreFloat3(&systemState.editorCameraDir, DirectX::XMVector3Normalize(editorCamera.GetLookAt() - camPos));

		// ----------------------------------------------- //
		
		// build frustum for this frame
		editorFrustum_.ConstructFrustum(projectionMatrix, viewMatrix);

		// update the scene for this frame
		renderGraphics_.Update(
			pDeviceContext_,
			entityMgr_,
			shaders_,
			lightsStore_, 
			systemState,
			userInterface_,
			cameraPos,
			cameraDir,
			deltaTime,
			totalGameTime);

		// render the scene onto the screen
		renderGraphics_.Render(
			pDevice_,
			pDeviceContext_,
			entityMgr_,
			meshStorage_,
			shaders_,
			systemState,
			d3d_,
			lightsStore_,
			userInterface_,
			editorFrustum_,

			WVO_,               // main_world * basic_view_matrix * ortho_matrix
			viewProj,           // view_matrix * projection_matrix
			cameraPos,
			cameraDir,
			deltaTime,
			totalGameTime,
			editorCamera.GetCameraDepth());
	
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		// Show the rendered scene on the screen
		this->d3d_.EndScene();
	}
	catch (EngineException & e)
	{
		Log::Error(e, true);
		ASSERT_TRUE(false, "can't render scene");
	}
	

	return;
}

//////////////////////////////////////////////////

void GraphicsClass::HandleKeyboardInput(const KeyboardEvent& kbe, const float deltaTime)
{
	// handle input from the keyboard to modify some rendering params

	static bool keyN_WasActive = false;
	static bool keyF_WasActive = false;
	static bool keyH_WasActive = false;
	static bool keyF2_WasActive = false;
	static bool keyF3_WasActive = false;


	// Switch the number of directional lights
	if (GetAsyncKeyState('0') & 0x8000)
		shaders_.lightShader_.SetNumberOfDirectionalLights_ForRendering(pDeviceContext_, 0);
	else if (GetAsyncKeyState('1') & 0x8000)
		shaders_.lightShader_.SetNumberOfDirectionalLights_ForRendering(pDeviceContext_, 1);
	else if (GetAsyncKeyState('2') & 0x8000)
		shaders_.lightShader_.SetNumberOfDirectionalLights_ForRendering(pDeviceContext_, 2);
	else if (GetAsyncKeyState('3') & 0x8000)
		shaders_.lightShader_.SetNumberOfDirectionalLights_ForRendering(pDeviceContext_, 3);

	
	// handle pressing of some keys
	if (kbe.IsPress())
	{
		UCHAR keyCode = kbe.GetKeyCode();

		// if F2 we change the rendering fill mode
		if (keyCode == VK_F2 && !keyF2_WasActive)
		{
			keyF2_WasActive = true;
			ChangeModelFillMode();
			Log::Debug(LOG_MACRO, "F2 key is pressed");
			return;
		}

		if (keyCode == VK_F3 && !keyF3_WasActive)
		{
			keyF3_WasActive = true;
			ChangeCullMode();
			Log::Debug(LOG_MACRO, "F3 key is pressed");
			return;
		}

		if (keyCode == KEY_G)
		{
			lightsStore_.spotLightsStore_.spotLightsArr_[0].spot += 1.0f;
		}

		if (keyCode == KEY_T)
		{
			lightsStore_.spotLightsStore_.spotLightsArr_[0].spot -= 1.0f;
		}

		// when press N we turn on/off the normals debugging
		if (keyCode == KEY_N && !keyN_WasActive)
		{
			keyN_WasActive = true;
			shaders_.lightShader_.EnableDisableDebugNormals(pDeviceContext_);
			Log::Debug(LOG_MACRO, "key N is pressed");
			return;
		}

		// when press F we turn on/off flashlight
		if (keyCode == KEY_F && !keyF_WasActive)
		{
			keyF_WasActive = true;
			shaders_.lightShader_.ChangeFlashLightState(pDeviceContext_);
			Log::Debug(LOG_MACRO, "key F is pressed");
			return;
		}

		// when press H we turn on/off the fog effect
		if (keyCode == KEY_H && !keyH_WasActive)
		{
			keyH_WasActive = true;
			shaders_.lightShader_.EnableDisableFogEffect(pDeviceContext_);
			Log::Debug(LOG_MACRO, "key H is pressed");
			return;
		}
	}


	// handle releasing of some keys
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
			case VK_F3:
			{
				keyF3_WasActive = false;
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
			case KEY_H:
			{
				keyH_WasActive = false;
				break;
			}
		}
	}

	// handle other possible inputs from the keyboard and update the zone according to it
	zone_.HandleMovementInput(editorCamera_, kbe, deltaTime);

	return;

}

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
			zone_.HandleMovementInput(editorCamera_, me, mPoint.x, mPoint.y, deltaTime);
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
			} 
		}
		*/
	}

	// check if the left mouse button has been released
	if (eventType == MouseEvent::EventType::LRelease)
	{
	}

	return;
}

///////////////////////////////////////////////////////////

void GraphicsClass::ChangeModelFillMode()
{
	// toggling on and toggling off the fill mode for the models

	isWireframeMode_ = !isWireframeMode_;


	if (!isWireframeMode_)
	{
		d3d_.SetRenderState(D3DClass::RASTER_PARAMS::FILL_MODE_SOLID);
	}
	else 
	{
		d3d_.SetRenderState(D3DClass::RASTER_PARAMS::FILL_MODE_WIREFRAME);
	}
};

void GraphicsClass::ChangeCullMode()
{
	// toggling on and toggling off the cull mode for the models

	isCullBackMode_ = !isCullBackMode_;

	if (isCullBackMode_)
	{
		d3d_.SetRenderState(D3DClass::RASTER_PARAMS::CULL_MODE_BACK);
	}
	else
	{
		d3d_.SetRenderState(D3DClass::RASTER_PARAMS::CULL_MODE_FRONT);
	}
}

///////////////////////////////////////////////////////////

// returns a pointer to the D3DClass instance
D3DClass & GraphicsClass::GetD3DClass() { return d3d_; }

// returns a pointer to the camera object
EditorCamera & GraphicsClass::GetEditorCamera() { return editorCamera_; }
CameraClass & GraphicsClass::GetCameraForRenderToTexture() { return cameraForRenderToTexture_; }

UserInterfaceClass & GraphicsClass::GetUserInterface() { return userInterface_; }

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






