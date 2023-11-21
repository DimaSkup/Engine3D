////////////////////////////////////////////////////////////////////////////////////////////
// Filename:     TerrainCellClass.h
// Description:  TerrainCellClass is a class that encapsulates the
//               functionality of rendering and other calculations for 
//               idividual terrain cells. Each terrain cell is created 
//               from subset of the terrain model and represents a unique
//               33x33 (or someting else) vertex section of that terrain.
//               This class also has additional buffers and structures to build
//               the orange line list bounding box around this cell for debugging
//               purposes.
// Created:      01.05.23
////////////////////////////////////////////////////////////////////////////////////////////
#pragma once


//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include "../Model/Model.h"
#include "../Model/TerrainCellLineBoxClass.h"   // line box model around a terrain cell


//////////////////////////////////
// Class name: TerrainCellClass
//////////////////////////////////
class TerrainCellClass : public Model
{
public:
	TerrainCellClass(ModelInitializerInterface* pModelInitializer);
	~TerrainCellClass();

	// for initialization of terrain cells we don't use this function but use
	// another Initialize one (look down)
	virtual bool Initialize(const std::string & filePath,
		ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext) override { return false; };

	bool Initialize(ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext,
		const std::vector<VERTEX> & terrainVerticesArr, 
		const UINT nodeIndexX, 
		const UINT nodeIndexY,	
		const UINT cellHeight, 
		const UINT cellWidth, 
		const UINT terrainWidth);

	void Shutdown();

	// render the terrain cell / cell line box
	void RenderCell();
	void RenderLineBuffers();

	//
	// GETTERS
	//
	UINT GetTerrainCellVertexCount() const;
	UINT GetTerrainCellIndexCount() const;
	UINT GetCellLinesIndexCount() const;
	void GetCellDimensions(float & maxWidth, 
		float & maxHeight, 
		float & maxDepth, 
		float & minWidth, 
		float & minHeight, 
		float & minDepth);


private:  // restrict a copying of this class instance
	TerrainCellClass(const TerrainCellClass & obj);
	TerrainCellClass & operator=(const TerrainCellClass & obj);

private:
	bool InitializeTerrainCell(const std::vector<VERTEX> & terrainVerticesArr,
		const UINT nodeIndexX,
		const UINT nodeIndexY,
		const UINT cellHeight,
		const UINT cellWidth,
		const UINT terrainWidth);

	bool InitializeCellLineBox();

	bool InitializeTerrainCellBuffers(const UINT nodeIndexX,
		const UINT nodeIndexY,
		const UINT cellHeight,
		const UINT cellWidth,
		const UINT terrainWidth,
		const std::vector<VERTEX> & terrainVerticesArr);

	bool InitializeCellLinesBuffers();

	void CalculateCellDimensions();

	void FillVerticesAndIndicesOfBoundingBox(std::vector<VERTEX> & verticesArr,
		std::vector<UINT> & indicesArr,
		UINT & index, 
		const DirectX::XMFLOAT3 & vertexPos);

public:
	std::vector<DirectX::XMFLOAT3> cellVerticesCoordsList_;

private:
	TerrainCellLineBoxClass* pLineBoxModel_ = nullptr;              // each terrain cell has its own line box around itself
	DirectX::XMFLOAT3 cellCenterPosition_{ 0.0f, 0.0f, 0.0f };      // the center position of this cell
	const DirectX::XMFLOAT4 lineColor_ { 1.0f, 0.5f, 0.0f, 1.0f };  // set the colour of the bounding lines to orange

	// dimensions of the node (terrain cell)
	float maxWidth_  = -1000000.0f;
	float maxHeight_ = -1000000.0f;
	float maxDepth_  = -1000000.0f;
	float minWidth_  = 1000000.0f;
	float minHeight_ = 1000000.0f;
	float minDepth_  = 1000000.0f;
};
