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


// initialize a triangle
bool Triangle::Initialize(ID3D11Device* pDevice)
{
	std::string triangleID{ "triangle" };

	// initialize the model
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
		VERTEX* pVertices = this->GetModelDataObj()->GetVerticesData();
		UINT* pIndices = this->GetModelDataObj()->GetIndicesData();

		// setup the vertices positions
		pVertices[0].position = { -0.5f, -0.5f, 0.0f }; // bottom left 
		pVertices[1].position = { 0.0f, 0.5f, 0.0f };   // top right
		pVertices[2].position = { 0.5f, -0.5f, 0.0f };  // bottom right

		// setup the vertices texture coords
		pVertices[0].texture = { 0.0f, 1.0f };  // bottom left 
		pVertices[1].texture = { 0.5f, 0.0f };  // top right
		pVertices[2].texture = { 1.0f, 1.0f };  // bottom right

		// setup the vertices normal
		pVertices[0].normal = { 0.0f, 0.0f, -1.0f };
		pVertices[1].normal = { 0.0f, 0.0f, -1.0f };
		pVertices[2].normal = { 0.0f, 0.0f, -1.0f };

		// setup the indices
		pIndices[0] = 0;
		pIndices[1] = 1;
		pIndices[2] = 2;

		// initialize model buffers with vertices/indices data
		bool result = Model::InitializeDefaultBuffers(pDevice, pData);
		COM_ERROR_IF_FALSE(result, "can't initialize the triangle's default buffers");

		// set the triangle's ID
		pData->SetID(triangleID);
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(THIS_FUNC, "can't initialize a triangle");
		return false;
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(THIS_FUNC, e.what());
		Log::Error(THIS_FUNC, "can't allocate memory for the triangle members");
		return false;
	}

	return true;
} // end Initialize