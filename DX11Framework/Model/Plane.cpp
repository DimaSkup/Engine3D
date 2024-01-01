////////////////////////////////////////////////////////////////////////////////////////////
// Filename:    Plane.h
// Description: an implementation of a plane model
//
// Created:     19.02.23
////////////////////////////////////////////////////////////////////////////////////////////
#include "Plane.h"

Plane::Plane(ModelInitializerInterface* pModelInitializer,
	ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext)
	: Model(pDevice, pDeviceContext)
{
	try
	{
		this->SetModelInitializer(pModelInitializer);
		this->modelType_ = "plane";
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(THIS_FUNC, "can't create a plane model");
		COM_ERROR_IF_FALSE(false, "can't create a plane model");
	}
}

Plane::~Plane()
{
}




bool Plane::Initialize(const std::string & filePath)  // filePath has no role here
{
	// we initialize a plane's model data in a separate way as it is faster than
	// each time reading from the data file or copying buffers from some another plane model;

	try
	{
		// this flag means that we want to create a default vertex buffer for the mesh of this sprite
		bool isVertexBufferDynamic = false;    

		// since each 2D sprite is just a plane it has 4 vertices and 6 indices
		UINT vertexCount = 4;
		UINT indexCount = 6;

		// arrays for vertices/indices data
		std::vector<VERTEX> verticesArr(vertexCount);
		std::vector<UINT> indicesArr(indexCount, 0);

		/////////////////////////////////////////////////////

		// setup the vertices positions
		verticesArr[0].position = { -1,  1,  0 };    // top left
		verticesArr[1].position = {  1, -1,  0 };    // bottom right 
		verticesArr[2].position = { -1, -1,  0 };    // bottom left
		verticesArr[3].position = {  1,  1,  0 };    // top right

		// setup the texture coords of each vertex
		verticesArr[0].texture = { 0, 0 };
		verticesArr[1].texture = { 1, 1 };
		verticesArr[2].texture = { 0, 1 };
		verticesArr[3].texture = { 1, 0 };

		// setup the indices
		indicesArr.insert(indicesArr.begin(), { 0, 1, 2, 0, 3, 1 });

		/////////////////////////////////////////////////////

		// each plane model has only one mesh so create it and fill in with data
		this->InitializeOneMesh(verticesArr, indicesArr, {}, isVertexBufferDynamic);
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		Log::Error(THIS_FUNC, "can't initialize a plane model");

		return false;
	}

	return true;

} // end Initialize