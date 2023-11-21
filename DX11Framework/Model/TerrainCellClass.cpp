////////////////////////////////////////////////////////////////////////////////////////////
// Filename:     TerrainCellClass.cpp
// Description:  implementation of the TerrainCellClass functional
//
// Created:      02.05.23
////////////////////////////////////////////////////////////////////////////////////////////
#include "TerrainCellClass.h"



TerrainCellClass::TerrainCellClass(ModelInitializerInterface* pModelInitializer)
{
	
	try
	{
		this->SetModelInitializer(pModelInitializer);

		// allocate memory for the model's common elements
		this->AllocateMemoryForElements(); 

		// allocate memory for the terrain cell line box model (cube around the terrain cell)
		pLineBoxModel_ = new TerrainCellLineBoxClass(pModelInitializer);

		// setup a model's type
		this->modelType_ = "terrain_cell";
	}
	catch (std::bad_alloc & e)
	{
		_DELETE(pLineBoxModel_);
		Log::Error(THIS_FUNC, e.what());
		COM_ERROR_IF_FALSE(false, "can't allocate memory for some terrain cell's parts");
	}
}

TerrainCellClass::~TerrainCellClass()
{
	//std::string debugMsg{ "destroyment of the " + pTerrainCellModel_->GetID() };
	//Log::Debug(THIS_FUNC, debugMsg.c_str());
	this->Shutdown();
}



////////////////////////////////////////////////////////////////////////////////////////////
// 
//                             COMMON PUBLIC FUNCTIONS
//
////////////////////////////////////////////////////////////////////////////////////////////
bool TerrainCellClass::Initialize(ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,
	const std::vector<VERTEX> & terrainVerticesArr,
	const UINT nodeIndexX,
	const UINT nodeIndexY,
	const UINT cellHeight,
	const UINT cellWidth,
	const UINT terrainWidth)
{
	try
	{
		bool result = false;
		this->pDevice_ = pDevice;
		this->pDeviceContext_ = pDeviceContext;

		// initialize a terrain cell by some index
		result = this->InitializeTerrainCell(terrainVerticesArr,
			nodeIndexX,
			nodeIndexY,
			cellHeight,
			cellWidth,
			terrainWidth);
		COM_ERROR_IF_FALSE(result, "can't initialize the terrain cell model");

		// initialize the bounding box lines of this terrain cell
		result = this->InitializeCellLineBox();
		COM_ERROR_IF_FALSE(result, "can't initialize a model with lines of the bounding box");
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(THIS_FUNC, "can't initialize the terrain cell or bounding line box");
		return false;
	}
		
	return true;
}


// release the memory from the terrain cell's elements/data
void TerrainCellClass::Shutdown()
{
	_DELETE(pLineBoxModel_);

	return;
}


// puts the terrain cell vertex and index buffer on the GPU for rendering;
// and renders it using a shader
void TerrainCellClass::RenderCell()
{
	// put the vertex and index buffers on the graphics pipeline to prepare 
	// them for drawing, and render it using a shader
	this->Render();
	
	return;
}


// puts the vertex and index buffer for the bounding box on the GPU for rendering;
// it is rendered as a line list
void TerrainCellClass::RenderLineBuffers()
{
	// put the vertex and index buffers on the graphics pipeline to prepare 
	// them for drawing, and render it using a shader
	pLineBoxModel_->Render(D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	return;
}




////////////////////////////////////////////////////////////////////////////////////////////
//
//                                 PUBLIC GETTERS
//
////////////////////////////////////////////////////////////////////////////////////////////

UINT TerrainCellClass::GetTerrainCellVertexCount() const
{
	return this->GetVertexCount();
}


UINT TerrainCellClass::GetTerrainCellIndexCount() const
{
	return this->GetIndexCount();
}


UINT TerrainCellClass::GetCellLinesIndexCount() const
{
	return pLineBoxModel_->GetIndexCount();
}


void TerrainCellClass::GetCellDimensions(float & maxWidth, 
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




////////////////////////////////////////////////////////////////////////////////////////////
//
//                              PRIVATE FUNCTIONS
//
////////////////////////////////////////////////////////////////////////////////////////////


bool TerrainCellClass::InitializeTerrainCell(const std::vector<VERTEX> & terrainVerticesArr,
	const UINT nodeIndexX,
	const UINT nodeIndexY,
	const UINT cellHeight,
	const UINT cellWidth,
	const UINT terrainWidth)
{
	
	bool result = false;

	// load the rendering buffers with the terrain data for this cell index
	result = InitializeTerrainCellBuffers(nodeIndexX,
		nodeIndexY,
		cellHeight,
		cellWidth,
		terrainWidth,
		terrainVerticesArr);
	COM_ERROR_IF_FALSE(result, "can't initialize buffers for the terrain cells");


	// add some terrain textures to this terrain cell
	this->GetTextureArrayObj()->AddTexture(L"data/textures/dirt01d.dds");
	this->GetTextureArrayObj()->AddTexture(L"data/textures/dirt01n.dds");

	// calculate the dimensions of this cell
	CalculateCellDimensions();

	return true;

} // end InitializeTerrainCell

///////////////////////////////////////////////////////////


bool TerrainCellClass::InitializeCellLineBox()
{
	bool result = false;

	// build the debug line buffers to produce the bounding box around this cell
	result = InitializeCellLinesBuffers();
	COM_ERROR_IF_FALSE(result, "can't build buffers for the cell bounding box");

	return true;

} // end InitializeCellLineBox

///////////////////////////////////////////////////////////

bool TerrainCellClass::InitializeTerrainCellBuffers(const UINT nodeIndexX,
	const UINT nodeIndexY,
	const UINT cellHeight,
	const UINT cellWidth,
	const UINT terrainWidth,
	const std::vector<VERTEX> & terrainVerticesArr)
{
	// creates the buffers used for rendering the terrain cell. It also creates a vertex list
	// that is used for other calculations such as determining the size of this cell.
	// The terrain model that was built in the TerrainClass is passed into this function, and
	// then an index into the terrain model is created based on the physical location of this
	// cell using nodeIndexX and nodeIndexY

	UINT modelIndex = 0;           // an index into the terrain model data
	UINT index = 0;                // an index into the vertices array

	try
	{
		// calculate the number of vertices/indices of this terrain cell
		const UINT vertexCount = (cellHeight - 1) * (cellWidth - 1) * 6;
		const UINT indexCount = vertexCount;

		// arrays for vertices/indices data
		std::vector<VERTEX> verticesArr(vertexCount);
		std::vector<UINT> indicesArr(indexCount, 0);

		///////////////////////////////////////////////////
		
		// create a public vertex array that will be used for accessing vertex information about this cell
		cellVerticesCoordsList_.resize(vertexCount);
		
		// setup the indices into the terrain model data and the local vertex/index array
		modelIndex = ((nodeIndexX * (cellWidth - 1)) + (nodeIndexY * (cellHeight - 1) * (terrainWidth - 1))) * 6;

		// load the vertex array and index array with data
		for (UINT j = 0; j < (cellHeight - 1); j++)
		{
			for (UINT i = 0; i < ((cellWidth - 1) * 6); i++)
			{
				// copy full data from the terrain vertex into the terrain cell vertex
				verticesArr[index] = terrainVerticesArr[modelIndex];  
				indicesArr[index] = index;
				modelIndex++;
				index++;
			}

			modelIndex += (terrainWidth * 6) - (cellWidth * 6);
		}

		///////////////////////////////////////////////////

		// keep a local copy of the vertex position data for this cell
		for (UINT i = 0; i < vertexCount; i++)
		{
			cellVerticesCoordsList_[i] = verticesArr[i].position;
		}

		// each terrain cell has only one mesh so create it and initialize with data
		this->InitializeOneMesh(verticesArr, indicesArr);

	}
	catch (std::bad_alloc & e)
	{
		Log::Error(THIS_FUNC, e.what());
		COM_ERROR_IF_FALSE(false, "can't allocate memory for the terrain cell vertices/indices/vertex_list");
	}

	return true;

} // end InitializeTerrainCellBuffers

///////////////////////////////////////////////////////////

void TerrainCellClass::CalculateCellDimensions()
{
	// this function is used to determine the size of this cell;
	// it uses the vertex list we created in InitializeDefaultBuffers to do so.

	float width = 0.0f;
	float height = 0.0f;
	float depth = 0.0f;
	UINT verticesOfThisCell = this->GetVertexCount();

	for (UINT i = 0; i < verticesOfThisCell; i++)
	{
		width  = cellVerticesCoordsList_[i].x;
		height = cellVerticesCoordsList_[i].y;
		depth  = cellVerticesCoordsList_[i].z;

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
	cellCenterPosition_.x = (maxWidth_ - minWidth_) + width;
	cellCenterPosition_.y = (maxHeight_ - minHeight_) + height;
	cellCenterPosition_.z = (maxDepth_ - minDepth_) + depth;

	return;
}

///////////////////////////////////////////////////////////

bool TerrainCellClass::InitializeCellLinesBuffers()
{
	// creates the bouding box that surrounds the terrain cell. It is made up of series of 
	// lines creating a box around the exact dimensions of the terrain cell. This is used
	// for debugging purposes mostly

	constexpr UINT vertexCount = 24;    // set the number of line box vertices in the vertex array
	constexpr UINT indexCount = 24;     // set the number of line box indices in the index array
	UINT index = 0;                             // an index in the vertices/indices array
	DirectX::XMFLOAT3 verticesPos[8] { };       // vertices of the bounding box
	HRESULT hr = S_OK;
			
	// arrays for vertices/indices data
	std::vector<VERTEX> verticesArr(vertexCount);
	std::vector<UINT> indicesArr(indexCount);

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


	// each line box has only one mesh so create it and initialize with data
	pLineBoxModel_->InitializeOneMesh(verticesArr, indicesArr);
	
	return true;
} // end InitializeCellLinesBuffers

///////////////////////////////////////////////////////////

void TerrainCellClass::FillVerticesAndIndicesOfBoundingBox(std::vector<VERTEX> & verticesArr,
	std::vector<UINT> & indicesArr,
	UINT & index,
	const DirectX::XMFLOAT3 & vertexPos)     // vertices data for the bounding line
{
	verticesArr[index].position = vertexPos;
	verticesArr[index].color = lineColor_;
	indicesArr[index] = index;
	index++;
}
