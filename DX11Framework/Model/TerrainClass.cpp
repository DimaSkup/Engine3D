////////////////////////////////////////////////////////////////////////////////////////////
// Filename::    TerrainClass.cpp
// Description:  the terrain class will encapsulate the model data and 
//               rendering functionality for drawing the terrain (or terrain grid)
//
// Created:      11.03.23
////////////////////////////////////////////////////////////////////////////////////////////
#include "TerrainClass.h"

using namespace DirectX;


TerrainClass::TerrainClass(ModelInitializerInterface* pModelInitializer)
{
	this->SetModelInitializer(pModelInitializer);
	this->AllocateMemoryForElements();
	this->modelType_ = "terrain";
}


TerrainClass::~TerrainClass()
{
	Log::Print("-------------------------------------------------");
	Log::Print("            THE TERRAIN DESTROYMENT:             ");
	Log::Print("-------------------------------------------------");
	Log::Debug(THIS_FUNC_EMPTY);

	// Shutting down of the terrain class, releasing of the memory, etc.
	this->Shutdown();                     

	Log::Print(THIS_FUNC, "The Terrain is destroyed");
}








////////////////////////////////////////////////////////////////////////////////////////////
//
//                                 PUBLIC FUNCTIONS
//
////////////////////////////////////////////////////////////////////////////////////////////


bool TerrainClass::Initialize(const std::string & filePath,
	ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext)
{
	// the Inialize() function will just call the functions for initializing the 
	// vertex and index buffers that will hold the terrain data

	Log::Debug(THIS_FUNC_EMPTY);
	assert(pDevice);
	
	bool result = false;
	Settings* pSettings = Settings::Get();
	
	//TerrainInitializator* pTerrainInit = pTerrainInitializer_.operator->;

	////////////////////////////////////////////////////////

	try
	{

		result = pTerrainInitializer_->Initialize(pSettings, pTerrainSetupData_);
		COM_ERROR_IF_FALSE(result, "can't initialize the terrain model");


		// since the terrain cells have been loaded with data 
		// we don't need vertices/indices data anymore
		//this->verticesArr_.clear();
		//this->indicesArr_.clear();


		// print a message about the initialization process
		std::string debugMsg = this->modelType_ + " is initialized!";
		Log::Debug(THIS_FUNC, debugMsg.c_str());

	}
	catch (COMException & e)
	{
		this->Shutdown();

		Log::Error(e, false);
		Log::Error(THIS_FUNC, "can't initialize the terrain");
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////

void TerrainClass::Shutdown()
{
	//_DELETE_ARR(pTerrainCells_);    // release the terrain cells

	// clear the arrays of vertices/indices data
	//this->verticesArr_.clear();
	//this->indicesArr_.clear();
}

///////////////////////////////////////////////////////////

void TerrainClass::Frame()
{
	// a function that is called to reset the render counts during each frame

	renderCount_ = 0;
	cellsDrawn_ = 0;
	cellsCulled_ = 0;

	return;
}

///////////////////////////////////////////////////////////

bool TerrainClass::CheckIfSeeCellByIndex(ID3D11DeviceContext* pDeviceContext,
	UINT cellID,
	FrustumClass* pFrustum)
{
	// the following function is used to define if we need to render a terrain cells as well as 
	// the orange bounding boxes around this cell by the cellID index.
	// 
	// Each of the render functions takes
	// as input the cell ID so it knows which cell to render or which cell to get the 
	// index count from. It takes as input the FrustumClass pointer so that it can perform
	// culling of the terrain cells.

	float maxWidth = 0.0f;
	float maxHeight = 0.0f;
	float maxDepth = 0.0f;
	float minWidth = 0.0f;
	float minHeight = 0.0f;
	float minDepth = 0.0f;

	// get the dimensions of the terrain cell
	terrainCellsArr_[cellID]->GetCellDimensions(maxWidth, maxHeight, maxDepth, minWidth, minHeight, minDepth);

	// check if the cell is visible. If it is not visible then just return and don't render it
	bool result = pFrustum->CheckRectangle2(maxWidth, maxHeight, maxDepth, minWidth, minHeight, minDepth);
	if (!result)
	{
		// increment the number of cells that were culled
		cellsCulled_++;

		// return that we don't see a cell by the input cellID
		return false;
	}

	// add the polygons in the cell to the render count
	renderCount_ += (terrainCellsArr_[cellID]->GetTerrainCellVertexCount() / 3);

	// increment the number of cells that were actually drawn
	cellsDrawn_++;

	// return that we see a cell by the input cellID
	return true;
}

///////////////////////////////////////////////////////////

void TerrainClass::RenderCellLines(ID3D11DeviceContext* pDeviceContext, UINT cellID)
{
	terrainCellsArr_[cellID]->RenderLineBuffers();
	return;
}

///////////////////////////////////////////////////////////











////////////////////////////////////////////////////////////////////////////////////////////
//
//                               PUBLIC GETTERS
// 
////////////////////////////////////////////////////////////////////////////////////////////

UINT TerrainClass::GetCellIndexCount(UINT cellID) const
{
	return terrainCellsArr_[cellID]->GetTerrainCellIndexCount();
}

///////////////////////////////////////////////////////////

UINT TerrainClass::GetCellLinesIndexCount(UINT cellID) const
{
	return terrainCellsArr_[cellID]->GetCellLinesIndexCount();
}

///////////////////////////////////////////////////////////

UINT TerrainClass::GetCellCount() const
{
	return terrainCellsArr_.size();
}

///////////////////////////////////////////////////////////

TerrainCellClass* TerrainClass::GetTerrainCellByIndex(UINT index) const
{
	return terrainCellsArr_[index];
}

///////////////////////////////////////////////////////////

UINT TerrainClass::GetRenderCount() const
{
	return renderCount_;
}

///////////////////////////////////////////////////////////

UINT TerrainClass::GetCellsDrawn() const
{
	// get the count of terrain cells which were drawn
	return cellsDrawn_;
}

///////////////////////////////////////////////////////////

UINT TerrainClass::GetCellsCulled() const
{
	// get the count of terrain cell which were culled
	return cellsCulled_;
}

///////////////////////////////////////////////////////////

float TerrainClass::GetWidth() const
{
	// get the terrain width
	return static_cast<float>(terrainWidth_);
}

///////////////////////////////////////////////////////////

float TerrainClass::GetHeight() const
{
	// get the terrain height
	return static_cast<float>(terrainHeight_);
}

///////////////////////////////////////////////////////////

bool TerrainClass::GetHeightAtPosition(float inputX, float inputZ, float & height)
{
	// GetHeightAtPosition() returns the current height over the terrain
	// given the X and Z inputs. If it can't find the height because the camera is
	// off the grid then the function returns false and doesn't populate the height
	// input/output variable. The function starts by looping through all the cells until
	// it finds which one the position is inside. Once it has the correct cell it loops
	// through all the triangles in just that cell and figures out which one we are 
	// currently above. Then it returns the height for that each position on that 
	// specific triangle


	UINT cellID = -1;
	UINT index = 0;
	bool foundHeight = false;

	DirectX::XMFLOAT3 vertex1{ 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 vertex2{ 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 vertex3{ 0.0f, 0.0f, 0.0f };

	float maxWidth = 0.0f;
	float maxHeight = 0.0f;
	float maxDepth = 0.0f;
	float minWidth = 0.0f;
	float minHeight = 0.0f;
	float minDepth = 0.0f;

	// loop through all of the terrain cells to find out which one the inputX and
	// inputZ would be inside
	for (size_t i = 0; i < cellCount_; i++)
	{
		// get the current cell dimensions
		terrainCellsArr_[i]->GetCellDimensions(maxWidth, maxHeight, maxDepth, minWidth, minHeight, minDepth);

		// check to see if the positions are in this cell
		if ((inputX < maxWidth) && (inputX > minWidth) && (inputZ < maxDepth) && (inputZ > minDepth))
		{
			cellID = static_cast<UINT>(i);
			i = cellCount_;   // go out from the for cycle
		}
	}


	// if we didn't find a cell then the input position is off the terrain grid
	if (cellID == -1)
	{
		return false;
	}


	// if this is the right cell then check all the triangles in this cell to see what 
	// the height of the triangle at this position is
	for (size_t i = 0; i < terrainCellsArr_[cellID]->GetTerrainCellVertexCount() / 3; i++)
	{
		index = static_cast<UINT>(i * 3);

		vertex1 = ppTerrainCells_[cellID]->cellVerticesCoordsList_[index];
		index++;

		vertex2 = ppTerrainCells_[cellID]->cellVerticesCoordsList_[index];
		index++;

		vertex3 = ppTerrainCells_[cellID]->cellVerticesCoordsList_[index];

		// check to see if this is the polygon we at looking for 
		foundHeight = CheckHeightOfTriangle(inputX, inputZ, height, vertex1, vertex2, vertex3);
		if (foundHeight)
		{
			return true;
		}
	}

	return false;
}

///////////////////////////////////////////////////////////












////////////////////////////////////////////////////////////////////////////////////////////
//
// PRIVATE FUNCTIONS
//
////////////////////////////////////////////////////////////////////////////////////////////


bool TerrainClass::CheckHeightOfTriangle(float x, float z, float & height,
	const DirectX::XMFLOAT3 & vertex0,
	const DirectX::XMFLOAT3 & vertex1,
	const DirectX::XMFLOAT3 & vertex2)
{
	// CheckHeightOfTriangle() is a function which is responsible for determining if a line 
	// intersects the given input triangle. It takes the three points of the triangle
	// and constructs three lines from those three points. Then it tests to see if the
	// position is on the inside of all three lines. If it is inside all three lines
	// then an intersection is found and the height is calculated and returned. If the point
	// is outside any of the three lines then false is returned as there is no intersection of
	// the line with the triangle.

	DirectX::XMVECTOR startVector{ x, 0.0f, z};            	// starting position of the ray that is being cast (camera position)
	DirectX::XMVECTOR directionVector{ 0.0f, -1.0f, 0.0f }; // the direction the ray is being cast (downward)

	DirectX::XMVECTOR vectorOfVertex0 = XMLoadFloat3(&vertex0);     // triangle's vertices
	DirectX::XMVECTOR vectorOfVertex1 = XMLoadFloat3(&vertex1);
	DirectX::XMVECTOR vectorOfVertex2 = XMLoadFloat3(&vertex2);

	DirectX::XMVECTOR edge1;                                        // the 1st edge of the given triangle
	DirectX::XMVECTOR edge2;                                        // the 2nd edge of the given triangle
	DirectX::XMVECTOR edge3;                                        // the 3rd edgt of the given triangle
	DirectX::XMVECTOR normal;                                       // normal vector of the given triangle

	DirectX::XMVECTOR Q; // intersection vector
	
	
	float distance = 0.0f;   // a distance from the origin to the plane
	float distance2 = 0.0f;
	float numerator = 0.0f;   // a numerator and denominator of the equation for definition of intersection between a line and a plane
	float denominator = 0.0f;
	float t = 0.0f;
	bool determinationResult = false;


	
	
	// calculate the two edges from the three points of triangle given
	edge1 = vectorOfVertex1 - vectorOfVertex0;
	edge2 = vectorOfVertex2 - vectorOfVertex0;

	// calculate the normal of 
	normal = XMVector3Cross(edge1, edge2);

	// normalize the normal vector
	normal = XMVector3Normalize(normal);

	// find the distance from the origin to the plane
	distance = XMVectorGetX(XMVector3Dot(-normal, vectorOfVertex0));

	// get the denominator of the equation
	denominator = XMVectorGetX(XMVector3Dot(normal, directionVector));

	// make sure the result doesn't get too close to zero to prevent divide by zero
	if (fabs(denominator) < 0.0001f)
	{
		return false;
	}

	// get the numerator of the equation
	numerator = -1.0f * (XMVectorGetX(XMVector3Dot(normal, startVector)) + distance);

	// calculate where we intersect the triangle (equation: t = -(ax0 + by0 + cz0 + d) / (avX + bvY + cvZ); )
	t = numerator / denominator;

	// find the intersection vector
	Q = startVector + (directionVector * t);

	// find the three edges of the triangle
	edge1 = vectorOfVertex1 - vectorOfVertex0;
	edge2 = vectorOfVertex2 - vectorOfVertex1;
	edge3 = vectorOfVertex0 - vectorOfVertex2;

	// if any result is false we return a false value
	determinationResult = CalculateDeterminant(Q, edge1, normal, vectorOfVertex0);
	if (!determinationResult)
		return false;

	determinationResult = CalculateDeterminant(Q, edge2, normal, vectorOfVertex1);
	if (!determinationResult)
		return false;

	determinationResult = CalculateDeterminant(Q, edge3, normal, vectorOfVertex2);
	if (!determinationResult)
		return false;
	
	// now we have our height
	height = XMVectorGetY(Q);

	return true;
}

///////////////////////////////////////////////////////////

bool TerrainClass::CalculateDeterminant(const XMVECTOR & Q,  // an intersection vector
	const XMVECTOR & edge,                                   // an edge of triangle
	const XMVECTOR & normal,                                 // a normal of triangle
	const XMVECTOR & vecOfVertex)                            // a vector of the triangle's vertex  
{
	DirectX::XMVECTOR edgeNormal;   // a normal of the triangle's edge
	DirectX::XMVECTOR temp;         // a temporal element for the determinant
	float determinant = 0.0f;

	edgeNormal = XMVector3Cross(edge, normal);

	// calculate the determinant to see if it is on the inside, outside, or directly on the edge
	temp = Q - vecOfVertex;

	determinant = XMVectorGetX(XMVector3Dot(edgeNormal, temp));

	// check if it is outside
	if (determinant > 0.001f)
	{
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////

