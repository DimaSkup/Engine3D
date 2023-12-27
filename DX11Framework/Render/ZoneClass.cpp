////////////////////////////////////////////////////////////////////
// Filename::    ZoneClass.cpp
// Description:  impementation of the ZoneClass functional;
//
// Created:      10.03.23
////////////////////////////////////////////////////////////////////
#include "ZoneClass.h"




ZoneClass::ZoneClass(Settings* pEngineSettings,
	ID3D11DeviceContext* pDeviceContext,
	EditorCamera* pEditorCamera,
	GameObjectsListClass* pGameObjList,
	DataContainerForShaders* pDataContainer)
{
	assert(pEngineSettings != nullptr);
	assert(pDeviceContext != nullptr);
	assert(pEditorCamera != nullptr);
	assert(pGameObjList != nullptr);
	assert(pDataContainer != nullptr);

	try
	{
		pEditorCamera_   = pEditorCamera;
		pEngineSettings_ = pEngineSettings;
		pDeviceContext_  = pDeviceContext;
		pGameObjList_    = pGameObjList;
		pDataContainer_  = pDataContainer;     // init ptr to data container for shaders

		pFrustum_ = new FrustumClass();        // create the frustum object
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

	_DELETE(pFrustum_);

	pDeviceContext_ = nullptr;
}


////////////////////////////////////////////////////////////////////////////////////////////
//
//                                 PUBLIC FUNCTIONS
//
////////////////////////////////////////////////////////////////////////////////////////////

bool ZoneClass::Initialize()
{
	Log::Print("-------------  ZONE CLASS: INITIALIZATION  ---------------");
	Log::Debug(THIS_FUNC_EMPTY);

	try
	{
		float farZ = pEngineSettings_->GetSettingFloatByKey("FAR_Z");
		cameraHeightOffset_ = pEngineSettings_->GetSettingFloatByKey("CAMERA_HEIGHT_OFFSET");

		// set the rendering of the bounding box around each terrain cell
		showCellLines_ = true;  

		// set the user locked to the terrain height for movement
		heightLocked_ = true;   

		// initialize the frustum object
		pFrustum_->Initialize(farZ);

		// ---------------------------------------------------- //

		// get pointers to the game objects which are part of the zone
		pSkyDomeGameObj_ = pGameObjList_->GetGameObjectByID("sky_dome");
		pSkyPlaneGameObj_ = pGameObjList_->GetGameObjectByID("sky_plane");
		pTerrainGameObj_ = pGameObjList_->GetGameObjectByID("terrain");

	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(THIS_FUNC, "can't initialize the zone class object");
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////

bool ZoneClass::Render(int & renderCount,
	D3DClass* pD3D,
	const float deltaTime,
	std::vector<LightClass*> & arrDiffuseLightSources,
	std::vector<LightClass*> & arrPointLightSources)
{
	// renders models which are related to the terrain 
	// (terrain plane, sky dome/box, sky plane (clouds))

	try
	{
		// update the delta time value (time between frames)
		deltaTime_ = deltaTime;

		// construct the frustum
		pFrustum_->ConstructFrustum(pEditorCamera_->GetProjectionMatrix(), pEditorCamera_->GetViewMatrix());

		// ATTENTION: this is for debug purpose because sometimes we don't want to render 
		if (pDataContainer_ == nullptr)
			return true;

		// modify some point light sources
		this->RenderPointLightsOnTerrain(arrDiffuseLightSources, arrPointLightSources);

		// setup some common data which we will use for rendering this frame
		pDataContainer_->view = pEditorCamera_->GetViewMatrix();
		pDataContainer_->orthoOrProj = pEditorCamera_->GetProjectionMatrix();
		pDataContainer_->ptrToDiffuseLightsArr = &arrDiffuseLightSources;
		pDataContainer_->ptrToPointLightsArr = &arrPointLightSources;

		// render the sky dome (or sky box) and the sky plane (clouds)
	    RenderSkyElements(renderCount, pD3D);

		// render terrain
		RenderTerrainElements(renderCount);
		
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		Log::Error(THIS_FUNC, "can't render");
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////

void ZoneClass::HandleMovementInput(const KeyboardEvent& kbe, const float deltaTime)
{
	// handle events from the keyboard

	// during each frame the position class object is updated with the 
	// frame time for calculation the updated position
	pEditorCamera_->SetFrameTime(deltaTime);

	// after the frame time update the position movement functions can be updated
	// with the current state of the input devices. The movement function will update
	// the position of the camera to the location for this frame
	pEditorCamera_->HandleKeyboardEvents(kbe);

	// handle keyboard input to control the zone state (state of the camera, terrain, etc.)
	this->HandleZoneControlInput(kbe);

	return;
}

///////////////////////////////////////////////////////////

void ZoneClass::HandleMovementInput(const MouseEvent& me, const float deltaTime)
{
	// handle events from the mouse

	// during each frame the position class object is updated with the 
	// frame time for calculation the updated position
	pEditorCamera_->SetFrameTime(deltaTime);

	// after the frame time update the position movement functions can be updated
	// with the current state of the input devices. The movement function will update
	// the position of the camera to the location for this frame
	pEditorCamera_->HandleMouseEvents(me);

	return;
}






////////////////////////////////////////////////////////////////////////////////////////////
//
//                               PRIVATE FUNCTIONS
//
////////////////////////////////////////////////////////////////////////////////////////////



void ZoneClass::HandleZoneControlInput(const KeyboardEvent& kbe)
{
	// handle keyboard input to control the zone state (state of the camera, terrain, etc.)

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

///////////////////////////////////////////////////////////

void ZoneClass::RenderSkyElements(int & renderCount, D3DClass* pD3D)
{
	// ---------------------------- SKY DOME RENDERING ---------------------------------- //

	// before rendering the sky elements we turn off both back face culling and the Z buffer.
	pD3D->SetRenderState(D3DClass::RASTER_PARAMS::CULL_MODE_FRONT);
	pD3D->TurnZBufferOff();

	// before rendering of any other models (at all) we must render the sky dome
	this->RenderSkyDome(renderCount);

	// turn back face culling back on
	pD3D->SetRenderState(D3DClass::RASTER_PARAMS::CULL_MODE_BACK);

	// enable additive blending so the clouds blend with the sky dome color
	pD3D->TurnOnAlphaBlendingForSkyPlane();

	


	// ---------------------------- SKY PLANE RENDERING --------------------------------- //

	// enabled additive blending so the clouds (sky plane) blend with the sky dome colour
	pD3D->TurnOnAlphaBlendingForSkyPlane();

	// render the sky plane onto the scene
	this->RenderSkyPlane(renderCount, pD3D);

	// after rendering the sky elements we turn off alpha blending
	// and turn on the Z buffer back and back face culling
	pD3D->TurnOffAlphaBlending();
	pD3D->TurnZBufferOn();

	return;
}

///////////////////////////////////////////////////////////

void ZoneClass::RenderTerrainElements(int & renderCount)
{
	// render the terrain
	this->RenderTerrainPlane(renderCount);

	return;
}

///////////////////////////////////////////////////////////

void ZoneClass::RenderTerrainPlane(int & renderCount)
{
	// if we didn't initialize a terrain we can't render it
	// so we just go out
	if (pTerrainGameObj_ == nullptr)
		return;

	// ---------------------------------------------------- //

	const DirectX::XMFLOAT3 & curCameraPos{ pEditorCamera_->GetPositionFloat3() };

	TerrainClass* pTerrainModel = static_cast<TerrainClass*>(pTerrainGameObj_->GetModel());

	float height = 0.0f;                // current terrain height
	
	bool result = false;


	// ---------------------------------------------------- //

	// do some terrain model calculations
	pTerrainModel->Frame();

	// each frame we use the updated position as input to determine the height the camera
	// should be located at. We then set the height of the camera slightly above the 
	// terrain height by 1.0f;
	// if the height is locked to the terrain then position of the camera on top of it
	if (heightLocked_)
	{
		// get the height of the triangle that is directly underbneath the given camera position
		pTerrainModel->GetHeightAtPosition(curCameraPos.x, curCameraPos.z, height);

		// the camera's position is just above the terrain's triangle by some height value
		pEditorCamera_->SetPosition(curCameraPos.x, height + cameraHeightOffset_, curCameraPos.z);
	}

	// ---------------------------------------------------- //


	// render the terrain cells (and cell lines if needed)
	for (UINT i = 0; i < pTerrainModel->GetCellCount(); i++)
	{
		// define if we see a terrain cell by the camera if so
		// we render this terrain cell by particular index using the shader
		bool cell_is_visible = pTerrainModel->CheckIfSeeCellByIndex(i, pFrustum_);

		if (cell_is_visible)
		{
			GameObject* pTerrainCellGameObj = pTerrainModel->GetTerrainCellGameObjByIndex(i);

			// render this terrain cell onto the screen
			pTerrainCellGameObj->Render();

			// if needed then render the bounding box around this terrain cell using the colour shader
			if (showCellLines_)
			{
				pTerrainModel->RenderCellLines(i);
			}

		} // if
	} // for

	return;

} // RenderTerrainPlane

///////////////////////////////////////////////////////////

void ZoneClass::RenderSkyDome(int & renderCount)
{
	// render sky dome (colors or textures of the sky)

	// if we didn't initialize a sky dome (or sky box) we can't render it
	// so we just go out
	if (pSkyDomeGameObj_ == nullptr)
		return;


	SkyDomeClass* pSkyDomeModel = static_cast<SkyDomeClass*>(pSkyDomeGameObj_->GetModel());

	// ---------------------------------------------------- //

	// translate the sky dome to be centered around the camera position
	//worldMatrix = XMMatrixTranslation(cameraPosition.x, cameraPosition.y, cameraPosition.z);
	pSkyDomeGameObj_->GetData()->SetPosition(pEditorCamera_->GetPositionFloat3());

	// setup some data about sky dome (or sky box) which we will use for rendering this frame
	pDataContainer_->skyDomeApexColor = pSkyDomeModel->GetApexColor();
	pDataContainer_->skyDomeCenterColor = pSkyDomeModel->GetCenterColor();

	// ---------------------------------------------------- //

	// prepare model's vertex/index buffers for rendering and
	// render the sky dome using the sky dome shader
	pSkyDomeGameObj_->Render();

	renderCount++;   // since this model was rendered then increase the count for this frame

	return;
}

///////////////////////////////////////////////////////////

void ZoneClass::RenderSkyPlane(int & renderCount, D3DClass* pD3D)
{

	// render sky plane (clouds)

	// if we didn't initialize a sky plane we can't render it
	// so we just go out
	if (pSkyPlaneGameObj_ == nullptr)
		return;

	// ---------------------------------------------------- //

	SkyPlaneClass* pSkyPlaneModel = static_cast<SkyPlaneClass*>(pSkyPlaneGameObj_->GetModel());
	const DirectX::XMFLOAT3 & cameraPosition{ pEditorCamera_->GetPositionFloat3() };  // we use the camera position to setup a position of the sky plane


	// translate the sky dome to be centered around the camera position
	pSkyPlaneGameObj_->GetData()->SetPosition(cameraPosition.x, cameraPosition.y, cameraPosition.z);

	// do some sky plane calculations
	pSkyPlaneModel->Frame(deltaTime_);

	// get clouds' translation data
	float* translationData = pSkyPlaneModel->GetTranslationData();

	// setup data container for the shader before rendering of the sky plane
	pDataContainer_->skyPlanesTranslation.x = translationData[0];
	pDataContainer_->skyPlanesTranslation.y = translationData[1];
	pDataContainer_->skyPlanesTranslation.z = translationData[2];
	pDataContainer_->skyPlanesTranslation.w = translationData[3];
	pDataContainer_->skyPlanesBrightness = pSkyPlaneModel->GetBrightness();

	// render the sky plane using the sky plane shader
	pSkyPlaneGameObj_->Render();

	renderCount++;   // since this model was rendered then increase the count for this frame

	return;

}

///////////////////////////////////////////////////////////

void ZoneClass::RenderPointLightsOnTerrain(std::vector<LightClass*> & arrDiffuseLightSources,
	                                       std::vector<LightClass*> & arrPointLightSources)
{
	GameObject* pGameObj = nullptr;
	const UINT numPointLights = Settings::Get()->GetSettingIntByKey("NUM_POINT_LIGHTS");  // the number of point light sources on the terrain


	// local timer							 
	static float t = 0.0f;
	static DWORD dwTimeStart = 0;
	DWORD dwTimeCur = GetTickCount();

	if (dwTimeStart == 0)
		dwTimeStart = dwTimeCur;
	t = (dwTimeCur - dwTimeStart) / 1000.0f;


	// adjust positions of some point light sources
	arrPointLightSources[0]->AdjustPosY(std::abs(5 * sin(t)));
	arrPointLightSources[1]->AdjustPosX(5 * sin(t));
	arrPointLightSources[1]->AdjustPosZ(5);


	// render spheres as like they are point light sources
	for (UINT i = 0; i < numPointLights; i++)
	{
		std::string sphereID{ "sphere(" + std::to_string(i + 1) + ")" };
		pGameObj = this->pGameObjList_->GetGameObjectByID(sphereID);

		// setup spheres positions and colors to be the same as a point light source by this index 
		pGameObj->GetData()->SetPosition(arrPointLightSources[i]->GetPosition());
		pGameObj->GetData()->SetScale(0.2f, 0.2f, 0.2f);

		pGameObj->GetModel()->SetRenderShaderName("ColorShaderClass");

		// setup data container for the shader before rendering of the point light sphere
		pDataContainer_->modelColor = arrPointLightSources[i]->GetDiffuseColor();

		// prepare a model for rendering and render it onto the screen
		pGameObj->Render();
	}

	return;
}