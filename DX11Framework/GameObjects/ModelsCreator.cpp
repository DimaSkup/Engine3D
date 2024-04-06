////////////////////////////////////////////////////////////////////////////////////////////
// Filename:        ModelsCreator.cpp
// Description:     implementation of the functional of the ModelsCreator class
//
// Created:         12.02.24
////////////////////////////////////////////////////////////////////////////////////////////

#include "ModelsCreator.h"
#include "TextureManagerClass.h"
#include "TerrainInitializer.h"
#include "ModelMath.h"


void ModelsCreator::LoadParamsForDefaultModels(
	Settings & settings,
	ModelsCreator::WAVES_PARAMS & wavesParams,
	ModelsCreator::CYLINDER_PARAMS & cylParams,
	ModelsCreator::SPHERE_PARAMS & sphereParams,
	ModelsCreator::GEOSPHERE_PARAMS & geosphereParams,
	ModelsCreator::PYRAMID_PARAMS & pyramidParams)
{
	// load params for waves
	wavesParams.numRows = settings.GetSettingIntByKey("WAVES_NUM_ROWS");
	wavesParams.numColumns = settings.GetSettingIntByKey("WAVES_NUM_COLUMNS");
	wavesParams.spatialStep = settings.GetSettingFloatByKey("WAVES_SPATIAL_STEP");
	wavesParams.timeStep = settings.GetSettingFloatByKey("WAVES_TIME_STEP");
	wavesParams.speed = settings.GetSettingFloatByKey("WAVES_SPEED");
	wavesParams.damping = settings.GetSettingFloatByKey("WAVES_DAMPING");

	// load params for cylinders
	cylParams.height = settings.GetSettingFloatByKey("CYLINDER_HEIGHT");
	cylParams.bottomRadius = settings.GetSettingFloatByKey("CYLINDER_BOTTOM_CAP_RADIUS");
	cylParams.topRadius = settings.GetSettingFloatByKey("CYLINDER_TOP_CAP_RADIUS");
	cylParams.sliceCount = settings.GetSettingIntByKey("CYLINDER_SLICE_COUNT");
	cylParams.stackCount = settings.GetSettingIntByKey("CYLINDER_STACK_COUNT");

	// load params for spheres
	sphereParams.radius = settings.GetSettingFloatByKey("SPHERE_RADIUS");
	sphereParams.sliceCount = settings.GetSettingIntByKey("SPHERE_SLICE_COUNT");
	sphereParams.stackCount = settings.GetSettingIntByKey("SPHERE_STACK_COUNT");

	// load params for geospheres
	geosphereParams.radius = settings.GetSettingFloatByKey("GEOSPHERE_RADIUS");
	geosphereParams.numSubdivisions = settings.GetSettingIntByKey("GEOSPHERE_NUM_SUBDIVISITIONS");

	// load params for pyramids
	pyramidParams.height = settings.GetSettingFloatByKey("PYRAMID_HEIGHT");
	pyramidParams.baseWidth = settings.GetSettingFloatByKey("PYRAMID_BASE_WIDTH");
	pyramidParams.baseDepth = settings.GetSettingFloatByKey("PYRAMID_BASE_DEPTH");
}

///////////////////////////////////////////////////////////

const UINT ModelsCreator::CreatePlane(ID3D11Device* pDevice, 
	ModelsStore & modelsStore,
	const DirectX::XMVECTOR & inPosition,
	const DirectX::XMVECTOR & inDirection,
	const DirectX::XMVECTOR & inPosModification,  // position modification factors
	const DirectX::XMVECTOR & inRotModification)  // rotation modification factors
{
	// THIS FUNCTION creates a basic empty plane model data and adds this model
	// into the ModelsStore storage

	// this flag means that we want to create a default vertex buffer for the mesh of this sprite
	const bool isVertexBufferDynamic = false;

	// since each 2D sprite is just a plane it has 4 vertices and 6 indices
	const UINT vertexCount = 4;
	const UINT indexCount = 6;

	// arrays for vertices/indices data
	std::vector<VERTEX> verticesArr(vertexCount);
	std::vector<UINT> indicesArr(indexCount);

	/////////////////////////////////////////////////////

	// setup the vertices positions

	// top left / bottom right 
	verticesArr[0].position = { -1,  1,  0 };    
	verticesArr[1].position = {  1, -1,  0 }; 

	// bottom left / top right
	verticesArr[2].position = { -1, -1,  0 };   
	verticesArr[3].position = {  1,  1,  0 };  

	// setup the texture coords of each vertex
	verticesArr[0].texture = { 0, 0 };
	verticesArr[1].texture = { 1, 1 };
	verticesArr[2].texture = { 0, 1 };
	verticesArr[3].texture = { 1, 0 };

	// setup the indices
	indicesArr.insert(indicesArr.begin(), { 0, 1, 2, 0, 3, 1 });

	// create an empty texture for this plane
	std::vector<TextureClass*> texturesArr { TextureManagerClass::Get()->GetTextureByKey("unhandled_texture") };

	/////////////////////////////////////////////////////

	// create a new model using prepared data and return its index
	return modelsStore.CreateNewModelWithData(
		pDevice,
		"plane",
		verticesArr,
		indicesArr,
		texturesArr,
		inPosition,
		inDirection,
		inPosModification,
		inRotModification);
}

///////////////////////////////////////////////////////////

const UINT ModelsCreator::CreateCube(ID3D11Device* pDevice, 
	ModelsStore & modelsStore,
	const std::string & filepath,
	const DirectX::XMVECTOR & inPosition,
	const DirectX::XMVECTOR & inDirection,
	const DirectX::XMVECTOR & inPosModification,  // position modification factors
	const DirectX::XMVECTOR & inRotModification)  // rotation modification factors

{
	// THIS FUNCTION creates a BASIC cube and stores it into the ModelsStore storage;
	//
	// we can create cube manually if the input filepath parameter is empty;
	// or load cube data from the file by filepath;


	// MANUALLY CREATE A CUBE
	if (filepath.empty())
	{
		GeometryGenerator geoGen;
		GeometryGenerator::MeshData cubeMesh;

		geoGen.CreateCubeMesh(cubeMesh);

		// create an empty texture for this plane
		std::vector<TextureClass*> texturesArr;
		//texturesArr.push_back(TextureManagerClass::Get()->GetTextureByKey("unloaded_texture"));
		texturesArr.push_back(TextureManagerClass::Get()->GetTextureByKey("data/textures/gigachad.dds"));

		const UINT cubeIdx = modelsStore.CreateNewModelWithData(
			pDevice,
			"cube",
			cubeMesh.vertices,
			cubeMesh.indices,
			texturesArr,
			inPosition,
			inDirection,
			inPosModification,
			inRotModification);

		return cubeIdx;
	}

	// CREATE A CUBE FROM FILE
	else
	{
		// create a new model using prepared data and return its index
		return modelsStore.CreateModelFromFile(
			pDevice,
			"data/models/minecraft-grass-block/source/Grass_Block.obj",
			"cube",
			//"data/models/default/cube_simple.obj",
			inPosition,
			inDirection,
			inPosModification,
			inRotModification);
	}
}

///////////////////////////////////////////////////////////

const UINT ModelsCreator::CreatePyramid(ID3D11Device* pDevice,
	ModelsStore & modelsStore,
	const float height,                                // height of the pyramid
	const float baseWidth,                             // width (length by X) of one of the base side
	const float baseDepth,                             // depth (length by Z) of one of the base side
	const DirectX::XMVECTOR & inPosition,
	const DirectX::XMVECTOR & inDirection,
	const DirectX::XMVECTOR & inPosModification,       // position modification factors
	const DirectX::XMVECTOR & inRotModification)       // rotation modification factors
{
	GeometryGenerator geoGen;
	GeometryGenerator::MeshData pyramidMesh;

	geoGen.CreatePyramidMesh(height, baseWidth, baseDepth, pyramidMesh);

	const UINT pyramidIdx = modelsStore.CreateNewModelWithData(pDevice,
		"pyramid",
		pyramidMesh.vertices,
		pyramidMesh.indices,
		{ TextureManagerClass::Get()->GetTextureByKey("unloaded_texture") },  // default texture
		inPosition,
		inDirection, 
		inPosModification,
		inRotModification);

	// return an index to the created pyramid model
	return pyramidIdx;
}

///////////////////////////////////////////////////////////

const UINT ModelsCreator::CreateWaves(ID3D11Device* pDevice,
	ModelsStore & modelsStore,
	const ModelsCreator::WAVES_PARAMS & params,
	const DirectX::XMVECTOR & inPosition,
	const DirectX::XMVECTOR & inDirection,
	const DirectX::XMVECTOR & inPosModification,  // position modification
	const DirectX::XMVECTOR & inRotModification)  // rotation modification
{
	//
	// create a new waves model
	//

	const bool isDynamic = true;  // a vertex buffer of waves will be dynamic
	VertexBuffer<VERTEX> VB;
	IndexBuffer IB;
	GeometryGenerator geoGen;
	GeometryGenerator::MeshData wavesMesh;

	// generate a waves mesh with random shape
	geoGen.CreateWavesMesh(
		params.numRows,
		params.numColumns,
		params.spatialStep,
		params.timeStep,
		params.speed,
		params.damping,
		modelsStore.waves_,
		wavesMesh);


	// create a vertex and index buffer for the waves mesh
	VB.Initialize(pDevice, "waves", wavesMesh.vertices, isDynamic);
	IB.Initialize(pDevice, wavesMesh.indices);

	// create a new waves model using created vertex and index buffers
	const UINT waves_idx = modelsStore.CreateNewModelWithData(pDevice, "waves",
		inPosition,
		inDirection,
		VB,
		IB,
		{ TextureManagerClass::Get()->GetTextureByKey("unloaded_texture") },
		inPosModification,
		inRotModification);

	return waves_idx;
}

///////////////////////////////////////////////////////////

const UINT ModelsCreator::CreateSphere(ID3D11Device* pDevice,
	ModelsStore & modelsStore,
	const float radius,
	const UINT sliceCount,
	const UINT stackCount,
	const DirectX::XMVECTOR & inPosition,
	const DirectX::XMVECTOR & inDirection,
	const DirectX::XMVECTOR & inPosModification,  // position modification factors
	const DirectX::XMVECTOR & inRotModification)  // rotation modification factors
{
	GeometryGenerator geoGen;
	GeometryGenerator::MeshData sphereMesh;

	geoGen.CreateSphereMesh(radius, sliceCount, stackCount, sphereMesh);

	return modelsStore.CreateNewModelWithData(pDevice,
		"sphere",
		sphereMesh.vertices,
		sphereMesh.indices,
		{ TextureManagerClass::Get()->GetTextureByKey("unloaded_texture") },  // default texture
		inPosition,
		inDirection,
		inPosModification,
		inRotModification);

#if 0
	// create a new model using prepared data and return its index
	return modelsStore.CreateModelFromFile(
		pDevice,
		"data/models/default/sphere.obj",
		"sphere",
		inPosition,
		inDirection);
#endif
}

///////////////////////////////////////////////////////////

const UINT ModelsCreator::CreateGeophere(ID3D11Device* pDevice,
	ModelsStore & modelsStore,
	const float radius,
	const UINT numSubdivisions)
{
	GeometryGenerator geoGen;
	GeometryGenerator::MeshData sphereMesh;

	geoGen.CreateGeosphereMesh(radius, numSubdivisions, sphereMesh);

	return modelsStore.CreateNewModelWithData(pDevice,
		"geosphere",
		sphereMesh.vertices,
		sphereMesh.indices,
		{ TextureManagerClass::Get()->GetTextureByKey("unloaded_texture") },  // default texture
		{ 5, 0, 0, 1 },//inPosition,
		{ 0, 0, 0, 0 },//inDirection,
		{ 0,0,0,0 }, // inPosModification,
		{ 0,0,0,0 }); // inRotModification);
}

///////////////////////////////////////////////////////////

const UINT ModelsCreator::CreateCylinder(ID3D11Device* pDevice,
	ModelsStore & modelsStore,
	const ModelsCreator::CYLINDER_PARAMS & cylParams,
	const DirectX::XMVECTOR & inPosition,
	const DirectX::XMVECTOR & inDirection,
	const DirectX::XMVECTOR & inPosModification,  // position modification;
	const DirectX::XMVECTOR & inRotModification)  // rotation modification;
{
	GeometryGenerator geoGen;
	GeometryGenerator::MeshData cylinderMeshes;

	// generate geometry of cylinder by input params
	geoGen.CreateCylinderMesh(
		cylParams.bottomRadius, 
		cylParams.topRadius,
		cylParams.height, 
		cylParams.sliceCount, 
		cylParams.stackCount, 
		cylinderMeshes);

	// add this cylinder into the models store
	const UINT cylinderID = modelsStore.CreateNewModelWithData(pDevice,
		"cylinder",
		cylinderMeshes.vertices,  // vertices data of cylinder
		cylinderMeshes.indices,   // indices data of vertices
		{ TextureManagerClass::Get()->GetTextureByKey("unloaded_texture") },  // default texture
		inPosition,
		inDirection,
		inPosModification,
		inRotModification);

	// return an index to the cylinder model
	return cylinderID;
}


///////////////////////////////////////////////////////////

const UINT ModelsCreator::CreateGrid(ID3D11Device* pDevice,
	ModelsStore & modelsStore,
	const float gridWidth,
	const float gridDepth,
	const DirectX::XMVECTOR & inPosition,          // initial position
	const DirectX::XMVECTOR & inDirection,
	const DirectX::XMVECTOR & inPosModification,   // position modification 
	const DirectX::XMVECTOR & inRotModification)
{
	// CREATE PLAIN GRID

	GeometryGenerator geoGen;
	GeometryGenerator::MeshData grid;

	geoGen.CreateGridMesh(
		gridWidth, 
		gridDepth, 
		(UINT)gridWidth + 1,  // num of quads by X
		(UINT)gridDepth + 1,  // num of quads by Z
		grid);

	// add this grid into the models store
	const UINT gridID = modelsStore.CreateNewModelWithData(pDevice,
		"grid",
		grid.vertices,
		grid.indices,
		{ TextureManagerClass::Get()->GetTextureByKey("unloaded_texture") }, // default texture
		inPosition,
		inDirection,
		inPosModification,
		inRotModification);

	// setup primitive topology for the vertex buffer of the grid
	//const UINT gridVertexBufferIdx = modelsStore.GetRelatedVertexBufferByModelIdx(gridID);
	//modelsStore.SetPrimitiveTopologyForVertexBufferByIdx(gridVertexBufferIdx, D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	// return an index to the grid model
	return gridID;
}

///////////////////////////////////////////////////////////

const UINT ModelsCreator::CreateGeneratedTerrain(ID3D11Device* pDevice,
	ModelsStore & modelsStore,
	const float terrainWidth,
	const float terrainDepth,
	const UINT verticesCountByX,
	const UINT verticesCountByZ)
{
	//
	// CREATE TERRAIN GRID
	//
	GeometryGenerator geoGen;
	GeometryGenerator::MeshData grid;

	geoGen.CreateGridMesh(terrainWidth, terrainDepth, verticesCountByX, verticesCountByZ, grid);

	// generate height for each grid vertex
	GenerateHeightsForGrid(grid);

	

	// compute normals, tangents, and bitangents for this terrain grid
	//ModelMath modelMath;
	//modelMath.CalculateModelVectors(grid.vertices, true);

#if 1
	// PAINT GRID VERTICES WITH RAINBOW
	PaintGridWithRainbow(grid, verticesCountByX, verticesCountByZ);
#elif 0
	// PAINT VERTICES OF GRID LIKE IT IS HILLS (according to its height)
	PaintGridAccordingToHeights(grid);
#endif
	

	// add this terrain grid into the models store
	const UINT terrainGridID = modelsStore.CreateNewModelWithData(pDevice,
		"terrain_grid",
		grid.vertices,
		grid.indices,
		std::vector<TextureClass*> { TextureManagerClass::Get()->GetTextureByKey("unloaded_texture") }, // default texture
		{ 0,0,0,1 },   // place at the center of the world
		{ 0,0,0,0 },   // no rotation
		{ 0,0,0,1 },   // no position changes
		{ 0,0,0,0 });  // no rotation changes

	// return an index to the terrain grid model
	return terrainGridID;
}

///////////////////////////////////////////////////////////

const UINT ModelsCreator::CreateTerrainFromFile(
	const std::string & terrainSetupFile,
	ID3D11Device* pDevice,
	ModelsStore & modelsStore)
{
	TerrainInitializer terrainInitializer;

	terrainInitializer.LoadSetupFile(terrainSetupFile);

	const TerrainInitializer::TerrainSetupData & setupData = terrainInitializer.GetSetupData();

	//
	// CREATE TERRAIN GRID
	//
	GeometryGenerator geoGen;
	GeometryGenerator::MeshData grid;

	geoGen.CreateGridMesh(
		static_cast<float>(setupData.terrainWidth), 
		static_cast<float>(setupData.terrainDepth),
		setupData.terrainWidth,        // how many quads will we have along X-axis
		setupData.terrainDepth,        // how many quads will we have along Z-axis
		grid);


	// generate height for each vertex and set color for it according to its height
	GenerateHeightsForGrid(grid);
	PaintGridAccordingToHeights(grid);

	// add this terrain grid into the models store
	const UINT terrainGridID = modelsStore.CreateNewModelWithData(pDevice,
		"terrain_grid",
		grid.vertices,
		grid.indices,
		{ TextureManagerClass::Get()->GetTextureByKey("unloaded_texture") },  // default texture
		{ 0,0,0,1 },   // place at the center of the world
		{ 0,0,0,0 },   // no rotation
		{ 0,0,0,1 },   // no position changes
		{ 0,0,0,0 });  // no rotation changes

	// return an index to the terrain grid model
	return terrainGridID;
}

///////////////////////////////////////////////////////////

const UINT ModelsCreator::CreateOneCopyOfModelByIndex(const UINT index,
	ModelsStore & modelsStore,
	ID3D11Device* pDevice)
{
	// create a copy of the origin model and return an ID of this copy
	return modelsStore.CreateOneCopyOfModelByIndex(pDevice, index);
}

///////////////////////////////////////////////////////////

const UINT ModelsCreator::CreateChunkBoundingBox(const UINT chunkDimension,
	ModelsStore & modelsStore,
	ID3D11Device* pDevice)
{
	// creates the bouding box that surrounds the terrain cell. It is made up of series of 
	// lines creating a box around the exact dimensions of the terrain cell. This is used
	// for debugging purposes mostly

	constexpr UINT vertexCount = 8;    // set the number of line box vertices in the vertex array
	const float halfDimension = 0.5f * (float)chunkDimension;
	const float min = -halfDimension;
	const float max = halfDimension;

	const DirectX::XMFLOAT3 minDimension { min, min, min };
	const DirectX::XMFLOAT3 maxDimension { max, max, max };

	// arrays for vertices/indices data
	std::vector<VERTEX> verticesDataArr(vertexCount);
	std::vector<UINT> indicesDataArr;

	// setup vertices position of the bounding box:

	// bottom side of the box
	verticesDataArr[0].position = { minDimension.x, minDimension.y, minDimension.z };  // near left
	verticesDataArr[1].position = { maxDimension.x, minDimension.y, minDimension.z };  // near right
	verticesDataArr[2].position = { maxDimension.x, minDimension.y, maxDimension.z };  // far right
	verticesDataArr[3].position = { minDimension.x, minDimension.y, maxDimension.z };

	// top side of the box
	verticesDataArr[4].position = { minDimension.x, maxDimension.y, minDimension.z };  // near left
	verticesDataArr[5].position = { maxDimension.x, maxDimension.y, minDimension.z };  // near right
	verticesDataArr[6].position = { maxDimension.x, maxDimension.y, maxDimension.z };  // far right
	verticesDataArr[7].position = { minDimension.x, maxDimension.y, maxDimension.z };


	// setup the indices for the cell lines box
	indicesDataArr.insert(indicesDataArr.begin(), {

		// bottom
		0, 1, 0,
		1, 2, 1,
		2, 3, 2,
		3, 0, 3,

		// front
		4, 5, 4,
		5, 1, 5,
		1, 0, 1,
		0, 4, 0,

		// top
		7, 6, 7,
		6, 5, 6,
		5, 4, 5,
		4, 7, 4,

		// back
		6, 7, 6,
		7, 3, 7,
		3, 2, 3,
		2, 6, 2,

		// left
		7, 4, 7,
		4, 0, 4,
		0, 3, 0,
		3, 7, 3,

		// right
		5, 6, 5,
		6, 2, 6,
		2, 1, 2,
		1, 5, 1
	});

	const UINT chunkBoundingBoxIdx = modelsStore.CreateNewModelWithData(pDevice,
		"chunk_bounding_box",
		verticesDataArr,
		indicesDataArr,
		{ TextureManagerClass::Get()->GetTextureByKey("unloaded_texture") },  // default texture
		DirectX::XMVectorZero(),  // position
		DirectX::XMVectorZero(),  // rotation
		DirectX::XMVectorZero(),  // position modificator
		DirectX::XMVectorZero()); // rotation modificator

	return chunkBoundingBoxIdx;
}


///////////////////////////////////////////////////////////////////////////////////////////
//                               PRIVATE HELPERS API
///////////////////////////////////////////////////////////////////////////////////////////

void ModelsCreator::GenerateHeightsForGrid(GeometryGenerator::MeshData & grid)
{
	// generate height for the input grid by some particular function;
	// (there can be several different types of height generation)

#if 1
	for (UINT idx = 0; idx < grid.vertices.size(); ++idx)
	{
		DirectX::XMFLOAT3 & pos = grid.vertices[idx].position;

		// a function for making hills for the terrain
		pos.y = 0.3f * (pos.z*sinf(0.1f * pos.x)) + (pos.x * cosf(0.1f * pos.z));

		// get hill normal
		// n = (-df/dx, 1, -df/dz)
		DirectX::XMVECTOR normalVec{
		   -0.03f*pos.z * cosf(0.1f*pos.x) - 0.3f*cosf(0.1f*pos.z),
		   1.0f,
		   -0.3f*sinf(0.1f*pos.x) + 0.03f*pos.x*sinf(0.1f*pos.z) };

		normalVec = DirectX::XMVector3Normalize(normalVec);
		DirectX::XMStoreFloat3(&grid.vertices[idx].normal, normalVec);
	}

#else
	// generate heights for the grid
	const float sin_step = DirectX::XM_PI / m * 3.0f;
	const float cos_step = DirectX::XM_PI / n * 5.0f;
	float valForSin = 0.0f;
	float valForCos = 0.0f;

	for (UINT i = 0; i < m; ++i)
	{
		valForSin = 0.0f;
		for (UINT j = 0; j < n; ++j)
		{
			const UINT idx = i*n + j;
			grid.vertices[idx].position.y = 30 * (sinf(valForSin) - cosf(valForCos));

			valForSin += sin_step;

		}
		valForCos += cos_step;
	}
#endif
}

///////////////////////////////////////////////////////////

void ModelsCreator::PaintGridAccordingToHeights(GeometryGenerator::MeshData & grid)
{
	// THIS FUNCTION sets a color for the vertices according to its height (Y-coord)

	//const DirectX::XMFLOAT4 sandyBeach(1.0f, 0.96f, 0.62f, 1.0f);
	//const DirectX::XMFLOAT4 lightYellowGreen(0.48f, 0.77f, 0.46f, 1.0f);
	//const DirectX::XMFLOAT4 darkYellowGreen(0.1f, 0.48f, 0.19f, 1.0f);
	//const DirectX::XMFLOAT4 darkBrown(0.45f, 0.39f, 0.34f, 1.0f);
	//const DirectX::XMFLOAT4 whiteSnow(1.0f, 1.0f, 1.0f, 1.0f);


	const DirectX::PackedVector::XMCOLOR sandyBeach(1.0f, 0.96f, 0.62f, 1.0f);
	const DirectX::PackedVector::XMCOLOR lightYellowGreen(0.48f, 0.77f, 0.46f, 1.0f);
	const DirectX::PackedVector::XMCOLOR darkYellowGreen(0.1f, 0.48f, 0.19f, 1.0f);
	const DirectX::PackedVector::XMCOLOR darkBrown(0.45f, 0.39f, 0.34f, 1.0f);
	const DirectX::PackedVector::XMCOLOR whiteSnow(1.0f, 1.0f, 1.0f, 1.0f);;

	for (VERTEX & vertex : grid.vertices)
	{
		const float py = vertex.position.y;

		if (py < -10.0f)
		{
			vertex.color = sandyBeach;
		}
		else if (py < 5.0f)
		{
			vertex.color = lightYellowGreen;
		}
		else if (py < 12.0f)
		{
			vertex.color = darkYellowGreen;
		}
		else if (py < 20.0f)
		{
			vertex.color = darkBrown;
		}
		else // is equal or above 20.0f
		{
			vertex.color = whiteSnow;
		}

	} // end for
}

///////////////////////////////////////////////////////////

void ModelsCreator::PaintGridWithRainbow(GeometryGenerator::MeshData & grid, 
	const UINT verticesCountByX,
	const UINT verticesCountByZ)
{
	// PAINT GRID VERTICES WITH RAINBOW

	const int quadsByX = (int)verticesCountByX - 1;
	const int quadsByZ = (int)verticesCountByZ - 1;
	const float du = 1.0f / quadsByX;
	const float dv = 1.0f / quadsByZ;

	// paint grid vertices with color
	for (UINT i = 0; i < (UINT)verticesCountByX; ++i)
	{
		for (UINT j = 0; j < (UINT)verticesCountByZ; ++j)
		{
			const UINT idx = i*verticesCountByX + j;
			grid.vertices[idx].color = { du * i, 0.5f, dv * j, 1.0f };
		}
	}
}