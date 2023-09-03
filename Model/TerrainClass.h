////////////////////////////////////////////////////////////////////
// Filename::    TerrainClass.h
// Description:  the terrain class will encapsulate the model data and 
//               rendering functionality for drawing the terrain (or terrain grid)
//
// Created:      11.03.23
////////////////////////////////////////////////////////////////////
#pragma once


//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <d3d11.h>
#include <DirectXMath.h>
#include <fstream>

#include "../Model/Model.h"
#include "../Model/TerrainCellClass.h"
#include "../Render/frustumclass.h"
#include "../Model/ModelMath.h"


//////////////////////////////////
// Class name: TerrainClass
//////////////////////////////////
class TerrainClass : public Model
{
private:
	struct HeightMapType
	{
		HeightMapType() 
			: position{ 0.0f, 0.0f, 0.0f },
			  normal { 0.0f, 0.0f, 0.0f },
			  color { 1.0f, 1.0f, 1.0f, 1.0f }
		{
		}

		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT4 color;   // RGBA color
	};


public:
	TerrainClass(ModelInitializerInterface* pModelInitializer);
	~TerrainClass();

	virtual bool Initialize(ID3D11Device* pDevice) override;
	
	// a function for culling and polygon calculations each frame
	void Frame();   

	// render a terrain cell by particular index
	TerrainCellClass* Render(ID3D11DeviceContext* pDeviceContext, 
		const UINT cellID, 
		FrustumClass* pFrustum);       

	// render line bounding box of a cell by particular index
	void RenderCellLines(ID3D11DeviceContext* pDeviceContext,
		const UINT cellID); 


	//
	// GETTERS
	//
	UINT GetCellIndexCount(UINT cellID) const;
	UINT GetCellLinesIndexCount(UINT cellID) const;
	UINT GetCellCount() const;

	// functions for rendering the polygon render count, the cells drawn count, and the cells culled count
	UINT GetRenderCount() const;
	UINT GetCellsDrawn() const;
	UINT GetCellsCulled() const;

	float GetWidth() const;
	float GetHeight() const;

	bool GetHeightAtPosition(float inputX, float inputZ, float & height); // a function to get the current height at the current position in the terrain
	
private:  // restrict a copying of this class instance
	TerrainClass(const TerrainClass & obj);
	TerrainClass & operator=(const TerrainClass & obj);


private:
	bool LoadSetupFile(const char* filepath);
	bool LoadBitmapHeightMap();  // the function for loading the height map into the height map array
	bool LoadRawHeightMap();     // a function for loading 16bit RAW height maps

	void SetTerrainCoordinates();
	bool CalculateNormals();
	void CalculateFacesNormals(DirectX::XMFLOAT3* pNormals);
	void CalculateTerrainVectors();  // the function for calculating the tagnent and binormal for the terrain model

	bool LoadColorMap();        // the function for loading the color map into the height map array
	bool BuildTerrainModel();   // the function for building the terrain vertices
	bool LoadTerrainCells(ID3D11Device* pDevice);

	bool CheckHeightOfTriangle(float inputX, 
		float inputZ, 
		float & height, 
		const DirectX::XMFLOAT3 & vertex1, 
		const DirectX::XMFLOAT3 & vertex2, 
		const DirectX::XMFLOAT3 & vertex3);  // the function for determining the height of a triangle from the terrain cell

	bool CalculateDeterminant(const  DirectX::XMVECTOR & Q,  // an intersection vector
		const  DirectX::XMVECTOR & edge,                     // an edge of triangle
		const  DirectX::XMVECTOR & normal,                   // a normal of triangle
		const  DirectX::XMVECTOR & vecOfVertex);             // a vector of the triangle's vertex  
	
	void SkipUntilSymbol(std::ifstream & fin, char symbol);  // go through input stream while we don't find a particular symbol

private:
	std::string modelType_{ "terrain" };

	UINT terrainHeight_ = 0;
	UINT terrainWidth_ = 0;
	float heightScale_ = 0.0f;                     // a degree of smoothing of the terrain
	char* terrainFilename_ = nullptr;              // a name of the terrain height map file
	char* colorMapFilename_ = nullptr;             // a name of the colour map file
	HeightMapType* pHeightMap_ = nullptr;          // a pointer to the height map data array
	TerrainCellClass** ppTerrainCells_ = nullptr;  // an array of pointers to pointer to terrain cell objects

	UINT cellCount_ = 0;                           // a count variable to keep track how many cells are in the array
	UINT renderCount_ = 0;
	UINT cellsDrawn_ = 0;
	UINT cellsCulled_ = 0;
};
