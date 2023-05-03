////////////////////////////////////////////////////////////////////
// Filename:     TerrainCellClass.cpp
// Description:  implementation of the TerrainCellClass functional
//
// Created:      02.05.23
////////////////////////////////////////////////////////////////////
#include "TerrainCellClass.h"

TerrainCellClass::TerrainCellClass()
{
}

TerrainCellClass::TerrainCellClass(const TerrainCellClass& obj)
{

}

TerrainCellClass::~TerrainCellClass()
{

}



////////////////////////////////////////////////////////////////////
//
//                       PUBLIC FUNCTIONS
//
////////////////////////////////////////////////////////////////////
bool TerrainCellClass::Initialize(ID3D11Device* pDevice,
	VERTEX* pTerrainModel, 
	UINT nodeIndexX,
	UINT nodeIndexY,
	UINT cellHeight,
	UINT cellWidth,
	UINT terrainWidth)
{
	bool result = false;

	try
	{
		// create a model class object to use its functional
		pModel_ = new ModelClass();

		// load the rendering buffers with the terrain data for this cell index
		result = InitializeBuffers(pDevice,
			nodeIndexX,
			nodeIndexY,
			cellHeight,
			cellWidth,
			terrainWidth,
			pTerrainModel);
		COM_ERROR_IF_FALSE(result, "can't initialize buffers for the terrain cells");

		// calculate the dimensions of this cell
		CalculateCellDimensions();

		// build the debug line buffers to produce the bounding box around this cell
		result = BuildLineBuffers(pDevice);
		COM_ERROR_IF_FALSE(result, "can't build line buffers");

	}
	catch (std::bad_alloc & e)
	{
		Log::Error(THIS_FUNC, e.what());
		COM_ERROR_IF_FALSE(false, "can't allocate memory for the model class object");
	}

	return true;
}


void TerrainCellClass::Shutdown()
{
	ShutdownLineBuffers();   // release the line rendering buffers
	ShutdownBuffers();       // release the cell rendering buffers

	return;
}


void TerrainCellClass::Render(ID3D11DeviceContext* pDeviceContext)
{
	// put the vertex and index buffers on the graphics pipeline to prepare them for drawing
	RenderBuffers(pDeviceContext);

	return;
}


// puts the vertex and index buffer for the bounding box on the GPU for rendering;
// it is rendered as a line list
void TerrainCellClass::RenderLineBuffers(ID3D11DeviceContext* pDeviceContext)
{
	pModel_->RenderBuffers(pDeviceContext, D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	return;
}


UINT TerrainCellClass::GetVertexCount() const
{
	return pModel_->GetVertexCount();
}


UINT TerrainCellClass::GetIndexCount() const
{
	return pModel_->GetIndexCount();
}


UINT TerrainCellClass::GetLineBuffersIndexCount() const
{
	return lineIndexCount_;;
}


void TerrainCellClass::GetCellDimentions(float&, float&, float&, float&, float&, float&)
{

}




////////////////////////////////////////////////////////////////////
//
//                      PRIVATE FUNCTIONS
//
////////////////////////////////////////////////////////////////////

// creates the buffers used for rendering the terrain cell. It also creates a vertex list
// that is used for other calculations such as determining the size of this cell.
// The terrain model that was built in the TerrainClass is passed into this function, and
// then an index into the terrain model is created based on the physical location of this
// cell using nodeIndexX and nodeIndexY
bool TerrainCellClass::InitializeBuffers(ID3D11Device* pDevice, 
	UINT nodeIndexX,
	UINT nodeIndexY,
	UINT cellHeight,
	UINT cellWidth,
	UINT terrainWidth,
	VERTEX* pTerrainModel)
{
	VERTEX* pVertices = nullptr;   // an array of terrain cells vertices
	UINT* pIndices = nullptr;     // an array of vertex indices of the terrain cells
	UINT modelIndex = 0;           // an index into the terrain model data
	UINT index = 0;                // an index in the vertices array
	
	// calculate the number of vertices in this terrain cell
	pModel_->SetVertexCount((cellHeight - 1) * (cellWidth - 1) * 6);

	// set the index count to the same as the vertex count
	pModel_->SetIndexCount(GetVertexCount());

	try
	{
		// create the vertex array
		pVertices = new VERTEX[GetVertexCount()];

		// create the index array
		pIndices = new UINT[GetIndexCount()];

		// setup the indices into the terrain model data and the local vertex/index array
		modelIndex = ((nodeIndexX * (cellWidth - 1)) + (nodeIndexY * (cellHeight - 1) * (terrainWidth - 1))) * 6;


		// load the vertex array and index array with data
		for (UINT j = 0; j < (cellHeight - 1); j++)
		{
			for (UINT i = 0; i < ((cellWidth - 1) * 6); i++)
			{
				pVertices[index] = pTerrainModel[modelIndex];  // copy full data from the terrain vertex into the terrain cell vertex
				pIndices[index] = index;
				modelIndex++;
				index++;
			}

			modelIndex += (terrainWidth * 6) - (cellWidth * 6);
		}

		// initialize the vertex and index buffers with the model data
		pModel_->InitializeBuffers(pDevice, pVertices, pIndices, GetVertexCount(), GetIndexCount());

		// create a public vertex array that will be used for accessing vertex information about this cell
		pVertexList_ = new DirectX::XMFLOAT3[GetVertexCount()];

		// keep a local copy of the vertex position data for this cell
		for (UINT i = 0; i < GetVertexCount(); i++)
		{
			pVertexList_[i] = pVertices[i].position;
		}

	}
	catch (std::bad_alloc & e)
	{
		Log::Error(THIS_FUNC, e.what());
		_DELETE_ARR(pVertices);
		_DELETE_ARR(pIndices);
		_DELETE_ARR(pVertexList_);
		COM_ERROR_IF_FALSE(false, "can't allocate memory for the terrain cell vertices/indices/vertex_list");
	}

	// release the arrays now that the buffers have been initialized
	_DELETE_ARR(pVertices);
	_DELETE_ARR(pIndices);

	return true;
}


// releases the vertex list and the two buffers used for rendering the terrain cell
void TerrainCellClass::ShutdownBuffers()
{
	_DELETE_ARR(pVertexList_);
	pModel_->ShutdownBuffers();

	return;
}


// puts the terrain cell vertex and index buffer on the GPU for rendering
void TerrainCellClass::RenderBuffers(ID3D11DeviceContext* pDeviceContext)
{
	pModel_->RenderBuffers(pDeviceContext);
	return;
}


void TerrainCellClass::CalculateCellDimensions()
{

}


bool TerrainCellClass::BuildLineBuffers(ID3D11Device* pDevice)
{

}


void TerrainCellClass::ShutdownLineBuffers()
{

}