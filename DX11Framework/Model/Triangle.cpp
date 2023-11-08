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
	this->GetModelDataObj()->SetID(modelType_);
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
		ModelData* pData = this->GetModelDataObj();
		const UINT vertexCount = 3;
		const UINT indexCount = 3;

		/////////////////////////////////////////////////////

		pData->SetVertexCount(vertexCount);
		pData->SetIndexCount(indexCount);

		// allocate memory for the vertex and index array
		this->GetModelDataObj()->AllocateVerticesAndIndicesArrays(vertexCount, indexCount);

		// get pointers to vertices and indices arrays to write into it directly
		std::vector<VERTEX> & verticesArr = this->GetModelDataObj()->GetVertices();
		std::vector<UINT> & indicesArr = this->GetModelDataObj()->GetIndices();

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

		// initialize the model
		bool result = Model::Initialize("no_path",
			pDevice,
			pDeviceContext);
		COM_ERROR_IF_FALSE(result, "can't initialize a triangle model");

	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(THIS_FUNC, "can't initialize a triangle");
		return false;
	}

	return true;
} // end Initialize