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
	const DirectX::XMVECTOR & inPosition,
	const DirectX::XMVECTOR & inDirection,
	const DirectX::XMVECTOR & inPosModification,  // position modification; if we don't set this param the model won't move
	const DirectX::XMVECTOR & inRotModification)  // rotation modification; if we don't set this param the model won't rotate
{
	GeometryGenerator geoGen;
	GeometryGenerator::MeshData sphereMesh;

	geoGen.CreateSphere(5, 100, 100, sphereMesh);

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

const UINT ModelsCreator::CreateCylinder(ID3D11Device* pDevice,
	ModelsStore & modelsStore,
	const DirectX::XMVECTOR & inPosition,
	const DirectX::XMVECTOR & inDirection,
	const DirectX::XMVECTOR & inPosModification,  // position modification;
	const DirectX::XMVECTOR & inRotModification,  // rotation modification;
	const float bottomRadius,
	const float topRadius,
	const float height,
	const UINT sliceCount,
	const UINT stackCount)
{
	GeometryGenerator geoGen;
	GeometryGenerator::MeshData cylinderMeshes;

	geoGen.CreateCylinder(bottomRadius, topRadius, height, sliceCount, stackCount, cylinderMeshes);

	// add this cylinder into the models store
	const UINT cylinderID = modelsStore.CreateNewModelWithData(pDevice,
		"cylinder",
		inPosition,
		inDirection,
		cylinderMeshes.vertices,
		cylinderMeshes.indices,
		std::vector<TextureClass*> { TextureManagerClass::Get()->GetTextureByKey("unloaded_texture") });

	// return an index to the cylinder model
	return cylinderID;
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
	GenerateHeightsForGrid(grid);

#if 1
	// PAINT GRID VERTICES WITH RAINBOW
	PaintGridWithRainbow(grid, verticesCountByX, verticesCountByZ);
#elif 1
	// PAINT VERTICES OF GRID LIKE IT IS HILLS (according to its height)
	PaintGridAccordingToHeights(grid);
#endif
	

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

const UINT ModelsCreator::CreateCopyOfModelByIndex(const UINT index,
	ModelsStore & modelsStore,
	ID3D11Device* pDevice)
{
	// create a copy of the origin model and return an ID of this copy
	return modelsStore.CreateCopyOfModelByIndex(pDevice, index);
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