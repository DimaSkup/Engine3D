////////////////////////////////////////////////////////////////////////////////////////////
// Filename::    TerrainClass.cpp
// Description:  the terrain class will encapsulate the model data and 
//               rendering functionality for drawing the terrain (or terrain grid)
//
// Created:      11.03.23
////////////////////////////////////////////////////////////////////////////////////////////
#include "TerrainClass.h"

using namespace DirectX;


TerrainClass::TerrainClass(ModelInitializerInterface* pModelInitializer,
	ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext)
	: Model(pDevice, pDeviceContext)
{
	this->SetModelInitializer(pModelInitializer);
	this->modelType_ = "terrain";
}


TerrainClass::~TerrainClass()
{
	Log::Print("-------------------------------------------------");
	Log::Print("            THE TERRAIN DESTROYMENT:             ");
	Log::Print("-------------------------------------------------");
	Log::Debug(LOG_MACRO);

	// Shutting down of the terrain class, releasing of the memory, etc.
	this->Shutdown();                     

	Log::Print(LOG_MACRO, "The Terrain is destroyed");
}








////////////////////////////////////////////////////////////////////////////////////////////
//
//                              PUBLIC COMMON FUNCTIONS
//
////////////////////////////////////////////////////////////////////////////////////////////


bool TerrainClass::Initialize(const std::string & filePath)
{
	// the Inialize() function will just call the functions for initializing the 
	// vertex and index buffers that will hold the terrain data

	Log::Debug(LOG_MACRO);

	bool result = false;
	Settings* pSettings = Settings::Get();
	std::unique_ptr<TerrainInitializerInterface> pTerrainInitializer = std::make_unique<TerrainInitializer>();
	
	////////////////////////////////////////////////////////

	try
	{
		// check that we've initialize some members of the model because we need it
		// during initialization of the terrain
		assert(pModelInitializer_ != nullptr);
		assert(pModelToShaderMediator_ != nullptr);

		pTerrainSetupData_->renderingShaderName = this->GetRenderShaderName();

		result = pTerrainInitializer->Initialize(pSettings, pTerrainSetupData_,
			this->pDevice_,
			this->pDeviceContext_,
			terrainCellsArr_,
			pModelInitializer_,
			pModelToShaderMediator_);
		COM_ERROR_IF_FALSE(result, "can't initialize the terrain model");


		// since the terrain cells have been loaded with data 
		// we don't need vertices/indices data anymore
		//this->verticesArr_.clear();
		//this->indicesArr_.clear();


		// print a message about the initialization process
		std::string debugMsg = this->modelType_ + " is initialized!";
		Log::Debug(LOG_MACRO, debugMsg.c_str());

	}
	catch (COMException & e)
	{
		this->Shutdown();

		Log::Error(e, false);
		Log::Error(LOG_MACRO, "can't initialize the terrain");
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

	cellsDrawn_ = 0;
	cellsCulled_ = 0;

	return;
}

///////////////////////////////////////////////////////////

bool TerrainClass::CheckIfSeeCellByIndex(UINT cellID,
	FrustumClass* pFrustum)
{
	// the following function is used to define if we need to render a terrain cells as well as 
	// the orange bounding boxes around this cell by the cellID index.
	// 
	// Each of the render functions takes
	// as input the cell ID so it knows which cell to render or which cell to get the 
	// index count from. It takes as input the FrustumClass pointer so that it can perform
	// culling of the terrain cells.

	DirectX::XMFLOAT3 maxDimensions{ 0.0f, 0.0f, 0.0f };  // max width / height / depth 
	DirectX::XMFLOAT3 minDimensions{ 0.0f, 0.0f, 0.0f };  // min width / height / depth

	TerrainCellClass* pTerrainCellModel = static_cast<TerrainCellClass*>(terrainCellsArr_[cellID]->GetModel());

	// get the dimensions of the terrain cell
	pTerrainCellModel->GetCellDimensions(maxDimensions, minDimensions);

	// check if the cell is visible. If it is not visible then just return and don't render it
	bool result = pFrustum->CheckRectangle2(maxDimensions.x,  // width
		maxDimensions.y,  // height
		maxDimensions.z,  // depth
		minDimensions.x,  // width
		minDimensions.y,  // height
		minDimensions.z); // depth
	if (!result)
	{
		// increment the number of cells that were culled
		cellsCulled_++;

		// return that we don't see a cell by the input cellID
		return false;
	}

	// increment the number of cells that were actually drawn
	cellsDrawn_++;

	// return that we see a cell by the input cellID
	return true;

} // end CheckIfSeeCellByIndex

///////////////////////////////////////////////////////////

void TerrainClass::RenderCellLines(UINT index)
{
	// render a line box around a terrain cell by the index
	this->GetTerrainCellModelByIndex(index)->RenderLineBuffers();

	return;
}

///////////////////////////////////////////////////////////











////////////////////////////////////////////////////////////////////////////////////////////
//
//                               PUBLIC GETTERS
// 
////////////////////////////////////////////////////////////////////////////////////////////

UINT TerrainClass::GetCellIndexCount(UINT index) const
{
	// return a number of vertices in the terrain cell by index
	return this->GetTerrainCellModelByIndex(index)->GetTerrainCellIndexCount();
}

///////////////////////////////////////////////////////////

UINT TerrainClass::GetCellLinesIndexCount(UINT index) const
{
	// return a number of indices in the terrain cell by index
	return this->GetTerrainCellModelByIndex(index)->GetCellLinesIndexCount();
}

///////////////////////////////////////////////////////////

UINT TerrainClass::GetCellCount() const
{
	// return a number of terrain cells in this terrain
	return static_cast<UINT>(terrainCellsArr_.size());
}

///////////////////////////////////////////////////////////

RenderableGameObject* TerrainClass::GetTerrainCellGameObjByIndex(const UINT index) const
{
	// return a ptr to the terrain cell game object by the index
	return terrainCellsArr_[index];
}

///////////////////////////////////////////////////////////

TerrainCellClass* TerrainClass::GetTerrainCellModelByIndex(const UINT index) const
{
	// return a ptr to the terrain cell model (TerrainCellClass) by the index
	TerrainCellClass* pTerrainCell = static_cast<TerrainCellClass*>(terrainCellsArr_[index]->GetModel());
	return pTerrainCell;
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

bool TerrainClass::GetHeightAtPosition(const float posX, 
	const float posZ,
	float & height)     // during execution we update this variable
{
	// GetHeightAtPosition() returns the current height over the terrain
	// given the X and Z inputs. If it can't find the height because the camera is
	// off the grid then the function returns false and doesn't populate the height
	// input/output variable. The function starts by looping through all the cells until
	// it finds which one the position is inside. Once it has the correct cell it loops
	// through all the triangles in just that cell and figures out which one we are 
	// currently above. Then it returns the height for that each position on that 
	// specific triangle

	//DirectX::XMFLOAT3 maxDimensions{ 0.0f, 0.0f, 0.0f };  // max width / height / depth 
	//DirectX::XMFLOAT3 minDimensions{ 0.0f, 0.0f, 0.0f };  // min width / height / depth

	UINT cellID = -1;
	bool isPosInsideCell = false;  


	// ---------------------------------------------------- //

	// loop through all of the terrain cells to find out which one the inputX and
	// inputZ would be inside
	for (UINT i = 0; i < this->GetCellCount(); i++)
	{
		// check if the current position is inside the cell
		if (this->GetTerrainCellModelByIndex(i)->CheckIfPosInsideCell(posX, posZ))
		{
			cellID = static_cast<UINT>(i);  // store the index of this cell
			i = this->GetCellCount();       // ... and go out from the for cycle
		} 
	} // for

	// if we didn't find a cell then the input position is off the terrain grid
	if (cellID == -1)
	{
		return false;
	}


	// ---------------------------------------------------- //

	DirectX::XMVECTOR vertex1{ 0.0f, 0.0f, 0.0f };
	DirectX::XMVECTOR vertex2{ 0.0f, 0.0f, 0.0f };
	DirectX::XMVECTOR vertex3{ 0.0f, 0.0f, 0.0f };
	UINT index = 0;

	// get a terrain cell model by the cellID
	TerrainCellClass* pTerrainCell = this->GetTerrainCellModelByIndex(cellID);

	// if this is the right cell then check all the triangles in this cell to see what 
	// the height of the triangle at this position is
	for (UINT i = 0; i < pTerrainCell->GetTerrainCellVertexCount() / 3; i++)
	{
		index = i * 3;

		vertex1 = pTerrainCell->GetVertexPosByIndex(index);
		index++;

		vertex2 = pTerrainCell->GetVertexPosByIndex(index);
		index++;

		vertex3 = pTerrainCell->GetVertexPosByIndex(index);

		// check to see if this is the polygon we are looking for 
		if (CheckHeightOfTriangle(posX, posZ, height, vertex1, vertex2, vertex3))
		{
			return true;
		}

	} // for

	return false;
} // end GetHeightAtPosition

///////////////////////////////////////////////////////////












////////////////////////////////////////////////////////////////////////////////////////////
//
// PRIVATE FUNCTIONS
//
////////////////////////////////////////////////////////////////////////////////////////////


void TerrainClass::SetupParamsAfterInitialization()
{
	// after initialization of the terrain model we have
	// to setup some properties of the class to use it later for different purposes;

	terrainWidth_ = pTerrainSetupData_->terrainWidth;
	terrainHeight_ = pTerrainSetupData_->terrainHeight;

	return;

} // end SetupParamsAfterInitialization

///////////////////////////////////////////////////////////

bool TerrainClass::CheckHeightOfTriangle(float posX, float posZ, float & height,
	const DirectX::XMVECTOR & v0,
	const DirectX::XMVECTOR & v1,
	const DirectX::XMVECTOR & v2)
{
	// CheckHeightOfTriangle() is a function which is responsible for determining if a line 
	// intersects the given input triangle. It takes the three points of the triangle
	// and constructs three lines from those three points. Then it tests to see if the
	// position is on the inside of all three lines. If it is inside all three lines
	// then an intersection is found and the height is calculated and returned. If the point
	// is outside any of the three lines then false is returned as there is no intersection of
	// the line with the triangle.

	DirectX::XMVECTOR rayOrigin{ posX, 0.0f, posZ };          // starting position of the ray that is being cast (camera position)

	// intersection(plane, ray.p0, ray.p1)
	DirectX::XMVECTOR inter_vector = DirectX::XMPlaneIntersectLine(
		DirectX::XMPlaneFromPoints(v0, v1, v2),   // plane
		rayOrigin,                                // ray.p0 (begin)
		rayOrigin + rayDirection_);               // ray.p1 (end)

	XMVECTOR u(v1 - v0);
	XMVECTOR v(v2 - v0);
	XMVECTOR w(inter_vector - v0);                  // a vector from v0 to the intersection point

	float uu = XMVectorGetX(XMVector3Dot(u, u));
	float uv = XMVectorGetX(XMVector3Dot(u, v));
	float vv = XMVectorGetX(XMVector3Dot(v, v));              
	float wu = XMVectorGetX(XMVector3Dot(w, u));
	float wv = XMVectorGetX(XMVector3Dot(w, v));
	float D_inv = 1.0f / (uv * uv - uu * vv);       // inverted denominator (1.0 / denominator)

	// get and test parametric coords
	float s = (uv * wv - vv * wu) * D_inv;
	if ((s < 0.0) || (s > 1.0))                     // iPoint is outside triangle
		return PARAM_LINE_NO_INTERSECT;
	float t = (uv * wu - uu * wv) * D_inv;
	if ((t < 0.0) || ((s + t) > 1.0))               // iPoint is outside triangle
		return PARAM_LINE_NO_INTERSECT;

	// get the height value of the intersection point
	height = XMVectorGetY(inter_vector);

	return true;

// OLD CODE
#if 0
	DirectX::XMVECTOR startVector{ posX, 0.0f, posZ };        // starting position of the ray that is being cast (camera position)
	DirectX::XMVECTOR directionVector{ 0.0f, -1.0f, 0.0f };   // the direction the ray is being cast (downward)
	DirectX::XMVECTOR edge1;                                  // the 1st edge of the given triangle
	DirectX::XMVECTOR edge2;                                  // the 2nd edge of the given triangle
	DirectX::XMVECTOR edge3;                                  // the 3rd edgt of the given triangle
	DirectX::XMVECTOR normal;                                 // normal vector of the given triangle

	DirectX::XMVECTOR Q; // intersection vector
	
	float distance = 0.0f;   // a distance from the origin to the plane
	float distance2 = 0.0f;
	float numerator = 0.0f;   // a numerator and denominator of the equation for definition of intersection between a line and a plane
	float denominator = 0.0f;
	float t = 0.0f;

	// ---------------------------------------------------- //
	
	// calculate the two edges from the three points of triangle given
	edge1 = v1 - v0;
	edge2 = v2 - v0;

	// calculate the normal (using cross product) of two vectors and normalize it
	normal = XMVector3Normalize(XMVector3Cross(edge1, edge2));

	// get the denominator of the equation: using this we define is we in positive half-space
	denominator = XMVectorGetX(XMVector3Dot(normal, directionVector));

	// make sure the result doesn't get too close to zero to prevent divide by zero
	if (fabs(denominator) < 0.0001f)
	{
		return false;
	}

	// ---------------------------------------------------- //

	// find the distance from the origin to the plane
	distance = XMVectorGetX(XMVector3Dot(-normal, v0));

	// get the numerator of the equation
	numerator = -1.0f * (XMVectorGetX(XMVector3Dot(normal, startVector)) + distance);

	// calculate a parameter for point where we intersect the triangle 
	// (equation: t = -(ax0 + by0 + cz0 + d) / (avX + bvY + cvZ); )
	t = numerator / denominator;

	// find the intersection vector (point of intersection): (p = p0 + v*t)
	Q = startVector + (directionVector * t);

	// find the three edges of the triangle
	//edge1 = vectorOfVertex1 - vectorOfVertex0;
	edge2 = v2 - v1;
	edge3 = v0 - v2;

	// if any result is false we return a false value
	if (!CalculateDeterminant(Q, edge1, normal, v0))
		return false;

	if (!CalculateDeterminant(Q, edge2, normal, v1))
		return false;

	if (!CalculateDeterminant(Q, edge3, normal, v2))
		return false;
	
	// now we have our height
	height = XMVectorGetY(Q);

#endif

	return true;
}

///////////////////////////////////////////////////////////

bool TerrainClass::CalculateDeterminant(const XMVECTOR & Q,  // an intersection vector
	const XMVECTOR & edge,                                   // an edge of triangle
	const XMVECTOR & normal,                                 // a normal of triangle
	const XMVECTOR & vecOfVertex)                            // a vector of the triangle's vertex  
{
	// a normal of the triangle's edge
	DirectX::XMVECTOR edgeNormal = XMVector3Cross(edge, normal);

	// a temporal element for the determinant
	DirectX::XMVECTOR temp = Q - vecOfVertex;

	// calculate the determinant to see if it is on the inside, outside, or directly on the edge
	float determinant = XMVectorGetX(XMVector3Dot(edgeNormal, temp));

	// if the determinant < zero it is inside of the triangle so return true 
	return (determinant < 0.001f);
}

///////////////////////////////////////////////////////////

