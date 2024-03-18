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






InitializeGraphics::InitializeGraphics(GraphicsClass* pGraphics)
{
	Log::Debug(LOG_MACRO);

	// check input params
	assert(pGraphics != nullptr);
	
	try
	{
		// as we will use these pointers too often during initialization we make
		// local copies of it
		pGraphics_ = pGraphics;
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(LOG_MACRO, e.what());
		Log::Error(LOG_MACRO, "can't allocate memory for members of the class");
	}
}




////////////////////////////////////////////////////////////////////////////////////////////
//
//                                PUBLIC FUNCTIONS
//
////////////////////////////////////////////////////////////////////////////////////////////

bool InitializeGraphics::InitializeDirectX(HWND hwnd,
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
		bool result = pGraphics_->d3d_.Initialize(hwnd,
			windowWidth,
			windowHeight,
			vSyncEnabled,
			isFullScreenMode,
			enable4xMSAA,
			nearZ,
			farZ);
		COM_ERROR_IF_FALSE(result, "can't initialize the Direct3D");

		// setup the rasterizer state
		pGraphics_->d3d_.SetRenderState(D3DClass::RASTER_PARAMS::CULL_MODE_BACK);
		pGraphics_->d3d_.SetRenderState(D3DClass::RASTER_PARAMS::FILL_MODE_SOLID);
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
	GraphicsClass::ShadersContainer & shaders)
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

bool InitializeGraphics::InitializeScene(ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,
	HWND hwnd,
	ModelsStore & modelsStore,
	Settings & settings,
	const UINT windowWidth,
	const UINT windowHeight,
	const float nearZ,               // near Z-coordinate of the frustum
	const float farZ,                // far Z-coordinate of the frustum
	const float fovDegrees,          // field of view
	const float cameraSpeed,         // camera movement speed
	const float cameraSensitivity)   // camera rotation speed
{
	// this function initializes all the scene elements

	try
	{
		bool result = false;

		///////////////////////////////////////////////////
		//  SETUP CAMERAS AND VIEW MATRICES
		///////////////////////////////////////////////////

		// calculate the aspect ratio
		const float aspectRatio = (float)windowWidth / (float)windowHeight;

		CameraClass & editorCamera = pGraphics_->GetEditorCamera();
		CameraClass & cameraForRenderToTexture = pGraphics_->GetCameraForRenderToTexture();

		editorCamera.Initialize(cameraSpeed, cameraSensitivity);              // initialize the editor's camera object
		cameraForRenderToTexture.Initialize(cameraSpeed, cameraSensitivity);  // initialize the camera which is used for rendering into textures
		

		// setup the editor camera
		editorCamera.SetPosition({ 0.0f, 0.0f, -3.0f });
		editorCamera.SetProjectionValues(fovDegrees, aspectRatio, nearZ, farZ);

		// initialize view matrices for the editor camera
		editorCamera.UpdateViewMatrix();
		pGraphics_->baseViewMatrix_ = editorCamera.GetViewMatrix(); // initialize a base view matrix with the camera for 2D user interface rendering


		// setup the camera for rendering to textures
		cameraForRenderToTexture.SetPosition({ 0.0f, 0.0f, -5.0f });
		cameraForRenderToTexture.SetProjectionValues(fovDegrees, aspectRatio, nearZ, farZ);



		///////////////////////////////////////////////////
		//  CREATE AND INIT RELATED TO TEXTURES STUFF
		///////////////////////////////////////////////////

		// initialize a textures manager
		pGraphics_->pTextureManager_ = std::make_unique<TextureManagerClass>(pDevice);

		// initialize the render to texture object
		result = pGraphics_->pRenderToTexture_->Initialize(pDevice, 256, 256, farZ, nearZ, 1);
		COM_ERROR_IF_FALSE(result, "can't initialize the render to texture object");



		///////////////////////////////////////////////////
		//  CREATE AND INIT SCENE ELEMENTS
		///////////////////////////////////////////////////

		// initialize all the models on the scene
		if (!InitializeModels(pDevice, pDeviceContext, modelsStore, settings, farZ))           
			return false;

		// initialize all the light sources on the scene
		if (!InitializeLight(settings))
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
} // end InitializeScene

///////////////////////////////////////////////////////////

void PreparePositionsRotationsForModelsToInit(
	const UINT numOfModels,
	_Inout_ std::vector<DirectX::XMFLOAT3> & outPosition,
	_Inout_ std::vector<DirectX::XMVECTOR> & outRotations)
{
	// GENERATE positions and rotations data for the models

	assert(numOfModels > 0);
	
	outPosition.resize(numOfModels);
	outRotations.resize(numOfModels);

	GeometryGenerator geoGen;
	GeometryGenerator::MeshData meshData;

	const float width = sqrtf((float)numOfModels);
	const float depth = width;

	geoGen.CreateGrid(
		width,
		depth,
		(UINT)width,
		(UINT)depth,
		meshData);

	// generate height for each vertex and set color for it according to its height
	for (size_t i = 0; i < meshData.vertices.size(); ++i)
	{
		DirectX::XMFLOAT3 & pos = meshData.vertices[i].position;

		// use a function for making hills for the terrain
		pos.y = 0.5f * (0.3f * (pos.z*sinf(0.1f * pos.x)) + (pos.x * cosf(0.1f * pos.z)));
	}

	// fill in the input array of positions
	for (UINT idx = 0; idx < meshData.vertices.size(); ++idx)
	{
		outPosition[idx] = meshData.vertices[idx].position;
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

#if 1
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
	const UINT numOfCubes)
{
	//
	// CREATE CUBES
	//

	// check if we want to create any cube if no we just return from the function
	if (numOfCubes == 0)
		return;


	// arrays for positions/rotations values of cubes
	std::vector<DirectX::XMFLOAT3> cubesPositions;
	std::vector<DirectX::XMVECTOR> cubesRotations;

	// arrays for positions/rotations modification values for the cubes
	std::vector<DirectX::XMVECTOR> positionModificators;
	std::vector<DirectX::XMVECTOR> rotationModificators;

	// generate positions/rotations data for cubes
	PreparePositionsRotationsForModelsToInit(
		numOfCubes,
		cubesPositions,
		cubesRotations);

	PreparePositionsRotationsModificatorsForModelsToInit(
		numOfCubes,
		positionModificators,
		rotationModificators);

	DirectX::XMFLOAT3 & pos = cubesPositions[0];

	// create a cube which will be a basic cube for creation of the other ones
	const UINT originCube_idx = modelsCreator.CreateCube(pDevice,
		modelsStore,
		{ pos.x, pos.y, pos.z },
		cubesRotations[0]);

	// create a cube models numOfCubes times
	for (UINT counter = 1; counter < numOfCubes; ++counter)
	{

		// upper cube
		const UINT idxOfCopy1 = modelsCreator.CreateCopyOfModelByIndex(
			originCube_idx,
			modelsStore,
			pDevice);

		pos = cubesPositions[counter];
		modelsStore.positions_[idxOfCopy1] = { floorf(pos.x), floorf(pos.y), floorf(pos.z) };
#if 0
		// bottom cube
		const UINT idxOfCopy2 = modelsCreator.CreateCopyOfModelByIndex(
			originCube_idx,
			modelsStore,
			pDevice);

		modelsStore.positions_[idxOfCopy2] = { floorf(pos.x), floorf(pos.y) - 1.0f, floorf(pos.z) };

		// bottom cube
		const UINT idxOfCopy3 = modelsCreator.CreateCopyOfModelByIndex(
			originCube_idx,
			modelsStore,
			pDevice);

		modelsStore.positions_[idxOfCopy3] = { floorf(pos.x), floorf(pos.y) - 2.0f, floorf(pos.z) };
#endif
	}


	// set that we want to render cubes using some particular shader
	const UINT cubeVertexBufferIdx = modelsStore.relatedToVertexBufferByIdx_[originCube_idx];
	modelsStore.useShaderForBufferRendering_[cubeVertexBufferIdx] = ModelsStore::DIFFUSE_LIGHT_SHADER;


	// set desired positions and rotations for copies of the origin cube
	const UINT skipFirstCube = 1;

	//std::copy(cubesPositions.begin() + skipFirstCube, cubesPositions.end(), modelsStore.positions_.begin() + originCube_idx);
	std::copy(cubesRotations.begin() + skipFirstCube, cubesRotations.end(), modelsStore.rotations_.begin() + originCube_idx);

	// apply the positions/rotations modificators
	std::copy(positionModificators.begin() + skipFirstCube, positionModificators.end(), modelsStore.positionsModificators_.begin() + originCube_idx);
	std::copy(rotationModificators.begin() + skipFirstCube, rotationModificators.end(), modelsStore.rotationModificators_.begin() + originCube_idx);

	// clear the transient initialization data
	cubesPositions.clear();
	cubesRotations.clear();
	positionModificators.clear();
	rotationModificators.clear();

	///////////////////////////////
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
		{ editorGridPositions[0].x, 0.0f, editorGridPositions[0].y, 1 },
		{ 0, 0, 0, 0 },
		editorGridMesh.vertices,
		editorGridMesh.indices,
		std::vector<TextureClass*> { TextureManagerClass::Get()->GetTextureByKey("unloaded_texture") });

	// set that we want to render editor grid using topology linelist
	const UINT editorVertexBufferIdx = modelsStore.relatedToVertexBufferByIdx_[originEditorGridIdx];
	modelsStore.usePrimTopologyForBuffer_[editorVertexBufferIdx] = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;

	for (UINT i = 1; i < editorGridPositionsCount*editorGridPositionsCount; ++i)
	{
		const UINT idxOfCopy = modelsStore.CreateCopyOfModelByIndex(pDevice, originEditorGridIdx);
		modelsStore.positions_[idxOfCopy] = { editorGridPositions[i].x, 0.0f, editorGridPositions[i].y, 1 };
	}


	editorGridPositions.clear();
}

///////////////////////////////////////////////////////////

bool InitializeGraphics::InitializeModels(ID3D11Device* pDevice, 
	ID3D11DeviceContext* pDeviceContext,
	ModelsStore & modelsStore,
	Settings & settings,
	const float farZ)
{
	// initialize all the list of models on the scene

	Log::Print("---------------- INITIALIZATION: MODELS -----------------");

	Log::Debug(LOG_MACRO);

	try
	{
		ModelsCreator modelsCreator;
		GeometryGenerator geoGen;

		// first of all we have to initialize the models store
		modelsStore.Initialize(settings);

		// create and initialize the frustum object
		pGraphics_->pFrustum_ = new FrustumClass();
		pGraphics_->pFrustum_->Initialize(farZ);

		///////////////////////////////

		// define how many models we want to create
		const UINT numOfCubes = settings.GetSettingIntByKey("CUBES_NUMBER");
		const UINT numOfCylinders = settings.GetSettingIntByKey("CYLINDERS_NUMBER");
		const UINT numOfSpheres = settings.GetSettingIntByKey("SPHERES_NUMBER");

		// CREATE CUBES
		CreateCubes(pDevice, modelsCreator, modelsStore, numOfCubes);

		// CREATE TERRAIN
		CreateTerrain(pDevice, modelsCreator, modelsStore);


		//
		// CREATE SPHERES
		//

		for (UINT idx = 0; idx < numOfSpheres; ++idx)
		{
			const UINT originSphere_idx = modelsCreator.CreateSphere(pDevice,
				modelsStore, 
				{ 0, 20, 0 },
				{ 0,0,0 });
			modelsStore.SetTextureByIndex(originSphere_idx, "data/textures/gigachad.dds", aiTextureType_DIFFUSE);

			// set that we want to render using this topology
			const UINT sphereVertexBufferIdx = modelsStore.relatedToVertexBufferByIdx_[originSphere_idx];
			modelsStore.usePrimTopologyForBuffer_[sphereVertexBufferIdx] = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

			// set that we want to render using some particular shader
			//const UINT sphereVertexBufferIdx = modelsStore.relatedToVertexBufferByIdx_[originSphere_idx];
			modelsStore.useShaderForBufferRendering_[sphereVertexBufferIdx] = ModelsStore::TEXTURE_SHADER;
		}

		// create a plane model
		//modelsCreator.CreatePlane(pDevice, models_, { 0,0,0 }, { 0,0,0 });

#if 0


		// load height data for the models
		const std::string bmpHeightmap{ "data/terrain/heightmap.bmp" };
		std::vector<uint8_t> heightData;

		UINT textureWidth;
		UINT textureHeight;
		ImageReader imageReader;
		imageReader.ReadRawImageData(bmpHeightmap, textureWidth, textureHeight, heightData);
#endif




		//
		// CREATE CYLINDERS
		//
		for (UINT idx = 0; idx < numOfCylinders; ++idx)
		{
			// define cylinder params
			const float bottomRadius = 5.0f;
			const float topRadius = 3.0f;
			const float height = 20.0f;
			const UINT sliceCount = 10;
			const UINT stackCount = 10;

			// create a new cylinder model
			const UINT cylinderIdx = modelsCreator.CreateCylinder(pDevice,
				modelsStore,
				{ 0, 10, 0, 1 },
				{ 0, 0, 0, 1 },
				DirectX::XMVectorZero(),
				DirectX::XMVectorZero(),
				bottomRadius, topRadius,
				height,
				sliceCount, stackCount);

			// set that we want to render cubes using some particular shader
			const UINT cylinderVertexBufferIdx = modelsStore.relatedToVertexBufferByIdx_[cylinderIdx];
			modelsStore.useShaderForBufferRendering_[cylinderVertexBufferIdx] = ModelsStore::DIFFUSE_LIGHT_SHADER;

		}


		//
		// CREATE AXIS
		//
#if 0
		// create a simple cube which will be a part of axis visual navigation
		modelsCreator.CreateCube(pDevice,
			modelsStore,
			{ 0,0,0 },
			{ 0,0,0 });
#endif

		// generate data for the axis
		GeometryGenerator::MeshData axisMeshData;
		geoGen.CreateAxis(axisMeshData);

		// create an axis model

		const UINT axisModelIdx = modelsStore.CreateNewModelWithData(pDevice,
			"axis",
			{ 0, 0.001f, 0, 1 },
			{ 0, 0, 0, 0 },
			axisMeshData.vertices,
			axisMeshData.indices,
			std::vector<TextureClass*> { TextureManagerClass::Get()->GetTextureByKey("unloaded_texture") });

		// set that we want to render axis using topology linelist
		const UINT axisVertexBufferIdx = modelsStore.relatedToVertexBufferByIdx_[axisModelIdx];
		modelsStore.usePrimTopologyForBuffer_[axisVertexBufferIdx] = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;

		
		//
		// CREATE EDITOR GRID
		//
		CreateEditorGrid(pDevice, modelsCreator, modelsStore);

		modelsStore.UpdateModels(0);
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
	

		// create and initialize the zone class object
		pGraphics_->pZone_ = new ZoneClass(&pGraphics_->GetEditorCamera());

		bool result = pGraphics_->pZone_->Initialize(farZ, cameraHeightOffset);
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

bool InitializeGraphics::InitializeLight(Settings & settings)
{
	// this function initializes all the light sources on the scene

	Log::Print("---------------- INITIALIZATION: LIGHT SOURCES -----------------");
	Log::Debug(LOG_MACRO);


	const DirectX::XMFLOAT3 ambientColorOn{ 0.3f, 0.3f, 0.3f };
	const DirectX::XMFLOAT3 ambientColorOff{ 0, 0, 0 };

	const UINT numDiffuseLights = settings.GetSettingIntByKey("NUM_DIFFUSE_LIGHTS");
	const UINT numPointLights   = settings.GetSettingIntByKey("NUM_POINT_LIGHTS");

	LightStore & lightStore = pGraphics_->lightsStore_;

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
		pGraphics_->userInterface_.Initialize(pDevice,
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