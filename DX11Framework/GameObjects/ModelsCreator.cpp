////////////////////////////////////////////////////////////////////////////////////////////
// Filename:        ModelsCreator.cpp
// Description:     implementation of the functional of the ModelsCreator class
//
// Created:         12.02.24
////////////////////////////////////////////////////////////////////////////////////////////

#include "ModelsCreator.h"
#include "TextureManagerClass.h"
#include "TerrainInitializer.h"

const UINT ModelsCreator::CreatePlane(ID3D11Device* pDevice, 
	ModelsStore & modelsStore,
	const DirectX::XMVECTOR & inPosition,
	const DirectX::XMVECTOR & inDirection,
	const DirectX::XMVECTOR & inPosModification,  // position modification; if we don't set this param the model won't move
	const DirectX::XMVECTOR & inRotModification)  // rotation modification; if we don't set this param the model won't rotate
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
		{ 2, 2, 0 },
		{ 0, 0, 0 },
		verticesArr,
		indicesArr,
		texturesArr);
}

///////////////////////////////////////////////////////////

const UINT ModelsCreator::CreateCube(ID3D11Device* pDevice, 
	ModelsStore & modelsStore,
	const DirectX::XMVECTOR & inPosition,
	const DirectX::XMVECTOR & inDirection,
	const DirectX::XMVECTOR & inPosModification,  // position modification; if we don't set this param the model won't move
	const DirectX::XMVECTOR & inRotModification)  // rotation modification; if we don't set this param the model won't rotate

{
	// THIS FUNCTION creates a basic empty plane model data and adds this model
	// into the ModelsStore storage
#if 0
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

	verticesArr[0].position = {  1,  1, -1 };
	verticesArr[1].position = {  1, -1, -1 };
	verticesArr[2].position = {  1,  1,  1 };
	verticesArr[3].position = {  1, -1,  1 };
	verticesArr[4].position = { -1,  1, -1 };
	verticesArr[5].position = { -1, -1, -1 };
	verticesArr[6].position = { -1,  1,  1 };
	verticesArr[7].position = { -1, -1,  1 };

	// setup the texture coords of each vertex
	verticesArr[0].texture = { 0, 0 };
	verticesArr[1].texture = { 1, 0 };
	verticesArr[2].texture = { 0, 1 };
	verticesArr[3].texture = { 1, 1 };

	// setup the indices
	indicesArr.insert(indicesArr.begin(), { 0, 1, 2, 0, 3, 1 });

	// create an empty texture for this plane
	std::vector<TextureClass> textures;
	textures.push_back(TextureClass(pDevice, Colors::UnhandledTextureColor, aiTextureType_DIFFUSE));

#endif

	

	
	/////////////////////////////////////////////////////

	// create a new model using prepared data and return its index
	return modelsStore.CreateModelFromFile(
		pDevice,
		"data/models/minecraft-grass-block/source/Grass_Block.obj",
		"cube",
		//"data/models/default/cube_simple.obj",
		inPosition,
		inDirection);

	return 0;
}

///////////////////////////////////////////////////////////

const UINT ModelsCreator::CreateSphere(ID3D11Device* pDevice,
	ModelsStore & modelsStore,
	const float radius,
	const UINT sliceCount,
	const UINT stackCount,
	const DirectX::XMVECTOR & inPosition,
	const DirectX::XMVECTOR & inDirection,
	const DirectX::XMVECTOR & inPosModification,  // position modification; if we don't set this param the model won't move
	const DirectX::XMVECTOR & inRotModification)  // rotation modification; if we don't set this param the model won't rotate
{
	GeometryGenerator geoGen;
	GeometryGenerator::MeshData sphereMesh;

	geoGen.CreateSphere(radius, sliceCount, stackCount, sphereMesh);

	return modelsStore.CreateNewModelWithData(pDevice,
		"sphere",
		inPosition,
		inDirection,
		sphereMesh.vertices,
		sphereMesh.indices,
		{ nullptr });

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

	geoGen.CreateGeosphere(radius, numSubdivisions, sphereMesh);

	return modelsStore.CreateNewModelWithData(pDevice,
		"geosphere",
		{ 5, 0, 0, 1 },//inPosition,
		{ 0, 0, 0, 0 },//inDirection,
		sphereMesh.vertices,
		sphereMesh.indices,
		{ nullptr });
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
	geoGen.CreateCylinder(
		cylParams.bottomRadius, 
		cylParams.topRadius,
		cylParams.height, 
		cylParams.sliceCount, 
		cylParams.stackCount, 
		cylinderMeshes);

	// add this cylinder into the models store
	const UINT cylinderID = modelsStore.CreateNewModelWithData(pDevice,
		"cylinder",
		inPosition,
		inDirection,
		cylinderMeshes.vertices,  // vertices data of cylinder
		cylinderMeshes.indices,   // indices data of vertices
		std::vector<TextureClass*> { TextureManagerClass::Get()->GetTextureByKey("unloaded_texture") });

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

	geoGen.CreateGrid(
		gridWidth, 
		gridDepth, 
		(UINT)gridWidth + 1,  // num of quads by X
		(UINT)gridDepth + 1,  // num of quads by Z
		grid);

	// add this grid into the models store
	const UINT gridID = modelsStore.CreateNewModelWithData(pDevice,
		"grid",
		inPosition,
		inDirection,
		grid.vertices,
		grid.indices,
		std::vector<TextureClass*> { TextureManagerClass::Get()->GetTextureByKey("unloaded_texture") });

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

	geoGen.CreateGrid(terrainWidth, terrainDepth, verticesCountByX, verticesCountByZ, grid);

	// generate height for each grid vertex
	//GenerateHeightsForGrid(grid);

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
		{ 0, 0, 0, 1 },
		{ 0, 0, 0, 1 },
		grid.vertices,
		grid.indices,
		std::vector<TextureClass*> { TextureManagerClass::Get()->GetTextureByKey("unloaded_texture") });

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

	geoGen.CreateGrid(
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
		{ 0, -15, 0, 1 },
		{ 0, 0, 0, 1 },
		grid.vertices,
		grid.indices,
		std::vector<TextureClass*> { TextureManagerClass::Get()->GetTextureByKey("unloaded_texture") });

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
		DirectX::XMVectorZero(),  // position
		DirectX::XMVectorZero(),  // rotation
		verticesDataArr,
		indicesDataArr,
		{ nullptr },              // bounding box has no texture
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
		pos.y = 0.5f * (0.3f * (pos.z*sinf(0.1f * pos.x)) + (pos.x * cosf(0.1f * pos.z)));
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

	for (VERTEX & vertex : grid.vertices)
	{
		const float py = vertex.position.y;

		if (py < -10.0f)
		{
			// sandy beach color
			vertex.color = DirectX::XMFLOAT4(1.0f, 0.96f, 0.62f, 1.0f);
		}
		else if (py < 5.0f)
		{
			// light yellow-green
			vertex.color = DirectX::XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
		}
		else if (py < 12.0f)
		{
			// dark yellow-green
			vertex.color = DirectX::XMFLOAT4(0.1f, 0.48f, 0.19f, 1.0f);
		}
		else if (py < 20.0f)
		{
			// dark brown
			vertex.color = DirectX::XMFLOAT4(0.45f, 0.39f, 0.34f, 1.0f);
		}
		else
		{
			// white snow
			vertex.color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
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