////////////////////////////////////////////////////////////////////
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
////////////////////////////////////////////////////////////////////
#pragma once


//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include "../Model/Model.h"
#include "../Model/TerrainCellLineBoxClass.h"   // line box model around a terrain cell

// shaders
#include "ModelToShaderMediator.h"
#include "../ShaderClass/ShadersContainer.h"


//////////////////////////////////
// Class name: TerrainCellClass
//////////////////////////////////
class TerrainCellClass : public Model
{
public:
	TerrainCellClass();
	~TerrainCellClass();

	virtual bool Initialize(ID3D11Device *pDevice) override { return true; };
	bool Initialize(ID3D11Device* pDevice, VERTEX* pTerrainModel, UINT nodeIndexX, UINT nodeIndexY,	UINT cellHeight, UINT cellWidth, UINT terrainWidth);
	void Shutdown();

	// render the terrain cell / cell line box
	void RenderCell(ID3D11DeviceContext* pDeviceContext);
	void RenderLineBuffers(ID3D11DeviceContext* pDeviceContext);

	//
	// GETTERS
	//
	UINT GetTerrainCellVertexCount() const;
	UINT GetTerrainCellIndexCount() const;
	UINT GetCellLinesIndexCount() const;
	void GetCellDimensions(float & maxWidth, float & maxHeight, float & maxDepth, float & minWidth, float & minHeight, float & minDepth);


private:  // restrict a copying of this class instance
	TerrainCellClass(const TerrainCellClass & obj);
	TerrainCellClass & operator=(const TerrainCellClass & obj);

private:
	bool InitializeTerrainCell(ID3D11Device* pDevice, VERTEX* pTerrainModel, UINT nodeIndexX, UINT nodeIndexY, UINT cellHeight, UINT cellWidth, UINT terrainWidth);
	bool InitializeCellLineBox(ID3D11Device* pDevice);

	bool InitializeTerrainCellBuffers(ID3D11Device* pDevice, UINT nodeIndexX, UINT nodeIndexY, UINT cellHeight, UINT cellWidth, UINT terrainWidth, VERTEX* pTerrainModel);
	bool InitializeCellLinesBuffers(ID3D11Device* pDevice);

	void CalculateCellDimensions();
	void FillVerticesAndIndicesOfBoundingBox(VERTEX* verticesArr, UINT* indicesArr, UINT & index, const DirectX::XMFLOAT3 & vertexPos);

public:
	DirectX::XMFLOAT3* pVertexList_ = nullptr;

private:
	ShaderClass* pTerrainShader_ = nullptr;
	ShaderClass* pColorShader_ = nullptr;
	TerrainCellLineBoxClass* pLineBoxModel_ = nullptr; // each terrain cell has its own line box around itself
	//Model* pTerrainCellModel_ = nullptr;
	//Model* pCellLinesModel_ = nullptr;
	DirectX::XMFLOAT3 cellCenterPosition_{ 0.0f, 0.0f, 0.0f };                    // the center position of this cell
	const DirectX::XMFLOAT4 lineColor_ { 1.0f, 0.5f, 0.0f, 1.0f };      // set the colour of the bounding lines to orange

	// dimensions of the node
	float maxWidth_  = -1000000.0f;
	float maxHeight_ = -1000000.0f;
	float maxDepth_  = -1000000.0f;
	float minWidth_  = 1000000.0f;
	float minHeight_ = 1000000.0f;
	float minDepth_  = 1000000.0f;
	
};
