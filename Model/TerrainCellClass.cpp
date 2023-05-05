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
	this->Shutdown();
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
	return lineIndexCount_;
}


void TerrainCellClass::GetCellDimentions(float & maxWidth, 
	float & maxHeight, 
	float & maxDepth,
	float & minWidth,
	float & minHeight, 
	float & minDepth)
{
	maxWidth = maxWidth_;
	maxHeight = maxHeight_;
	maxDepth = maxDepth_;

	minWidth = minWidth_;
	minHeight = minHeight_;
	minDepth = minDepth_;

	return;
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


// this function is used to determine the size of this cell;
// it uses the vertex list we created in InitializeBuffers to do so.
void TerrainCellClass::CalculateCellDimensions()
{
	float width = 0.0f;
	float height = 0.0f;
	float depth = 0.0f;

	for (UINT i = 0; i < GetVertexCount(); i++)
	{
		width = pVertexList_[i].x;
		height = pVertexList_[i].y;
		depth = pVertexList_[i].z;

		// check if the width exceeds the minimum or maximum
		if (width > maxWidth_)
		{
			maxWidth_ = width;
		}
		if (width < minWidth_)
		{
			minWidth_ = width;
		}

		// check if the height exceeds the minimum or maximum
		if (height > maxHeight_)
		{
			maxHeight_ = height;
		}
		if (height < minHeight_)
		{
			minHeight_ = height;
		}

		// check if the depth exceeds the minimum of maximum
		if (depth > maxDepth_)
		{
			maxDepth_ = depth;
		}
		if (depth < minDepth_)
		{
			minDepth_ = depth;
		}
	}

	// calculate the center position of this cell
	position_.x = (maxWidth_ - minWidth_) + width;
	position_.y = (maxHeight_ - minHeight_) + height;
	position_.z = (maxDepth_ - minDepth_) + depth;

	return;
}


// creates the bouding box that surrounds the terrain cell. It is made up of series of 
// lines creating a box around the exact dimensions of the terrain cell. This is used
// for debugging purposes mostly
bool TerrainCellClass::BuildLineBuffers(ID3D11Device* pDevice)
{
	constexpr UINT vertexCount = 24;                           // set the number of vertices in the vertex array
	constexpr UINT indexCount = 24;                           // set the number of indices in the index array
	ColorVertexType verticesArr[vertexCount] {};                 // an array of vertices
	UINT indicesArr[indexCount] = { 0 };                              // an array of indices       
	UINT index = 0;                                           // an index in the vertices/indices array
	DirectX::XMFLOAT3 verticesPos[8] { };
	HRESULT hr = S_OK;
	

	pLineVertexBuffer_ = std::make_unique<VertexBuffer<ColorVertexType>>();   // create the vertex buffer
	pLineIndexBuffer_ = std::make_unique<IndexBuffer>();                      // create the index buffer
	if (!pLineVertexBuffer_ || !pLineIndexBuffer_)
	{
		COM_ERROR_IF_FALSE(false, "can't allocate memory for the vertex/index buffer object");
	}
		

	// setup vertices position of the bounding box:

	// upper side of the box
	verticesPos[0] = { minWidth_, minHeight_, minDepth_ };
	verticesPos[1] = { maxWidth_, minHeight_, minDepth_ };
	verticesPos[2] = { maxWidth_, minHeight_, maxDepth_ };
	verticesPos[3] = { minWidth_, minHeight_, maxDepth_ };

	// lower side of the box
	verticesPos[4] = { minWidth_, maxHeight_, minDepth_ };
	verticesPos[5] = { maxWidth_, maxHeight_, minDepth_ };
	verticesPos[6] = { maxWidth_, maxHeight_, maxDepth_ };
	verticesPos[7] = { minWidth_, maxHeight_, maxDepth_ };

	// fill in the vertex and index array with data:
	// 8 horizontal lines:

	// lower side horizontal lines
	FillVerticesAndIndicesOfBoundingBox(verticesArr, indicesArr, index, verticesPos[0]);  // near
	FillVerticesAndIndicesOfBoundingBox(verticesArr, indicesArr, index, verticesPos[1]);  // near
	FillVerticesAndIndicesOfBoundingBox(verticesArr, indicesArr, index, verticesPos[3]);  // far
	FillVerticesAndIndicesOfBoundingBox(verticesArr, indicesArr, index, verticesPos[2]);  // far

	FillVerticesAndIndicesOfBoundingBox(verticesArr, indicesArr, index, verticesPos[0]);  // left
	FillVerticesAndIndicesOfBoundingBox(verticesArr, indicesArr, index, verticesPos[3]);  // left
	FillVerticesAndIndicesOfBoundingBox(verticesArr, indicesArr, index, verticesPos[1]);  // right
	FillVerticesAndIndicesOfBoundingBox(verticesArr, indicesArr, index, verticesPos[2]);  // right

	// upper side horizontal lines
	FillVerticesAndIndicesOfBoundingBox(verticesArr, indicesArr, index, verticesPos[4]);  // near
	FillVerticesAndIndicesOfBoundingBox(verticesArr, indicesArr, index, verticesPos[5]);  // near
	FillVerticesAndIndicesOfBoundingBox(verticesArr, indicesArr, index, verticesPos[7]);  // far
	FillVerticesAndIndicesOfBoundingBox(verticesArr, indicesArr, index, verticesPos[6]);  // far

	FillVerticesAndIndicesOfBoundingBox(verticesArr, indicesArr, index, verticesPos[4]);  // left
	FillVerticesAndIndicesOfBoundingBox(verticesArr, indicesArr, index, verticesPos[7]);  // left
	FillVerticesAndIndicesOfBoundingBox(verticesArr, indicesArr, index, verticesPos[5]);  // right
	FillVerticesAndIndicesOfBoundingBox(verticesArr, indicesArr, index, verticesPos[6]);  // right



	// 4 vertical lines: 
	FillVerticesAndIndicesOfBoundingBox(verticesArr, indicesArr, index, verticesPos[6]);  // far right
	FillVerticesAndIndicesOfBoundingBox(verticesArr, indicesArr, index, verticesPos[2]);  // far right
	FillVerticesAndIndicesOfBoundingBox(verticesArr, indicesArr, index, verticesPos[7]);  // far left
	FillVerticesAndIndicesOfBoundingBox(verticesArr, indicesArr, index, verticesPos[3]);  // far left

	FillVerticesAndIndicesOfBoundingBox(verticesArr, indicesArr, index, verticesPos[5]);  // near right
	FillVerticesAndIndicesOfBoundingBox(verticesArr, indicesArr, index, verticesPos[1]);  // near right
	FillVerticesAndIndicesOfBoundingBox(verticesArr, indicesArr, index, verticesPos[4]);  // near left
	FillVerticesAndIndicesOfBoundingBox(verticesArr, indicesArr, index, verticesPos[0]);  // near left

	// initialize vertex/index buffer with data
	hr = pLineVertexBuffer_->InitializeDefault(pDevice, verticesArr, vertexCount);
	COM_ERROR_IF_FAILED(hr, "can't initialize the line vertex buffer with data");

	hr = pLineIndexBuffer_->Initialize(pDevice, indicesArr, indexCount);
	COM_ERROR_IF_FAILED(hr, "can't initialize the line index buffer with data");

	// store the index count for rendering
	lineIndexCount_ = indexCount;

	return true;
}


void TerrainCellClass::FillVerticesAndIndicesOfBoundingBox(ColorVertexType* verticesArr,
	UINT* indicesArr,
	UINT & index,
	const DirectX::XMFLOAT3 & vertexPos)     // vertices data for the bounding line
{
	verticesArr[index].position = vertexPos;
	verticesArr[index].color = lineColor_;
	indicesArr[index] = index;
	index++;
}
