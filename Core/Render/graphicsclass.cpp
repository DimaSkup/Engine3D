////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
// Revising: 14.10.22
////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"


#include "../Common/Assert.h"
#include "../Common/MathHelper.h"
#include "../Common/Utils.h"

// ImGui stuff
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include <random>


using namespace DirectX;

GraphicsClass::GraphicsClass() 
{
	Log::Debug();

	try
	{	
		// get a pointer to the engine settings class
		pIntersectionWithGameObjects_ = new IntersectionWithGameObjects();             // execution of picking of some model
	}
	catch (std::bad_alloc& e)
	{
		Shutdown();
		Log::Error(e.what());
		throw EngineException("can't allocate memory for the member of the GraphicsClass");
	}
}

// the class destructor
GraphicsClass::~GraphicsClass() 
{
	Log::Debug("start of destroying");
	Shutdown();
	Log::Debug("is destroyed");
}




// ---------------------------------------------------------------------------------
//
//                             PUBLIC METHODS
//
// ---------------------------------------------------------------------------------


bool GraphicsClass::Initialize(HWND hwnd, SystemState & systemState)
{
	// Initializes all the main parts of graphics rendering module

	try
	{
		InitializeGraphics initGraphics;
		Settings& settings = engineSettings_;
		bool result = false;

		Log::Print("------------------------------------------------------------");
		Log::Print("              INITIALIZATION: GRAPHICS SYSTEM               ");
		Log::Print("------------------------------------------------------------");


		result = initGraphics.InitializeDirectX(d3d_, hwnd, settings);
		Assert::True(result, "can't initialize D3DClass");

		// after initialization of the DirectX we can use pointers to the device and device context
		d3d_.GetDeviceAndDeviceContext(pDevice_, pDeviceContext_);

		InitCamerasHelper(initGraphics, settings);
		InitSceneHelper(initGraphics, settings);
		InitGuiHelper(initGraphics, settings);
		
		// create frustums for frustum culling
		frustums_.push_back(BoundingFrustum());  // editor camera

		// matrix for 2D rendering
		WVO_ = worldMatrix_ * baseViewMatrix_ * d3d_.GetOrthoMatrix();

		render_.Initialize(pDevice_, pDeviceContext_, WVO_);
	}
	catch (EngineException & e)
	{
		Log::Error(e, true);
		Log::Error("can't initialize the graphics class");
		return false;
	}
	

	Log::Print(" is successfully initialized");
	return true;
}

///////////////////////////////////////////////////////////


void GraphicsClass::Shutdown()
{
	// Shutdowns all the graphics rendering parts, releases the memory
	Log::Debug();
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

	const DirectX::XMMATRIX& viewMatrix = editorCamera.GetViewMatrix();  // update the view matrix for this frame
	const DirectX::XMMATRIX& projMatrix = editorCamera.GetProjectionMatrix(); // update the projection matrix
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
	entityMgr_.lightSystem_.UpdateSpotLights(cameraPos, cameraDir);
	
#if 0
	// ----------------------------------------------------
	// update params for rendering for this frame
	RenderGraphics::RenderParams& params = renderGraphics_.params_;

	params.proj      = editorCamera_.GetProjectionMatrix();
	params.viewProj  = viewProj_;                // view * projection
	params.cameraPos = cameraPos;
	params.cameraDir = cameraDir;                // the direction where the camera is looking at

	params.deltaTime     = deltaTime;            // time passed since the previous frame
	params.totalGameTime = totalGameTime;        // time passed since the start of the application


	// build the frustum from the projection matrix in view space.
	BoundingFrustum::CreateFromMatrix(frustums_[0], params.proj);

	// perform frustum culling on all of our entities
	//ComputeFrustumCulling(sysState);
#endif


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

		Render3D();

		// RENDER 2D STUFF
		d3d_.TurnZBufferOff();        // turn off the Z-buffer and enable alpha blending to begin 2D rendering
		d3d_.TurnOnBlending(RenderStates::STATES::ALPHA_ENABLE);
		d3d_.TurnOnRSfor2Drendering();

		userInterface_.Render(
			pDeviceContext_,
			entityMgr_, 
			render_.GetShadersContainer().fontShader_);

		d3d_.TurnOffRSfor2Drendering();
		d3d_.TurnOffBlending();  // turn off alpha blending now that the text has been rendered
		d3d_.TurnZBufferOn();    // turn the Z buffer back on now that the 2D rendering has completed

		// render ImGui stuff onto the screen
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		// Show the rendered scene on the screen
		d3d_.EndScene();
	}
	catch (EngineException& e)
	{
		Log::Error(e, true);
		throw EngineException("can't render a frame");
	}
}

///////////////////////////////////////////////////////////

void GraphicsClass::ComputeFrustumCulling(SystemState& sysState)
{

	const bool frustumCullingEnabled = true;
	
	ECS::EntityManager& mgr = entityMgr_;

	sysState.visibleObjectsCount = 0;
	mgr.renderSystem_.ClearVisibleEntts();

	if (frustumCullingEnabled)
	{
		XMVECTOR detView = XMMatrixDeterminant(editorCamera_.GetViewMatrix());
		XMMATRIX invView = XMMatrixInverse(&detView, editorCamera_.GetViewMatrix());

		// get transformation data of each available entity
		const std::vector<XMMATRIX>& worlds = mgr.GetWorldComponent().worlds_;
		Assert::True(worlds.size() == mgr.ids_.size(), "the number of world matrices and the number of entitites must be equal");

		std::vector<XMFLOAT3>  positions;
		std::vector<XMVECTOR>  dirQuats;
		std::vector<float>     uniScales;
		std::vector<ptrdiff_t> dataIdxs;

		entityMgr_.transformSystem_.GetTransformDataOfEntts(
			mgr.ids_,
			dataIdxs,
			positions,
			dirQuats,
			uniScales);

		

		// go through each entity and define if it is visible
		for (ptrdiff_t idx = 0; idx < std::ssize(mgr.ids_); ++idx)
		{
			XMVECTOR detWorld = XMMatrixDeterminant(worlds[idx]);
			XMMATRIX invWorld = XMMatrixInverse(&detWorld, worlds[idx]);

			// view space to the objects's local space
			XMMATRIX toLocal = XMMatrixMultiply(invView, invWorld);

			// transform the camera frustum from view space to the object's local space
			DirectX::BoundingFrustum localspaceFrustum;
		
			//bool isUnit = DirectX::Internal::XMQuaternionIsUnit(DirectX::XMQuaternionNormalize(dirQuats[idx]));
			bool isUnit = true;

			if (!isUnit)
			{
				EntityName name = mgr.nameSystem_.GetNameById(mgr.ids_[idx]);
				int i = 0;
				++i;
			}

			frustums_[0].Transform(
				localspaceFrustum,
				uniScales[idx],
				dirQuats[idx],
				XMLoadFloat3(&positions[idx]));

			// perform the box/frustum intersection test in local space
			//if (localspaceFrustum.Intersects()
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
		render_.GetLightShader().SetDirLightsCount(pDeviceContext_, 0);

	else if (GetAsyncKeyState('1') & 0x8000)
		render_.GetLightShader().SetDirLightsCount(pDeviceContext_, 1);

	else if (GetAsyncKeyState('2') & 0x8000)
		render_.GetLightShader().SetDirLightsCount(pDeviceContext_, 2);

	else if (GetAsyncKeyState('3') & 0x8000)
		render_.GetLightShader().SetDirLightsCount(pDeviceContext_, 3);
		
	
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

				Log::Debug("F2 key is pressed");
				break;
			}
			case VK_F3:
			{
				// change the rendering cull mode
				if (prevKeyCode != VK_F3) 
					ChangeCullMode();

				Log::Debug("F3 key is pressed");
				break;
			}
			case KEY_N:
			{
				// turn on/off the normals debugging
				if (prevKeyCode != KEY_N)
					render_.GetLightShader().EnableDisableDebugNormals(pDeviceContext_);

				Log::Debug("key N is pressed");
				break;
			}
			case KEY_T:
			{
				// turn on/off the tangents debugging
				if (prevKeyCode != KEY_T)
					render_.GetLightShader().EnableDisableDebugTangents(pDeviceContext_);

				Log::Debug("key T is pressed");
				break;
			}
			case KEY_B:
			{
				// turn on/off the binormals debugging
				if (prevKeyCode != KEY_B)
					render_.GetLightShader().EnableDisableDebugBinormals(pDeviceContext_);

				Log::Debug("key B is pressed");
				break;
			}
			case KEY_L:
			{
				// turn on/off flashlight
				if (prevKeyCode != KEY_L)
					render_.GetLightShader().ChangeFlashLightState(pDeviceContext_);

				Log::Debug("key L is pressed");
				break;
			}
			case KEY_Y:
			{
				// change (decrease) flashlight radius
				//entityMgr_.light_.spotLights_.data_[0].spot_ += 1.0f;
				break;
			}
			case KEY_U:
			{
				// change (increase) flashlight radius
				//entityMgr_.light_.spotLights_.data_[0].spot_ -= 1.0f;
				break;
			}
			case KEY_H:
			{
				// turn on/off the fog effect
				if (prevKeyCode != KEY_H)
					render_.GetLightShader().EnableDisableFogEffect(pDeviceContext_);
				
				Log::Debug("key H is pressed");
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

	return;
}

///////////////////////////////////////////////////////////

void GraphicsClass::ChangeModelFillMode()
{
	// toggling on / toggling off the fill mode for the models

	using enum RenderStates::STATES;

	isWireframeMode_ = !isWireframeMode_;
	RenderStates::STATES fillParam = (isWireframeMode_) ? FILL_MODE_WIREFRAME : FILL_MODE_SOLID;

	d3d_.SetRasterState(fillParam);
};

void GraphicsClass::ChangeCullMode()
{
	// toggling on and toggling off the cull mode for the models

	using enum RenderStates::STATES;

	isCullBackMode_ = !isCullBackMode_;
	RenderStates::STATES cullParam = (isCullBackMode_) ? CULL_MODE_BACK : CULL_MODE_FRONT;

	d3d_.SetRasterState(cullParam);
}

///////////////////////////////////////////////////////////

// memory allocation and releasing
void* GraphicsClass::operator new(size_t i)
{
	if (void* ptr = _aligned_malloc(i, 16))
		return ptr;

	Log::Error("can't allocate memory for this object");
	throw std::bad_alloc{};
}

///////////////////////////////////////////////////////////

void GraphicsClass::operator delete(void* ptr)
{
	_aligned_free(ptr);
}


// ************************************************************************************
// 
//                               PRIVATE HELPERS
// 
// ************************************************************************************

void GraphicsClass::InitCamerasHelper(InitializeGraphics& init, Settings& settings)
{
	bool result = init.InitializeCameras(
		editorCamera_,
		cameraForRenderToTexture_,
		baseViewMatrix_,           // init the base view matrix which is used for 2D rendering
		settings);
	Assert::True(result, "can't initialize cameras / view matrices");
}

///////////////////////////////////////////////////////////

void GraphicsClass::InitSceneHelper(InitializeGraphics& init, Settings& settings)
{
	// initializer the textures container
	textureManager_.Initialize(pDevice_);

	// initialize scene objects: cubes, spheres, trees, etc.
	bool result = init.InitializeScene(
		d3d_,
		entityMgr_,
		meshStorage_,
		settings,
		renderToTexture_,
		pDevice_,
		pDeviceContext_);
	Assert::True(result, "can't initialize the scene elements (models, etc.)");
}

///////////////////////////////////////////////////////////

void GraphicsClass::InitGuiHelper(InitializeGraphics& init, Settings& settings)
{
	// initialize the GUI of the game/engine (interface elements, debug text, etc.)
	bool result = init.InitializeGUI(d3d_, userInterface_, settings);
	Assert::True(result, "can't initialize the GUI");
}

///////////////////////////////////////////////////////////

void GraphicsClass::Render3D()
{
	//
	// this function prepares and renders all the visible models onto the screen
	//

	try
	{

	// ---------------------------------------------
	// Update data for this frame

	DirectX::XMFLOAT3 cameraPos;
	editorCamera_.GetPositionFloat3(cameraPos);

	std::vector<Render::DirLight> dirLightsForRender;
	std::vector<Render::PointLight> pointLightsForRender;
	std::vector<Render::SpotLight> spotLightsForRender;

	SetupLightsForFrame(
		entityMgr_.lightSystem_,
		dirLightsForRender,
		pointLightsForRender,
		spotLightsForRender);
	
	// update lighting data, camera pos, etc. for this frame
	render_.UpdatePerFrame(
		pDeviceContext_,
		viewProj_,
		cameraPos,
		dirLightsForRender,
		pointLightsForRender,
		spotLightsForRender);

	dirLightsForRender.clear();
	pointLightsForRender.clear();
	spotLightsForRender.clear();


	// ---------------------------------------------
	// prepare all the visible entities for rendering


	std::vector<EntityID> visibleEntts;// = entityMgr.GetAllEnttsIDs();
	ECS::RenderStatesSystem::RenderStatesData rsDataToRender;

	// TEMPORARY (RENDER ALL THE ENTITIES):
	// currently we don't have any frustum culling so just
	// render all the entitites which have the Rendered component
	entityMgr_.renderSystem_.GetAllEnttsIDs(visibleEntts);

	// separate entts into opaque and blended
	entityMgr_.renderStatesSystem_.GetRenderStates(visibleEntts, rsDataToRender);

	// render entts with no blending, no alpha clipping, etc, just fill_solid, cull_back
	RenderEntts(rsDataToRender.enttsWithDefaultStates_);

	// render entts with default render states but also with alpha clipping
	render_.GetShadersContainer().lightShader_.SetAlphaClipping(pDeviceContext_, true);
	RenderEntts(rsDataToRender.enttsOnlyWithAlphaClipping_);
	render_.GetShadersContainer().lightShader_.SetAlphaClipping(pDeviceContext_, false);

	u32 startInstanceIdx = 0;

	// go through each blending state and render responsible entts
	for (size idx = 0; idx < std::ssize(rsDataToRender.blendingStates_); ++idx)
	{
		const ECS::RENDER_STATES bs = rsDataToRender.blendingStates_[idx];
		const u32 instancesCount = rsDataToRender.instancesPerBlendingState_[idx];

		std::vector<EntityID> enttsToRender;
		std::vector<bool> alphaClippingFlagsToRender;
		
		CoreUtils::GetRangeOfArr(
			rsDataToRender.enttsWithBlending_,
			startInstanceIdx,
			startInstanceIdx + instancesCount,
			enttsToRender);
#if 0
		CoreUtils::GetRangeOfArr(
			alphaClippingFlags,
			startInstanceIdx,
			startInstanceIdx + instancesCount,
			alphaClippingFlagsToRender);
#endif
		d3d_.TurnOnBlending(RenderStates::STATES(bs));

		RenderEntts(enttsToRender);

		startInstanceIdx += instancesCount;
	}

#if 0
	// rendering of blended models
	for (size idx = 0; idx < std::ssize(enttsWithBlending); ++idx)
	{
		
		d3d_.TurnOnBlending(RenderStates::STATES(*blendStates[idx].begin()));

		const EntityID id = enttsWithBlending[idx];
		const EntityName& name = entityMgr_.nameSystem_.GetNameById(id);
	
		if (name == "wireFence")
		{
			uint8_t prevRasterStateHash = d3d_.GetRenderStates().GetCurrentRSHash();
			d3d_.SetRasterState(RenderStates::CULL_MODE_BACK);

			RenderEntts({ id });

			d3d_.GetRenderStates().SetRasterStateByHash(pDeviceContext_, prevRasterStateHash);

		}
		else
		{
			RenderEntts({ id });
		}
	}
#endif

	// turn off blending after rendering of all the visible blended entities
	d3d_.TurnOffBlending();

	}
	catch (EngineException& e)
	{
		Log::Error(e);
		Log::Error("can't render 3D entts onto the scene");
	}
}

///////////////////////////////////////////////////////////

void GraphicsClass::RenderEntts(const std::vector<EntityID>& enttsIDs)
{
	try
	{
		TextureManager* pTexMgr = TextureManager::Get();
		MeshStorage* pMeshStorage = MeshStorage::Get();
		ECS::EntityManager& enttMgr = entityMgr_;

		Mesh::DataForRendering meshesData;   // for rendering

		
		std::vector<DirectX::XMMATRIX> worldMatrices;
		std::vector<ECS::RENDERING_SHADERS> shaderTypes;
		std::vector<MeshID> meshesIDsToRender;

		std::vector<EntityID> enttsSortedByMeshes;
		std::vector<size> numInstancesPerMesh;

		// prepare entts data for rendering
		enttMgr.transformSystem_.GetWorldMatricesOfEntts(enttsIDs, worldMatrices);
		enttMgr.renderSystem_.GetRenderingDataOfEntts(enttsIDs, shaderTypes);

		enttMgr.meshSystem_.GetMeshesIDsRelatedToEntts(
			enttsIDs,
			meshesIDsToRender,     // arr of meshes IDs which will be rendered
			enttsSortedByMeshes,
			numInstancesPerMesh);

		// prepare meshes data for rendering
		pMeshStorage->GetMeshesDataForRendering(meshesIDsToRender, meshesData);

		// ------------------------------------------------------
		// go through each mesh and render it

		size startInstanceLocation = 0;

		for (size_t idx = 0; idx < meshesIDsToRender.size(); ++idx)
		{
			// entities which are related to the current mesh
			std::vector<EntityID> relatedEntts = 
			{
				enttsSortedByMeshes.begin() + startInstanceLocation,
				enttsSortedByMeshes.begin() + startInstanceLocation + numInstancesPerMesh[idx]
			};

			startInstanceLocation += numInstancesPerMesh[idx];

			MeshName meshName = meshesData.names_[idx];

			std::vector<DirectX::XMMATRIX> worldMatricesToRender;
			std::vector<DirectX::XMMATRIX> texTransforms;


			// get SRV (shader resource view) of each texture of the mesh and
			// entities which have the Textured component (own textures)
			SRVsArr texSRVs;
			std::vector<u32> numInstances;

			GetTexturesSRVsForMeshAndEntts(
				relatedEntts,
				meshesData.texIDs_[idx],
				*pTexMgr,
				entityMgr_.texturesSystem_,
				texSRVs,
				numInstances);

			// get world matrices of entts related to this mesh
			GetEnttsWorldMatricesForRendering(
				enttsIDs,
				relatedEntts,
				worldMatrices,
				worldMatricesToRender);

			entityMgr_.texTransformSystem_.GetTexTransformsForEntts(
				relatedEntts,
				texTransforms);

			// prepare materials for each mesh instance
			const Mesh::Material& mat = meshesData.materials_[idx];
			Render::Material meshMaterial(mat.ambient_, mat.diffuse_, mat.specular_, mat.reflect_);
			std::vector<Render::Material> meshesMaterials(relatedEntts.size(), meshMaterial);

			render_.UpdateInstancedBuffer(
				pDeviceContext_,
				worldMatricesToRender,
				texTransforms,
				meshesMaterials);
		
			render_.RenderInstances(
				pDeviceContext_,
				meshesData.pVBs_[idx],
				meshesData.pIBs_[idx],
				texSRVs,
				numInstances,
				meshesData.indexCount_[idx],
				sizeof(VERTEX));
		

		}  // end for-loop through each mesh type
	}
	catch (EngineException& e)
	{
		Log::Error(e);
	}
}

///////////////////////////////////////////////////////////

void GraphicsClass::SetupLightsForFrame(
	const ECS::LightSystem& lightSys,
	std::vector<Render::DirLight>& outDirLights,
	std::vector<Render::PointLight>& outPointLights,
	std::vector<Render::SpotLight>& outSpotLights)
{
	const ECS::DirLights& dirLights = lightSys.GetDirLights();
	const ECS::PointLights& pointLights = lightSys.GetPointLights();
	const ECS::SpotLights& spotLights = lightSys.GetSpotLights();

	const size numDirLights = dirLights.GetCount();
	const size numPointLights = pointLights.GetCount();
	const size numSpotLights = spotLights.GetCount();

	outDirLights.resize(numDirLights);
	outPointLights.resize(numPointLights);
	outSpotLights.resize(numSpotLights);

	// --------------------------------

	for (size idx = 0; idx < numDirLights; ++idx)
	{
		outDirLights[idx].ambient_ = dirLights.data_[idx].ambient_;
		outDirLights[idx].diffuse_ = dirLights.data_[idx].diffuse_;
		outDirLights[idx].specular_ = dirLights.data_[idx].specular_;
		outDirLights[idx].direction_ = dirLights.data_[idx].direction_;
	}

	// --------------------------------

	for (size idx = 0; idx < numPointLights; ++idx)
	{
		const ECS::PointLight& srcLight = pointLights.data_[idx];

		outPointLights[idx].ambient_  = srcLight.ambient_;
		outPointLights[idx].diffuse_  = srcLight.diffuse_;
		outPointLights[idx].specular_ = srcLight.specular_;
		outPointLights[idx].position_ = srcLight.position_;
		outPointLights[idx].range_    = srcLight.range_;
		outPointLights[idx].att_      = srcLight.att_;
	}

	// --------------------------------

	for (size idx = 0; idx < numSpotLights; ++idx)
	{
		const ECS::SpotLight& srcLight = spotLights.data_[idx];

		outSpotLights[idx].ambient_   = srcLight.ambient_;
		outSpotLights[idx].diffuse_   = srcLight.diffuse_;
		outSpotLights[idx].specular_  = srcLight.specular_;
		outSpotLights[idx].position_  = srcLight.position_;
		outSpotLights[idx].range_     = srcLight.range_;
		outSpotLights[idx].direction_ = srcLight.direction_;
		outSpotLights[idx].spot_      = srcLight.spot_;
		outSpotLights[idx].att_       = srcLight.att_;
	}
}

///////////////////////////////////////////////////////////

void GraphicsClass::GetEnttsWorldMatricesForRendering(
	const std::vector<EntityID>& visibleEntts,
	const std::vector<EntityID>& enttsIDsToGetMatrices,
	const std::vector<DirectX::XMMATRIX>& inWorldMatrices,   // world matrices of all the currently visible entts
	std::vector<DirectX::XMMATRIX>& outWorldMatrices)
{
	std::vector<ptrdiff_t> dataIdxs;

	const size numMatricesToGet = std::ssize(enttsIDsToGetMatrices);
	outWorldMatrices.reserve(numMatricesToGet);
	dataIdxs.reserve(numMatricesToGet);

	// get data idxs of entts
	for (const EntityID& id : enttsIDsToGetMatrices)
		dataIdxs.push_back(Utils::GetIdxInSortedArr(visibleEntts, id));
		
	// get world matrices
	for (const ptrdiff_t idx : dataIdxs)
		outWorldMatrices.emplace_back(inWorldMatrices[idx]);
}

///////////////////////////////////////////////////////////

void GraphicsClass::PrepareTexturesSRV_ToRender(
	const std::vector<TextureClass*>& textures,
	std::vector<ID3D11ShaderResourceView* const*>& outTexturesSRVs)
{
	// get a bunch of pointers to SRVs (shader resource views) by input textures array

	outTexturesSRVs.reserve(textures.size());

	for (const TextureClass* pTexture : textures)
	{
		ID3D11ShaderResourceView* const* ppSRV = (pTexture) ? pTexture->GetTextureResourceViewAddress() : nullptr;
		outTexturesSRVs.push_back(ppSRV);
	}
}

///////////////////////////////////////////////////////////

void GraphicsClass::GetTexturesSRVsForMeshAndEntts(
	std::vector<EntityID>& inOutEnttsIds,
	const TexIDsArr& meshTexturesIDs,
	TextureManager& texMgr,
	ECS::TexturesSystem& texSys,
	SRVsArr& outTexSRVs,
	std::vector<u32>& outNumInstances)
{
	// get own textures of entities (if it has the Textured component)
	std::vector<TexID> enttsTexIDsArrays;
	std::vector<EntityID> enttsNoTexComp;           // use related mesh textures set
	std::vector<EntityID> enttsWithTexComp;         // use own textures set

	texSys.GetTexIDsByEnttsIDs(
		inOutEnttsIds,
		enttsNoTexComp,
		enttsWithTexComp,
		enttsTexIDsArrays);

	inOutEnttsIds.clear();
	Utils::AppendArray(inOutEnttsIds, enttsNoTexComp);
	Utils::AppendArray(inOutEnttsIds, enttsWithTexComp);

	bool meshTexIsNeeded = (bool)(std::ssize(enttsNoTexComp));

	// get arr of SRV of the current mesh
	// one set of mesh textures (if it is) + number of entities with own textures (by 1 per textures set)
	const size uniqueTexSets = meshTexIsNeeded + std::ssize(enttsWithTexComp);

	std::vector<TexID> texIDs;   // mesh textures IDs + entts textures IDs
	texIDs.reserve(uniqueTexSets * TextureClass::TEXTURE_TYPE_COUNT);

	// concatenate arrays of mesh textures IDs and THEN entts textures IDs
	if (meshTexIsNeeded)
		Utils::AppendArray(texIDs, meshTexturesIDs);


	Utils::AppendArray(texIDs, enttsTexIDsArrays);


	// --------------------------------
	// fill in out data

	texMgr.GetSRVsByTexIDs(texIDs, outTexSRVs);

	if (meshTexIsNeeded)
		outNumInstances.push_back((u32)std::ssize(enttsNoTexComp));

	// instances each with its own textures set
	std::vector<u32>instancesPerTexSet(std::ssize(enttsWithTexComp), 1);
	Utils::AppendArray(outNumInstances, instancesPerTexSet);
}

