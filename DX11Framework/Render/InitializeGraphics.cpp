////////////////////////////////////////////////////////////////////////////////////////////
// Filename:     InitializeGraphics.cpp
// Description:  there are functions for initialization of DirectX
//               and graphics parts of the engine;
//
// Created:      02.12.22
////////////////////////////////////////////////////////////////////////////////////////////
#include "InitializeGraphics.h"

#include "../GameObjects/ModelInitializer.h"
#include "../GameObjects/TerrainInitializer.h"
#include "../GameObjects/GeometryGenerator.h"


// includes all of the shaders (are used for initialization of these shaders and 
// set them into the shaders_container)

#include "../EffectsAndShaders/TerrainShaderClass.h"         // for rendering the terrain 
#include "../EffectsAndShaders/SpecularLightShaderClass.h"   // for light effect with specular
#include "../EffectsAndShaders/MultiTextureShaderClass.h"    // for multitexturing
#include "../EffectsAndShaders/LightMapShaderClass.h"        // for light mapping
#include "../EffectsAndShaders/AlphaMapShaderClass.h"        // for alpha mapping
#include "../EffectsAndShaders/BumpMapShaderClass.h"         // for bump mapping
#include "../EffectsAndShaders/SkyDomeShaderClass.h"         // for rendering the sky dome
#include "../EffectsAndShaders/SkyPlaneShaderClass.h"        // for rendering the sky plane
#include "../EffectsAndShaders/DepthShaderClass.h"           // for coloring objects according to its depth position

#include "../EffectsAndShaders/SpriteShaderClass.h"          // for rendering 2D sprites
#include "../EffectsAndShaders/ReflectionShaderClass.h"      // for rendering planar reflection

#include "../ImageReaders/ImageReader.h"               // for reading images data


using namespace DirectX;



InitializeGraphics::InitializeGraphics()
{
	Log::Debug(LOG_MACRO);
}


////////////////////////////////////////////////////////////////////////////////////////////
//
//                                PUBLIC FUNCTIONS
//
////////////////////////////////////////////////////////////////////////////////////////////

bool InitializeGraphics::InitializeDirectX(
	D3DClass & d3d,
	HWND hwnd,
	const UINT windowWidth,
	const UINT windowHeight,
	const float nearZ,            // near Z-coordinate of the screen/frustum
	const float farZ,             // far Z-coordinate of the screen/frustum (screen depth)
	const bool vSyncEnabled,
	const bool isFullScreenMode,
	const bool enable4xMSAA)
{
	// this function initializes the DirectX stuff 
	// (device, deviceContext, swapChain, rasterizerState, viewport, etc)

	try 
	{
		bool result = d3d.Initialize(hwnd,
			windowWidth,
			windowHeight,
			vSyncEnabled,
			isFullScreenMode,
			enable4xMSAA,
			nearZ,
			farZ);
		COM_ERROR_IF_FALSE(result, "can't initialize the Direct3D");

		// setup the rasterizer state with default params
		d3d.SetRenderState(D3DClass::RASTER_PARAMS::CULL_MODE_BACK);
		d3d.SetRenderState(D3DClass::RASTER_PARAMS::FILL_MODE_SOLID);
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(LOG_MACRO, "can't initialize DirectX");
		return false;
	}

	return true;
} // end InitializeDirectX

/////////////////////////////////////////////////

bool InitializeGraphics::InitializeShaders(ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,
	Shaders::ShadersContainer & shaders)
{
	// this function initializes all the shader classes (color, texture, light, etc.)
	// and the HLSL shaders as well

	Log::Print("---------------------------------------------------------");
	Log::Print("                INITIALIZATION: SHADERS                  ");
	Log::Print("---------------------------------------------------------");

	try
	{
		bool result = false;

		result = shaders.colorShader_.Initialize(pDevice, pDeviceContext);
		COM_ERROR_IF_FALSE(result, "can't initialize the color shader class");

		result = shaders.textureShader_.Initialize(pDevice, pDeviceContext);
		COM_ERROR_IF_FALSE(result, "can't initialize the texture shader class");

		result = shaders.lightShader_.Initialize(pDevice, pDeviceContext);
		COM_ERROR_IF_FALSE(result, "can't initialize the light shader class");

		result = shaders.pointLightShader_.Initialize(pDevice, pDeviceContext);
		COM_ERROR_IF_FALSE(result, "can't initialize the point light shader class");
	


#if 0

		


		// NOTE: when you add a new shader class you have to include a header of this class
		//       into the ShadersContainer.h
		//
		// make shaders objects (later all the pointers will be stored in the shaders container)
		// so we don't need clear this vector with pointers
		pointersToShaders.push_back(new TextureShaderClass());
		pointersToShaders.push_back(new SpecularLightShaderClass());
		pointersToShaders.push_back(new LightShaderClass());
		pointersToShaders.push_back(new MultiTextureShaderClass());
		pointersToShaders.push_back(new AlphaMapShaderClass());
		pointersToShaders.push_back(new TerrainShaderClass());
		pointersToShaders.push_back(new SkyDomeShaderClass());
		pointersToShaders.push_back(new DepthShaderClass());
		pointersToShaders.push_back(new BumpMapShaderClass());
		pointersToShaders.push_back(new SkyPlaneShaderClass());
		pointersToShaders.push_back(new LightMapShaderClass());
		//pointersToShaders.push_back(new FontShaderClass());
		pointersToShaders.push_back(new PointLightShaderClass());
		pointersToShaders.push_back(new SpriteShaderClass());
		pointersToShaders.push_back(new ReflectionShaderClass());
		

		// add pairs [shader_name => shader_ptr] into the shaders container
		for (const auto & pShader : pointersToShaders)
		{
			graphics_->pShadersContainer_->SetShaderByName(pShader->GetShaderName(), pShader);
		}

		// go through each shader and initialize it
		for (auto & elem : graphics_->pShadersContainer_->GetShadersList())
		{
			result = elem.second->Initialize(pDevice, pDeviceContext, hwnd);
			COM_ERROR_IF_FALSE(result, "can't initialize the " + elem.second->GetShaderName() + " object");

			// after the initialization we add this shader into the main model_to_shader mediator
			graphics_->pModelsToShaderMediator_->AddShader(elem.second);
		}

#endif

	}
	catch (std::bad_alloc & e)
	{
		Log::Error(LOG_MACRO, e.what());
		return false;
	}
	catch (COMException & exception) // if we have some error during initialization of shaders we handle such an error here
	{
		Log::Error(exception, true);
		return false;
	}

	
	return true;
}  // end InitializeShaders

/////////////////////////////////////////////////

bool InitializeGraphics::InitializeScene(
	D3DClass & d3d,
	ModelsStore & modelsStore,
	LightStore & lightStore,
	Settings & settings,
	FrustumClass & editorFrustum,
	TextureManagerClass & textureManager,
	RenderToTextureClass & renderToTexture,
	ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,
	HWND hwnd,
	const float nearZ,               // near Z-coordinate of the frustum/camera
	const float farZ)                // far Z-coordinate of the frustum/camera
	
{
	// this function initializes some main elements of the scene:
	// models, light sources, textures

	try
	{
		bool result = false;

		// initialize the editor frustum object
		editorFrustum.Initialize(farZ);

		///////////////////////////////////////////////////
		//  CREATE AND INIT RELATED TO TEXTURES STUFF
		///////////////////////////////////////////////////

		// initialize the textures manager
		textureManager.Initialize(pDevice);

		// initialize the render to texture object
		result = renderToTexture.Initialize(pDevice, 256, 256, farZ, nearZ, 1);
		COM_ERROR_IF_FALSE(result, "can't initialize the render to texture object");

		///////////////////////////////////////////////////
		//  CREATE AND INIT SCENE ELEMENTS
		///////////////////////////////////////////////////

		// initialize all the models on the scene
		if (!InitializeModels(pDevice, pDeviceContext, modelsStore, settings, farZ))           
			return false;

		// initialize all the light sources on the scene
		if (!InitializeLight(settings, lightStore))
			return false;

		Log::Print(LOG_MACRO, "is initialized");
	}

	catch (COMException& exception)
	{
		Log::Error(exception, false);
		Log::Error(LOG_MACRO, "can't initialize the scene");

		return false;
	}


	return true;
} 

/////////////////////////////////////////////////

bool InitializeGraphics::InitializeCameras(
	CameraClass & editorCamera,
	CameraClass & cameraForRenderToTexture,
	DirectX::XMMATRIX & baseViewMatrix,      // is used for 2D rendering
	const UINT windowWidth,
	const UINT windowHeight,
	const float nearZ,                       // near Z-coordinate of the frustum
	const float farZ,                        // far Z-coordinate of the frustum
	const float fovDegrees,                  // field of view
	const float cameraSpeed,                 // camera movement speed
	const float cameraSensitivity)           // camera rotation speed
{
	try
	{
		// calculate the aspect ratio
		const float aspectRatio = (float)windowWidth / (float)windowHeight;

		editorCamera.Initialize(cameraSpeed, cameraSensitivity);              // initialize the editor's camera object
		cameraForRenderToTexture.Initialize(cameraSpeed, cameraSensitivity);  // initialize the camera which is used for rendering into textures


		// setup the editor camera
		editorCamera.SetPosition({ 0.0f, 0.0f, -3.0f });
		editorCamera.SetProjectionValues(fovDegrees, aspectRatio, nearZ, farZ);

		// initialize view matrices for the editor camera
		editorCamera.UpdateViewMatrix();
		baseViewMatrix = editorCamera.GetViewMatrix(); // initialize a base view matrix with the camera for 2D user interface rendering

		// setup the camera for rendering to textures
		cameraForRenderToTexture.SetPosition({ 0.0f, 0.0f, -5.0f });
		cameraForRenderToTexture.SetProjectionValues(fovDegrees, aspectRatio, nearZ, farZ);
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(LOG_MACRO, "can't initialize the cameras objects");
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////

void PreparePositionsRotationsForCubesToInit(
	const UINT numOfModels,
	_Inout_ std::vector<DirectX::XMVECTOR> & outPositions,
	_Inout_ std::vector<DirectX::XMVECTOR> & outRotations)
{
	// GENERATE positions and rotations data for the cubes

	assert(numOfModels > 0);
	
	

	GeometryGenerator geoGen;
	GeometryGenerator::MeshData meshData;

	// generate positions for cubes like they make a terrain in minecraft 
	const float width = sqrtf((float)numOfModels);
	const float depth = width;

	// generate a basic grid which will be used for cubes placement
	geoGen.CreateGrid(
		width,
		depth,
		(UINT)width+1,  // +1 because we want to place each new cube right after the previous, without gaps
		(UINT)depth+1,
		meshData);

	// allocate memory for position/rotation data for each cube
	outPositions.resize(numOfModels);
	outRotations.resize(numOfModels);

	// generate height for each vertex and set color for it according to its height;
	// and fill in the input array of positions

	for (size_t i = 0; i < numOfModels; ++i)
	{
		DirectX::XMFLOAT3 & pos = meshData.vertices[i].position;

		// use a function for making hills for the terrain
		pos.y = floorf(0.3f * (0.3f * (pos.z*sinf(0.1f * pos.x)) + (pos.x * cosf(0.1f * pos.z))));

		// store the position and rotation for this cube
		outPositions[i] = XMLoadFloat3(&pos);
		outRotations[i] = XMVectorZero();
	}


#if 0
	const int range = 100;
	const float widthOfCubeField = sqrtf((float)numOfModels);
	const float heightOfCubeField = widthOfCubeField;
	const float widthOfSingleCube = 2.0f;

	UINT data_idx = 0;

	// fill in the input data arrays with generated positions and rotations data
	for (float w_idx = 0; w_idx < widthOfCubeField; ++w_idx)
	{
		for (float h_idx = 0; h_idx < heightOfCubeField; ++h_idx)
		{
			const float posX = w_idx * widthOfSingleCube;
			const float posY = static_cast<float>(rand() % 2) * widthOfSingleCube;
			const float posZ = h_idx * widthOfSingleCube;

			// fill in arrays
			outPosition[data_idx] = { posX, posY, posZ };
			outRotations[data_idx] = DirectX::XMVectorZero();

			++data_idx;
		}
	}
#endif
#if 0
	// generate absolutely random positions/rotations for the models
	for (UINT idx = 0; idx < numOfModels; ++idx)
	{
		const float randX_pos = static_cast<float>(rand() % range);
		const float randY_pos = static_cast<float>(rand() % range);
		const float randZ_pos = static_cast<float>(rand() % range);

		const float randX_rot = static_cast<float>((rand() % 100) * 0.01f);
		const float randY_rot = static_cast<float>((rand() % 100) * 0.01f);
		const float randZ_rot = static_cast<float>((rand() % 100) * 0.01f);

		randPositions[idx] = { randX_pos, randY_pos, randZ_pos };
		randRotations[idx] = { randX_rot, randY_rot, randZ_rot };
	}
#endif
}

void PreparePositionsRotationsModificatorsForModelsToInit(
	const UINT numOfModels,
	_Inout_ std::vector<DirectX::XMVECTOR> & randPosModificators,
	_Inout_ std::vector<DirectX::XMVECTOR> & randRotModificators)
{
	assert(numOfModels);

	randPosModificators.resize(numOfModels);
	randRotModificators.resize(numOfModels);

	// prepare modification data for the cubes
	const int range = 100;
	const float slower = 0.001f;

	// prepare modification data for the cubes
	for (UINT idx = 1; idx < numOfModels; ++idx)
	{
		
		randPosModificators[idx] = DirectX::XMVectorZero();
		randRotModificators[idx] = DirectX::XMVectorZero();

#if 0
		// random position modificator
		const float randX_pos = static_cast<float>((rand() % range) * slower);
		const float randY_pos = static_cast<float>((rand() % range) * slower);
		const float randZ_pos = static_cast<float>((rand() % range) * slower);

		// random rotation modificator
		const float randX_rot = static_cast<float>((rand() % range) * slower);
		const float randY_rot = static_cast<float>((rand() % range) * slower);
		const float randZ_rot = static_cast<float>((rand() % range) * slower);

		// set random position/rotation for this index
		randPosModificators[idx] = { randX_pos, randY_pos, randZ_pos };
		randRotModificators[idx] = { randX_rot, randY_rot, randZ_rot };
#endif
	}
}

///////////////////////////////////////////////////////////

void CreateTerrain(ID3D11Device* pDevice,
	ModelsCreator & modelsCreator,
	ModelsStore & modelsStore)
{
	//
	// CREATE TERRAIN
	//

	const std::string terrainSetupFilepath{ "data/terrain/setup_load_bmp_height_map.txt" };

#if 0
	// CREATE A TERRAIN GRID FROM FILE
	const UINT terrainGridIdx = modelsCreator.CreateTerrainFromFile(terrainSetupFilepath,
		pDevice,
		modelsStore);

	// get an index of the terrain grid vertex buffer and set a rendering shader for it
	const UINT terrainGridVertexBuffer = modelsStore.relatedToVertexBufferByIdx_[terrainGridIdx];
	modelsStore.useShaderForBufferRendering_[terrainGridVertexBuffer] = ModelsStore::DIFFUSE_LIGHT_SHADER;

	// set a texture for this terrain grid
	modelsStore.SetTextureByIndex(terrainGridIdx, "data/textures/dirt01d.dds", aiTextureType_DIFFUSE);

#elif 1
	// MANUALLY GENERATE A TERRAIN GRID
	TerrainInitializer terrainInitializer;

	// load terrain params from the setup file
	terrainInitializer.LoadSetupFile(terrainSetupFilepath);
	const TerrainInitializer::TerrainSetupData & setupData = terrainInitializer.GetSetupData();

	// generate a terrain based on the setup params and get its idx
	const UINT terrainGridIdx = modelsCreator.CreateGeneratedTerrain(pDevice,
		modelsStore,
		(float)setupData.terrainWidth,
		(float)setupData.terrainDepth,
		setupData.terrainWidth + 1,
		setupData.terrainDepth + 1);

	
#endif
}

///////////////////////////////////////////////////////////

void CreateCubes(ID3D11Device* pDevice, 
	ModelsCreator & modelsCreator, 
	ModelsStore & modelsStore,
	const ModelsStore::RENDERING_SHADERS renderingShaderType,  
	const UINT numOfCubes,
	const std::vector<XMVECTOR> & inPositions = {})
{
	//
	// CREATE CUBES
	//

	// check if we want to create any cube if no we just return from the function
	if (numOfCubes == 0) return;

	std::vector<UINT> copiedCubesIndices;   // will contain indices of the copies from the origin cube
	const DirectX::XMVECTOR defaultZeroVec{ 0, 0, 0, 1 };   

	// create a cube which will be a BASIC CUBE for creation of the other ones;
	// (here we use default zero vector but if we have some input data for positions/rotations/etc.
	//  we'll apply it later to all the models including the origin cube)
	const UINT originCube_idx = modelsCreator.CreateCube(pDevice,
		modelsStore,
		defaultZeroVec,   
		defaultZeroVec,
		defaultZeroVec,
		defaultZeroVec);

	// if we want to create some copies of the origin cube
	const UINT numOfCopies = numOfCubes - 1;

	if (numOfCopies > 0)
	{
		// create (numOfCubes-1) copies of the origin cube (-1 because we've already created one cube)
		copiedCubesIndices = modelsStore.CreateBunchCopiesOfModelByIndex(originCube_idx, numOfCopies);
	}
	
	// ----------------------------------------------------- //


	// if we don't have any positions data as input we generate this data manually
	if (inPositions.size() == 0)
	{
		// arrays for positions/rotations values of cubes
		std::vector<DirectX::XMVECTOR> cubesPositions;
		std::vector<DirectX::XMVECTOR> cubesRotations;

		// arrays for positions/rotations modification values for the cubes
		std::vector<DirectX::XMVECTOR> positionModificators;
		std::vector<DirectX::XMVECTOR> rotationModificators;

		// generate positions/rotations data for cubes
		PreparePositionsRotationsForCubesToInit(
			numOfCubes,
			cubesPositions,
			cubesRotations);

		// generate modificators for positions/rotattion of cubes
		PreparePositionsRotationsModificatorsForModelsToInit(
			numOfCubes,
			positionModificators,
			rotationModificators);

		// apply the positions/rotations for cubes
		std::copy(cubesPositions.begin(), cubesPositions.end(), modelsStore.positions_.begin() + originCube_idx);
		std::copy(cubesRotations.begin(), cubesRotations.end(), modelsStore.rotations_.begin() + originCube_idx);

		// apply the positions/rotations modificators
		//std::copy(positionModificators.begin() + skipOriginCube, positionModificators.end(), modelsStore.positionModificators_.begin() + originCube_idx);
		//std::copy(rotationModificators.begin() + skipOriginCube, rotationModificators.end(), modelsStore.rotationModificators_.begin() + originCube_idx);

		// clear the transient initialization data
		cubesPositions.clear();
		cubesRotations.clear();
		positionModificators.clear();
		rotationModificators.clear();

		// apply positions/rotations/scales/etc. to the cubes
		copiedCubesIndices.push_back(originCube_idx);
		modelsStore.UpdateWorldMatricesForModelsByIdxs(copiedCubesIndices);
	}

	// we have some positions/rotation/etc. data as input to the function so apply this data
	else
	{
		// we must have equal number of input positions and the number of created cubes
		assert(inPositions.size() == numOfCubes);

		// setup positions for the created cubes
		std::copy(inPositions.begin(), inPositions.end(), modelsStore.positions_.begin() + originCube_idx);

		// apply positions/rotations/scales/etc. to the cubes
		copiedCubesIndices.push_back(originCube_idx);
		modelsStore.UpdateWorldMatricesForModelsByIdxs(copiedCubesIndices);
	}
	
	// ----------------------------------------------------- //

	// setup rendering shader for the vertex buffer of cube
	const UINT cubeVertexBufferIdx = modelsStore.GetRelatedVertexBufferByModelIdx(originCube_idx);
	modelsStore.SetRenderingShaderForVertexBufferByIdx(cubeVertexBufferIdx, renderingShaderType);



	return;
}

///////////////////////////////////////////////////////////

void CreateCylinders(ID3D11Device* pDevice,
	ModelsStore & modelsStore,
	ModelsCreator & modelsCreator,
	const ModelsCreator::CYLINDER_PARAMS & cylParams,
	const ModelsStore::RENDERING_SHADERS renderingShaderType,
	const UINT numOfCylinders)
{
	// we don't want to create any cylinder so just go out
	if (numOfCylinders == 0)
		return;

	// ----------------------------------------------------- //

	// PREPARE DATA FOR CYLINDERS
	assert(numOfCylinders == 10);

	// define transformations from local spaces to world space
	std::vector<XMVECTOR> cylPos(10);

	// we create 5 rows of 2 cylinders and spheres per row
	for (UINT i = 0; i < 5; ++i)
	{
		cylPos[i * 2 + 0] = { -5.0f, 1.5f, -10.0f + i*5.0f };
		cylPos[i * 2 + 1] = { +5.0f, 1.5f, -10.0f + i*5.0f };
	}


	// create a new BASIC cylinder model
	const UINT originCyl_Idx = modelsCreator.CreateCylinder(
		pDevice,
		modelsStore, 
		cylParams);
	
	// set that we want to render cubes using some particular shader
	const UINT cylinderVertexBufferIdx = modelsStore.relatedToVertexBufferByIdx_[originCyl_Idx];
	modelsStore.useShaderForBufferRendering_[cylinderVertexBufferIdx] = renderingShaderType;

	// set a default texture for the basic cylinder model
	modelsStore.SetTextureByIndex(originCyl_Idx, "data/textures/gigachad.dds", aiTextureType_DIFFUSE);

	// if we want to create more than only one cylinder model;
	// notice: -1 because we've already create one cylinder (basic)
	const UINT numOfCopies = numOfCylinders - 1;
	std::vector<UINT> cylIndices = modelsStore.CreateBunchCopiesOfModelByIndex(originCyl_Idx, numOfCopies);

	// apply generated positions/rotations/scales/etc. to the cylinders
	std::copy(cylPos.begin(), cylPos.end(), modelsStore.positions_.begin() + originCyl_Idx);

	// since we set new positions for cylinder we have to update its world matrices
	cylIndices.push_back(originCyl_Idx);
	modelsStore.UpdateWorldMatricesForModelsByIdxs(cylIndices);
}

///////////////////////////////////////////////////////////

void CreateSpheres(ID3D11Device* pDevice,
	ModelsStore & modelsStore,
	ModelsCreator & modelsCreator,
	const ModelsCreator::SPHERE_PARAMS & sphereParams,
	const ModelsStore::RENDERING_SHADERS renderingShaderType,
	const UINT numOfSpheres)
{
	// we don't want to create any cylinder so just go out
	if (numOfSpheres == 0)
		return;


	// -------------------------------------------------------------- // 

	// PREPARE DATA FOR SPHERES
	assert(numOfSpheres > 10);

	// define transformations from local spaces to world space
	std::vector<XMVECTOR> spheresPos(numOfSpheres);
	std::vector<XMVECTOR> spheresScales(numOfSpheres);

	
	// we create 5 rows of 2 cylinders and spheres per row
	for (UINT i = 0; i < 5; ++i)
	{
		spheresPos[i * 2 + 0] = { -5.0f, 3.5f, -10.0f + i*5.0f };
		spheresPos[i * 2 + 1] = { +5.0f, 3.5f, -10.0f + i*5.0f };

		spheresScales[i * 2 + 0] = { 1, 1, 1, 1 };  // default scale
		spheresScales[i * 2 + 1] = { 1, 1, 1, 1 };  // default scale
	}

	// set position and scale for the central sphere
	spheresPos.back() = { 0,11,0 };
	spheresScales.back() = { 3, 3, 3 };

	// -------------------------------------------------------------- // 

	// create a new BASIC sphere model
	const UINT originSphere_idx = modelsCreator.CreateSphere(pDevice,
		modelsStore,
		sphereParams.radius,
		sphereParams.sliceCount,
		sphereParams.stackCount,
		spheresPos[0],
		{ 0, 0, 0, 1 },
		DirectX::XMVectorZero(),   // by default we have no position modification
		DirectX::XMVectorZero());  // by default we have no rotation modification

	// set default texture for the basic sphere model
	modelsStore.SetTextureByIndex(originSphere_idx, "data/textures/gigachad.dds", aiTextureType_DIFFUSE);

	// setup primitive topology for the vertex buffer of sphere
	const UINT sphereVertexBufferIdx = modelsStore.GetRelatedVertexBufferByModelIdx(originSphere_idx);
	modelsStore.SetPrimitiveTopologyForVertexBufferByIdx(sphereVertexBufferIdx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// setup rendering shader for the vertex buffer
	modelsStore.SetRenderingShaderForVertexBufferByIdx(sphereVertexBufferIdx, renderingShaderType); // ModelsStore::DIFFUSE_LIGHT_SHADER);

	// -------------------------------------------------------------- // 

	// create copies of the origin sphere model (-1 because we've already create one (basic) sphere)
	// and get indices of all the copied models
	const std::vector<UINT> copiedModelsIndices (modelsStore.CreateBunchCopiesOfModelByIndex(originSphere_idx, numOfSpheres - 1));

	// apply generated positions/rotations/scales to the spheres
	std::copy(spheresPos.begin(), spheresPos.end(), modelsStore.positions_.begin() + originSphere_idx);
	std::copy(spheresScales.begin(), spheresScales.end(), modelsStore.scales_.begin() + originSphere_idx);

	modelsStore.UpdateWorldMatrixForModelByIdx(originSphere_idx);
	modelsStore.UpdateWorldMatricesForModelsByIdxs(copiedModelsIndices);
	

	modelsStore.SetModelAsModifiable(originSphere_idx + 10);
	modelsStore.SetRotationModificator(originSphere_idx + 10, XMQuaternionRotationMatrix(XMMatrixRotationY(DirectX::XM_PI * 0.01f)));
}

///////////////////////////////////////////////////////////

void CreateEditorGrid(ID3D11Device* pDevice,
	ModelsCreator & modelsCreator,
	ModelsStore & modelsStore)
{
	//
	// CREATE EDITOR GRID
	//
	GeometryGenerator geoGen;

	std::vector<DirectX::XMFLOAT2> editorGridPositions;

	const float fullWidthOfEditorGrid = 100.0f;
	const float halfWidthOfEditorGrid = 0.5f * fullWidthOfEditorGrid;
	const float halfDepthOfEditorGrid = halfWidthOfEditorGrid;
	const float editorGridWidth = 10.0f;
	const float editorGridDepth = editorGridWidth;
	const UINT editorGridVertexCountByX = 11;
	const UINT editorGridVertexCountByZ = editorGridVertexCountByX;
	const UINT editorGridPositionsCount = static_cast<UINT>(fullWidthOfEditorGrid / editorGridWidth);


	for (UINT i = 0; i < editorGridPositionsCount; ++i)
	{
		const float posZ = halfDepthOfEditorGrid - i*editorGridDepth;

		for (UINT j = 0; j < editorGridPositionsCount; ++j)
		{
			const float posX = -halfWidthOfEditorGrid + j*editorGridWidth;

			editorGridPositions.push_back({ posX, posZ });
		}
	}

	// generate data for the editor grid
	GeometryGenerator::MeshData editorGridMesh;
	//geoGen.CreateGrid(10.0f, 10.0f, 10, 10, editorGridMesh);
	geoGen.CreateGrid(
		editorGridWidth,
		editorGridDepth,
		editorGridVertexCountByX,
		editorGridVertexCountByZ,
		editorGridMesh);


	// create an editor grid model
	const UINT originEditorGridIdx = modelsStore.CreateNewModelWithData(pDevice,
		"editor_grid",
		editorGridMesh.vertices,
		editorGridMesh.indices,
		{ TextureManagerClass::Get()->GetTextureByKey("unloaded_texture") },
		{ editorGridPositions[0].x, 0.0f, editorGridPositions[0].y, 1 },  // position
		{ 0, 0, 0, 0 },   // rotation 
		{ 0, 0, 0, 0 },   // position changes
		{ 0, 0, 0, 0 });  // rotation changes

	// set that we want to render editor grid using topology linelist
	const UINT editorVertexBufferIdx = modelsStore.relatedToVertexBufferByIdx_[originEditorGridIdx];
	modelsStore.usePrimTopologyForBuffer_[editorVertexBufferIdx] = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;

	for (UINT i = 1; i < editorGridPositionsCount*editorGridPositionsCount; ++i)
	{
		const UINT idxOfCopy = modelsStore.CreateOneCopyOfModelByIndex(pDevice, originEditorGridIdx);
		modelsStore.positions_[idxOfCopy] = { editorGridPositions[i].x, 0.0f, editorGridPositions[i].y, 1 };
	}


	editorGridPositions.clear();
}

///////////////////////////////////////////////////////////

void CreateAxis(ID3D11Device* pDevice,
	ModelsCreator & modelsCreator,
	ModelsStore & modelsStore)
{
#if 0
	// create a simple cube which will be a part of axis visual navigation
	modelsCreator.CreateCube(pDevice,
		modelsStore,
		{ 0,0,0 },
		{ 0,0,0 });
#endif

	// generate data for the axis
	GeometryGenerator geoGen;
	GeometryGenerator::MeshData axisMeshData;
	geoGen.CreateAxis(axisMeshData);

	// create an axis model

	const UINT axisModelIdx = modelsStore.CreateNewModelWithData(pDevice,
		"axis",
		axisMeshData.vertices,
		axisMeshData.indices,
		{ TextureManagerClass::Get()->GetTextureByKey("unloaded_texture") },
		{ 0, 0.0001f, 0, 1 }, // position (Y = 0.0001f because if we want to render axis and chunks bounding boxes at the same time there can be z-fighting)
		{ 0, 0, 0, 0 },       // rotation 
		{ 0, 0, 0, 0 },       // position changes
		{ 0, 0, 0, 0 });      // rotation changes);

	// set that we want to render axis using topology linelist
	const UINT axisVertexBufferIdx = modelsStore.relatedToVertexBufferByIdx_[axisModelIdx];
	modelsStore.usePrimTopologyForBuffer_[axisVertexBufferIdx] = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
}

///////////////////////////////////////////////////////////

void ComputeChunksToModels(ModelsStore & modelsStore)
{
	// prepare data for computation of relations between chunks and models
	//std::vector<uint32_t> modelsIDs = modelsStore.IDs_;
	std::vector<XMVECTOR> minChunksDimensionsArr = modelsStore.minChunksDimensions_;
	std::vector<XMVECTOR> maxChunksDimensionsArr = modelsStore.maxChunksDimensions_;
	std::vector<std::vector<uint32_t>> relationsChunksToModelsArr;

	// compute relations between chunks and models
	modelsStore.ComputeRelationsModelsToChunks(modelsStore.chunksCount_,
		modelsStore.numOfModels_,
		minChunksDimensionsArr,
		maxChunksDimensionsArr,
		relationsChunksToModelsArr);

	// store the relations into the models storage
	std::move(relationsChunksToModelsArr.begin(), relationsChunksToModelsArr.end(), modelsStore.relationsChunksToModels_.begin());

	// clear transiend data
	//modelsIDs.clear();
	minChunksDimensionsArr.clear();
	maxChunksDimensionsArr.clear();
	relationsChunksToModelsArr.clear();
}

///////////////////////////////////////////////////////////

void CreateChunkBoundingBoxes(ID3D11Device* pDevice,
	ModelsCreator & modelsCreator,
	ModelsStore & modelsStore,
	const UINT chunkDimension)
{
	try
	{
		// get positions for these chunks bounding boxes
		const std::vector<DirectX::XMVECTOR> & positions = modelsStore.GetChunksCenterPositions();

		// how many bounding boxes we want to create
		const UINT chunksBoundingBoxesCount = (UINT)positions.size();

		assert(chunksBoundingBoxesCount > 0);

		// create chunk BASIC bounding box
		const UINT chunkBoundingBoxIdx = modelsCreator.CreateChunkBoundingBox(chunkDimension, modelsStore, pDevice);

		// set that we want to render cubes using some particular shader
		const UINT chunkBoxVertexBufferIdx = modelsStore.GetRelatedVertexBufferByModelIdx(chunkBoundingBoxIdx);
		modelsStore.SetPrimitiveTopologyForVertexBufferByIdx(chunkBoxVertexBufferIdx, D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

		// create copies of BASIC bounding box (-1 because we've already create a one box (BASIC))
		modelsStore.CreateBunchCopiesOfModelByIndex(chunkBoundingBoxIdx, chunksBoundingBoxesCount - 1);

		// set positions for bounding boxes
		std::copy(positions.begin(), positions.end(), modelsStore.positions_.begin() + chunkBoundingBoxIdx);
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		COM_ERROR_IF_FALSE(false, "can't create bounding boxes for chunks");
	}

}

///////////////////////////////////////////////////////////

void CreateGeospheres(ID3D11Device* pDevice,
	ModelsCreator & modelsCreator,
	ModelsStore & modelsStore,
	const UINT numOfGeospheres,
	const std::vector<XMVECTOR> & inPositions = {})
{
	// if we don't want to render any geosphere just go out
	if (numOfGeospheres == 0)
		return;

	// create BASIC geosphere models
	const UINT origin_GeoSphereIdx = modelsCreator.CreateGeophere(pDevice, modelsStore, 3.0f, 10);

	// set that we want to render cubes using some particular shader
	const UINT vertexBufferIdx = modelsStore.relatedToVertexBufferByIdx_[origin_GeoSphereIdx];
	modelsStore.useShaderForBufferRendering_[vertexBufferIdx] = ModelsStore::DIFFUSE_LIGHT_SHADER;

	// set texture for geosphere
	modelsStore.SetTextureByIndex(origin_GeoSphereIdx, "data/textures/gigachad.dds", aiTextureType_DIFFUSE);

	// if we want to create more than only one geosphere (-1 because we've already create one (BASIC))
	modelsStore.CreateBunchCopiesOfModelByIndex(origin_GeoSphereIdx, numOfGeospheres - 1);

	// if we have some input positions for this exact number of geospheres we use it
	if (inPositions.size() == numOfGeospheres)
	{
		std::copy(inPositions.begin(), inPositions.end(), modelsStore.positions_.begin() + origin_GeoSphereIdx);
	}
}

///////////////////////////////////////////////////////////

bool InitializeGraphics::InitializeModels(ID3D11Device* pDevice, 
	ID3D11DeviceContext* pDeviceContext,
	ModelsStore & modelsStore,
	Settings & settings,
	const float farZ)
{
	// // initialize all the models on the scene

	Log::Print("---------------- INITIALIZATION: MODELS -----------------");

	Log::Debug(LOG_MACRO);

	try
	{
		ModelsCreator modelsCreator;
		GeometryGenerator geoGen;

		// create structure objects which will contain params of some geometry objects
		ModelsCreator::CYLINDER_PARAMS cylParams;
		ModelsCreator::SPHERE_PARAMS sphereParams;
		ModelsCreator::GEOSPHERE_PARAMS geosphereParams;
		ModelsCreator::PYRAMID_PARAMS pyramidParams;

		// --------------------------------------------------- //

		// first of all we have to initialize the models store
		modelsStore.Initialize(settings);

		// --------------------------------------------------- //

		// define how many models we want to create
		const UINT numOfCubes = settings.GetSettingIntByKey("CUBES_NUMBER");
		const UINT numOfCylinders = settings.GetSettingIntByKey("CYLINDERS_NUMBER");
		const UINT numOfSpheres = settings.GetSettingIntByKey("SPHERES_NUMBER");
		const UINT numOfGeospheres = settings.GetSettingIntByKey("GEOSPHERES_NUMBER");
		const UINT chunkDimension = settings.GetSettingIntByKey("CHUNK_DIMENSION");
		const UINT isCreateChunkBoundingBoxes = settings.GetSettingBoolByKey("IS_CREATE_CHUNKS_BOUNDING_BOXES");

		// define shader types for each model type
		ModelsStore::RENDERING_SHADERS spheresRenderingShader = ModelsStore::RENDERING_SHADERS::TEXTURE_SHADER;
		ModelsStore::RENDERING_SHADERS cylindersRenderingShader = ModelsStore::RENDERING_SHADERS::TEXTURE_SHADER;
		ModelsStore::RENDERING_SHADERS cubesRenderingShader = ModelsStore::RENDERING_SHADERS::COLOR_SHADER;
		ModelsStore::RENDERING_SHADERS pyramidRenderingShader = ModelsStore::RENDERING_SHADERS::TEXTURE_SHADER;
		ModelsStore::RENDERING_SHADERS terrainRenderingShader = ModelsStore::RENDERING_SHADERS::COLOR_SHADER;
		ModelsStore::RENDERING_SHADERS gridRenderingShader = ModelsStore::RENDERING_SHADERS::TEXTURE_SHADER;

		
		modelsCreator.LoadParamsForDefaultModels(settings, cylParams, sphereParams, geosphereParams, pyramidParams);


		// --------------------------------------------------- //

		// CREATE CUBES
		CreateCubes(pDevice, 
			modelsCreator, 
			modelsStore,
			cubesRenderingShader,
			numOfCubes,
			{ {10, 1, 0} });

		// CREATE SPHERES
		CreateSpheres(pDevice, 
			modelsStore, 
			modelsCreator,
			sphereParams,
			spheresRenderingShader, 
			numOfSpheres);

		// CREATE PLAIN GRID
		const UINT gridIdx = modelsCreator.CreateGrid(pDevice, modelsStore, 20, 20);

		// setup the grid
		modelsStore.SetTextureByIndex(gridIdx, "data/textures/dirt01.dds", aiTextureType_DIFFUSE);
		modelsStore.SetRenderingShaderForVertexBufferByIdx(modelsStore.GetRelatedVertexBufferByModelIdx(gridIdx), gridRenderingShader);

		// CREATE TERRAIN
		//CreateTerrain(pDevice, modelsCreator, modelsStore);

		// CREATE CYLINDERS
		CreateCylinders(
			pDevice, 
			modelsStore, 
			modelsCreator, 
			cylParams, 
			cylindersRenderingShader,
			numOfCylinders);

		// CREATE CHUNK BOUNDING BOX
		if (isCreateChunkBoundingBoxes)
			CreateChunkBoundingBoxes(pDevice, modelsCreator, modelsStore, chunkDimension);

		// CREATE GEOSPHERES
		CreateGeospheres(pDevice, modelsCreator, modelsStore, numOfGeospheres, {});


		// create a plane model
		//modelsCreator.CreatePlane(pDevice, models_, { 0,0,0 }, { 0,0,0 });

		// CREATE PYRAMID
		const UINT pyramidIdx = modelsCreator.CreatePyramid(
			pDevice,
			modelsStore,
			pyramidParams.height,
			pyramidParams.baseWidth,
			pyramidParams.baseDepth,
			{ 0,0,0,1 },
			{ 0,0,0,0 },
			DirectX::XMVectorZero(),   // by default no position modification
			DirectX::XMVectorZero());  // by default no rotation modification

		// setup the pyramid model
		modelsStore.SetTextureByIndex(pyramidIdx, "data/textures/brick01.dds", aiTextureType_DIFFUSE);
		modelsStore.SetRenderingShaderForVertexBufferByIdx(modelsStore.GetRelatedVertexBufferByModelIdx(pyramidIdx), pyramidRenderingShader);

		

		//
		// CREATE AXIS
		//
		CreateAxis(pDevice, modelsCreator, modelsStore);

		//
		// CREATE EDITOR GRID
		//
		//CreateEditorGrid(pDevice, modelsCreator, modelsStore);

		//modelsStore.UpdateModels(0);

		// COMPUTE CHUNKS TO MODELS RELATIONS
		ComputeChunksToModels(modelsStore);
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(LOG_MACRO, e.what());
		Log::Error(LOG_MACRO, "can't allocate memory for some element");
		return false;
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		Log::Error(LOG_MACRO, "can't initialize models");
		return false;
	}

	return true;
} // end InitializeModels

/////////////////////////////////////////////////

bool InitializeGraphics::InitializeSprites(const UINT screenWidth,
	const UINT screenHeight)
{
	Log::Debug(LOG_MACRO);

	const UINT crosshairWidth = 25;
	const UINT crosshairHeight = crosshairWidth;
	const char* animatedSpriteSetupFilename{ "data/models/sprite_data_01.txt" };
	const char* crosshairSpriteSetupFilename{ "data/models/sprite_crosshair.txt" };

	////////////////////////////////////////////////

#if 0

	// initialize an animated sprite
	pGameObj = pRenderableGameObjCreator_->Create2DSprite(animatedSpriteSetupFilename,
		"animated_sprite",
		{ 0, 500 },
		screenWidth, screenHeight);


	////////////////////////////////////////////////
	// compute a crosshair's center location
	POINT renderCrossAt{ screenWidth / 2 - crosshairWidth, screenHeight / 2 - crosshairHeight };

	// initialize a crosshair
	pGameObj = pRenderableGameObjCreator_->Create2DSprite(crosshairSpriteSetupFilename,
		"sprite_crosshair",
		renderCrossAt,
		screenWidth, screenHeight);
#endif

	return true;

} // end InitializeSprites

/////////////////////////////////////////////////
  
bool InitializeGraphics::InitializeTerrainZone(
	ZoneClass & zone,
	CameraClass & editorCamera,
	Settings & settings,
	const float farZ)                          // screen depth           
{

	// this function initializes the main wrapper for all of the terrain processing

	Log::Debug("\n\n\n");
	Log::Print("--------------- INITIALIZATION: TERRAIN ZONE  -----------------");

	try
	{
		std::string terrainSetupFilename{ "" };

		// the offset of the camera above the terrain
		const float cameraHeightOffset = settings.GetSettingFloatByKey("CAMERA_HEIGHT_OFFSET");

		// define if we want to load a height map for the terrain either in a RAW format
		// or in a BMP format
		bool loadRawHeightMap = settings.GetSettingBoolByKey("TERRAIN_LOAD_RAW_HEIGHT_MAP");

		// define which setup file we will use for initialization of this terrain
		if (loadRawHeightMap)
			terrainSetupFilename = settings.GetSettingStrByKey("TERRAIN_SETUP_FILE_LOAD_RAW");
		else
			terrainSetupFilename = settings.GetSettingStrByKey("TERRAIN_SETUP_FILE_LOAD_BMP");
	

		// initialize the zone class object
		bool result = zone.Initialize(editorCamera, farZ, cameraHeightOffset);
		COM_ERROR_IF_FALSE(result, "can't initialize the zone class instance");

		return true;
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(LOG_MACRO, e.what());
		return false;
	}
	catch (COMException & exception)
	{
		Log::Error(exception, true);
		Log::Error(LOG_MACRO, "can't initialize the terrain zone");
		return false;
	}


	return true;

} // InitializeTerrainZone

/////////////////////////////////////////////////

bool InitializeGraphics::InitializeLight(
	Settings & settings,
	LightStore & lightStore)
{
	// this function initializes all the light sources on the scene

	Log::Print("---------------- INITIALIZATION: LIGHT SOURCES -----------------");
	Log::Debug(LOG_MACRO);

	const DirectX::XMFLOAT3 ambientColorOn{ 0.3f, 0.3f, 0.3f };
	const DirectX::XMFLOAT3 ambientColorOff{ 0, 0, 0 };

	const UINT numDiffuseLights = settings.GetSettingIntByKey("NUM_DIFFUSE_LIGHTS");
	const UINT numPointLights   = settings.GetSettingIntByKey("NUM_POINT_LIGHTS");

	// set up the DIFFUSE light
	lightStore.CreateDiffuseLight(ambientColorOn,
		{ 1, 1, 1 },                               // diffuse color
		{ 0, 0, 0 },                               // specular color
		{ 1, -1, 1 },                              // direction
		32.0f,                                     // specular power
		1.0f);                                     // diffuse light intensity

	// --------------------------------------------------------------------- //
	// set up the point light sources


	const int range = 60;

	// prepare random colors and positions for point light sources and initialize these lights
	for (size_t idx = 0; idx < numPointLights; ++idx)
	{
		const float red   = (float)(rand() % 255) * 0.01f;   // rand(0, 255) / 100.0f
		const float green = (float)(rand() % 255) * 0.01f;
		const float blue  = (float)(rand() % 255) * 0.01f;

		const float posX = (float)(rand() % range);
		//const float posY = (float)(rand() % range);
		const float posY = static_cast<float>((rand() % 5)) + 3.0f;
		const float posZ = (float)(rand() % range);

		const float modPosX = (float)(rand() % range) * 0.005f;
		const float modPosY = (float)(rand() % range) * 0.005f;
		const float modPosZ = (float)(rand() % range) * 0.005f;

		lightStore.CreatePointLight(
		{ posX, posY, posZ },              // light position
		{ red, green, blue },              // light colour
		{ modPosX, modPosY, modPosZ });    // positions modificator
	}


	return true;
}

/////////////////////////////////////////////////

bool InitializeGraphics::InitializeGUI(D3DClass & d3d, 
	UserInterfaceClass & UI,
	Settings & settings,
	ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,
	const UINT windowWidth,
	const UINT windowHeight)
{
	// this function initializes the GUI of the game/engine (interface elements, text, etc.);

	Log::Print("---------------- INITIALIZATION: GUI -----------------------");
	Log::Debug(LOG_MACRO);

	try
	{
		const std::string fontDataFilePath    = settings.GetSettingStrByKey("FONT_DATA_FILE_PATH");
		const std::string fontTextureFilePath = settings.GetSettingStrByKey("FONT_TEXTURE_FILE_PATH");

		std::string videoCardName{ "" };
		int videoCardMemory = 0;

		// get an information about the video card
		d3d.GetVideoCardInfo(videoCardName, videoCardMemory);

		// initialize the user interface
		UI.Initialize(pDevice,
			pDeviceContext,
			fontDataFilePath,
			fontTextureFilePath,
			windowWidth,
			windowHeight,
			videoCardMemory,
			videoCardName);
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(LOG_MACRO, e.what());
		COM_ERROR_IF_FALSE(false, "can't allocate memory for GUI elements");
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		return false;
	}

	return true;

} // InitializeGUI







////////////////////////////////////////////////////////////////////////////////////////////
//
//                               PRIVATE FUNCTIONS
//
////////////////////////////////////////////////////////////////////////////////////////////


#if 0
void GameObjectsListClass::GenerateRandomDataForRenderableGameObjects()
{
	// this function generates random color/position values 
	// for the game objects on the scene

	DirectX::XMFLOAT3 color{ 1, 1, 1 };   // white
	DirectX::XMFLOAT3 position{ 0, 0, 0 };
	const float posMultiplier = 50.0f;
	const float gameObjCoordsStride = 20.0f;


	// seed the random generator with the current time
	srand(static_cast<unsigned int>(time(NULL)));

	for (auto & elem : renderableGameObjectsList_)
	{
		// generate a random RGB colour for the game object
		color.x = static_cast<float>(rand()) / RAND_MAX;
		color.y = static_cast<float>(rand()) / RAND_MAX;
		color.z = static_cast<float>(rand()) / RAND_MAX;

		// generate a random position in from of the viewer for the game object
		position.x = (static_cast<float>(rand()) / RAND_MAX) * posMultiplier + gameObjCoordsStride;
		position.y = (static_cast<float>(rand()) / RAND_MAX) * posMultiplier + 5.0f;
		position.z = (static_cast<float>(rand()) / RAND_MAX) * posMultiplier + gameObjCoordsStride;

		elem.second->SetColor(color);
		elem.second->SetPosition(position);
	}


	return;

} // end GenerateDataForGameObjects
#endif