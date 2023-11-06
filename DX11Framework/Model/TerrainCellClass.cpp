////////////////////////////////////////////////////////////////////
// Filename:     TerrainCellClass.cpp
// Description:  implementation of the TerrainCellClass functional
//
// Created:      02.05.23
////////////////////////////////////////////////////////////////////
#include "TerrainCellClass.h"



TerrainCellClass::TerrainCellClass(ModelInitializerInterface* pModelInitializer)
{
	
	try
	{
		this->SetModelInitializer(pModelInitializer);

		// allocate memory for the model's common elements
		this->AllocateMemoryForElements(); 

		// allocate memory for the terrain cell line box model
		pLineBoxModel_ = new TerrainCellLineBoxClass(pModelInitializer);
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



////////////////////////////////////////////////////////////////////
//
//                       PUBLIC FUNCTIONS
//
////////////////////////////////////////////////////////////////////
bool TerrainCellClass::Initialize(ID3D11Device* pDevice,
	VERTEX* pTerrainModel, 
	const UINT nodeIndexX,
	const UINT nodeIndexY,
	const UINT cellHeight,
	const UINT cellWidth,
	const UINT terrainWidth)
{
	try
	{
		bool result = false;

		// initialize a terrain cell by some index
		result = this->InitializeTerrainCell(pDevice,
			pTerrainModel,
			nodeIndexX,
			nodeIndexY,
			cellHeight,
			cellWidth,
			terrainWidth);
		COM_ERROR_IF_FALSE(result, "can't initialize the terrain cell model");

		// initialize the bounding box lines of this terrain cell
		result = this->InitializeCellLineBox(pDevice);
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
	_DELETE_ARR(pVertexList_);       // release the cell rendering buffers

	return;
}


// puts the terrain cell vertex and index buffer on the GPU for rendering;
// and renders it using a shader
void TerrainCellClass::RenderCell(ID3D11DeviceContext* pDeviceContext)
{
	// put the vertex and index buffers on the graphics pipeline to prepare 
	// them for drawing, and render it using a shader
	this->Render(pDeviceContext);
	
	return;
}


// puts the vertex and index buffer for the bounding box on the GPU for rendering;
// it is rendered as a line list
void TerrainCellClass::RenderLineBuffers(ID3D11DeviceContext* pDeviceContext)
{
	// put the vertex and index buffers on the graphics pipeline to prepare 
	// them for drawing, and render it using a shader
	pLineBoxModel_->Render(pDeviceContext, D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	return;
}




//////////////////
//
// GETTERS
//
//////////////////

UINT TerrainCellClass::GetTerrainCellVertexCount() const
{
	return this->GetModelDataObj()->GetVertexCount();
}


UINT TerrainCellClass::GetTerrainCellIndexCount() const
{
	return this->GetModelDataObj()->GetIndexCount();
}


UINT TerrainCellClass::GetCellLinesIndexCount() const
{
	return pLineBoxModel_->GetModelDataObj()->GetIndexCount();
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




////////////////////////////////////////////////////////////////////
//
//                      PRIVATE FUNCTIONS
//
////////////////////////////////////////////////////////////////////


bool TerrainCellClass::InitializeTerrainCell(ID3D11Device* pDevice,
	VERTEX* pTerrainModel,
	const UINT nodeIndexX,
	const UINT nodeIndexY,
	const UINT cellHeight,
	const UINT cellWidth,
	const UINT terrainWidth)
{
	
	bool result = false;
	std::string cellIDName{ "terrainCell_" + std::to_string(nodeIndexX) + "_" + std::to_string(nodeIndexY) };

	// load the rendering buffers with the terrain data for this cell index
	result = InitializeTerrainCellBuffers(pDevice,
		nodeIndexX,
		nodeIndexY,
		cellHeight,
		cellWidth,
		terrainWidth,
		pTerrainModel);
	COM_ERROR_IF_FALSE(result, "can't initialize buffers for the terrain cells");


	// add some terrain textures
	this->GetTextureArray()->AddTexture(L"data/textures/dirt01d.dds");
	this->GetTextureArray()->AddTexture(L"data/textures/dirt01n.dds");

	// set an id of this terrain cell
	this->GetModelDataObj()->SetID(cellIDName);

	// calculate the dimensions of this cell
	CalculateCellDimensions();

	return true;
}


bool TerrainCellClass::InitializeCellLineBox(ID3D11Device* pDevice)
{
	std::string cellLinesID{ this->GetModelDataObj()->GetID() + "_bounding_box" };
	bool result = false;

	// build the debug line buffers to produce the bounding box around this cell
	result = InitializeCellLinesBuffers(pDevice);
	COM_ERROR_IF_FALSE(result, "can't build buffers for the cell bounding box");

	// set an id of this cell bounding box model
	pLineBoxModel_->GetModelDataObj()->SetID(cellLinesID);

	return true;
}


// creates the buffers used for rendering the terrain cell. It also creates a vertex list
// that is used for other calculations such as determining the size of this cell.
// The terrain model that was built in the TerrainClass is passed into this function, and
// then an index into the terrain model is created based on the physical location of this
// cell using nodeIndexX and nodeIndexY
bool TerrainCellClass::InitializeTerrainCellBuffers(ID3D11Device* pDevice,
	const UINT nodeIndexX,
	const UINT nodeIndexY,
	const UINT cellHeight,
	const UINT cellWidth,
	const UINT terrainWidth,
	VERTEX* pTerrainModel)
{
	VERTEX* pVertices = nullptr;   // an array of terrain cells vertices
	UINT* pIndices = nullptr;      // an array of vertex indices of the terrain cells
	UINT vertexCount = 0;
	UINT indexCount = 0;
	UINT modelIndex = 0;           // an index into the terrain model data
	UINT index = 0;                // an index into the vertices array


	// calculate the number of vertices/indices of this terrain cell
	vertexCount = (cellHeight - 1) * (cellWidth - 1) * 6;
	indexCount = vertexCount;           

	try
	{
		// allocate memory for the vertices/indices arrays
		this->GetModelDataObj()->AllocateVerticesAndIndicesArrays(vertexCount, indexCount);

		// create a public vertex array that will be used for accessing vertex information about this cell
		pVertexList_ = new DirectX::XMFLOAT3[vertexCount];

		// we use the direct pointers to vertex/index array for better performance during initialization it with data
		pVertices = *(this->GetModelDataObj()->GetAddressOfVerticesData());
		pIndices = *(this->GetModelDataObj()->GetAddressOfIndicesData());

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
		bool result = this->InitializeDefaultBuffers(pDevice, this->GetModelDataObj());
		COM_ERROR_IF_FALSE(result, "can't initialize buffers for the terrain cell model");

		// keep a local copy of the vertex position data for this cell
		for (UINT i = 0; i < vertexCount; i++)
		{
			pVertexList_[i] = pVertices[i].position;
		}

	}
	catch (std::bad_alloc & e)
	{
		this->GetModelDataObj()->Shutdown();  // delete vertices/indices data
		_DELETE_ARR(pVertexList_);
		Log::Error(THIS_FUNC, e.what());
		COM_ERROR_IF_FALSE(false, "can't allocate memory for the terrain cell vertices/indices/vertex_list");
	}

	// release the arrays now that the buffers have been initialized
	this->GetModelDataObj()->Shutdown();

	return true;
}


// this function is used to determine the size of this cell;
// it uses the vertex list we created in InitializeDefaultBuffers to do so.
void TerrainCellClass::CalculateCellDimensions()
{
	float width = 0.0f;
	float height = 0.0f;
	float depth = 0.0f;

	for (UINT i = 0; i < this->GetModelDataObj()->GetVertexCount(); i++)
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
	cellCenterPosition_.x = (maxWidth_ - minWidth_) + width;
	cellCenterPosition_.y = (maxHeight_ - minHeight_) + height;
	cellCenterPosition_.z = (maxDepth_ - minDepth_) + depth;

	return;
}


// creates the bouding box that surrounds the terrain cell. It is made up of series of 
// lines creating a box around the exact dimensions of the terrain cell. This is used
// for debugging purposes mostly
bool TerrainCellClass::InitializeCellLinesBuffers(ID3D11Device* pDevice)
{
	constexpr UINT vertexCount = 24;            // set the number of vertices in the vertex array
	constexpr UINT indexCount = 24;             // set the number of indices in the index array
	VERTEX* pVertices = nullptr;
	UINT* pIndices = nullptr;
	UINT index = 0;                             // an index in the vertices/indices array
	DirectX::XMFLOAT3 verticesPos[8] { };       // vertices of the bounding box
	HRESULT hr = S_OK;
			
	// allocate memory for the vertices/indices arrays
	pLineBoxModel_->GetModelDataObj()->AllocateVerticesAndIndicesArrays(vertexCount, indexCount);

	// we use the direct pointers to vertex/index array for better performance during initialization it with data
	pVertices = *(pLineBoxModel_->GetModelDataObj()->GetAddressOfVerticesData());
	pIndices = *(pLineBoxModel_->GetModelDataObj()->GetAddressOfIndicesData());


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
	FillVerticesAndIndicesOfBoundingBox(pVertices, pIndices, index, verticesPos[0]);  // near
	FillVerticesAndIndicesOfBoundingBox(pVertices, pIndices, index, verticesPos[1]);  // near
	FillVerticesAndIndicesOfBoundingBox(pVertices, pIndices, index, verticesPos[3]);  // far
	FillVerticesAndIndicesOfBoundingBox(pVertices, pIndices, index, verticesPos[2]);  // far

	FillVerticesAndIndicesOfBoundingBox(pVertices, pIndices, index, verticesPos[0]);  // left
	FillVerticesAndIndicesOfBoundingBox(pVertices, pIndices, index, verticesPos[3]);  // left
	FillVerticesAndIndicesOfBoundingBox(pVertices, pIndices, index, verticesPos[1]);  // right
	FillVerticesAndIndicesOfBoundingBox(pVertices, pIndices, index, verticesPos[2]);  // right

	// upper side horizontal lines
	FillVerticesAndIndicesOfBoundingBox(pVertices, pIndices, index, verticesPos[4]);  // near
	FillVerticesAndIndicesOfBoundingBox(pVertices, pIndices, index, verticesPos[5]);  // near
	FillVerticesAndIndicesOfBoundingBox(pVertices, pIndices, index, verticesPos[7]);  // far
	FillVerticesAndIndicesOfBoundingBox(pVertices, pIndices, index, verticesPos[6]);  // far

	FillVerticesAndIndicesOfBoundingBox(pVertices, pIndices, index, verticesPos[4]);  // left
	FillVerticesAndIndicesOfBoundingBox(pVertices, pIndices, index, verticesPos[7]);  // left
	FillVerticesAndIndicesOfBoundingBox(pVertices, pIndices, index, verticesPos[5]);  // right
	FillVerticesAndIndicesOfBoundingBox(pVertices, pIndices, index, verticesPos[6]);  // right



	// 4 vertical lines: 
	FillVerticesAndIndicesOfBoundingBox(pVertices, pIndices, index, verticesPos[6]);  // far right
	FillVerticesAndIndicesOfBoundingBox(pVertices, pIndices, index, verticesPos[2]);  // far right
	FillVerticesAndIndicesOfBoundingBox(pVertices, pIndices, index, verticesPos[7]);  // far left
	FillVerticesAndIndicesOfBoundingBox(pVertices, pIndices, index, verticesPos[3]);  // far left

	FillVerticesAndIndicesOfBoundingBox(pVertices, pIndices, index, verticesPos[5]);  // near right
	FillVerticesAndIndicesOfBoundingBox(pVertices, pIndices, index, verticesPos[1]);  // near right
	FillVerticesAndIndicesOfBoundingBox(pVertices, pIndices, index, verticesPos[4]);  // near left
	FillVerticesAndIndicesOfBoundingBox(pVertices, pIndices, index, verticesPos[0]);  // near left

																					  // initialize the vertex and index buffers with the model data
	bool result = pLineBoxModel_->InitializeDefaultBuffers(pDevice, pLineBoxModel_->GetModelDataObj());
	COM_ERROR_IF_FALSE(result, "can't initialize buffers for the cell lines model");


	
	return true;
}


void TerrainCellClass::FillVerticesAndIndicesOfBoundingBox(VERTEX* verticesArr,
	UINT* indicesArr,
	UINT & index,
	const DirectX::XMFLOAT3 & vertexPos)     // vertices data for the bounding line
{
	verticesArr[index].position = vertexPos;
	verticesArr[index].color = lineColor_;
	indicesArr[index] = index;
	index++;
}