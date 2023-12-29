////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
// Revising: 14.10.22
////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"

#include <random>

// the class constructor
GraphicsClass::GraphicsClass() 
{
	Log::Debug(THIS_FUNC_EMPTY);

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
		
		pShadersContainer_ = new ShadersContainer();                // create a container for the shaders classes
		pModelsToShaderMediator_ = new ModelToShaderMediator();
		
		//pTextureManager_ = new TextureManagerClass(pSettings->GetSettingStrByKey("PATH_TO_TEXTURES_DIR"));
		pCamera_ = new EditorCamera(cameraSpeed, cameraSensitivity);        // create the editor camera object
		pCameraForRenderToTexture_ = new CameraClass();                     // this camera is used for rendering into textures
		pRenderToTexture_ = new RenderToTextureClass();
		pIntersectionWithGameObjects_ = new IntersectionWithGameObjects();  // execution of picking of some model
	}
	catch (std::bad_alloc & e)
	{
		this->Shutdown();
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

	pGameObjectsList_ = new GameObjectsListClass();
	pInitGraphics_    = new InitializeGraphics(this);
	
	if (!pInitGraphics_->InitializeDirectX(hwnd))
		return false;

	if (!pInitGraphics_->InitializeShaders(hwnd))
		return false;

	// initialize models: cubes, spheres, trees, etc.
	if (!pInitGraphics_->InitializeScene(hwnd))
		return false;

	if (!pInitGraphics_->InitializeGUI(hwnd, this->baseViewMatrix_)) // initialize the GUI of the game/engine (interface elements, text, etc.)
		return false;

	// initialize terrain and sky elements; 
	// (ATTENTION: initialize the terrain zone only after the shader & models initialization)
	if (!pInitGraphics_->InitializeTerrainZone())
		return false;

	

	// initialize 2D sprites
	result = pInitGraphics_->InitializeSprites();
	COM_ERROR_IF_FALSE(result, "can't create and initialize 2D sprites");


	// after all the initialization create an instance of RenderGraphics class which will
	// be used for rendering onto the screen
	pRenderGraphics_ = new RenderGraphics(this, 
		pEngineSettings_, 
		this->pD3D_->GetDevice(), 
		this->pD3D_->GetDeviceContext(),
		this->pGameObjectsList_->GetGameObjectsRenderingList().begin()->second->GetDataContainerForShaders());


	Log::Print(THIS_FUNC, " is successfully initialized");
	return true;
}

//////////////////////////////////////////////////

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
	_DELETE(pGameObjectsList_);


	_DELETE(pCamera_);
	_DELETE(pZone_);
	_DELETE(pRenderGraphics_);
	

	_DELETE(pRenderToTexture_);
	_DELETE(pCameraForRenderToTexture_);

	_DELETE(pModelsToShaderMediator_);
	_DELETE(pShadersContainer_);

	_SHUTDOWN(pD3D_);
	_DELETE(pInitGraphics_);

	return;
} // Shutdown()

//////////////////////////////////////////////////

bool GraphicsClass::RenderFrame(SystemState* systemState, HWND hwnd, float deltaTime)
{
	//
	// Executes rendering of each frame
	//

	// update the delta time (the time between frames)
	SetDeltaTime(deltaTime);

	// Clear all the buffers before frame rendering
	this->pD3D_->BeginScene();

	// update world/view/proj/ortho matrices
	pD3D_->GetWorldMatrix(worldMatrix_);
	viewMatrix_ = GetCamera()->GetViewMatrix();
	projectionMatrix_ = GetCamera()->GetProjectionMatrix();
	pD3D_->GetOrthoMatrix(orthoMatrix_);
	

	systemState->editorCameraPosition = GetCamera()->GetPositionFloat3();
	systemState->editorCameraRotation = GetCamera()->GetRotationFloat3();

	bool result = RenderScene(systemState, hwnd);  // render all the stuff on the screen
	COM_ERROR_IF_FALSE(result, "can't render the scene");

	// Show the rendered scene on the screen
	this->pD3D_->EndScene();

	return true;
}

//////////////////////////////////////////////////

void GraphicsClass::HandleKeyboardInput(const KeyboardEvent& kbe, 
	HWND hwnd,
	const float deltaTime)
{
	// handle input from the keyboard to modify some rendering params

	/*
	
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


		Model* pCube = pInitGraphics_->CreateCube();

		pCube->GetModelDataObj()->SetColor(1.0f, 1.0f, 1.0f, 1.0f);
		pCube->GetModelDataObj()->SetPosition(posX, posY, posZ);
		Log::Debug(THIS_FUNC, "N key is pressed");
		return;
	}
	else if (kbe.IsRelease() && kbe.GetKeyCode() == KEY_N)
	{
		keyN_WasActive = false;
	}
	
	*/

	// handle other inputs from the keyboard
	this->pZone_->HandleMovementInput(kbe, deltaTime);

	return;

} // end HandleKeyboardInput

//////////////////////////////////////////////////

void GraphicsClass::HandleMouseInput(const MouseEvent& me, 
	const POINT & windowDimensions,
	const float deltaTime)
{
	// this function handles the input events from the mouse

	MouseEvent::EventType eventType = me.GetType();

	// handle camera rotation, etc.
	if (eventType == MouseEvent::EventType::Move ||
		eventType == MouseEvent::EventType::RAW_MOVE)
		this->pZone_->HandleMovementInput(me, deltaTime);


	// in the input handling that is called each frame we now check to see if the user
	// has pressed or released the LMB. If the LMB was pressed then we perform the 
	// intersection check with particular shape (the sphere, the rectangle, etc.) using
	// the current 2D mouse coordinates

	// check if the left mouse button has been pressed
	if (me.GetType() == MouseEvent::EventType::LPress)
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
	if (me.GetType() == MouseEvent::EventType::LRelease)
	{
		isBeginCheck_ = false;
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
		pD3D_->SetRenderState(D3DClass::RASTER_PARAMS::FILL_MODE_SOLID);
	}
	else // turn off wire frame rendering of the terrain if it was on
	{
		pD3D_->SetRenderState(D3DClass::RASTER_PARAMS::FILL_MODE_WIREFRAME);
	}
};

///////////////////////////////////////////////////////////

// returns a pointer to the D3DClass instance
D3DClass* GraphicsClass::GetD3DClass() const { return pD3D_; }

// returns a pointer to the camera object
EditorCamera* GraphicsClass::GetCamera() const { return pCamera_; };

// returns a pointer to the shader container instance
ShadersContainer* GraphicsClass::GetShadersContainer() const { return pShadersContainer_; }

// returns a pointer to the game objects list class instance
GameObjectsListClass* GraphicsClass::GetGameObjectsList() const { return pGameObjectsList_; }

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
	{
		return ptr;
	}

	Log::Get()->Error(THIS_FUNC, "can't allocate memory for this object");
	throw std::bad_alloc{};
}

///////////////////////////////////////////////////////////

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
		pRenderGraphics_->RenderGUI(this, systemState, deltaTime_);
	}
	catch (COMException& exception)
	{
		Log::Error(exception);
		return false;
	}

	return true;
}
