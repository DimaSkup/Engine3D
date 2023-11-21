////////////////////////////////////////////////////////////////////
// Filename:    Triangle.cpp
// Description: realization of a triangle model functional
// Revising:    22.11.22
////////////////////////////////////////////////////////////////////
#include "Triangle.h"



Triangle::Triangle(ModelInitializerInterface* pModelInitializer)
{
	this->SetModelInitializer(pModelInitializer);
	this->AllocateMemoryForElements();
}

Triangle::~Triangle()
{
}




////////////////////////////////////////////////////////////////////////////////////////////
//
//                                 PUBLIC FUNCTIONS
// 
////////////////////////////////////////////////////////////////////////////////////////////


bool Triangle::Initialize(const std::string & filePath,
	ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext)
{
	// initialize the model;
	// NOTE: the filePath can be empty since we generate triangle's data manually

	try
	{
		// make local pointers to the device and device context
		this->pDevice_ = pDevice;
		this->pDeviceContext_ = pDeviceContext;

		// each triangle has only 3 vertices and only 3 indices
		const UINT vertexCount = 3;
		const UINT indexCount = 3;
		
		// arrays for vertices/indices data
		std::vector<VERTEX> verticesArr(vertexCount);
		std::vector<UINT> indicesArr(indexCount);

		/////////////////////////////////////////////////////

		// setup the vertices positions
		verticesArr[0].position = { -0.5f, -0.5f, 0.0f }; // bottom left 
		verticesArr[1].position = { 0.0f, 0.5f, 0.0f };   // top right
		verticesArr[2].position = { 0.5f, -0.5f, 0.0f };  // bottom right

		// setup the vertices texture coords
		verticesArr[0].texture = { 0.0f, 1.0f };  // bottom left 
		verticesArr[1].texture = { 0.5f, 0.0f };  // top right
		verticesArr[2].texture = { 1.0f, 1.0f };  // bottom right

		// setup the vertices normal (all the vertices have the same normal vector)
		verticesArr[0].normal = { 0.0f, 0.0f, -1.0f };
		verticesArr[1].normal = verticesArr[0].normal;
		verticesArr[2].normal = verticesArr[0].normal;

		// setup the indices
		indicesArr[0] = 0;
		indicesArr[1] = 1;
		indicesArr[2] = 2;

		/////////////////////////////////////////////////////

		// each triangle has only one mesh so create it and fill in with data
		this->InitializeOneMesh(verticesArr, indicesArr);

	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(THIS_FUNC, "can't initialize a triangle");

		return false;
	}

	return true;

} // end Initialize