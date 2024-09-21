////////////////////////////////////////////////////////////////////////////////////////////
// Filename::    ZoneClass.cpp
// Description:  impementation of the ZoneClass functional;
//
// Created:      10.03.23
////////////////////////////////////////////////////////////////////////////////////////////
#include "ZoneClass.h"


//////////////////////////////////
// INCLUDES FOR INTERNAL NEEDS
//////////////////////////////////


ZoneClass::ZoneClass()
{
}

ZoneClass::~ZoneClass()
{
	Log::Debug();
}


////////////////////////////////////////////////////////////////////////////////////////////
//
//                                 PUBLIC FUNCTIONS
//
////////////////////////////////////////////////////////////////////////////////////////////

bool ZoneClass::Initialize(
	CameraClass & editorCamera,
	const float farZ,                  // screen depth
	const float cameraHeightOffset)    // the offset of the camera above the terrain
{
	Log::Print("-------------  ZONE CLASS: INITIALIZATION  ---------------");
	Log::Debug();

	try
	{
		// set the height of the camera above the terrain
		cameraHeightOffset_ = cameraHeightOffset;

		// set the rendering of the bounding box around each terrain cell
		showCellLines_ = true;  

		// set the user locked to the terrain height for movement
		heightLocked_ = true;   

		// initialize the frustum object
		editorFrustum_.Initialize(farZ);

		// ---------------------------------------------------- //

	}
	catch (EngineException & e)
	{
		Log::Error(e, true);
		Log::Error("can't initialize the zone class object");
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////

bool ZoneClass::Render(D3DClass* pD3D,
	CameraClass & editorCamera,
	const float deltaTime,
	const float timerValue)
{
	// renders models which are related to the terrain 
	// (terrain plane, sky dome/box, sky plane (clouds));
	//
	// NOTE: we've already set some data into the data_for_shaders_container  
	//       during RenderGraphics::RenderModels() execution

	try
	{
		// update the delta time value (time between frames)
		deltaTime_ = deltaTime;

		// update the value of the local timer
		localTimer_ = timerValue;

		// construct the frustum for this frame
		editorFrustum_.ConstructFrustum(editorCamera.GetProjectionMatrix(), editorCamera.GetViewMatrix());

		// render the sky dome (or sky box) and the sky plane (clouds)
	   // RenderSkyElements(pD3D);

		// render terrain
		//RenderTerrainElements();
		
	}
	catch (EngineException & e)
	{
		Log::Error(e, false);
		Log::Error("can't render");
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////

void ZoneClass::HandleMovementInput(
	EditorCamera & editorCamera,
	const KeyboardEvent& kbe, 
	const float deltaTime)
{
	// handle events from the keyboard

	// after the frame time update the position movement functions can be updated
	// with the current state of the input devices. The movement function will update
	// the position of the camera to the location for this frame
	editorCamera.HandleKeyboardEvents(kbe, deltaTime);

	// handle keyboard input to control the zone state (state of the camera, terrain, etc.)
	HandleZoneControlInput(kbe);

	return;
}

///////////////////////////////////////////////////////////

void ZoneClass::HandleMovementInput(
	EditorCamera & editorCamera,
	const MouseEvent& me,
	const float deltaTime)
{
	// get the delta values of x and y
	const MousePoint mPoint = me.GetPos();

	// update the rotation data of the camera
	// with the current state of the input devices. The movement function will update
	// the position of the camera to the location for this frame
	editorCamera.HandleMouseMovement(mPoint.x, mPoint.y, deltaTime);

	return;
}

///////////////////////////////////////////////////////////







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
#if 0
///////////////////////////////////////////////////////////

void ZoneClass::RenderSkyElements(D3DClass* pD3D)
{
	// ---------------------------- SKY DOME RENDERING ---------------------------------- //

	// before rendering the sky elements we turn off both back face culling and the Z buffer.
	pD3D->SetRenderState(D3DClass::RASTER_PARAMS::CULL_MODE_FRONT);
	pD3D->TurnZBufferOff();

	// before rendering of any other models (at all) we must render the sky dome
	this->RenderSkyDome();

	// turn back face culling back on
	pD3D->SetRenderState(D3DClass::RASTER_PARAMS::CULL_MODE_BACK);

	// ---------------------------- SKY PLANE RENDERING --------------------------------- //

	// enabled additive blending so the clouds (sky plane) blend with the sky dome colour
	pD3D->TurnOnAlphaBlendingForSkyPlane();

	// render the sky plane onto the scene
	this->RenderSkyPlane();

	// after rendering the sky elements we turn off alpha blending
	// and turn on the Z buffer back and back face culling
	pD3D->TurnOffAlphaBlending();
	pD3D->TurnZBufferOn();

	return;
}

///////////////////////////////////////////////////////////

void ZoneClass::RenderTerrainElements()
{
	// render the terrain
	this->RenderTerrainPlane();

	return;
}

///////////////////////////////////////////////////////////

void ZoneClass::RenderTerrainPlane()
{
	TerrainClass* pTerrainModel = static_cast<TerrainClass*>(pTerrainGameObj_->GetModel());

	// do some terrain model calculations
	pTerrainModel->Update();

	// each frame we use the updated position as input to determine the height the camera
	// should be located at. We then set the height of the camera slightly above the 
	// terrain height by 1.0f;
	// if the height is locked to the terrain then position of the camera on top of it
	if (heightLocked_)
	{
		float height = 0.0f;                // current terrain height
	
		// get the height of the triangle that is directly underbneath the given camera position
		pTerrainModel->GetHeightAtPosition(pDataForShaders_->cameraPos.x, pDataForShaders_->cameraPos.z, height);

		// the camera's position is just above the terrain's triangle by some height value
		pEditorCamera_->SetPosition(pDataForShaders_->cameraPos.x, height + cameraHeightOffset_, pDataForShaders_->cameraPos.z);

		// update the data about the camera current position
		pDataForShaders_->cameraPos = pEditorCamera_->GetPosition();
	}

	// ---------------------------------------------------- //


	// render the terrain cells (and cell lines if needed)
	for (UINT i = 0; i < pTerrainModel->GetCellCount(); i++)
	{
		// define if we see a terrain cell by the camera if so
		// we render this terrain cell by particular index using the shader
		if (pTerrainModel->CheckIfSeeCellByIndex(i, editorFrustum_))
		{
			// render a terrain cell onto the screen
			pTerrainModel->RenderCellByIndex(i);

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

void ZoneClass::RenderSkyDome()
{
	// render sky dome (colors or textures of the sky)

	// if we didn't initialize a sky dome (or sky box) we can't render it
	// so we just go out
	if (pSkyDomeGameObj_ == nullptr)
		return;


	SkyDomeClass* pSkyDomeModel = static_cast<SkyDomeClass*>(pSkyDomeGameObj_->GetModel());

	// ---------------------------------------------------- //

	// translate the sky dome to be centered around the camera position
	pSkyDomeGameObj_->SetPosition(pDataForShaders_->cameraPos);

	// setup some data about sky dome (or sky box) which we will use for rendering this frame
	pDataForShaders_->skyDomeApexColor = pSkyDomeModel->GetApexColor();
	pDataForShaders_->skyDomeCenterColor = pSkyDomeModel->GetCenterColor();

	// ---------------------------------------------------- //

	// prepare model's vertex/index buffers for rendering and
	// render the sky dome using the sky dome shader
	pSkyDomeGameObj_->Render();

	return;
}

///////////////////////////////////////////////////////////

void ZoneClass::RenderSkyPlane()
{

	// render sky plane (clouds)

	// if we didn't initialize a sky plane we can't render it
	// so we just go out
	if (pSkyPlaneGameObj_ == nullptr)
		return;

	// ---------------------------------------------------- //

	SkyPlaneClass* pSkyPlaneModel = static_cast<SkyPlaneClass*>(pSkyPlaneGameObj_->GetModel());

	// translate the sky dome to be centered around the camera position
	pSkyPlaneGameObj_->SetPosition(pDataForShaders_->cameraPos);

	// do some sky plane calculations
	pSkyPlaneModel->Update(deltaTime_);

	// get clouds' translation data
	const std::vector<float> & translationData = pSkyPlaneModel->GetTranslationData();

	// setup data container for the shader before rendering of the sky plane:
	//    here we have two cloud textures translations by X-axis and Z-axis
	//    (first: x,y)(second: z,w)
	pDataForShaders_->skyPlanesTranslation.x = translationData[0];  // first cloud X translation
	pDataForShaders_->skyPlanesTranslation.y = translationData[1];  // 
	pDataForShaders_->skyPlanesTranslation.z = translationData[2];
	pDataForShaders_->skyPlanesTranslation.w = translationData[3];
	pDataForShaders_->skyPlanesBrightness = pSkyPlaneModel->GetBrightness();

	// render the sky plane using the sky plane shader
	pSkyPlaneGameObj_->Render();

	return;

}

///////////////////////////////////////////////////////////

#endif