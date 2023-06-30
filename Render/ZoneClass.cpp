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
		pEngineSettings_ = pEngineSettings;

		float cameraSpeed = Settings::Get()->GetSettingFloatByKey("CAMERA_SPEED");;
		float cameraSensitivity = Settings::Get()->GetSettingFloatByKey("CAMERA_SENSITIVITY");;

		pCamera_ = new EditorCamera(cameraSpeed, cameraSensitivity);    // create the editor camera object
		pFrustum_ = new FrustumClass();   // create the frustum object
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
void ZoneClass::Render(const std::map<std::string, ModelClass*> & modelsList,
	int & renderCount,
	D3DClass* pD3D)
{
	DirectX::XMMATRIX projectionMatrix;

	// get the projection matrix from the D3DClass object
	projectionMatrix = pCamera_->GetProjectionMatrix();

	// construct the frustum
	pFrustum_->ConstructFrustum(projectionMatrix, pCamera_->GetViewMatrix());


	// before rendering the sky elements we turn off both back face culling and the Z buffer.
	pD3D->SetRenderState(D3DClass::RASTER_PARAMS::CULL_MODE_FRONT);
	pD3D->TurnZBufferOff();

	// before rendering of any other models we must render the sky dome
	auto modelsListIterator = modelsList.find("sky_dome");
	if (modelsListIterator == modelsList.end())
		COM_ERROR_IF_FALSE(false, "can't find the SKY DOME model in the models list");

	this->RenderSkyDome(modelsListIterator->second, renderCount, pD3D);


	//
	//modelsListIterator = modelsList.find("sky_plane");
	//if (modelsListIterator == modelsList.end())
	//	COM_ERROR_IF_FALSE(false, "can't find the SKY PLANE model in the models list");

	// after rendering the sky elements we turn off alpha blending
	// and turn on the Z buffer back and back face culling
	pD3D->TurnOffAlphaBlending();
	pD3D->TurnZBufferOn();
	pD3D->SetRenderState(D3DClass::RASTER_PARAMS::CULL_MODE_BACK);

	// then we render the terrain
	modelsListIterator = modelsList.find("terrain");
	if (modelsListIterator == modelsList.end())
		COM_ERROR_IF_FALSE(false, "can't find the TERRAIN model in the models list");

	this->RenderTerrain(modelsListIterator->second, renderCount, pD3D, pFrustum_);
	

	


	return;
}


void ZoneClass::HandleMovementInput(const KeyboardEvent& kbe, float deltaTime)
{
	static bool keyIsActive = false;

	// during each frame the position class object is updated with the 
	// frame time for calculation the updated position
	pCamera_->SetFrameTime(deltaTime);

	// after the frame time update the position movement functions can be updated
	// with the current state of the input devices. The movement function will update
	// the position of the camera to the location for this frame
	pCamera_->HandleKeyboardEvents(kbe);

	// determine if we should render the lines around each terrain cell
	BYTE lpKeyState[256];
	GetKeyboardState(lpKeyState);

	if (GetAsyncKeyState(VK_F3) && (keyIsActive == false))
	{
		keyIsActive = true;
		showCellLines_ = !showCellLines_;
	}
	else if (kbe.IsRelease())
	{
		keyIsActive = false;
	}

	return;
}


void ZoneClass::HandleMovementInput(const MouseEvent& me, float deltaTime)
{
	// during each frame the position class object is updated with the 
	// frame time for calculation the updated position
	pCamera_->SetFrameTime(deltaTime);



	// after the frame time update the position movement functions can be updated
	// with the current state of the input devices. The movement function will update
	// the position of the camera to the location for this frame
	pCamera_->HandleMouseEvents(me);

}



////////////////////////////////////////////////////////////////////
//
//                     PRIVATE FUNCTIONS
//
////////////////////////////////////////////////////////////////////

void ZoneClass::RenderTerrain(ModelClass* pTerrainModel, int & renderCount, D3DClass* pD3D, FrustumClass* pFrustum)
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
		pTerrain->Render(pD3D->GetDeviceContext(), i, pFrustum);

		// if needed then render the bounding box around this terrain cell using the colour shader
		if (showCellLines_)
		{
			pTerrain->RenderCellLines(pD3D->GetDeviceContext(), i);
		}
	}
	return;
}

void ZoneClass::RenderSkyDome(ModelClass* pSkyDome, int & renderCount, D3DClass* pD3D)
{
	DirectX::XMMATRIX worldMatrix;   // a world matrix for the sky dome model
	DirectX::XMFLOAT3 cameraPosition{ GetCamera()->GetPositionFloat3() };  // we use the camera position to create a world matrix centered around the camera

	// translate the sky dome to be centered around the camera position
	worldMatrix = XMMatrixTranslation(cameraPosition.x, cameraPosition.y, cameraPosition.z);
	pSkyDome->SetPosition(cameraPosition.x, cameraPosition.y, cameraPosition.z);

	// render the sky dome using the sky dome shader
	pSkyDome->Render(pD3D->GetDeviceContext());

	renderCount++;            // since this model was rendered then increase the count for this frame

	return;
}


void ZoneClass::RenderSkyPlane(ModelClass* pSkyPlaneModel, int & renderCounts, D3DClass* pD3D)
{
	SkyPlaneClass* pSkyPlane = static_cast<SkyPlaneClass*>(pSkyPlaneModel);

	// enabled additive blending so the clouds blend with the sky dome colour
	pD3D->TurnOnAlphaBlendingForSkyPlane();

	// render the sky plane using the sky plane shader
	pSkyPlane->Render(pD3D->GetDeviceContext());

	// do some sky plane calculations
	pSkyPlane->Frame();
}