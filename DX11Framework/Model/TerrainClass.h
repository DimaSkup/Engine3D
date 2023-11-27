////////////////////////////////////////////////////////////////////////////////////////////
// Filename::    TerrainClass.h
// Description:  the terrain class will encapsulate the model data and 
//               rendering functionality for drawing the terrain (or terrain grid)
//
// Created:      11.03.23
////////////////////////////////////////////////////////////////////////////////////////////
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
#include "../Model/TerrainInitializerInterface.h"
#include "../Model/TerrainInitializer.h"
#include "../Model/GameObject.h"


//////////////////////////////////
// Class name: TerrainClass
//////////////////////////////////
class TerrainClass : public Model
{
private:


public:
	TerrainClass(ModelInitializerInterface* pModelInitializer,
		ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext);
	~TerrainClass();

	virtual bool Initialize(const std::string & filePath) override;

	void Shutdown();
	
	// a function for culling and polygon calculations each frame
	void Frame();   

	// render a terrain cell by particular index
	bool CheckIfSeeCellByIndex(ID3D11DeviceContext* pDeviceContext,
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
	GameObject* GetTerrainCellByIndex(UINT index) const;

	// functions for rendering the polygon render count, the cells drawn count, and the cells culled count
	UINT GetRenderCount() const;
	UINT GetCellsDrawn() const;
	UINT GetCellsCulled() const;

	float GetWidth() const;
	float GetHeight() const;

	// a function to get the current height at the current position on the terrain
	bool GetHeightAtPosition(const float inputX, const float inputZ, float & height); 
	
private:  // restrict a copying of this class instance
	TerrainClass(const TerrainClass & obj);
	TerrainClass & operator=(const TerrainClass & obj);


private:
	void SetupParamsAfterInitialization();


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
	

private:
	std::unique_ptr<TerrainInitializerInterface> pTerrainInitializer_ = std::make_unique<TerrainInitializer>();
	std::shared_ptr<TerrainSetupData> pTerrainSetupData_ = std::make_shared<TerrainSetupData>();
	
	// an array of pointers to pointer to terrain cell game objects
	std::vector<GameObject*> terrainCellsArr_;

	UINT terrainWidth_ = 0;
	UINT terrainHeight_ = 0;

	// some veriables to describe terrain rendering state for each frame
	UINT renderCount_ = 0;    // a number of terrain vertices which were rendered onto the screen
	UINT cellsDrawn_ = 0;     // a number of terrain cells which were rendered
	UINT cellsCulled_ = 0;    // a number of terrain cells which were culled (not rendered)
};
