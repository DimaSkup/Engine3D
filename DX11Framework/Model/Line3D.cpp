////////////////////////////////////////////////////////////////////////////////////////////
// Filename:    Line3D.cpp
// Description: realization of a 3D line model functional
// Revising:    20.10.23
////////////////////////////////////////////////////////////////////////////////////////////
#include "Line3D.h"




Line3D::Line3D(ModelInitializerInterface* pModelInitializer)
{
	this->SetModelInitializer(pModelInitializer);
	this->AllocateMemoryForElements();

	// set the line's ID
	this->GetModelDataObj()->SetID(modelType_);

	startPoint_.position = { 1, 5, 3 };
	endPoint_.position = { 1, 5, -3 };
}

Line3D::~Line3D()
{
}




////////////////////////////////////////////////////////////////////////////////////////////
//                                  PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////


// initialize a triangle
bool Line3D::Initialize(const std::string & filePath, 
	ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext)
{
	

	// initialize the model
	try
	{
		ModelData* pData = this->GetModelDataObj();
		const UINT vertexCount = 2;
		const UINT indexCount = 2;

		/////////////////////////////////////////////////////

		// allocate memory for the vertex and index array
		this->GetModelDataObj()->AllocateVerticesAndIndicesArrays(vertexCount, indexCount);

		// get pointers to vertices and indices arrays to write into it directly
		std::vector<VERTEX> & verticesArr = pData->GetVertices();
		std::vector<UINT> & indicesArr = pData->GetIndices();

		// setup the verices
		verticesArr[0] = startPoint_;
		verticesArr[1] = endPoint_;
	
		verticesArr[0].color = { 1, 1, 1, 1 };
		verticesArr[1].color = verticesArr[0].color;

		// setup the indices
		indicesArr[0] = 0;
		indicesArr[1] = 1;
		//pIndices[2] = 0;

		Model::Initialize(filePath, pDevice, pDeviceContext);

		
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(THIS_FUNC, "can't initialize a 3D line");
		return false;
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(THIS_FUNC, e.what());
		Log::Error(THIS_FUNC, "can't allocate memory for a 3D line");
		return false;
	}

	return true;
} // end Initialize

///////////////////////////////////////////////////////////

void Line3D::Render(ID3D11DeviceContext* pDeviceContext,
	D3D_PRIMITIVE_TOPOLOGY topologyType)
{
	// because we want to render a line we have to set a primitive topology 
	// type to be D3D11_PRIMITIVE_TOPOLOGY_LINELIST
	Model::Render(pDeviceContext, D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
}

void Line3D::SetStartPoint(const float x, const float y, const float z)
{
	startPoint_.position.x = x;
	startPoint_.position.y = y;
	startPoint_.position.z = z;

} // end SetStartPoint

///////////////////////////////////////////////////////////

void Line3D::SetEndPoint(const float x, const float y, const float z)
{
	endPoint_.position.x = x;
	endPoint_.position.y = y;
	endPoint_.position.z = z;

} // end SetEndPoint

///////////////////////////////////////////////////////////

void Line3D::SetStartPoint(const DirectX::XMFLOAT3 & startPos)
{
	startPoint_.position = startPos;

} // end SetStartPoint

///////////////////////////////////////////////////////////

void Line3D::SetEndPoint(const DirectX::XMFLOAT3 & endPos)
{
	endPoint_.position = endPos;

} // end SetEndPoint