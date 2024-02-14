////////////////////////////////////////////////////////////////////////////////////////////
// Filename:        ModelsCreator.cpp
// Description:     implementation of the functional of the ModelsCreator class
//
// Created:         12.02.24
////////////////////////////////////////////////////////////////////////////////////////////

#include "ModelsCreator.h"


const UINT ModelsCreator::CreatePlane(ID3D11Device* pDevice, 
	ModelsStore & modelsStore,
	const DirectX::XMVECTOR & inPosition,
	const DirectX::XMVECTOR & inDirection)
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
	std::vector<TextureClass> textures;
	textures.push_back(TextureClass(pDevice, Colors::UnhandledTextureColor, aiTextureType_DIFFUSE));

	/////////////////////////////////////////////////////

	// create a new model using prepared data and return its index
	return modelsStore.CreateModelWithData(
		pDevice,
		{ 2, 2, 0 },
		{ 0, 0, 0 },
		verticesArr,
		indicesArr,
		textures);
}

///////////////////////////////////////////////////////////

const UINT ModelsCreator::CreateCube(ID3D11Device* pDevice, 
	ModelsStore & modelsStore,
	const DirectX::XMVECTOR & inPosition,
	const DirectX::XMVECTOR & inDirection)
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
#if 0
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
	return modelsStore.CreateModel(
		pDevice,
		"data/models/default/cube.obj",
		inPosition,
		inDirection);

	return 0;
}

///////////////////////////////////////////////////////////

const UINT ModelsCreator::CreateCopyOfModelByIndex(const UINT index,
	ModelsStore & modelsStore,
	ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,
	const DirectX::XMVECTOR & position,
	const DirectX::XMVECTOR & rotation)
{
	// origin models elements
	const VertexBuffer<VERTEX> & originVertexBuffer = modelsStore.vertexBuffers_[index];
	const IndexBuffer & originIndexBuffer = modelsStore.indexBuffers_[index];
	const TextureClass & originTexture = modelsStore.textures_[index];

	// elements of the copy 
	VertexBuffer<VERTEX> copyVertexBuffer;
	IndexBuffer copyIndexBuffer;
	//TextureClass copyTexture();

	std::vector<TextureClass> textures;
	textures.push_back(TextureClass(originTexture));

	copyVertexBuffer.CopyBuffer(pDevice, pDeviceContext, originVertexBuffer);
	copyIndexBuffer.CopyBuffer(pDevice, pDeviceContext, originIndexBuffer);

	// create a copy of the origin model using prepared data
	return modelsStore.CreateModelWithData(
		pDevice,
		position,
		rotation,
		copyVertexBuffer,
		copyIndexBuffer,
		textures);	
}