////////////////////////////////////////////////////////////////////
// Filename::    ZoneClass.cpp
// Description:  impementation of the ZoneClass functional;
//
// Created:      10.03.23
////////////////////////////////////////////////////////////////////
#include "ZoneClass.h"

ZoneClass::ZoneClass()
{
	try
	{
		pCamera_ = new EditorCamera();    // create the editor camera object
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

bool ZoneClass::Initialize(SETTINGS::settingsParams* settingsList)
{
	Log::Print("----------- ZONE CLASS: INITIALIZATION: THE CAMERA --------------");
	Log::Debug(THIS_FUNC_EMPTY);

	float windowWidth = static_cast<float>(settingsList->WINDOW_WIDTH);
	float windowHeight = static_cast<float>(settingsList->WINDOW_HEIGHT);
	float aspectRatio = windowWidth / windowHeight;


	// set up the EditorCamera object
	pCamera_->SetPosition({ 0.0f, 0.0f, -3.0f });
	pCamera_->SetProjectionValues(settingsList->FOV_DEGREES, aspectRatio, settingsList->NEAR_Z, settingsList->FAR_Z);

	// set the rendering of the bounding box around each terrain cell
	showCellLines_ = true;

	// initialize the frustum object
	pFrustum_->Initialize(settingsList->FAR_Z);

	return true;
}



// renders models which are related to the terrain
void ZoneClass::Render(const std::map<std::string, ModelClass*> & modelsList,
	int & renderCount,
	D3DClass* pD3D)
{
	DirectX::XMMATRIX projectionMatrix;

	// get the projection matrix from the D3DClass object
	pD3D->GetProjectionMatrix(projectionMatrix);

	// construct the frustum
	pFrustum_->ConstructFrustum(projectionMatrix, pCamera_->GetViewMatrix());

	// before rendering of any other models we must render the sky dome
	auto modelsListIterator = modelsList.find("sky_dome");
	if (modelsListIterator == modelsList.end())
		COM_ERROR_IF_FALSE(false, "can't find the sky dome model in the models list");

	this->RenderSkyDome(modelsListIterator->second, renderCount, pD3D);


	// then we render the terrain
	modelsListIterator = modelsList.find("terrain");
	if (modelsListIterator == modelsList.end())
		COM_ERROR_IF_FALSE(false, "can't find the sky dome model in the models list");

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
		std::string debugMsg{ "" };
		debugMsg = (showCellLines_) ? "show cell lines" : "don't show cell lines";
		Log::Debug(THIS_FUNC, debugMsg.c_str());
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

void ZoneClass::RenderTerrain(ModelClass* pTerrain, int & renderCount, D3DClass* pD3D, FrustumClass* pFrustum)
{
	TerrainClass* pTerrainModel = static_cast<TerrainClass*>(pTerrain);
	bool result = false;

	//pTerrain->SetPosition(-256 / 2, -10.0f, -256 / 2);   // move the terrain to the location it should be rendered at

	pTerrainModel->Frame();

	// render the terrain cells (and cell lines if needed)
	for (UINT i = 0; i < pTerrainModel->GetCellCount(); i++)
	{
		// render the terrain cell buffers 
		result = pTerrainModel->Render(pD3D->GetDeviceContext(), i, pFrustum);

		// if needed then render the bounding box around this terrain cell using the colour shader
		if (showCellLines_)
		{
			pTerrainModel->RenderCellLines(pD3D->GetDeviceContext(), i);
		}
	}

	renderCount++;            // since this model was rendered then increase the count for this frame

	return;
}

void ZoneClass::RenderSkyDome(ModelClass* pSkyDome, int & renderCount, D3DClass* pD3D)
{
	DirectX::XMMATRIX worldMatrix;   // a world matrix for the sky dome model
	DirectX::XMFLOAT3 cameraPosition{ GetCamera()->GetPositionFloat3() };

	// before rendering the sky dome we turn off both back face culling and the Z buffer.
	// Then we use the camera position to create a world matrix centered around the camera
	pD3D->SetRenderState(D3DClass::RASTER_PARAMS::CULL_MODE_FRONT);
	pD3D->TurnZBufferOff();

	// translate the sky dome to be centered around the camera position
	worldMatrix = XMMatrixTranslation(cameraPosition.x, cameraPosition.y, cameraPosition.z);
	pSkyDome->SetPosition(cameraPosition.x, cameraPosition.y, cameraPosition.z);

	// render the sky dome using the sky dome the sky dome shader
	pSkyDome->Render(pD3D->GetDeviceContext());

	// turn the Z buffer back and back face culling on
	pD3D->TurnZBufferOn();
	pD3D->SetRenderState(D3DClass::RASTER_PARAMS::CULL_MODE_BACK);

	renderCount++;            // since this model was rendered then increase the count for this frame

	return;
}
