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
		// get ptr to the settings container
		Settings* pSettings = Settings::Get();

		// get default configurations for the editor's camera
		float cameraSpeed = pSettings->GetSettingFloatByKey("CAMERA_SPEED");;
		float cameraSensitivity = pSettings->GetSettingFloatByKey("CAMERA_SENSITIVITY");

		// get a pointer to the engine settings class
		pEngineSettings_ = pSettings;
		pFrustum_ = new FrustumClass();
		
		pCamera_ = new EditorCamera(cameraSpeed, cameraSensitivity);                   // create the editor camera object
		pCameraForRenderToTexture_ = new CameraClass(cameraSpeed, cameraSensitivity);  // this camera is used for rendering into textures
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
{}

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
	bool result = false;

	// --------------------------------------------------------------------------- //
	//              INITIALIZE ALL THE PARTS OF GRAPHICS SYSTEM                    //
	// --------------------------------------------------------------------------- //

	Log::Print("------------------------------------------------------------");
	Log::Print("              INITIALIZATION: GRAPHICS SYSTEM               ");
	Log::Print("------------------------------------------------------------");

	InitializeGraphics initGraphics(this);
	
	if (!initGraphics.InitializeDirectX(hwnd))
		return false;

	// after initialization of the DirectX we can use pointers to the device and device context
	ID3D11Device* pDevice = this->d3d_.GetDevice();
	ID3D11DeviceContext* pDeviceContext = this->d3d_.GetDeviceContext();

	if (!initGraphics.InitializeShaders(pDevice, pDeviceContext, hwnd))
	{
		Log::Error(LOG_MACRO, "can't initialize shaders");
		return false;
	}

	// initialize models: cubes, spheres, trees, etc.
	if (!initGraphics.InitializeScene(pDevice, pDeviceContext, hwnd))
	{
		Log::Error(LOG_MACRO, "can't initialize the scene elements (models, etc.)");
		return false;
	}

	if (!initGraphics.InitializeGUI(d3d_, pDevice, pDeviceContext)) // initialize the GUI of the game/engine (interface elements, text, etc.)
	{
		Log::Error(LOG_MACRO, "can't initialize the GUI");
		return false;
	}

	// initialize terrain and sky elements; 
	// (ATTENTION: initialize the terrain zone only after the shader & models initialization)
	if (!initGraphics.InitializeTerrainZone())
	{
		Log::Error(LOG_MACRO, "can't initialize the scene elements (models, etc.)");
		return false;
	}

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
	pRenderGraphics_ = new RenderGraphics(this, pEngineSettings_, pDevice, pDeviceContext);

	// create a cube model
	models_.CreateModel(pDevice, 100, "data/models/default/cube.obj", { 0, 0, 0 }, { 0, 0, 0 });
	models_.SetTextureByIndex(0, "data/textures/box01d.dds", aiTextureType_DIFFUSE);

	Log::Print(LOG_MACRO, " is successfully initialized");
	return true;
}

//////////////////////////////////////////////////

// Shutdowns all the graphics rendering parts, releases the memory
void GraphicsClass::Shutdown()
{
	Log::Debug(LOG_MACRO);

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
	
	_DELETE(pFrustum_);

	_DELETE(pCamera_);
	_DELETE(pZone_);
	_DELETE(pRenderGraphics_);
	

	_DELETE(pRenderToTexture_);
	_DELETE(pCameraForRenderToTexture_);

	d3d_.Shutdown();

	return;
} // Shutdown()

//////////////////////////////////////////////////

void GraphicsClass::RenderFrame(HWND hwnd, 
	SystemState & systemState,
	const float deltaTime,
	const int gameCycles)
{
	//
	// Executes rendering of each frame
	//


	// render all the stuff on the screen
	try
	{

		// Clear all the buffers before frame rendering
		this->d3d_.BeginScene();

		// update world/view/proj/ortho matrices
		CameraClass* pCamera = GetCamera();

		pCamera->UpdateViewMatrix();             // rebuild the view matrix for this frame
		viewMatrix_ = pCamera->GetViewMatrix();  // update the view matrix for this frame
		projectionMatrix_ = pCamera->GetProjectionMatrix(); // update the projection matrix
		viewProj_ = viewMatrix_ * projectionMatrix_;

		systemState.editorCameraPosition = pCamera->GetPosition();
		systemState.editorCameraRotation = pCamera->GetRotation();

		ID3D11Device* pDevice = nullptr;
		ID3D11DeviceContext* pDeviceContext = nullptr;

		d3d_.GetDeviceAndDeviceContext(pDevice, pDeviceContext);

		pRenderGraphics_->Render(d3d_,
			pDevice,
			pDeviceContext,
			WVO_,
			hwnd, 
			systemState, 
			deltaTime,
			gameCycles,
			models_);

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

	///////////////////////////////////////////////////////

	// if we press N we enable/disable using normals (vectors) values as color of pixel
	if (kbe.IsPress() && kbe.GetKeyCode() == KEY_N && !keyN_WasActive)
	{
		keyN_WasActive = true;   

		Log::Debug(LOG_MACRO, "N key is pressed");
		return;
	}
	else if (kbe.IsRelease() && kbe.GetKeyCode() == KEY_N)
	{

		keyN_WasActive = false;
	}
	
	///////////////////////////////////////////////////////

	if (kbe.IsPress() && kbe.GetKeyCode() == KEY_F && !keyF_WasActive)
	{
		keyF_WasActive = true;

		Log::Debug(LOG_MACRO, "F key is pressed");
		return;
	}
	else if (kbe.IsRelease() && kbe.GetKeyCode() == KEY_F)
	{

		keyF_WasActive = false;
	}
	

	// handle other inputs from the keyboard
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

	// handle camera rotation, etc.
	if (eventType == MouseEvent::EventType::Move ||	eventType == MouseEvent::EventType::RAW_MOVE)
		this->pZone_->HandleMovementInput(me, deltaTime);


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
				pCamera_,
				pD3D_->GetWorldMatrix());



			// render the picked model on the bottom right plane
			pRenderGraphics_->SetCurrentlyPickedModel(pIntersectedModel);

			

			pInitGraphics_->CreateLine3D(pCamera_->GetPositionFloat3(),
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
EditorCamera* GraphicsClass::GetCamera() const { return pCamera_; };

// get an array of diffuse light sources (for instance: sun)
const std::vector<LightClass*> & GraphicsClass::GetDiffuseLigthsArr() { return arrDiffuseLights_; }

// get an array of point light sources (for instance: candle, lightbulb)
const std::vector<LightClass*> & GraphicsClass::GetPointLightsArr() { return arrPointLights_; }


///////////////////////////////////////////////////////////

// matrices getters
const DirectX::XMMATRIX & GraphicsClass::GetWorldMatrix()      const { return worldMatrix_; }
const DirectX::XMMATRIX & GraphicsClass::GetViewMatrix()       const { return viewMatrix_; }
const DirectX::XMMATRIX & GraphicsClass::GetBaseViewMatrix()   const { return baseViewMatrix_; }
const DirectX::XMMATRIX & GraphicsClass::GetProjectionMatrix() const { return projectionMatrix_; }
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







