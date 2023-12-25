////////////////////////////////////////////////////////////////////////////////////////////
// Filename:     TerrainCellClass.cpp
// Description:  implementation of the TerrainCellClass functional
//
// Created:      02.05.23
////////////////////////////////////////////////////////////////////////////////////////////
#include "TerrainCellClass.h"


TerrainCellClass::TerrainCellClass(ModelInitializerInterface* pModelInitializer,
	ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext)
	: Model(pDevice, pDeviceContext)
{
	
	try
	{
		this->SetModelInitializer(pModelInitializer);

		// allocate memory for the terrain cell line box model (cube around the terrain cell)
		pLineBoxModel_ = new TerrainCellLineBoxClass(pModelInitializer, pDevice, pDeviceContext);

		// setup a model's type
		this->modelType_ = "terrain_cell";
	}
	catch (std::bad_alloc & e)
	{
		this->Shutdown();

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

bool TerrainCellClass::Initialize(InitTerrainCellData* pInitData,
	const std::vector<VERTEX> & terrainVerticesArr,
	const std::vector<UINT> & terrainIndicesArr)
{
	// check input params
	COM_ERROR_IF_ZERO(terrainVerticesArr.size(), "the input array of vertices is empty");
	COM_ERROR_IF_ZERO(terrainIndicesArr.size(), "the input array of indices is empty");

	try
	{
		bool result = false;

		// initialize a terrain cell by some index
		result = this->InitializeTerrainCell(pInitData,
			terrainVerticesArr,
			terrainIndicesArr);
		COM_ERROR_IF_FALSE(result, "can't initialize the terrain cell model");

		// setup this terrain cell for rendering
		this->SetModelToShaderMediator(pInitData->pModelToShaderMediator);
		this->SetRenderShaderName(pInitData->renderingShaderName);

		// initialize the bounding box lines of this terrain cell
		result = this->InitializeCellLineBox();
		COM_ERROR_IF_FALSE(result, "can't initialize a model with lines of the bounding box");
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		Log::Error(THIS_FUNC, "can't initialize the terrain cell or bounding line box");
		return false;
	}
		
	return true;
} // end Initialize

///////////////////////////////////////////////////////////

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

DirectX::XMVECTOR TerrainCellClass::GetVertexPosByIndex(const UINT index) const
{
	// return a position of vertex by index in a XMVECTOR format
	return this->cellVerticesCoordsList_[index];
}

///////////////////////////////////////////////////////////

void TerrainCellClass::GetVertexPosByIndex(DirectX::XMVECTOR & vertexPos, const UINT index)
{
	// store a position of vertex by index in the input vertexPos variable 
	vertexPos = this->cellVerticesCoordsList_[index];
	return;
}

///////////////////////////////////////////////////////////

UINT TerrainCellClass::GetTerrainCellVertexCount() const
{
	// return a number of vertices in this terrain cell
	return this->GetVertexCount();
}

///////////////////////////////////////////////////////////

UINT TerrainCellClass::GetTerrainCellIndexCount() const
{
	// return a number of indices in this terrain cell
	return this->GetIndexCount();
}

///////////////////////////////////////////////////////////


UINT TerrainCellClass::GetCellLinesIndexCount() const
{
	// return a number of indices in the line box around this terrain cell
	return pLineBoxModel_->GetIndexCount();
}

///////////////////////////////////////////////////////////

void TerrainCellClass::GetCellDimensions(DirectX::XMFLOAT3 & max, 
	DirectX::XMFLOAT3 & min)
{
	// setup values of maximal and minimal dimensions of this cell
	max.x = maxWidth_;
	max.y = maxHeight_;
	max.z = maxDepth_;

	min.x = minWidth_;
	min.y = minHeight_;
	min.z = minDepth_;

	return;
}

///////////////////////////////////////////////////////////

bool TerrainCellClass::CheckIfPosInsideCell(const float posX, const float posZ) const
{
	return ((posX < maxWidth_) && (posX > minWidth_) &&
		    (posZ < maxDepth_) && (posZ > minDepth_));
}




////////////////////////////////////////////////////////////////////////////////////////////
//
//                              PRIVATE FUNCTIONS
//
////////////////////////////////////////////////////////////////////////////////////////////


bool TerrainCellClass::InitializeTerrainCell(InitTerrainCellData* pInitData,
	const std::vector<VERTEX> & terrainVerticesArr,
	const std::vector<UINT> & terrainIndicesArr)
{
	
	bool result = false;

	// load the rendering buffers with the terrain data for this cell index
	result = InitializeTerrainCellBuffers(pInitData,
		terrainVerticesArr,
		terrainIndicesArr);
	COM_ERROR_IF_FALSE(result, "can't initialize buffers for the terrain cells");


	
	// calculate the dimensions of this cell
	CalculateCellDimensions();

	return true;

} // end InitializeTerrainCell

///////////////////////////////////////////////////////////


bool TerrainCellClass::InitializeCellLineBox()
{
	try
	{
		// build the debug line buffers to produce the bounding box around this cell
		bool result = InitializeCellLinesBuffers();
		COM_ERROR_IF_FALSE(result, "can't build buffers for the cell bounding box");

		// setup this line box
		pLineBoxModel_->SetModelToShaderMediator(this->pModelToShaderMediator_);
		pLineBoxModel_->SetRenderShaderName("ColorShaderClass"); 
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		Log::Error(THIS_FUNC, "can't initialize a cell's line box");
		return false;
	}

	return true;

} // end InitializeCellLineBox

///////////////////////////////////////////////////////////


bool TerrainCellClass::InitializeTerrainCellBuffers(InitTerrainCellData* pInitData,
	const std::vector<VERTEX> & terrainVerticesArr,
	const std::vector<UINT> & terrainIndicesArr)
{
	// creates the buffers used for rendering the terrain cell. It also creates a vertex list
	// that is used for other calculations such as determining the size of this cell.
	// The terrain model that was built in the TerrainClass is passed into this function, and
	// then an index into the terrain model is created based on the physical location of this
	// cell using nodeIndexX and nodeIndexY

	UINT modelIndex = 0;                           // an index into the terrain model data
	UINT index = 0;                                // an index into the vertices array
	const UINT numVerticesInQuad = 6;              // the number of vertices in a single terrain quad
	const UINT quadWidthOfCell = pInitData->cellWidth - 1;    // cell width in quads
	const UINT quadHeightOfCell = pInitData->cellHeight - 1;  // cell height in vertices
	const UINT modelIndexStride = (pInitData->terrainWidth * numVerticesInQuad) - (pInitData->cellWidth * numVerticesInQuad); // how many vertices we have to go through before we get to the next line of quads of the cell
	const UINT vertexNumInCellRow = quadWidthOfCell * numVerticesInQuad;

	try
	{
		// arrays for vertices/indices data
		std::vector<VERTEX> verticesArr(quadHeightOfCell * quadWidthOfCell * numVerticesInQuad);
		std::vector<UINT> indicesArr(verticesArr.size());

		// create a public vertex array that will be used for accessing vertex information about this cell
		cellVerticesCoordsList_.resize(verticesArr.size());

		///////////////////////////////////////////////////

		// setup the indices into the terrain model data and the local vertex/index array

		modelIndex = ((pInitData->nodeIndexX * quadWidthOfCell) +                        // horizontal offset
			          (pInitData->nodeIndexY * quadHeightOfCell * (pInitData->terrainWidth - 1))) * // vertical offset
			          numVerticesInQuad;


		auto itVertexFrom = terrainVerticesArr.begin() + modelIndex;
		auto itVertexTo = terrainVerticesArr.begin() + modelIndex + vertexNumInCellRow;

		// load the vertex array and index array with data
		for (UINT j = 0; j < quadHeightOfCell; j++)
		{
			// copy vertices
			std::copy(terrainVerticesArr.begin() + modelIndex, terrainVerticesArr.begin() + modelIndex + vertexNumInCellRow, verticesArr.begin() + index);
			modelIndex += vertexNumInCellRow;

			//std::copy(itVertexFrom, itVertexTo, verticesArr.begin() + index);
			// move iterators to change the range
			//itVertexFrom += vertexNumInCellRow;
			//itVertexTo += vertexNumInCellRow;

			// copy indices:
			//std::copy(terrainIndicesArr.begin() + modelIndex, terrainIndicesArr.begin() + modelIndex + vertexNumInCellRow, indicesArr.begin() + index);
			//index += vertexNumInCellRow;

			// go through the line of quads and copy it into the final arrays
			for (UINT i = 0; i < vertexNumInCellRow; i++)
			{
				// copy full data from the terrain vertex into the terrain cell vertex
				//verticesArr[index] = terrainVerticesArr[modelIndex];
				indicesArr[index] = index;

				// store the position as a XMVECTOR so later it will be easier 
				// for using it during intersection computation
				cellVerticesCoordsList_[index] = XMLoadFloat3(&verticesArr[index].position);


				
				index++;
			}

			// move to the next line of quads
			modelIndex += modelIndexStride;
		}

		///////////////////////////////////////////////////

		// each terrain cell has only one mesh so create it and initialize with data
		this->InitializeOneMesh(verticesArr, indicesArr, pInitData->texturesPaths);

	}
	catch (std::bad_alloc & e)
	{
		Log::Error(THIS_FUNC, e.what());
		COM_ERROR_IF_FALSE(false, "can't allocate memory for the terrain cell vertices/indices/vertex_list");
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		return false;
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
		width  = DirectX::XMVectorGetX(cellVerticesCoordsList_[i]);
		height = DirectX::XMVectorGetY(cellVerticesCoordsList_[i]);
		depth  = DirectX::XMVectorGetZ(cellVerticesCoordsList_[i]);

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
	pLineBoxModel_->InitializeOneMesh(verticesArr, indicesArr, {});
	
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
