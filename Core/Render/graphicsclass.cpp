////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
// Revising: 14.10.22
////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"


#include "../Common/Assert.h"
#include "../Common/MathHelper.h"
#include "../Common/Utils.h"

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

	// ---------------------------------------------

	// DIRTY HACK: update the camera height according to the terrain height function
	DirectX::XMFLOAT3 prevCamPos;
	editorCamera.GetPositionFloat3(prevCamPos);

	prevCamPos.y = 0.1f * (prevCamPos.z * sinf(0.1f * prevCamPos.x) +
		                   prevCamPos.x * cosf(0.1f * prevCamPos.z)) + 1.5f;

	//editorCamera.SetPosition({ prevCamPos.x, prevCamPos.y, prevCamPos.z });

	// ---------------------------------------------

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

	// update the entities and related data
	entityMgr_.Update(totalGameTime, deltaTime);
	entityMgr_.lightSystem_.UpdateSpotLights(cameraPos, cameraDir);
	
	// build the frustum from the projection matrix in view space.
	BoundingFrustum::CreateFromMatrix(frustums_[0], projMatrix);

	// perform frustum culling on all of our currently loaded entities
	ComputeFrustumCulling(sysState);

	// update user interface for this frame
	userInterface_.Update(pDeviceContext_, sysState);

	// Update shaders common data for this frame
	UpdateShadersDataPerFrame();

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

	// temp
	//mgr.renderSystem_.SetVisibleEntts(mgr.ids_);

#if 1
	//std::vector<EntityID> allIDs = mgr.ids_;
	

	if (frustumCullingEnabled)
	{
		const std::vector<EntityID> enttsRenderable = mgr.renderSystem_.GetAllEnttsIDs();
		const size renderEnttsCount = std::ssize(enttsRenderable);

		XMVECTOR detView = XMMatrixDeterminant(editorCamera_.GetViewMatrix());
		XMMATRIX invView = XMMatrixInverse(&detView, editorCamera_.GetViewMatrix());

		// get world matrix of each entity which is set as renderable
		std::vector<XMMATRIX> worlds;
		mgr.transformSystem_.GetWorldMatricesOfEntts(enttsRenderable, worlds);

		std::vector<XMMATRIX> invWorlds;
		std::vector<XMMATRIX> enttsLocal;

		invWorlds.reserve(renderEnttsCount);
		enttsLocal.reserve(renderEnttsCount);

		// precompute world inverse matrices for each renderable entt
		XMVECTOR detWorld;

		for (size idx = 0; idx < renderEnttsCount; ++idx)
			invWorlds.emplace_back(XMMatrixInverse(&detWorld, worlds[idx]));

		// precompute local space matrices for each renderable entt
		for (size idx = 0; idx < renderEnttsCount; ++idx)
			enttsLocal.emplace_back(XMMatrixMultiply(invView, invWorlds[idx]));

		// clear some arrs since we don't need already
		worlds.clear();
		invWorlds.clear();

		// get arr of AABB / bounding spheres for each renderable entt
		std::vector<DirectX::BoundingBox> AABBs;
		mgr.boundingSystem_.GetBoundingDataByIDs(enttsRenderable, AABBs);


		std::vector<EntityID> visibleEntts(std::ssize(enttsRenderable));
		u32 visibleEnttsCount = 0;


		// go through each entity and define if it is visible
		for (ptrdiff_t idx = 0; idx < renderEnttsCount; ++idx)
		{
			// decompose the matrix into its individual parts
			XMVECTOR scale;
			XMVECTOR dirQuat;
			XMVECTOR translation;
			XMMatrixDecompose(&scale, &dirQuat, &translation, enttsLocal[idx]);

			//EntityName name = mgr.nameSystem_.GetNameById(enttsRenderable[idx]);

			// transform the camera frustum from view space to the object's local space
			DirectX::BoundingFrustum localspaceFrustum;

			frustums_[0].Transform(
				localspaceFrustum,
				XMVectorGetX(scale),
				dirQuat,
				translation);

			DirectX::BoundingBox enttsBoundingBox;

			EntityID enttID = enttsRenderable[idx];

			
			// perform the box/frustum intersection test in local space
			if (localspaceFrustum.Intersects(AABBs[idx]))
			{
				visibleEntts[visibleEnttsCount++] = enttsRenderable[idx];
			}
		}

		visibleEntts.resize(visibleEnttsCount);
		mgr.renderSystem_.SetVisibleEntts(visibleEntts);
		sysState.visibleObjectsCount = visibleEnttsCount;
	}
#endif
	
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
					render_.GetLightShader().SwitchFogEffect(pDeviceContext_);
				
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
		case MouseEvent::EventType::Move:
		case MouseEvent::EventType::RAW_MOVE:
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
	RenderStates::STATES fillParam = (isWireframeMode_) ? FILL_WIREFRAME : FILL_SOLID;

	d3d_.SetRS(fillParam);
};

///////////////////////////////////////////////////////////

void GraphicsClass::ChangeCullMode()
{
	// toggling on and toggling off the cull mode for the models

	using enum RenderStates::STATES;

	isCullBackMode_ = !isCullBackMode_;
	RenderStates::STATES cullParam = (isCullBackMode_) ? CULL_BACK : CULL_FRONT;

	d3d_.SetRS(cullParam);
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

void GraphicsClass::UpdateShadersDataPerFrame()
{
	// Update shaders common data for this frame: 
	// viewProj matrix, camera position, light sources data, etc.

	Render::Render::PerFrameData& perFrameData = render_.perFrameData_;

	perFrameData.viewProj = DirectX::XMMatrixTranspose(viewProj_);
	editorCamera_.GetPositionFloat3(perFrameData.cameraPos);

	SetupLightsForFrame(
		entityMgr_.lightSystem_,
		perFrameData.dirLights,
		perFrameData.pointLights,
		perFrameData.spotLights);

	// update lighting data, camera pos, etc. for this frame
	render_.UpdatePerFrame(pDeviceContext_, perFrameData);
}

///////////////////////////////////////////////////////////

void GraphicsClass::ClearRenderingDataBeforeFrame()
{
	// clear rendering data from the previous frame / instances set

	for (auto& it : meshesData_)
	{
		it.second.Clear();
	}
	
	meshesData_.clear();
	render_.renderDataStorage_.Clear();
	rsDataToRender_.Clear();
}

///////////////////////////////////////////////////////////

void GraphicsClass::Render3D()
{
	//
	// this function prepares and renders all the visible models onto the screen
	//


	try
	{
	ClearRenderingDataBeforeFrame();
	

	// prepare all the visible entities for rendering
	const std::vector<EntityID>& visibleEntts = entityMgr_.renderSystem_.GetAllVisibleEntts();

	// separate entts into opaque and blended
	entityMgr_.renderStatesSystem_.GetRenderStates(visibleEntts, rsDataToRender_);

	// prepare data for each entts set
	PrepareEnttsDataForRendering(rsDataToRender_.enttsDefault_.ids_, "default_states");
	PrepareEnttsDataForRendering(rsDataToRender_.enttsAlphaClipping_.ids_, "alpha_clip_cull_none");
	PrepareEnttsDataForRendering(rsDataToRender_.enttsBlended_.ids_, "blending");
	//PrepareEnttsDataForRendering(rsDataToRender.enttsReflects_, "reflection_planes");

	// render as usual
	RenderEntts("default_states");
	RenderEntts("alpha_clip_cull_none");
	//RenderEntts("blending");

#if 0
	// render reflections of each entt
	RenderEnttsReflections(rsDataToRender.enttsWithDefaultStates_, "default_states");
	RenderEnttsReflections(rsDataToRender.enttsAlphaClippingAndCullModelNone_, "alpha_clip_cull_none");
	RenderEnttsReflections(rsDataToRender.enttsWithBlending_, "blending");

	//
	// draw the mirror to the back buffer as usual but with transparency blending
	// so the reflection shows through
	//
	RenderEntts(rsDataToRender.enttsReflects_, "reflection_planes");
	d3d_.GetRenderStates().ResetBS(pDeviceContext_);

	// render shadow of each entt
	//RenderEnttsShadows();

#endif
	

#if 0
	

	// render entts with default render states but also with alpha clipping
	render_.GetShadersContainer().lightShader_.SetAlphaClipping(pDeviceContext_, true);
	d3d_.SetRS(RenderStates::STATES::CULL_NONE);

	

	render_.GetShadersContainer().lightShader_.SetAlphaClipping(pDeviceContext_, false);
	d3d_.SetRS(RenderStates::STATES::CULL_BACK);

	// clear rendering data since we already don't need it
	meshesData_.Clear();
	render_.perFrameData_.Clear();
	render_.instanceBuffData_.Clear();
	render_.perInstanceData_.Clear();


	u32 startInstanceIdx = 0;

	// go through each blending state and render responsible entts
	for (size idx = 0; idx < std::ssize(rsDataToRender.blendingStates_); ++idx)
	{
		const ECS::RenderStates bs = rsDataToRender.blendingStates_[idx];
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

	// turn off blending after rendering of all the visible blended entities
	d3d_.TurnOffBlending();
#endif
#if 0


	//
	// render shadows
	//

	renderStates.SetDSS(pDeviceContext_, NO_DOUBLE_BLEND, 0);

	XMVECTOR shadowPlane{ 0, 1, 0, 0 }; // xz plane
	XMVECTOR toMainLight = -XMLoadFloat3(&render_.perFrameData_.dirLights[0].direction_);
	XMMATRIX S = DirectX::XMMatrixShadow(shadowPlane, toMainLight);
	XMMATRIX shadowOffsetY = DirectX::XMMatrixTranslation(0, 0.001f, 0);

	if (rsDataToRender.enttsWithDefaultStates_.size())
		RenderEnttsShadows(rsDataToRender.enttsWithDefaultStates_, S, shadowOffsetY);


	// restore default states
	renderStates.ResetRS(pDeviceContext_);
	renderStates.ResetBS(pDeviceContext_);
	renderStates.ResetDSS(pDeviceContext_);
#endif

	}
	catch (EngineException& e)
	{
		Log::Error(e);
		Log::Error("can't render 3D entts onto the scene");
	}
}

///////////////////////////////////////////////////////////

void GraphicsClass::PrepareEnttsDataForRendering(
	const std::vector<EntityID>& enttsIds,
	const std::string& enttsSetKey)
{
	// if we haven't any entts to get we just go out
	if (enttsIds.empty()) return;


	using InstanceBufferData = Render::Render::InstanceBufferData;
	using InstancesDataToRender = Render::Render::InstancesDataToRender;

	Render::Render::RenderDataStorage& storage = render_.renderDataStorage_;

	storage.instanceBuffData_[enttsSetKey];
	storage.perInstanceData_[enttsSetKey];
	storage.keys_.insert(enttsSetKey);
	meshesData_[enttsSetKey];

	// 1. data is used to fill in the instance buffer of shaders
	// 2. data per each set of instances (multiple instances but the same geometry, textures, etc)
	// 3. meshes data for this set of instances
	InstanceBufferData& instanceBuffData = render_.renderDataStorage_.instanceBuffData_.at(enttsSetKey);
	InstancesDataToRender& perInstanceData = render_.renderDataStorage_.perInstanceData_.at(enttsSetKey);
	Mesh::DataForRendering& meshesData = meshesData_.at(enttsSetKey);

	ECS::EntityManager& enttMgr = entityMgr_;
	std::vector<MeshID> meshesIDsToRender;
	std::vector<EntityID> enttsSortedByMeshes;

	// --------------------------------------------

	enttMgr.meshSystem_.GetMeshesIDsRelatedToEntts(
		enttsIds,
		meshesIDsToRender,     // arr of meshes IDs which will be rendered
		enttsSortedByMeshes,
		perInstanceData.numInstancesPerMesh);

	// prepare meshes data for rendering
	MeshStorage::Get()->GetMeshesDataForRendering(meshesIDsToRender, meshesData);

	// --------------------------------------------

	enttMgr.transformSystem_.GetWorldMatricesOfEntts(enttsSortedByMeshes, instanceBuffData.worlds);
	enttMgr.texTransformSystem_.GetTexTransformsForEntts(enttsSortedByMeshes, instanceBuffData.texTransforms);

	// TEMPORARY: 
	// prepare materials for each mesh instance (but not for each entt)
	for (size idx = 0; idx < std::ssize(meshesData.materials_); ++idx)
	{
		const ptrdiff_t instanceCount = perInstanceData.numInstancesPerMesh[idx];
		const Mesh::Material& mat = meshesData.materials_[idx];

		Render::Material meshMat(mat.ambient_, mat.diffuse_, mat.specular_, mat.reflect_);
		CoreUtils::AppendArray(instanceBuffData.meshesMaterials, std::vector<Render::Material>(instanceCount, meshMat));
	}

	// ------------------------------------------

	// prepare instances data for shaders
	std::vector<EntityID> enttsTextured;     // ids of entts which have the Textured component

	// get SRV (shader resource view) of each texture of the mesh and
	// entities which have the Textured component (own textures)
	GetTexSRVsForEntts(
		enttsSortedByMeshes,
		meshesData.texIDs_,
		std::ssize(perInstanceData.numInstancesPerMesh),
		perInstanceData.texturesSRVs,
		enttsTextured);

	GenInstancesTexSetData(
		enttsSortedByMeshes,
		enttsTextured,
		perInstanceData.numInstancesPerMesh,
		perInstanceData.enttsMaterialTexIdxs,
		perInstanceData.enttsPerTexSet,
		perInstanceData.numOfTexSet);

	perInstanceData.vertexSize = sizeof(Vertex3D);
}

///////////////////////////////////////////////////////////

void GraphicsClass::RenderEntts(const std::string& enttsSetKey)
{
	try
	{
		//
		// render entts with default render states or with alpha clipping + cull_none
		//

		const Render::Render::RenderDataStorage& storage = render_.renderDataStorage_;
	
		// if we haven't any entts to rendering by input key we just go out
		bool haveEntts = storage.keys_.contains(enttsSetKey);
		if (!haveEntts) return;

#if 1
		if (enttsSetKey == "default_states")
		{

			d3d_.GetRenderStates().ResetRS(pDeviceContext_);
			d3d_.GetRenderStates().ResetBS(pDeviceContext_);


			const Render::Render::InstanceBufferData instanceBuffData = storage.instanceBuffData_.at(enttsSetKey);
			const Render::Render::InstancesDataToRender& perInstanceData = storage.perInstanceData_.at(enttsSetKey);

			UpdateInstanceBuffAndRenderInstances(
				pDeviceContext_,
				instanceBuffData,
				perInstanceData,
				meshesData_.at(enttsSetKey));
		}
		else if (enttsSetKey == "alpha_clip_cull_none")
		{
			using enum RenderStates::STATES;
			d3d_.GetRenderStates().SetRS(pDeviceContext_, { CULL_NONE });
			render_.GetLightShader().SetAlphaClipping(pDeviceContext_, true);

			const Render::Render::InstanceBufferData instanceBuffData = storage.instanceBuffData_.at(enttsSetKey);
			const Render::Render::InstancesDataToRender& perInstanceData = storage.perInstanceData_.at(enttsSetKey);

			UpdateInstanceBuffAndRenderInstances(
				pDeviceContext_,
				instanceBuffData,
				perInstanceData,
				meshesData_.at(enttsSetKey));
		}
#endif

		
		


		
	

	}
	catch (EngineException& e)
	{
		Log::Error(e);
		Log::Error("can't render entts by key: " + enttsSetKey);
	}
}

///////////////////////////////////////////////////////////

void GraphicsClass::UpdateInstanceBuffAndRenderInstances(
	ID3D11DeviceContext* pDeviceContext,
	const Render::Render::InstanceBufferData& instanceBuffData,
	const Render::Render::InstancesDataToRender& perInstanceData,
	const Mesh::DataForRendering& meshesData)
{
	render_.UpdateInstancedBuffer(pDeviceContext_, instanceBuffData);

	// render prepared instances using shaders
	render_.RenderInstances(
		pDeviceContext_,
		perInstanceData,
		meshesData.pVBs_,
		meshesData.pIBs_,
		meshesData.indexCount_);
}

///////////////////////////////////////////////////////////
#if 0
void GraphicsClass::RenderEnttsReflections(const std::vector<EntityID>& enttsIds) 
{
	const std::vector<XMMATRIX>& origWorlds = render_.instanceBuffData_.worlds;
	const size instancesCount = std::ssize(origWorlds);
	std::vector<XMMATRIX> reflectedWorlds(instancesCount);


	// build reflection matrix to reflect the entts
	XMVECTOR mirrorPlane{ 0, 1, 0, 0 };               // xz plane
	XMMATRIX R = XMMatrixReflect(mirrorPlane);

	// compute reflected world matrices
	for (int idx = 0; const XMMATRIX& m : origWorlds)
		reflectedWorlds[idx] = m * R;

	// update the instance buffer with reflected world matrix of each entt
	render_.UpdateInstancedBufferWorlds(pDeviceContext_, reflectedWorlds);

	// ---------------------------------------------
	// render reflection planes (it can be water or mirror)

	using enum RenderStates::STATES;
	RenderStates& renderStates = d3d_.GetRenderStates();

	// prepare the Output Merget stage for rendering of the reflection plane (mirror)
	renderStates.SetBS(pDeviceContext_, NO_RENDER_TARGET_WRITES);
	renderStates.SetDSS(pDeviceContext_, MARK_MIRROR, 1);

	// get some entts IDs
	EntityID waterEnttID = entityMgr_.nameSystem_.GetIdByName("water");

	entityMgr_.meshSystem_.GetMeshesIDsRelatedToEntts(
		enttsIds,
		reflectionPlaneMeshID,     // arr of meshes IDs which will be rendered
		enttsSortedByMeshes,
		numInstancesPerMesh);
	MeshStorage::Get()->GetMeshesDataForRendering

	// render reflected entts
	render_.RenderInstances(
		pDeviceContext_,
		render_.perInstanceData_,
		meshesData_.pVBs_,
		meshesData_.pIBs_,
		meshesData_.indexCount_);

	// draw mirror
	RenderEntts({ waterEnttID });

	// restore states
	renderStates.ResetBS(pDeviceContext_);
	renderStates.ResetDSS(pDeviceContext_);

	// ------------------------------------------

	// reflect the light sources as well
	std::vector<Render::DirLight> reflectedDirLights = render_.perFrameData_.dirLights;
	const std::vector<ECS::DirLight>& dirLightsData = entityMgr_.lightSystem_.GetDirLights().data_;

	for (int i = 0; const ECS::DirLight & light : dirLightsData)
	{
		XMVECTOR direction = XMLoadFloat3(&light.direction_);
		XMVECTOR reflectedDirection = DirectX::XMVector3TransformNormal(direction, R);
		XMStoreFloat3(&reflectedDirLights[i].direction_, reflectedDirection);
	}

	render_.GetLightShader().SetDirLights(pDeviceContext_, reflectedDirLights);

	// reflection changes winding order, so cull clockwise triangles instead
	d3d_.SetRS({ FILL_SOLID, CULL_BACK, FRONT_CLOCKWISE });

	// Only draw reflection into visible mirror pixels as marked by the stencil buffer
	renderStates.SetDSS(pDeviceContext_, DRAW_REFLECTION, 1);

	// render reflected entts
	render_.RenderInstances(
		pDeviceContext_,
		render_.perInstanceData_,
		meshesData_.pVBs_,
		meshesData_.pIBs_,
		meshesData_.indexCount_);

#if 0
	// if we have any entts with alpha clipping and cull mode none
	if (rsDataToRender.enttsAlphaClippingAndCullModelNone_.size())
	{
		// render entts with default render states but also with alpha clipping
		render_.GetShadersContainer().lightShader_.SetAlphaClipping(pDeviceContext_, true);
		d3d_.SetRS({ FILL_SOLID, CULL_NONE, FRONT_CLOCKWISE });

		RenderEnttsReflections(rsDataToRender.enttsAlphaClippingAndCullModelNone_, R);

		render_.GetShadersContainer().lightShader_.SetAlphaClipping(pDeviceContext_, false);
	}
#endif
	// restore default states
	renderStates.ResetRS(pDeviceContext_);
	renderStates.ResetDSS(pDeviceContext_);

	// restore light directions
	render_.GetLightShader().SetDirLights(pDeviceContext_, render_.perFrameData_.dirLights);

	
}

///////////////////////////////////////////////////////////

void GraphicsClass::RenderEnttsShadows(
	const std::vector<EntityID>& enttsIds,
	const DirectX::XMMATRIX& S,               // shadow matrix
	const DirectX::XMMATRIX& shadowOffsetY)
{
	try
	{
		const std::vector<XMMATRIX>& origWorlds = render_.instanceBuffData_.worlds;
		const size instancesCount = std::ssize(origWorlds);
		std::vector<XMMATRIX> shadowMatrices(instancesCount);


		// update world matrix to render shadows
		for (int idx = 0; const XMMATRIX& m : origWorlds)
			shadowMatrices[idx] = m * S * shadowOffsetY;

		// prepare shadow materials for each instance

		const Render::Material shadowMat(
			{ 0,0,0,1 },
			{ 0,0,0,0.5f },
			{ 0,0,0, 16.0f },
			{ 0,0,0,0 });
		std::vector<Render::Material> shadowMaterials(instancesCount, shadowMat);
	
		render_.UpdateInstancedBufferWorlds(pDeviceContext_, shadowMatrices);
		render_.UpdateInstancedBufferMaterials(pDeviceContext_, shadowMaterials);

		render_.RenderInstances(
			pDeviceContext_,
			render_.perInstanceData_,
			meshesData_.pVBs_,
			meshesData_.pIBs_,
			meshesData_.indexCount_);
	}
	catch (EngineException& e)
	{
		Log::Error(e);
	}
}

#endif

///////////////////////////////////////////////////////////

void GraphicsClass::SetupLightsForFrame(
	const ECS::LightSystem& lightSys,
	std::vector<Render::DirLight>& outDirLights,
	std::vector<Render::PointLight>& outPointLights,
	std::vector<Render::SpotLight>& outSpotLights)
{
	// convert light source data from the ECS into Render format
	// (they are the same so we simply need to copy data)

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

	size dirLightSize   = sizeof(ECS::DirLight);
	size pointLightSize = sizeof(ECS::PointLight);
	size spotLightSize  = sizeof(ECS::SpotLight);

	// copy data of directional/point/spot light sources
	for (size idx = 0; idx < numDirLights; ++idx)
		memcpy(&outDirLights[idx], &dirLights.data_[idx], dirLightSize);

	for (size idx = 0; idx < numPointLights; ++idx)
		memcpy(&outPointLights[idx], &pointLights.data_[idx], pointLightSize);

	for (size idx = 0; idx < numSpotLights; ++idx)
		memcpy(&outSpotLights[idx], &spotLights.data_[idx], spotLightSize);
}

///////////////////////////////////////////////////////////

void GraphicsClass::GetTexSRVsForEntts(
	const std::vector<EntityID>& inEntts,        // in: entts sorted by meshes ids
	const std::vector<std::vector<TexID>>& meshesTexIds,
	const size meshesCount,
	std::vector<SRV*>& outTexSRVs,
	std::vector<EntityID>& outEnttsWithOwnTex)
{
	// NOTICE:
	// wtf I mean under the "skin" or "own texture(s)"? 
	// if some entt has a set of textures (has ECS::Textured component)
	// that means it will be textured in a differ way from its mesh textures;
	// for instance: different boxes has different textures (not default mesh textures)
	
	// get IDs of entts which have the Textured component
	entityMgr_.texturesSystem_.FilterEnttsWhichHaveOwnTex(inEntts, outEnttsWithOwnTex);

	// get textures IDs of entts which have the Textured component
	std::vector<TexID> enttsOwnTexIDs;
	entityMgr_.texturesSystem_.GetTexIDsByEnttsIDs(outEnttsWithOwnTex, enttsOwnTexIDs);

	// ---------------------------------------------

	// concatenate arrs of meshes textures IDs and arrs of entts own textures
	std::vector<TexID> texIDs;
	const u32 texCountPerSet = TextureClass::TEXTURE_TYPE_COUNT;


	texIDs.resize((meshesCount + std::ssize(outEnttsWithOwnTex)) * 2);
	u32 idx = 0;

	// define what texture types we need for our shaders
	std::vector<aiTextureType> necessaryTexTypes = { aiTextureType_DIFFUSE, aiTextureType_SPECULAR };

	// add meshes textures IDs
	for (const std::vector<TexID>& meshTexIds : meshesTexIds)
	{
		for (const aiTextureType type : necessaryTexTypes)
			texIDs[idx++] = meshTexIds[type];
	}
		
	// add textures IDs of each entt which has the Textured component (own skins)
	for (size i = 0; i < std::ssize(outEnttsWithOwnTex); ++i)
	{
		for (const aiTextureType type : necessaryTexTypes)
			texIDs[idx++] = enttsOwnTexIDs[i * texCountPerSet + type];
	}

	// get textures shader resource views by textures ids
	TextureManager::Get()->GetSRVsByTexIDs(texIDs, outTexSRVs);
}

///////////////////////////////////////////////////////////

void GraphicsClass::GenInstancesTexSetData(
	const std::vector<EntityID>& inAllEntts,
	const std::vector<EntityID>& enttsWithOwnTex,
	const std::vector<ptrdiff_t>& numInstancesPerMesh,       // how many entts instances will be rendered using geometry of the mesh)
	std::vector<u32>& outTexSetIdxs,
	std::vector<u32>& outInstancesPerTexSet,
	u32& outNumUniqueTexSet)
{
	const size enttsCount = std::ssize(inAllEntts);
	const size meshesCount = std::ssize(numInstancesPerMesh);

	// get an arr of indices to textures set for each input entt
	// so we will know what texture set to use for rendering particular instance
	outTexSetIdxs.resize(enttsCount);

	int enttsTexSetIdx = 0;

	for (int texIdx = 0; enttsTexSetIdx < meshesCount; ++enttsTexSetIdx)
	{
		for (int i = 0; i < numInstancesPerMesh[enttsTexSetIdx]; ++i)
			outTexSetIdxs[texIdx++] = enttsTexSetIdx;
	}

	// setup idx to textures set for entts which have the Textured component
	std::vector<ptrdiff_t> idxsToEnttsWithTex;
	CoreUtils::GetIdxsInArr(inAllEntts, enttsWithOwnTex, idxsToEnttsWithTex);

	for (const ptrdiff_t idx : idxsToEnttsWithTex)
		outTexSetIdxs[idx] = enttsTexSetIdx++;

	// how many UNIQUE textures sets we have
	outNumUniqueTexSet = enttsTexSetIdx;


	// ---------------------------------------------
	// define how many instances we have per texture set
	// (somewhat branchless method)

	outInstancesPerTexSet.resize(std::size(outTexSetIdxs));
	u32 pos = 0;
	outInstancesPerTexSet[pos]++;

	// define how many instance will be rendered per textures set
	for (size i = 1; i < std::ssize(outTexSetIdxs); ++i)
	{
		pos += (outTexSetIdxs[i] != outTexSetIdxs[i - 1]);
		outInstancesPerTexSet[pos]++;
	}

	outInstancesPerTexSet.resize(pos + 1);


	// ---------------------------------------------
	// remove duplicates in sequences of the same textures set idxs
	pos = 0;

	for (size i = 1; i < std::ssize(outTexSetIdxs); ++i)
	{
		pos += (outTexSetIdxs[i] != outTexSetIdxs[i - 1]);
		outTexSetIdxs[pos] = outTexSetIdxs[i];
	}

	outTexSetIdxs.resize(pos + 1);
}