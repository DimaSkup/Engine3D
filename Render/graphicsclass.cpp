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
		pRenderGraphics_ = new RenderGraphics(this, pEngineSettings_);
		pFrustum_ = new FrustumClass();
		pUserInterface_ = new UserInterfaceClass();

		pTextureManager_ = new TextureManagerClass();
		pCamera_ = new EditorCamera(cameraSpeed, cameraSensitivity);    // create the editor camera object
		pCameraForRenderToTexture_ = new CameraClass();                 // this camera is used for rendering into textures
		pRenderToTexture_ = new RenderToTextureClass();
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
	_DELETE(pModelList_);


	_DELETE(pCamera_);
	_DELETE(pZone_);
	_DELETE(pRenderGraphics_);

	_DELETE(pRenderToTexture_);
	_DELETE(pCameraForRenderToTexture_);

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

	RenderScene(systemState, hwnd);  // render all the stuff on the screen

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

	return;

} // end HandleKeyboardInput

//////////////////////////////////////////////////

void GraphicsClass::HandleMouseInput(const MouseEvent& me, 
	const POINT & windowDimensions,
	const float deltaTime)
{
	// handle input from the mouse

	MouseEvent::EventType eventType = me.GetType();

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
		// if the used has clicked on the screen with the mouse then perform an intersection test
		if (isBeginCheck_ == false)
		{
			isBeginCheck_ = true;
			MousePoint mPoint = me.GetPos();
			TestIntersection(mPoint.x, mPoint.y, windowDimensions);

			if (isIntersect_ == true)
			{
				Log::Print("INTERSECT");
			}
			else
			{
				Log::Error("MISS");
			}
		}
	}

	// check if the left mouse button has been released
	if (me.GetType() == MouseEvent::EventType::LRelease)
	{
		isBeginCheck_ = false;
	}

	return;
} // end HandleMouseInput

//////////////////////////////////////////////////

void GraphicsClass::TestIntersection(const int mouseX, const int mouseY,
	const POINT & windowDimensions)
{
	/*

	This function takes as input the 2D mouse coordinates and then forms a vector in 3D 
	space which it uses to then check for an intersection with the sphere (or some another
	shape). That vector is called the picking ray. The picking ray has the origin and 
	a direction. With the 3D coordinate (origin) and 3D vector/normal (direction) we can
	create a line in 3D space and find out what it collides with.

	Usually we are very used to a vertex shader that takes a 3D point (vertice) and moves
	it from 3D space onto the 2D screen so it can be rendered as the pixel. Well now we are
	doing the exact opposite and moving a 2D point from the screen to view to projection
	to make a 2D point, we will now instead take a 2D point and go from projection to view
	to world and turn it into a 3D point.

	To do the reverse process we first start by taking the mouse coordinates and moving 
	them into the -1 to +1 range on both axis. When we have that we then divide by the 
	screen aspect using the projection matrix. With that value we can then multiply it by
	the inverse view matrix (inverse because we are going in reverse direction) to get the
	direction vector in view space. We can set the origin of the vector in view space
	to just be the location of the camera.

	With the direction vector and origin in view space we can now complete the final process
	of moving it into 3D world space. To do so we first need to get the world matrix and
	translate it by the position of the sphere. With the updated world matrix we once again
	need to invert it (since the process is going in the opposite direction) and then we 
	can multiply the origin and direction by the inverted world matrix. We also normalize
	the direction after the multiplication. This gives us the origin and direction of the 
	vector in 3D world space so that we can do tests with other objects that are also
	in 3D world space.

	Now that we have the origin of the vector and the direction of the vector we can
	perform an interstection test. For instance it can be a ray-sphere intersection test,
	but you could perform any kind of intersection test that you have the vector in 3D 
	world space

	*/

	DirectX::XMMATRIX inverseViewMatrix;
	DirectX::XMMATRIX inverseWorldMatrix;
	DirectX::XMMATRIX translateMatrix;
	DirectX::XMMATRIX worldMatrix;
	DirectX::XMMATRIX projMatrix{ pCamera_->GetProjectionMatrix() };
	DirectX::XMVECTOR direction;
	DirectX::XMVECTOR rayOrigin;
	DirectX::XMVECTOR rayDirection;
	DirectX::XMFLOAT4X4 fInvViewMatrix;
	float pointX = 0.0f;
	float pointY = 0.0f;
	bool intersect = false;
	DirectX::XMFLOAT3 spherePosition{ 20.0f, 3.0f, 25.0f };


	
	// move the mouse cursor coordinates into the -1 to +1 range
	pointX = (2.0f * static_cast<float>(mouseX) / static_cast<float>(windowDimensions.x)) - 1.0f;
	pointY = ((2.0f * static_cast<float>(mouseY) / static_cast<float>(windowDimensions.y)) - 1.0f) * -1.0f;

	// adjust the points using the projection matrix to account for the aspect ration of the viewport;
	pointX = pointX / (DirectX::XMVectorGetX(projMatrix.r[1]));
	pointY = pointY / (DirectX::XMVectorGetY(projMatrix.r[2]));

	// get the inverse of the view matrix
	inverseViewMatrix = DirectX::XMMatrixInverse(nullptr, pCamera_->GetViewMatrix());

	// convert the inverse of the view matrix into a 4x4 float type
	DirectX::XMStoreFloat4x4(&fInvViewMatrix, inverseViewMatrix);

	// calculate the direction of the picking ray in view space
	direction.m128_f32[0] = (pointX * fInvViewMatrix._11) + (pointY * fInvViewMatrix._21) + fInvViewMatrix._31;
	direction.m128_f32[1] = (pointX * fInvViewMatrix._12) + (pointY * fInvViewMatrix._22) + fInvViewMatrix._32;
	direction.m128_f32[2] = (pointX * fInvViewMatrix._13) + (pointY * fInvViewMatrix._23) + fInvViewMatrix._33;

	
	// get the world matrix and translate to the location of the sphere
	pD3D_->GetWorldMatrix(worldMatrix);
	translateMatrix = DirectX::XMMatrixTranslation(spherePosition.x, spherePosition.y, spherePosition.z);
	worldMatrix = worldMatrix * translateMatrix;

	// now get the inverse of the translated world matrix
	inverseWorldMatrix = DirectX::XMMatrixInverse(nullptr, worldMatrix);

	// now transform the ray origin and the ray direction from view space to world space
	rayOrigin = DirectX::XMVector3TransformCoord(pCamera_->GetPositionVector(), inverseWorldMatrix);
	rayDirection = DirectX::XMVector3TransformNormal(direction, inverseWorldMatrix);

	// normalize the ray direction
	rayDirection = DirectX::XMVector3Normalize(rayDirection);

	// now perform the ray-sphere intersection test
	isIntersect_ = RaySphereIntersect(rayOrigin, rayDirection, 1.0f);

	return;

} // TestIntersection

///////////////////////////////////////////////////////////

bool GraphicsClass::RaySphereIntersect(const DirectX::XMVECTOR & rayOrigin,
	const DirectX::XMVECTOR & rayDirection,
	const float radius)
{
	// this function performs the math of a basic ray-sphere intersection test

	float a = 0.0f;
	float b = 0.0f;
	float c = 0.0f;
	float discriminant = 0.0f;
	
	DirectX::XMFLOAT3 fRayOrigin;
	DirectX::XMFLOAT3 fRayDirection;

	DirectX::XMStoreFloat3(&fRayOrigin, rayOrigin);
	DirectX::XMStoreFloat3(&fRayDirection, rayDirection);


	// calculate the a, b and c coefficients
	a = (fRayDirection.x * fRayDirection.x) + (fRayDirection.y * fRayDirection.y) + (fRayDirection.z * fRayDirection.z);
	b = ((fRayDirection.x * fRayOrigin.x) + (fRayDirection.y * fRayOrigin.y) + (fRayDirection.z * fRayOrigin.z)) * 2.0f;
	c = (fRayOrigin.x * fRayOrigin.x) + (fRayOrigin.y * fRayOrigin.y) + (fRayOrigin.z * fRayOrigin.z) - (radius * radius);

	// find the discriminant
	discriminant = (b * b) - (4 * a * c);

	// if discriminant is negative the picking ray missed the sphere, otherwise it intersected the sphere
	return (discriminant > 0.0f);

} // end RaySphereIntersect

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
EditorCamera* GraphicsClass::GetCamera() const { return pCamera_; };

// returns a pointer to the shader container instance
ShadersContainer* GraphicsClass::GetShadersContainer() const { return pShadersContainer_; }

// returns a pointe to the models list class instance
ModelListClass* GraphicsClass::GetModelsList() const { return pModelList_; }

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
		pRenderGraphics_->RenderGUI(this, systemState, deltaTime_);
	}
	catch (COMException& exception)
	{
		Log::Error(exception);
		return false;
	}

	return true;
}
