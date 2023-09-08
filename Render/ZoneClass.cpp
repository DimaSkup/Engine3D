////////////////////////////////////////////////////////////////////
// Filename::    ZoneClass.cpp
// Description:  impementation of the ZoneClass functional;
//
// Created:      10.03.23
////////////////////////////////////////////////////////////////////
#include "ZoneClass.h"

ZoneClass::ZoneClass(Settings* pEngineSettings,
	EditorCamera* pEditorCamera,
	ModelListClass* pModelList,
	ShadersContainer* pShadersContainer)
{
	assert(pEngineSettings != nullptr);
	assert(pEditorCamera != nullptr);
	assert(pModelList != nullptr);
	assert(pShadersContainer != nullptr);

	try
	{
		pEditorCamera_ = pEditorCamera;
		pEngineSettings_ = pEngineSettings;

		pModelsList_ = pModelList;
		pShadersContainer_ = pShadersContainer;

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


////////////////////////////////////////////////////////////////////
//
//                        PUBLIC FUNCTIONS
//
////////////////////////////////////////////////////////////////////

bool ZoneClass::Initialize()
{
	Log::Print("-------------  ZONE CLASS: INITIALIZATION  ---------------");
	Log::Debug(THIS_FUNC_EMPTY);

	try
	{
		float farZ = pEngineSettings_->GetSettingFloatByKey("FAR_Z");

		showCellLines_ = true;  // set the rendering of the bounding box around each terrain cell
		heightLocked_ = true;   // set the user locked to the terrain height for movement

								// initialize the frustum object
		pFrustum_->Initialize(farZ);

		// get pointers to shaders which are used to render the terrain, sky dome, sky plane, etc.
		pColorShader_ = static_cast<ColorShaderClass*>(pShadersContainer_->GetShaderByName("ColorShaderClass"));
		pSkyDomeShader_ = static_cast<SkyDomeShaderClass*>(pShadersContainer_->GetShaderByName("SkyDomeShaderClass"));
		pSkyPlaneShader_ = static_cast<SkyPlaneShaderClass*>(pShadersContainer_->GetShaderByName("SkyPlaneShaderClass"));
		pTerrainShader_ = static_cast<TerrainShaderClass*>(pShadersContainer_->GetShaderByName("TerrainShaderClass"));
		pPointLightShader_ = static_cast<PointLightShaderClass*>(pShadersContainer_->GetShaderByName("PointLightShaderClass"));

		// get pointers to models which are part of the zone
		pSkyDome_ = static_cast<SkyDomeClass*>(pModelsList_->GetModelByID("sky_dome"));
		pSkyPlane_ = static_cast<SkyPlaneClass*>(pModelsList_->GetModelByID("sky_plane"));
		pTerrain_ = static_cast<TerrainClass*>(pModelsList_->GetModelByID("terrain"));
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(THIS_FUNC, "can't initialize the zone class object");
		return false;
	}

	return true;
}



// renders models which are related to the terrain
bool ZoneClass::Render(int & renderCount,
	D3DClass* pD3D,
	const float deltaTime,
	std::vector<LightClass*> & arrDiffuseLightSources,
	std::vector<LightClass*> & arrPointLightSources)
{
	try
	{
		// update the delta time value (time between frames)
		deltaTime_ = deltaTime;

		// construct the frustum
		pFrustum_->ConstructFrustum(pEditorCamera_->GetProjectionMatrix(), pEditorCamera_->GetViewMatrix());

		// render the zone
		RenderSkyElements(renderCount, pD3D);

		RenderTerrainElements(pD3D->GetDeviceContext(), 
			renderCount,
			arrDiffuseLightSources,
			arrPointLightSources);
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		Log::Error(THIS_FUNC, "can't render");
		return false;
	}

	return true;
}


// handle events from the keyboard
void ZoneClass::HandleMovementInput(const KeyboardEvent& kbe, const float deltaTime)
{
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

// handle events from the mouse
void ZoneClass::HandleMovementInput(const MouseEvent& me, const float deltaTime)
{
	// during each frame the position class object is updated with the 
	// frame time for calculation the updated position
	pEditorCamera_->SetFrameTime(deltaTime);

	// after the frame time update the position movement functions can be updated
	// with the current state of the input devices. The movement function will update
	// the position of the camera to the location for this frame
	pEditorCamera_->HandleMouseEvents(me);

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
	// ---------------------------- SKY DOME RENDERING ---------------------------------- //

	// before rendering the sky elements we turn off both back face culling and the Z buffer.
	pD3D->SetRenderState(D3DClass::RASTER_PARAMS::CULL_MODE_FRONT);
	pD3D->TurnZBufferOff();

	// before rendering of any other models (at all) we must render the sky dome
	this->RenderSkyDome(pD3D->GetDeviceContext(), renderCount);

	// turn back face culling back on
	pD3D->SetRenderState(D3DClass::RASTER_PARAMS::CULL_MODE_BACK);

	// enable additive blending so the clouds blend with the sky dome color
	pD3D->TurnOnAlphaBlendingForSkyPlane();

	


	// ---------------------------- SKY PLANE RENDERING --------------------------------- //

	// render the sky plane onto the scene
	this->RenderSkyPlane(renderCount, pD3D);

	// after rendering the sky elements we turn off alpha blending
	// and turn on the Z buffer back and back face culling
	pD3D->TurnOffAlphaBlending();
	pD3D->TurnZBufferOn();

	return;
}


void ZoneClass::RenderTerrainElements(ID3D11DeviceContext* pDeviceContext, 
	int & renderCount,
	std::vector<LightClass*> & arrDiffuseLightSources,
	std::vector<LightClass*> & arrPointLightSources)
{
	// render the terrain
	this->RenderTerrainPlane(pDeviceContext, 
		renderCount,
		pFrustum_, 
		arrDiffuseLightSources,
		arrPointLightSources);

	return;
}



void ZoneClass::RenderTerrainPlane(ID3D11DeviceContext* pDeviceContext, 
	int & renderCount,
	FrustumClass* pFrustum,
	std::vector<LightClass*> & arrDiffuseLightSources,
	std::vector<LightClass*> & arrPointLightSources)
{
	Model* pModel = nullptr;
	bool result = false;
	DirectX::XMFLOAT3 curCameraPos{ pEditorCamera_->GetPositionFloat3() };
	float height = 0.0f;                // current terrain height
	float cameraHeightOffset = 0.5f;    // camera's height above the terrain
	const UINT numPointLights = Settings::Get()->GetSettingIntByKey("NUM_POINT_LIGHTS");  // the number of point light sources on the terrain

	std::vector<DirectX::XMFLOAT4> arrPointLightsPositions(numPointLights);
	std::vector<DirectX::XMFLOAT4> arrPointLightsColors(numPointLights);

	// setup the two arrays (color and position) from the point lights.
	for (UINT i = 0; i < numPointLights; i++)
	{
		arrPointLightsPositions[i] = arrPointLightSources[i]->GetPosition();      // create the diffuse color array from the point lights colors
		arrPointLightsColors[i] = arrPointLightSources[i]->GetDiffuseColor();     // create the light position array from the point lights positions
	}

	/*
	
	// setup the plane which will be illuminated by point light sources
	pModel = pModelsList_->GetModelByID("plane(1)");

	pModel->GetModelDataObj()->SetPosition(0.0f, 0.8f, 0.0f);
	pModel->GetModelDataObj()->SetRotationInDegrees(0.0f, -90.0f, 0.0f);

	// put the model's buffers into the rendering pipeline
	pModel->Render(pDeviceContext);   

	// render a model using the shader
	result = pPointLightShader_->Render(pDeviceContext,
		pModel->GetModelDataObj()->GetIndexCount(),
		pModel->GetModelDataObj()->GetWorldMatrix(),
		pEditorCamera_->GetViewMatrix(),
		pEditorCamera_->GetProjectionMatrix(),
		pModel->GetTextureArray()->GetTextureResourcesArray(),
		arrPointLightsColors.data(),
		arrPointLightsPositions.data());
	COM_ERROR_IF_FALSE(result, "can't render the plane using the point light shader");
	
	*/



	// render spheres as like they are point light sources
	for (UINT i = 0; i < numPointLights; i++)
	{
		std::string sphereID{ "sphere(" + std::to_string(i + 1) + ")" };
		pModel = pModelsList_->GetModelByID(sphereID);

		// setup spheres positions and colors to be the same as a point light source by this index 
		pModel->GetModelDataObj()->SetColor(arrPointLightsColors[i]);
		pModel->GetModelDataObj()->SetPosition(arrPointLightsPositions[i]);
		pModel->GetModelDataObj()->SetScale(0.2f, 0.2f, 0.2f);

		// prepare a model for rendering
		pModel->Render(pDeviceContext);

		// render a model using the shader
		result = pColorShader_->Render(pDeviceContext,
			pModel->GetModelDataObj()->GetIndexCount(),
			pModel->GetModelDataObj()->GetWorldMatrix(),
			pEditorCamera_->GetViewMatrix(),
			pEditorCamera_->GetProjectionMatrix(),
			pModel->GetModelDataObj()->GetColor());
		COM_ERROR_IF_FALSE(result, "can't render the sphere using the color shader");
	}

	


	// do some terrain calculations
	pTerrain_->Frame();




	// each frame we use the updated position as input to determine the height the camera
	// should be located at. We then set the height of the camera slightly above the 
	// terrain height by 1.0f;
	// if the height is locked to the terrain then position of the camera on top of it
	if (heightLocked_)
	{
		// get the height of the triangle that is directly underbneath the given camera position
		pTerrain_->GetHeightAtPosition(curCameraPos.x, curCameraPos.z, height);

		// the camera's position is just above the terrain's triangle by some height value
		pEditorCamera_->SetPosition(curCameraPos.x, height + cameraHeightOffset, curCameraPos.z);
	}


	// render the terrain cells (and cell lines if needed)
	for (UINT i = 0; i < pTerrain_->GetCellCount(); i++)
	{
		// define if we see this terrain cell by the camera if so
		// we prepare this terrain cell's vertex/index buffers for rendering
		TerrainCellClass* pTerrainCell = pTerrain_->Render(pDeviceContext, i, pFrustum);

		// render a terrain cell by particular index using the shader (if we see it)
		if (pTerrainCell)
		{
			bool result = pTerrainShader_->Render(pDeviceContext,
				pTerrainCell->GetModelDataObj()->GetIndexCount(),
				pTerrainCell->GetModelDataObj()->GetWorldMatrix(),
				pEditorCamera_->GetViewMatrix(),
				pEditorCamera_->GetProjectionMatrix(),
				pTerrainCell->GetTextureArray()->GetTextureResourcesArray(),
				*(arrDiffuseLightSources.data()),  // get an array of pointers to diffuse lights objects
				arrPointLightsPositions.data(),
				arrPointLightsColors.data());
			COM_ERROR_IF_FALSE(result, "can't render a terrain cell using the terrain shader");
		}

		// if needed then render the bounding box around this terrain cell using the colour shader
		if (showCellLines_)
		{
			//pTerrain->RenderCellLines(pD3D->GetDeviceContext(), i);
		}
	}

	return;
}

// render sky dome (colors or textures of the sky)
void ZoneClass::RenderSkyDome(ID3D11DeviceContext* pDeviceContext, int & renderCount)
{
	// translate the sky dome to be centered around the camera position
	//worldMatrix = XMMatrixTranslation(cameraPosition.x, cameraPosition.y, cameraPosition.z);
	pSkyDome_->GetModelDataObj()->SetPosition(pEditorCamera_->GetPositionFloat3());

	// prepare model's vertex/index buffers for rendering
	pSkyDome_->Render(pDeviceContext);

	// render the sky dome using the sky dome shader
	bool result = pSkyDomeShader_->Render(pDeviceContext,
		pSkyDome_->GetModelDataObj()->GetIndexCount(),
		pSkyDome_->GetModelDataObj()->GetWorldMatrix(),
		pEditorCamera_->GetViewMatrix(),
		pEditorCamera_->GetProjectionMatrix(),
		pSkyDome_->GetTextureArray()->GetTextureResourcesArray(),
		pSkyDome_->GetApexColor(),
		pSkyDome_->GetCenterColor());
	COM_ERROR_IF_FALSE(result, "can't render the sky dome");

	renderCount++;   // since this model was rendered then increase the count for this frame

	return;
}

// render sky plane (clouds)
void ZoneClass::RenderSkyPlane(int & renderCount, D3DClass* pD3D)
{
	// we use the camera position to setup a position of the sky plane
	DirectX::XMFLOAT3 cameraPosition{ pEditorCamera_->GetPositionFloat3() };  

	// translate the sky dome to be centered around the camera position
	pSkyPlane_->GetModelDataObj()->SetPosition(cameraPosition.x, cameraPosition.y, cameraPosition.z);


	// enabled additive blending so the clouds blend with the sky dome colour
	pD3D->TurnOnAlphaBlendingForSkyPlane();


	// do some sky plane calculations
	pSkyPlane_->Frame(deltaTime_);

	// prepare model's vertex/index buffers for rendering
	pSkyPlane_->Render(pD3D->GetDeviceContext());

	// get clouds' translation data
	float* pCloudTranslationData = pSkyPlane_->GetTranslationData();

	// render the sky plane using the sky plane shader
	pSkyPlaneShader_->Render(pD3D->GetDeviceContext(),
		pSkyPlane_->GetModelDataObj()->GetIndexCount(),
		pSkyPlane_->GetModelDataObj()->GetWorldMatrix(),
		pEditorCamera_->GetViewMatrix(),
		pEditorCamera_->GetProjectionMatrix(),
		pSkyPlane_->GetTextureArray()->GetTextureResourcesArray(),
		pCloudTranslationData[0],
		pCloudTranslationData[1],
		pCloudTranslationData[2],
		pCloudTranslationData[3],
		pSkyPlane_->GetBrightness());

	renderCount++;   // since this model was rendered then increase the count for this frame

	return;
}