// ************************************************************************************
// Filename:        ModelsCreator.cpp
// Description:     implementation of the functional of the ModelsCreator class
//
// Created:         12.02.24
// ************************************************************************************

#include "ModelsCreator.h"
#include "TextureManagerClass.h"
#include "TerrainInitializer.h"
#include "ModelMath.h"
#include "MeshStorage.h"
#include "../Engine/Settings.h"
#include "ModelInitializer.h"


ModelsCreator::ModelsCreator()
{
}

// ************************************************************************************


void ModelsCreator::Initialize(MeshStorage* pMeshStorage)
{
	assert(pMeshStorage != nullptr);
	pMeshStorage_ = pMeshStorage;

	defaultTexturesMap_ =
	{
		{ aiTextureType_DIFFUSE, TextureManagerClass::Get()->GetTextureByKey("unloaded_texture") },
		{ aiTextureType_LIGHTMAP, TextureManagerClass::Get()->GetTextureByKey("data/textures/white_lightmap.dds") }
	};
}

///////////////////////////////////////////////////////////

const std::vector<MeshID> ModelsCreator::CreateFromFile(
	ID3D11Device* pDevice,
	const std::string filepath)
{
	ModelInitializer modelInitializer;
	std::vector<MeshID> meshIDs;
	std::vector<Mesh::MeshData> meshes;

	modelInitializer.InitializeFromFile(pDevice, meshes, filepath);
	
	for (Mesh::MeshData& data : meshes)
	{
		data.textures.insert({ aiTextureType_LIGHTMAP, TextureManagerClass::Get()->GetTextureByKey("data/textures/white_lightmap.dds") });

		const MeshID id = pMeshStorage_->CreateMeshWithRawData(pDevice, data.name, data.vertices, data.indices, data.textures);
		meshIDs.push_back(id);
	}

	return meshIDs;
}

///////////////////////////////////////////////////////////

const std::string ModelsCreator::CreatePlane(ID3D11Device* pDevice)
{
	// create new empty plane mesh and store it into the storage;
	// return: plane mesh ID

	// since each 2D sprite is just a plane it has 4 vertices and 6 indices
	const UINT vertexCount = 4;
	const UINT indexCount = 6;

	// arrays for vertices/indices data
	std::vector<VERTEX> verticesArr(vertexCount);
	std::vector<UINT> indicesArr(indexCount);

	// ------------------------------------------------------- //

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

	// ------------------------------------------------------- //

	// store the mesh and return its ID
	return pMeshStorage_->CreateMeshWithRawData(
		pDevice,
		"plane",              // ID (can be modified inside)
		verticesArr,
		indicesArr,
		defaultTexturesMap_);
}

///////////////////////////////////////////////////////////

const std::string ModelsCreator::CreateCube(ID3D11Device* pDevice)

{
	// THIS FUNCTION creates a cube mesh and stores it into the storage;
	// return: cube mesh ID

	GeometryGenerator geoGen;
	Mesh::MeshData cubeMesh;

	// generate vertices and indices for a cube
	geoGen.GenerateCubeMesh(cubeMesh);

	// store the mesh and return its ID
	return pMeshStorage_->CreateMeshWithRawData(
		pDevice,
		"cube",                 // ID (can be modified inside)
		cubeMesh.vertices,
		cubeMesh.indices,
		defaultTexturesMap_);
}

///////////////////////////////////////////////////////////

const std::string ModelsCreator::CreateSkullModel(ID3D11Device* pDevice)
{
	// load skull's mesh data from the file, store this mesh into the storage
	// and return its ID

	std::ifstream fin("data/models/skull.txt");

	if (!fin)
	{
		MessageBoxA(0, "data/models/skull.txt not found", 0, 0);
		ASSERT_TRUE(false, "can't open a file which contains skull model data");
	}

	UINT vCount = 0;
	UINT tCount = 0;
	std::string ignore;

	fin >> ignore >> vCount;
	fin >> ignore >> tCount;
	fin >> ignore >> ignore >> ignore >> ignore;

	std::vector<VERTEX> vertices(vCount);
	for (UINT idx = 0; idx < vCount; ++idx)
	{
		fin >> vertices[idx].position.x >> vertices[idx].position.y >> vertices[idx].position.z;
		fin >> vertices[idx].normal.x >> vertices[idx].normal.y >> vertices[idx].normal.z;
	}

	fin >> ignore >> ignore >> ignore;

	const UINT skullIndexCount = 3 * tCount;
	std::vector<UINT> indices(skullIndexCount);

	for (UINT idx = 0; idx < tCount; ++idx)
	{
		fin >> indices[idx * 3 + 0] >> indices[idx * 3 + 1] >> indices[idx * 3 + 2];
	}

	fin.close();


	// --------------------------------------------------------------------------------

	// store the mesh and return its ID
	return pMeshStorage_->CreateMeshWithRawData(
		pDevice,
		"skull",              // ID (can be modified inside)
		vertices,
		indices,
		defaultTexturesMap_);

#if 0

	// set skull material (material varies per object)
	Material& mat = modelsStore.materials_[skullModel_idx];

	mat.ambient = DirectX::XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	mat.diffuse = DirectX::XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	mat.specular = DirectX::XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f);


#endif
}

///////////////////////////////////////////////////////////

const std::string ModelsCreator::CreateCylinder(
	ID3D11Device* pDevice,
	const ModelsCreator::CYLINDER_PARAMS& cylParams)
{
	// generate new cylinder mesh and store it into the storage;
	// return: mesh ID

	const std::string meshID{ "cylinder" };   // supposed mesh ID
	GeometryGenerator geoGen;
	Mesh::MeshData mesh;
	

	// generate geometry of cylinder by input params
	geoGen.GenerateCylinderMesh(
		cylParams.bottomRadius,
		cylParams.topRadius,
		cylParams.height,
		cylParams.sliceCount,
		cylParams.stackCount,
		mesh);

	// create a new cylinder model and return its index
	return pMeshStorage_->CreateMeshWithRawData(
		pDevice,
		meshID,
		mesh.vertices,
		mesh.indices,
		defaultTexturesMap_);
}


///////////////////////////////////////////////////////////

const std::string ModelsCreator::CreatePyramid(
	ID3D11Device* pDevice,
	const float height,                                // height of the pyramid
	const float baseWidth,                             // width (length by X) of one of the base side
	const float baseDepth)                             // depth (length by Z) of one of the base side
{
	const std::string meshID{ "pyramid" };   // supposed mesh ID
	GeometryGenerator geoGen;
	Mesh::MeshData mesh;

	// generate pyramid's vertices and indices by input params
	geoGen.GeneratePyramidMesh(height, baseWidth, baseDepth, mesh);

	// store the mesh into the mesh storage and return ID of this mesh
	return pMeshStorage_->CreateMeshWithRawData(
		pDevice,
		meshID,
		mesh.vertices,
		mesh.indices,
		defaultTexturesMap_);
}

#if 0
///////////////////////////////////////////////////////////

const UINT ModelsCreator::CreateWaves(ID3D11Device* pDevice,
	EntityStore & modelsStore,
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
	geoGen.GenerateWavesMesh(
		params.numRows,
		params.numColumns,
		params.spatialStep,
		params.timeStep,
		params.speed,
		params.damping,
		modelsStore.waves_,
		wavesMesh);

	// initialize the vertex and index buffer with the raw vertices and indices data
	VB.Initialize(pDevice, "waves", wavesMesh.vertices, isDynamic);
	IB.Initialize(pDevice, wavesMesh.indices);

	// create a new waves model using created vertex and index buffers
	const UINT waves_idx = modelsStore.CreateNewModelWithBuffers(pDevice,
		VB,
		IB,
		"waves",              // text id
		defaultTexturesMap_);

	return waves_idx;
}
#endif



///////////////////////////////////////////////////////////

const std::string ModelsCreator::CreateSphere(
	ID3D11Device* pDevice,
	const float radius,
	const UINT sliceCount,
	const UINT stackCount)
{
	// generate new sphere mesh and store it into the storage;
	// return: mesh ID

	GeometryGenerator geoGen;
	Mesh::MeshData sphereMesh;

	// generate sphere's vertices and indices by input params
	geoGen.GenerateSphereMesh(radius, sliceCount, stackCount, sphereMesh);

	// store the mesh and return its ID
	return pMeshStorage_->CreateMeshWithRawData(
		pDevice,
		"sphere",                // ID (can be modified inside)
		sphereMesh.vertices,
		sphereMesh.indices,
		defaultTexturesMap_);
}

///////////////////////////////////////////////////////////







#if 0


const UINT ModelsCreator::CreateGeophere(ID3D11Device* pDevice,
	EntityStore & modelsStore,
	const float radius,
	const UINT numSubdivisions)
{
	GeometryGenerator geoGen;
	GeometryGenerator::MeshData sphereMesh;

	// generate geosphere's vertices and indices by input params
	geoGen.GenerateGeosphereMesh(radius, numSubdivisions, sphereMesh);

	// create a new geosphere model and return its index
	return modelsStore.CreateNewModelWithRawData(pDevice,
		"geosphere",
		sphereMesh.vertices,
		sphereMesh.indices,
		defaultTexturesMap_);
}

///////////////////////////////////////////////////////////

const UINT ModelsCreator::CreateGrid(ID3D11Device* pDevice,
	EntityStore & modelsStore,
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

	// generate grid's vertices and indices by input params
	geoGen.GenerateFlatGridMesh(
		gridWidth, 
		gridDepth, 
		(UINT)gridWidth + 1,  // num of quads by X
		(UINT)gridDepth + 1,  // num of quads by Z
		grid);

	// create a new grid model and return its index
	return modelsStore.CreateNewModelWithRawData(pDevice,
		"grid",
		grid.vertices,
		grid.indices,
		defaultTexturesMap_);
}

///////////////////////////////////////////////////////////

const UINT ModelsCreator::CreateGeneratedTerrain(ID3D11Device* pDevice,
	EntityStore & modelsStore,
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

	// generate terrain grid's vertices and indices by input params
	geoGen.GenerateFlatGridMesh(
		terrainWidth, 
		terrainDepth, 
		verticesCountByX, 
		verticesCountByZ, grid);

	// generate height for each vertex of the terrain grid
	GenerateHeightsForGrid(grid);


	// compute normals, tangents, and bitangents for this terrain grid
	//ModelMath modelMath;
	//modelMath.CalculateModelVectors(grid.vertices, true);

#if 0
	// PAINT GRID VERTICES WITH RAINBOW
	PaintGridWithRainbow(grid, verticesCountByX, verticesCountByZ);
#elif 0
	// PAINT VERTICES OF GRID LIKE IT IS HILLS (according to its height)
	PaintGridAccordingToHeights(grid);
#endif
	

	// add this terrain grid into the models store
	const UINT terrainGridIdx = modelsStore.CreateNewModelWithRawData(pDevice,
		"terrain_grid",
		grid.vertices,
		grid.indices,
		defaultTexturesMap_);

	// scale a texture of the terrain grid by particular scale factor
	modelsStore.texTransform_[terrainGridIdx] = DirectX::XMMatrixScaling(7, 7, 7);

	// return an index to the terrain grid model
	return terrainGridIdx;
}

///////////////////////////////////////////////////////////

const UINT ModelsCreator::CreateTerrainFromFile(
	const std::string& terrainSetupFile,
	ID3D11Device* pDevice,
	EntityStore& modelsStore)
{
	TerrainInitializer terrainInitializer;

	terrainInitializer.LoadSetupFile(terrainSetupFile);
	const TerrainInitializer::TerrainSetupData & setupData = terrainInitializer.GetSetupData();

	//
	// CREATE TERRAIN GRID
	//
	GeometryGenerator geoGen;
	GeometryGenerator::MeshData grid;

	// generate grid's vertices and indices by input params
	geoGen.GenerateFlatGridMesh(
		static_cast<float>(setupData.terrainWidth),
		static_cast<float>(setupData.terrainDepth),
		setupData.terrainWidth,        // how many quads will we have along X-axis
		setupData.terrainDepth,        // how many quads will we have along Z-axis
		grid);

	// generate height for each vertex and set color for it according to its height
	GenerateHeightsForGrid(grid);
	PaintGridAccordingToHeights(grid);

	// add this terrain grid into the models store
	const UINT terrainGrid_idx = modelsStore.CreateNewModelWithRawData(pDevice,
		"terrain_grid",
		grid.vertices,
		grid.indices,
		defaultTexturesMap_);

	return terrainGrid_idx;
}

///////////////////////////////////////////////////////////

const UINT ModelsCreator::CreateOneCopyOfModelByIndex(const UINT index,
	EntityStore & modelsStore,
	ID3D11Device* pDevice)
{
	// create a single copy of the origin model and return an ID of this copy
	return modelsStore.CreateOneCopyOfModelByIndex(pDevice, index);
}

///////////////////////////////////////////////////////////

const UINT ModelsCreator::CreateChunkBoundingBox(const UINT chunkDimension,
	EntityStore & modelsStore,
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


	const UINT chunkBoundingBoxIdx = modelsStore.CreateNewModelWithRawData(pDevice,
		"chunk_bounding_box",
		verticesDataArr,
		indicesDataArr,
		defaultTexturesMap_);

	return chunkBoundingBoxIdx;
}

///////////////////////////////////////////////////////////




// ************************************************************************************
//                               PRIVATE HELPERS API
// ************************************************************************************

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


#endif