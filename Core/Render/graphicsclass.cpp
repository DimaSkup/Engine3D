////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
// Revising: 14.10.22
////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"

#include "InitializeGraphics.h"        // for initialization of the graphics

// ImGui stuff
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include <random>

using namespace DirectX;

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
		InitializeGraphics initGraphics;   // graphics initializer
		Settings& settings = engineSettings_;
		bool result = false;

		// --------------------------------------------------------------------------- //
		//              INITIALIZE ALL THE PARTS OF GRAPHICS SYSTEM                    //
		// --------------------------------------------------------------------------- //

		Log::Print("------------------------------------------------------------");
		Log::Print("              INITIALIZATION: GRAPHICS SYSTEM               ");
		Log::Print("------------------------------------------------------------");



		

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


		// initialize all the shader classes
		result = initGraphics.InitializeShaders(pDevice, pDeviceContext, shadersContainer_);
		ASSERT_TRUE(result, "can't initialize shaders");


		// ------------------------------------------------
		
		// initialize the cameras and view matrices
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

		// initializer the textures container
		textureManager_.Initialize(pDevice);

		// initialize models: cubes, spheres, trees, etc.
		result = initGraphics.InitializeScene(
			d3d_,
			entityMgr_,
			meshStorage_,
			lightsStorage_,
			settings,
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

		// create frustums for frustum culling
		frustums_.push_back(BoundingFrustum());  // editor camera
		frustums_.push_back(BoundingFrustum());  // game camera

		// set the value of main_world and ortho matrices;
		// as they aren't supposed to change we do it only once and only here;
		d3d_.GetWorldMatrix(worldMatrix_);
		d3d_.GetOrthoMatrix(orthoMatrix_);

		// compute the WVO matrix which will be used for 2D rendering (UI, etc.)
		WVO_ = worldMatrix_ * baseViewMatrix_ * orthoMatrix_;

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

///////////////////////////////////////////////////////////


void GraphicsClass::Shutdown()
{
	// Shutdowns all the graphics rendering parts, releases the memory
	Log::Debug(LOG_MACRO);
	d3d_.Shutdown();
}

///////////////////////////////////////////////////////////

void GraphicsClass::UpdateScene(
	SystemState& sysState,
	const float deltaTime,
	const float totalGameTime)
{
	// update all the graphics related stuff for this frame

	EditorCamera& editorCamera = GetEditorCamera();

	// update view/proj matrices
	editorCamera.UpdateViewMatrix();             

	const DirectX::XMMATRIX viewMatrix = editorCamera.GetViewMatrix();  // update the view matrix for this frame
	const DirectX::XMMATRIX projMatrix = editorCamera.GetProjectionMatrix(); // update the projection matrix
	viewProj_ = viewMatrix * projMatrix;

	// update the cameras states
	XMStoreFloat3(&sysState.editorCameraPos, editorCamera.GetPosition());
	XMStoreFloat3(&sysState.editorCameraDir, XMVector3Normalize(editorCamera.GetLookAt() - editorCamera.GetPosition()));

	const XMFLOAT3& cameraPos = sysState.editorCameraPos;
	const XMFLOAT3& cameraDir = sysState.editorCameraDir;

	// reset render counters (do it before frustum culling)
	sysState.visibleObjectsCount = 0;
	sysState.visibleVerticesCount = 0;

	// update user interface for this frame
	userInterface_.Update(pDeviceContext_, sysState);

	// update the entities and related data
	entityMgr_.Update(totalGameTime, deltaTime);

	// build the frustum from the projection matrix in view space.
	BoundingFrustum::CreateFromMatrix(frustums_[0], projMatrix);

	// perform frustum culling on all of our entities
	ComputeFrustumCulling(sysState);

	// ----------------------------------------------------
	//  UPDATE THE LIGHT SOURCES 

	DirectionalLightsStorage& dirLights = lightsStorage_.dirLightsStorage_;
	SpotLightsStorage& spotLights = lightsStorage_.spotLightsStorage_;
	PointLightsStorage& pointLights = lightsStorage_.pointLightsStorage_;

	XMFLOAT3& pointLightPos = pointLights.data_[0].position;

	// circle light over the land surface
	pointLightPos.x = 10.0f * cosf(0.2f * totalGameTime);
	pointLightPos.z = 10.0f * sinf(0.2f * totalGameTime);
	pointLightPos.y = 10.0f;


	// the spotlight takes on the camera position and is aimed in the same direction 
	// the camera is looking. In this way, it looks like we are holding a flashlight
	SpotLight& flashlight = spotLights.data_[0];
	flashlight.position = cameraPos;
	flashlight.direction = cameraDir;

	// circle light over the land surface
	DirectX::XMFLOAT3& sun = dirLights.data_[0].direction;
	sun.x = 10.0f * cosf(0.2f * totalGameTime);
	sun.z = 10.0f * sinf(0.2f * totalGameTime);


	// ----------------------------------------------------
	// update the shaders params for this frame

	shadersContainer_.lightShader_.SetLights(
		pDeviceContext_,
		cameraPos,
		dirLights.data_,
		pointLights.data_,
		spotLights.data_);
}

///////////////////////////////////////////////////////////

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
		UpdateScene(systemState, deltaTime, totalGameTime);

		// Clear all the buffers before frame rendering
		d3d_.BeginScene();	

		// render the scene onto the screen
		renderGraphics_.Render(
			pDevice_,
			pDeviceContext_,

			entityMgr_,
			meshStorage_,
			shadersContainer_,
			systemState,
			d3d_,
			lightsStorage_,
			userInterface_,

			WVO_,               // main_world * basic_view_matrix * ortho_matrix
			viewProj_,           // view_matrix * projection_matrix
			systemState.editorCameraPos,
			systemState.editorCameraDir,
			deltaTime,
			totalGameTime);
	
		// render ImGui stuff onto the screen
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		// Show the rendered scene on the screen
		d3d_.EndScene();
	}
	catch (EngineException & e)
	{
		Log::Error(e, true);
		ASSERT_TRUE(false, "can't render scene");
	}
	

	return;
}

///////////////////////////////////////////////////////////

void GraphicsClass::ComputeFrustumCulling(
	SystemState& sysState)
{
	const bool frustumCullingEnabled = true;
	sysState.visibleObjectsCount = 0;

	const std::vector<XMMATRIX> worlds = entityMgr_.GetWorldComponent().worlds_;
	entityMgr_.renderComponent_.visibleEnttsIDs_.clear();

	if (frustumCullingEnabled)
	{
		XMVECTOR detView = XMMatrixDeterminant(editorCamera_.GetViewMatrix());
		XMMATRIX invView = XMMatrixInverse(&detView, editorCamera_.GetViewMatrix());

		// go through each mesh type from the ECS Mesh component
		for (const auto& it : entityMgr_.meshComponent_.meshToEntities_)
		{
			const MeshID meshID = it.first;
			const std::vector<EntityID>& enttsIDs = { it.second.begin(), it.second.end() };     

			std::vector<XMFLOAT3> positions;
			std::vector<XMFLOAT3> directions;
			std::vector<XMFLOAT3> scales;
			std::vector<ptrdiff_t> dataIdxs;

			entityMgr_.transformSystem_.GetTransformDataOfEntts(
				enttsIDs,
				dataIdxs,
				positions,
				directions,
				scales);

			for (ptrdiff_t idx = 0; idx < std::ssize(enttsIDs); ++idx)
			{
				// transform the camera frustum from view space to the object's local space
				DirectX::BoundingFrustum localspaceFrustum;
				frustums_[0].Transform(
					localspaceFrustum,
					scales[idx].x,
					XMLoadFloat3(&directions[idx]),
					XMLoadFloat3(&positions[idx]));

				// perform the box/frustum intersection test in local space
				//if (localspaceFrustum.Intersects()
			}
			
		}
	}

	//XMVECTOR detView = XMMatrixDeterminant()
	//frustums_[0].Contains(entityMgr_.boundingBoxes_.)
}

///////////////////////////////////////////////////////////

void GraphicsClass::HandleKeyboardInput(
	const KeyboardEvent& kbe, 
	const float deltaTime)
{
	// handle input from the keyboard to modify some rendering params


	// Switch the number of directional lights
	if (GetAsyncKeyState('0') & 0x8000)
		shadersContainer_.lightShader_.SetNumberOfDirectionalLights_ForRendering(pDeviceContext_, 0);
	else if (GetAsyncKeyState('1') & 0x8000)
		shadersContainer_.lightShader_.SetNumberOfDirectionalLights_ForRendering(pDeviceContext_, 1);
	else if (GetAsyncKeyState('2') & 0x8000)
		shadersContainer_.lightShader_.SetNumberOfDirectionalLights_ForRendering(pDeviceContext_, 2);
	else if (GetAsyncKeyState('3') & 0x8000)
		shadersContainer_.lightShader_.SetNumberOfDirectionalLights_ForRendering(pDeviceContext_, 3);

	
	static UCHAR prevKeyCode = 0;

	// handle pressing of some keys
	if (kbe.IsPress())
	{
		UCHAR keyCode = kbe.GetKeyCode();

		switch (keyCode)
		{
			case VK_F2:
			{
				// change the rendering fill mode
				if (prevKeyCode != VK_F2) 
					ChangeModelFillMode();

				Log::Debug(LOG_MACRO, "F2 key is pressed");
				break;
			}
			case VK_F3:
			{
				// change the rendering cull mode
				if (prevKeyCode != VK_F3) 
					ChangeCullMode();

				Log::Debug(LOG_MACRO, "F3 key is pressed");
				break;
			}
			case KEY_N:
			{
				// turn on/off the normals debugging
				if (prevKeyCode != KEY_N)
					shadersContainer_.lightShader_.EnableDisableDebugNormals(pDeviceContext_);

				Log::Debug(LOG_MACRO, "key N is pressed");
				break;
			}
			case KEY_T:
			{
				// turn on/off the tangents debugging
				if (prevKeyCode != KEY_T)
					shadersContainer_.lightShader_.EnableDisableDebugTangents(pDeviceContext_);

				Log::Debug(LOG_MACRO, "key T is pressed");
				break;
			}
			case KEY_B:
			{
				// turn on/off the binormals debugging
				if (prevKeyCode != KEY_B)
					shadersContainer_.lightShader_.EnableDisableDebugBinormals(pDeviceContext_);

				Log::Debug(LOG_MACRO, "key B is pressed");
				break;
			}
			case KEY_L:
			{
				// turn on/off flashlight
				if (prevKeyCode != KEY_L)
					shadersContainer_.lightShader_.ChangeFlashLightState(pDeviceContext_);

				Log::Debug(LOG_MACRO, "key L is pressed");
				break;
			}
			case KEY_Y:
			{
				// change flashlight radius
				lightsStorage_.spotLightsStorage_.data_[0].spot += 1.0f;
				break;
			}
			case KEY_U:
			{
				// change flashlight radius
				lightsStorage_.spotLightsStorage_.data_[0].spot -= 1.0f;
				break;
			}
			case KEY_H:
			{
				// turn on/off the fog effect
				if (prevKeyCode != KEY_H)
					shadersContainer_.lightShader_.EnableDisableFogEffect(pDeviceContext_);
				
				Log::Debug(LOG_MACRO, "key H is pressed");
				break;
			}
		}

		// store the values of currently pressed key for the next frame
		prevKeyCode = keyCode;
	}


	// handle releasing of some keys
	if (kbe.IsRelease())
	{
		prevKeyCode = 0;
		//UCHAR keyCode = kbe.GetKeyCode();
	}

	// handle other possible inputs from the keyboard and update the zone according to it
	zone_.HandleMovementInput(editorCamera_, kbe, deltaTime);

	return;

}

///////////////////////////////////////////////////////////

void GraphicsClass::HandleMouseInput(const MouseEvent& me, 
	const MouseEvent::EventType eventType,
	const POINT & windowDimensions,
	const float deltaTime)
{
	// this function handles the input events from the mouse

	switch (eventType)
	{
		case MouseEvent::EventType::Move | MouseEvent::EventType::RAW_MOVE:
		{
			// update the camera rotation
			zone_.HandleMovementInput(editorCamera_, me, deltaTime);

			break;
		}
		case MouseEvent::EventType::LPress:
		{
			break;
		}
	}
		


	// in the input handling that is called each frame we now check to see if the user
	// has pressed or released the LMB. If the LMB was pressed then we perform the 
	// intersection check with particular shape (the sphere, the rectangle, etc.) using
	// the current 2D mouse coordinates

	// check if the left mouse button has been pressed
	
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


	return;
}

///////////////////////////////////////////////////////////

void GraphicsClass::ChangeModelFillMode()
{
	// toggling on / toggling off the fill mode for the models

	using PARAMS = D3DClass::RASTER_PARAMS;

	isWireframeMode_ = !isWireframeMode_;
	PARAMS fillParam = (isWireframeMode_) ? PARAMS::FILL_MODE_WIREFRAME : PARAMS::FILL_MODE_SOLID;

	d3d_.SetRenderState(fillParam);
};

void GraphicsClass::ChangeCullMode()
{
	// toggling on and toggling off the cull mode for the models

	using PARAMS = D3DClass::RASTER_PARAMS;

	isCullBackMode_ = !isCullBackMode_;
	PARAMS cullParam = (isCullBackMode_) ? PARAMS::CULL_MODE_BACK : PARAMS::CULL_MODE_FRONT;

	d3d_.SetRenderState(cullParam);
}

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







