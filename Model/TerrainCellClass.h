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
#include <DirectXMath.h>
#include "modelclass.h"

// shaders
#include "ModelToShaderMediator.h"
#include "../ShaderClass/ShadersContainer.h"


//////////////////////////////////
// Class name: TerrainCellClass
//////////////////////////////////
class TerrainCellClass : public GraphicsComponent
{
private:
	//struct ColorVertexType
	//{
	//	DirectX::XMFLOAT3 position{ 0.0f, 0.0f, 0.0f };
	//	DirectX::XMFLOAT4 color{ 1.0f, 1.0f, 1.0f, 1.0f };
	//};

public:
	TerrainCellClass();
	TerrainCellClass(const TerrainCellClass& obj);
	~TerrainCellClass();

	bool Initialize(ID3D11Device* pDevice, VERTEX* pTerrainModel, UINT nodeIndexX, UINT nodeIndexY,	UINT cellHeight, UINT cellWidth, UINT terrainWidth);
	void Shutdown();

	// render the terrain cell/cell lines
	void RenderCell(ID3D11DeviceContext* pDeviceContext);
	void RenderLineBuffers(ID3D11DeviceContext* pDeviceContext);


	// GETTERS
	UINT GetTerrainCellVertexCount() const;
	UINT GetTerrainCellIndexCount() const;
	UINT GetCellLinesIndexCount() const;
	void GetCellDimentions(float&, float&, float&, float&, float&, float&);

private:
	bool InitializeTerrainCell(ID3D11Device* pDevice, VERTEX* pTerrainModel, UINT nodeIndexX, UINT nodeIndexY, UINT cellHeight, UINT cellWidth, UINT terrainWidth);
	bool InitializeCellLines(ID3D11Device* pDevice);

	bool InitializeTerrainCellBuffers(ID3D11Device* pDevice, UINT nodeIndexX, UINT nodeIndexY, UINT cellHeight, UINT cellWidth, UINT terrainWidth, VERTEX* pTerrainModel);
	bool InitializeCellLinesBuffers(ID3D11Device* pDevice);

	void CalculateCellDimensions();
	

	void FillVerticesAndIndicesOfBoundingBox(VERTEX* verticesArr, UINT* indicesArr, UINT & index, const DirectX::XMFLOAT3 & vertexPos);

public:
	DirectX::XMFLOAT3* pVertexList_ = nullptr;

private:
	//ModelClass* pModel_ = nullptr;
	ModelClass* pTerrainCellModel_ = nullptr;
	ModelClass* pCellLinesModel_ = nullptr;
	//ModelToShaderMediator* pTerrainToShaderMediator_ = nullptr;
	//ModelToShaderMediator* pLinesToShaderMediator_ = nullptr;
	//UINT lineIndexCount_ = 0;                                           // a number of all the lines which create bounding boxes around terrain cells
	//std::unique_ptr<VertexBuffer<ColorVertexType>> pLineVertexBuffer_;  // a vertex buffer for series of lines which create bouding boxes around terrain cells
	//std::unique_ptr<IndexBuffer> pLineIndexBuffer_;                     // an index buffer for series of lines which create bouding boxes around terrain cells
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
