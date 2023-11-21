////////////////////////////////////////////////////////////////////////////////////////////
// Filename:    Line3D.cpp
// Description: realization of a 3D line model functional
// Revising:    20.10.23
////////////////////////////////////////////////////////////////////////////////////////////
#include "Line3D.h"




Line3D::Line3D(ModelInitializerInterface* pModelInitializer)
{
	try
	{
		this->SetModelInitializer(pModelInitializer);
		this->AllocateMemoryForElements();

		// setup default positions of the line's start point and end point
		startPoint_.position = { 0, 0, 0 };
		endPoint_.position = { 100, 100, 100 };
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		COM_ERROR_IF_FALSE(false, "can't allocate memory for the SpriteClass members");
	}
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
	// initialize a 3D line

	try
	{
		// make local pointers to the device and device context
		this->pDevice_ = pDevice;
		this->pDeviceContext_ = pDeviceContext;

		// each line has only 2 vertices and only 2 indices
		const UINT vertexCount = 2;
		const UINT indexCount = 2;

		// arrays for vertices/indices data
		std::vector<VERTEX> verticesArr(vertexCount);
		std::vector<UINT> indicesArr(indexCount);

		/////////////////////////////////////////////////////

		// setup the verices
		verticesArr[0] = startPoint_;
		verticesArr[1] = endPoint_;
	
		verticesArr[0].color = { 1, 1, 1, 1 };
		verticesArr[1].color = verticesArr[0].color;

		// setup the indices
		indicesArr[0] = 0;
		indicesArr[1] = 1;
	
		/////////////////////////////////////////////////////
		
		// each 3D line has only one mesh so create it and initialize with data
		this->InitializeOneMesh(verticesArr, indicesArr);

	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(THIS_FUNC, "can't initialize a 3D line");
		return false;
	}

	return true;
} // end Initialize

///////////////////////////////////////////////////////////

void Line3D::Render(D3D_PRIMITIVE_TOPOLOGY topologyType)
{
	// because we want to render a line we have to set a primitive topology 
	// type to be D3D11_PRIMITIVE_TOPOLOGY_LINELIST
	Model::Render(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
}

///////////////////////////////////////////////////////////

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