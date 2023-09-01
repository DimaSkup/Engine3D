////////////////////////////////////////////////////////////////////
// Filename::    ZoneClass.cpp
// Description:  impementation of the ZoneClass functional;
//
// Created:      10.03.23
////////////////////////////////////////////////////////////////////
#include "ZoneClass.h"

ZoneClass::ZoneClass(Settings* pEngineSettings)
{
	assert(pEngineSettings != nullptr);

	try
	{
		//pTerrainShader_ = pTerrainShader;  // get a shader for rendering the terrain cell model
		//pColorShader_ = pColorShader; 	   // get a shader for rendering the cell lines model
		//pSkyDomeShader_ = pSkyDomeShader;  // get a shader for rendering the sky dome

		pEngineSettings_ = pEngineSettings;

		float cameraSpeed = Settings::Get()->GetSettingFloatByKey("CAMERA_SPEED");;
		float cameraSensitivity = Settings::Get()->GetSettingFloatByKey("CAMERA_SENSITIVITY");

		pCamera_ = new EditorCamera(cameraSpeed, cameraSensitivity);    // create the editor camera object
		pFrustum_ = new FrustumClass();                                 // create the frustum object
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(THIS_FUNC, e.what());
		COM_ERROR_IF_FALSE(false, "can't allocate memory for the ZoneClass elements");
	}
}



ZoneClass::~ZoneClass()
{
	Log::Debug(THIS_FUNC_EMPTY);

	_DELETE(pCamera_);
	_DELETE(pFrustum_);

	pDeviceContext_ = nullptr;
}


////////////////////////////////////////////////////////////////////
//
//                        PUBLIC FUNCTIONS
//
////////////////////////////////////////////////////////////////////

bool ZoneClass::Initialize()
{
	Log::Print("----------- ZONE CLASS: INITIALIZATION: THE CAMERA --------------");
	Log::Debug(THIS_FUNC_EMPTY);

	// get some settings values
	float windowWidth  = pEngineSettings_->GetSettingFloatByKey("WINDOW_WIDTH");
	float windowHeight = pEngineSettings_->GetSettingFloatByKey("WINDOW_HEIGHT");
	float fovDegrees   = pEngineSettings_->GetSettingFloatByKey("FOV_DEGREES");
	float nearZ        = pEngineSettings_->GetSettingFloatByKey("NEAR_Z");
	float farZ         = pEngineSettings_->GetSettingFloatByKey("FAR_Z");
	
	// calculate the aspect ratio
	float aspectRatio = windowWidth / windowHeight;

	// setup the EditorCamera object
	pCamera_->SetPosition({ 0.0f, 0.0f, -3.0f });
	pCamera_->SetProjectionValues(fovDegrees, aspectRatio, nearZ, farZ);

	// initialize the frustum object
	pFrustum_->Initialize(farZ);

	// set the rendering of the bounding box around each terrain cell
	showCellLines_ = true;

	// set the user locked to the terrain height for movement
	heightLocked_ = true;

	return true;
}



// renders models which are related to the terrain
void ZoneClass::Render(int & renderCount,
	D3DClass* pD3D,
	float deltaTime)
{
	DirectX::XMMATRIX projectionMatrix;

	// update the delta time value (time between frames)
	deltaTime_ = deltaTime;

	// get the projection matrix from the D3DClass object
	projectionMatrix = pCamera_->GetProjectionMatrix();

	// construct the frustum
	pFrustum_->ConstructFrustum(projectionMatrix, pCamera_->GetViewMatrix());

	// render the zone
	RenderSkyElements(renderCount, pD3D);
	//RenderTerrainElements(renderCount, pD3D);

	return;
}


// handle events from the keyboard
void ZoneClass::HandleMovementInput(const KeyboardEvent& kbe, float deltaTime)
{
	// during each frame the position class object is updated with the 
	// frame time for calculation the updated position
	pCamera_->SetFrameTime(deltaTime);

	// after the frame time update the position movement functions can be updated
	// with the current state of the input devices. The movement function will update
	// the position of the camera to the location for this frame
	pCamera_->HandleKeyboardEvents(kbe);


	// handle keyboard input to control the zone state (state of the camera, terrain, etc.)
	this->HandleZoneControlInput(kbe);

	return;
}


// handle events from the mouse
void ZoneClass::HandleMovementInput(const MouseEvent& me, float deltaTime)
{
	// during each frame the position class object is updated with the 
	// frame time for calculation the updated position
	pCamera_->SetFrameTime(deltaTime);

	// after the frame time update the position movement functions can be updated
	// with the current state of the input devices. The movement function will update
	// the position of the camera to the location for this frame
	pCamera_->HandleMouseEvents(me);

	return;
}


// returns a pointer to the camera object
EditorCamera* ZoneClass::GetCamera() const _NOEXCEPT { return pCamera_; };


// when an instance of a models list is created we have to setup a local pointer to it
void ZoneClass::SetModelsList(ModelListClass* pModelsList)
{
	assert(pModelsList != nullptr);
	pModelsList_ = pModelsList;
	return;
}


////////////////////////////////////////////////////////////////////
//
//                     PRIVATE FUNCTIONS
//
////////////////////////////////////////////////////////////////////


// handle keyboard input to control the zone state (state of the camera, terrain, etc.)
void ZoneClass::HandleZoneControlInput(const KeyboardEvent& kbe)
{
	static bool keyF3IsActive = false;
	static bool keyF4IsActive = false;

	// determine if we should render the line box around each terrain cell
	if (GetAsyncKeyState(VK_F3) && (keyF3IsActive == false))
	{
		keyF3IsActive = true;
		showCellLines_ = !showCellLines_;
	}
	else if (kbe.IsRelease())
	{
		keyF3IsActive = false;
	}


	// determine if we should lock the camera height to the height 
	// of the current terrain mesh
	if (GetAsyncKeyState(VK_F4) && (keyF4IsActive == false))
	{
		keyF4IsActive = true;
		heightLocked_ = !heightLocked_;
	}
	else if (kbe.IsRelease())
	{
		keyF4IsActive = false;
	}

	return;
}


//
// RENDER FUNCTIONS
//

void ZoneClass::RenderSkyElements(int & renderCount, D3DClass* pD3D)
{
	// before rendering the sky elements we turn off both back face culling and the Z buffer.
	pD3D->SetRenderState(D3DClass::RASTER_PARAMS::CULL_MODE_FRONT);
	pD3D->TurnZBufferOff();

	// before rendering of any other models (at all) we must render the sky dome
	this->RenderSkyDome(GetModelByID("sky_dome"), renderCount, pD3D);

	// turn back face culling back on
	pD3D->SetRenderState(D3DClass::RASTER_PARAMS::CULL_MODE_BACK);

	// enable additive blending so the clouds blend with the sky dome color
	pD3D->TurnOnAlphaBlendingForSkyPlane();

	// render the sky plane onto the scene
	//this->RenderSkyPlane(GetModelByID("sky_plane"), renderCount, pD3D);

	// after rendering the sky elements we turn off alpha blending
	// and turn on the Z buffer back and back face culling
	pD3D->TurnOffAlphaBlending();
	pD3D->TurnZBufferOn();

	return;
}


void ZoneClass::RenderTerrainElements(int & renderCount, D3DClass* pD3D)
{
	// render the terrain
	this->RenderTerrain(GetModelByID("terrain"), renderCount, pD3D, pFrustum_);

	return;
}



void ZoneClass::RenderTerrain(Model* pTerrainModel, int & renderCount, D3DClass* pD3D, FrustumClass* pFrustum)
{
	TerrainClass* pTerrain = static_cast<TerrainClass*>(pTerrainModel);
	bool result = false;
	bool foundHeight = false;  // did we find the current terrain height?
	float height = 0.0f;       // current terrain height
	DirectX::XMFLOAT3 curCameraPos{ pCamera_->GetPositionFloat3() };
	float cameraHeightOffset = 0.5f;


	// do some terrain calculations
	pTerrain->Frame();

	// each frame we use the updated position as input to determine the height the camera
	// should be located at. We then set the height of the camera slightly above the 
	// terrain height by 1.0f;
	// if the height is locked to the terrain then position of the camera on top of it
	if (heightLocked_)
	{
		// get the height of the triangle that is directly underbneath the given camera position
		foundHeight = pTerrain->GetHeightAtPosition(curCameraPos.x, curCameraPos.z, height);
		if (foundHeight)
		{
			// if there was a triangle under the camera then position the camera just above it by one meter
			pCamera_->SetPosition(curCameraPos.x, height + cameraHeightOffset, curCameraPos.z);
		}
	}


	// render the terrain cells (and cell lines if needed)
	for (UINT i = 0; i < pTerrain->GetCellCount(); i++)
	{
		pTerrain->Render(pD3D->GetDeviceContext(), i, pFrustum); // render a terrain cell by particular index

		// if needed then render the bounding box around this terrain cell using the colour shader
		if (showCellLines_)
		{
			pTerrain->RenderCellLines(pD3D->GetDeviceContext(), i);
		}
	}

	return;
}

void ZoneClass::RenderSkyDome(Model* pSkyDome, int & renderCount, D3DClass* pD3D)
{
	//DirectX::XMMATRIX worldMatrix;   // a world matrix for the sky dome model
	DirectX::XMFLOAT3 cameraPosition{ GetCamera()->GetPositionFloat3() };  // we use the camera position to create a world matrix centered around the camera

	// translate the sky dome to be centered around the camera position
	//worldMatrix = XMMatrixTranslation(cameraPosition.x, cameraPosition.y, cameraPosition.z);
	pSkyDome->GetModelDataObj()->SetPosition(cameraPosition.x, cameraPosition.y, cameraPosition.z);

	// render the sky dome using the sky dome shader
	pSkyDome->Render(pD3D->GetDeviceContext());

	//pSkyDomeShader_->Render()

	renderCount++;   // since this model was rendered then increase the count for this frame

	return;
}

void ZoneClass::RenderSkyPlane(Model* pSkyPlaneModel, int & renderCount, D3DClass* pD3D)
{
	SkyPlaneClass* pSkyPlane = static_cast<SkyPlaneClass*>(pSkyPlaneModel);

	// we use the camera position to setup a position of the sky plane
	DirectX::XMFLOAT3 cameraPosition{ GetCamera()->GetPositionFloat3() };  

	// translate the sky dome to be centered around the camera position
	pSkyPlane->GetModelDataObj()->SetPosition(cameraPosition.x, cameraPosition.y, cameraPosition.z);


	// enabled additive blending so the clouds blend with the sky dome colour
	pD3D->TurnOnAlphaBlendingForSkyPlane();


	// do some sky plane calculations
	pSkyPlane->Frame(deltaTime_);

	// render the sky plane using the sky plane shader
	pSkyPlane->Render(pD3D->GetDeviceContext());

	renderCount++;   // since this model was rendered then increase the count for this frame

	return;
}

// get a model ptr by this model's ID 
Model* ZoneClass::GetModelByID(const std::string & modelID)
{
	return pModelsList_->GetModelByID(modelID);
}