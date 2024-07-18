// ************************************************************************************
// Filename:        ModelsCreator.cpp
// Description:     implementation of the functional of the ModelsCreator class
//
// Created:         12.02.24
// ************************************************************************************

#include "ModelsCreator.h"

#include "TextureManager.h"
#include "ModelMath.h"
#include "MeshStorage.h"
#include "ModelLoader.h"
#include "GeometryGenerator.h"

#include "../Engine/Settings.h"


ModelsCreator::ModelsCreator()
{
}

// ************************************************************************************

const std::vector<MeshID> ModelsCreator::ImportFromFile(
	ID3D11Device* pDevice,
	const std::string& filePath)
{
	// create meshes loading its vertices/indices/texture data/etc. from a file
	// input:  filePath - a path to the data file
	// return: array of meshes IDs

	std::vector<MeshID> meshIDs;

	try
	{
		ModelLoader modelLoader;
		MeshStorage* pMeshStorage = MeshStorage::Get();
		std::vector<Mesh::MeshData> meshes;

		// load vertices/indices/textures/etc. of meshes from a file by filePath
		modelLoader.LoadFromFile(pDevice, meshes, filePath);

		// go through the array of raw meshes and store them into the mesh storage
		for (Mesh::MeshData& data : meshes)
		{
			// create a new mesh using the prepared data
			const MeshID id = pMeshStorage->CreateMeshWithRawData(pDevice, data);

			meshIDs.push_back(id);
		}
	}
	catch (const std::bad_alloc& e)
	{
		Log::Error(LOG_MACRO, e.what());
		THROW_ERROR("can't create load meshes from a file by path: " + filePath);
	}
	catch (EngineException& e)
	{
		Log::Error(e, false);
		THROW_ERROR("can't create load meshes from a file by path: " + filePath);
	}

	return meshIDs;
}

///////////////////////////////////////////////////////////


MeshID ModelsCreator::Create(ID3D11Device* pDevice, const Mesh::MeshType& type)
{
	// create new BASIC mesh (model) by input type;
	//
	// input:  type of the BASIC mesh
	// return: an ID of the created mesh

	switch (type)
	{
		case Mesh::MeshType::Plane:
		{
			return CreatePlane(pDevice);
		}
		case Mesh::MeshType::Cube:
		{
			return CreateCube(pDevice);
		}
		case Mesh::MeshType::Skull:
		{
			return CreateSkull(pDevice);
		}
		case Mesh::MeshType::Pyramid:
		{
			return CreatePyramid(pDevice);
		}
		case Mesh::MeshType::Sphere:
		{
			return CreateSphere(pDevice);
		}
		case Mesh::MeshType::Cylinder:
		{
			return CreateCylinder(pDevice);
		}
		default:
		{
			THROW_ERROR("Unknown mesh type");
		}
	}
}


// ************************************************************************************
// 
//                                 HELPERS API
// 
// ************************************************************************************


const std::vector<TextureClass*> ModelsCreator::GetDefaultTexPtrsArr() const
{
	// make and return a default set of ptrs to textures objects

	const u32 texCount = TextureClass::TEXTURE_TYPE_COUNT;
	TextureClass* pTex = TextureManager::Get()->GetByName("unloaded");
	
	return std::vector<TextureClass*>(texCount, pTex);
}

///////////////////////////////////////////////////////////

const std::vector<TexID> ModelsCreator::GetDefaultTexIDsArr() const
{
	// make and return a default set of ptrs to textures objects

	const u32 texCount = TextureClass::TEXTURE_TYPE_COUNT;
	TexID texID = TextureManager::Get()->GetIDByName("unloaded");

	return std::vector<TexID>(texCount, texID);
}

///////////////////////////////////////////////////////////

MeshID ModelsCreator::CreatePlane(ID3D11Device* pDevice)
{
	// create new empty plane mesh and store it into the storage;
	// return: plane mesh ID

	GeometryGenerator geoGen;
	Mesh::MeshData data;

	data.name   = "plane";
	data.path   = "data/models/default/plane.txt";
	data.texIDs = GetDefaultTexIDsArr();

	geoGen.GeneratePlaneMesh(data);

	// store the mesh and return its ID
	return MeshStorage::Get()->CreateMeshWithRawData(pDevice, data);
}

///////////////////////////////////////////////////////////

MeshID ModelsCreator::CreateCube(ID3D11Device* pDevice)
{
	// THIS FUNCTION creates a cube mesh and stores it into the storage;
	// return: cube mesh ID

	GeometryGenerator geoGen;
	Mesh::MeshData data;

	data.name   = "cube";
	data.path   = "data/models/default/cube.txt";
	data.texIDs = GetDefaultTexIDsArr();

	// generate vertices and indices for a cube
	geoGen.GenerateCubeMesh(data);

	// store the mesh and return its ID
	return MeshStorage::Get()->CreateMeshWithRawData(pDevice, data);
}


///////////////////////////////////////////////////////////

MeshID ModelsCreator::CreateSkull(ID3D11Device* pDevice)
{
	// load skull's mesh data from the file, store this mesh into the storage
	// and return its ID

	const std::string dataFilepath = "data/models/skull.txt";
	std::ifstream fin(dataFilepath);

	if (!fin)
	{
		const std::string errorMsg = { dataFilepath + " not found" };
		MessageBoxA(0, errorMsg.c_str(), 0, 0);
		THROW_ERROR(errorMsg);
	}

	// -------------------------------------------------------------------------

	Mesh::MeshData data;
	UINT vCount = 0;     // vertices count
	UINT tCount = 0;     // texture coords count
	std::string ignore;

	// read in vertices, textures coords count and skip separators
	fin >> ignore >> vCount;
	fin >> ignore >> tCount;
	fin >> ignore >> ignore >> ignore >> ignore;

	// read in vertices positions and normal vectors
	data.vertices.resize(vCount);

	for (VERTEX& v : data.vertices)
	{
		fin >> v.position.x >> v.position.y	>> v.position.z;
		fin >> v.normal.x   >> v.normal.y	>> v.normal.z;
	}

	// skip separators
	fin >> ignore >> ignore >> ignore;

	// read in indices
	data.indices.resize(3 * tCount);

	for (UINT& index : data.indices)
	{
		fin >> index;
	}

	fin.close();


	// -------------------------------------------------------------------------

	data.name = "skull";
	data.path = dataFilepath;
	data.texIDs = GetDefaultTexIDsArr();

	// store the mesh and return its ID
	return MeshStorage::Get()->CreateMeshWithRawData(pDevice, data);
}

///////////////////////////////////////////////////////////

MeshID ModelsCreator::CreatePyramid(
	ID3D11Device* pDevice,
	const Mesh::PyramidMeshParams& meshParams)
{
	// generate a new pyramid mesh and store it into the mesh storage;
	// 
	// input:  (if passed NULL then default) geometry params for a mesh generation;
	// return: ID of created mesh

	GeometryGenerator geoGen;
	Mesh::MeshData mesh;

	mesh.name = "pyramid";
	mesh.path = "data/models/default/pyramid.txt";

	// generate pyramid's vertices and indices by input params
	geoGen.GeneratePyramidMesh(
		meshParams.height,         // height of the pyramid
		meshParams.baseWidth,      // width (length by X) of one of the base side
		meshParams.baseDepth,      // depth (length by Z) of one of the base side
		mesh);

	// setup default textures for the mesh
	mesh.texIDs = GetDefaultTexIDsArr();

	// store the mesh into the mesh storage and return ID of this mesh
	return MeshStorage::Get()->CreateMeshWithRawData(pDevice, mesh);

}

///////////////////////////////////////////////////////////

MeshID ModelsCreator::CreateSphere(
	ID3D11Device* pDevice,
	const Mesh::SphereMeshParams& meshParams)
{
	// generate a new sphere mesh and store it into the mesh storage;
	// 
	// input:  (if passed NULL then default) geometry params for a mesh generation;
	// return: ID of created mesh

	GeometryGenerator geoGen;
	Mesh::MeshData mesh;

	mesh.name = "sphere";
	mesh.path = "data/models/default/sphere.txt";
	mesh.texIDs = GetDefaultTexIDsArr();

	// generate sphere's vertices and indices by input params
	geoGen.GenerateSphereMesh(
		meshParams.radius,
		meshParams.sliceCount,
		meshParams.stackCount,
		mesh);

	// store the mesh and return its ID
	return MeshStorage::Get()->CreateMeshWithRawData(pDevice, mesh);
}

///////////////////////////////////////////////////////////

MeshID ModelsCreator::CreateCylinder(
	ID3D11Device* pDevice,
	const Mesh::CylinderMeshParams& meshParams)
{
	// generate new cylinder mesh and store it into the storage;
	// 
	// input:  (if passed NULL then default) geometry params for a mesh generation;
	// return: mesh ID

	GeometryGenerator geoGen;
	Mesh::MeshData mesh;

	mesh.name = "cylinder";
	mesh.path = "data/models/default/cylinder.txt";
	mesh.texIDs = GetDefaultTexIDsArr();

	// generate geometry of cylinder by input params
	geoGen.GenerateCylinderMesh(
		meshParams.bottomRadius,
		meshParams.topRadius,
		meshParams.height,
		meshParams.sliceCount,
		meshParams.stackCount,
		mesh);

	// create a new cylinder model and return its index
	return MeshStorage::Get()->CreateMeshWithRawData(pDevice, mesh);
}

///////////////////////////////////////////////////////////

MeshID ModelsCreator::CreateGrid(
	ID3D11Device* pDevice, 
	const u32 width, 
	const u32 depth)
{
	// CREATE PLAIN GRID MESH
	// 
	// input:  width and height for mesh generation 
	// return: an ID of created mesh

	GeometryGenerator geoGen;
	Mesh::MeshData mesh;

	std::stringstream ss;
	ss << "grid_" << width << "_" << depth;

	mesh.name = ss.str();
	mesh.path = "data/models/" + mesh.name + ".txt";
	mesh.texIDs = GetDefaultTexIDsArr();              // setup default textures for the mesh
	ss.clear();

	// generate grid's vertices and indices by input params
	geoGen.GenerateFlatGridMesh(
		static_cast<float>(width),
		static_cast<float>(depth),
		width + 1,     // num of quads (cells count) by X 
		depth + 1,     // num of quads (cells count) by Z
		mesh);
	
	// create a new grid mesh (model) and return its index
	return MeshStorage::Get()->CreateMeshWithRawData(pDevice, mesh);
}


#if 0

const UINT ModelsCreator::CreateWaves(ID3D11Device* pDevice,
	EntityStore& modelsStore,
	const UINT numRows,
	const UINT numColumns,
	const float spatialStep,
	const float timeStep,
	const float speed,
	const float damping)
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



#if 0


const UINT ModelsCreator::CreateGeophere(ID3D11Device* pDevice,
	EntityStore& modelsStore,
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

const UINT ModelsCreator::CreateGeneratedTerrain(ID3D11Device* pDevice,
	EntityStore& modelsStore,
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
	const TerrainInitializer::TerrainSetupData& setupData = terrainInitializer.GetSetupData();

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
	EntityStore& modelsStore,
	ID3D11Device* pDevice)
{
	// create a single copy of the origin model and return an ID of this copy
	return modelsStore.CreateOneCopyOfModelByIndex(pDevice, index);
}

///////////////////////////////////////////////////////////

const UINT ModelsCreator::CreateChunkBoundingBox(const UINT chunkDimension,
	EntityStore& modelsStore,
	ID3D11Device* pDevice)
{
	// creates the bouding box that surrounds the terrain cell. It is made up of series of 
	// lines creating a box around the exact dimensions of the terrain cell. This is used
	// for debugging purposes mostly

	constexpr UINT vertexCount = 8;    // set the number of line box vertices in the vertex array
	const float halfDimension = 0.5f * (float)chunkDimension;
	const float min = -halfDimension;
	const float max = halfDimension;

	const DirectX::XMFLOAT3 minDimension{ min, min, min };
	const DirectX::XMFLOAT3 maxDimension{ max, max, max };

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



void ModelsCreator::GenerateHeightsForGrid(GeometryGenerator::MeshData& grid)
{
	// generate height for the input grid by some particular function;
	// (there can be several different types of height generation)

#if 1
	for (UINT idx = 0; idx < grid.vertices.size(); ++idx)
	{
		DirectX::XMFLOAT3& pos = grid.vertices[idx].position;

		// a function for making hills for the terrain
		pos.y = 0.3f * (pos.z * sinf(0.1f * pos.x)) + (pos.x * cosf(0.1f * pos.z));

		// get hill normal
		// n = (-df/dx, 1, -df/dz)
		DirectX::XMVECTOR normalVec{
		   -0.03f * pos.z * cosf(0.1f * pos.x) - 0.3f * cosf(0.1f * pos.z),
		   1.0f,
		   -0.3f * sinf(0.1f * pos.x) + 0.03f * pos.x * sinf(0.1f * pos.z) };

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
			const UINT idx = i * n + j;
			grid.vertices[idx].position.y = 30 * (sinf(valForSin) - cosf(valForCos));

			valForSin += sin_step;

		}
		valForCos += cos_step;
	}
#endif
}

///////////////////////////////////////////////////////////

void ModelsCreator::PaintGridAccordingToHeights(GeometryGenerator::MeshData& grid)
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

	for (VERTEX& vertex : grid.vertices)
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

void ModelsCreator::PaintGridWithRainbow(GeometryGenerator::MeshData& grid,
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
			const UINT idx = i * verticesCountByX + j;
			grid.vertices[idx].color = { du * i, 0.5f, dv * j, 1.0f };
		}
	}
}


#endif

