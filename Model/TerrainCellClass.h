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


//////////////////////////////////
// Class name: TerrainCellClass
//////////////////////////////////
class TerrainCellClass
{
public:
	TerrainCellClass();
	TerrainCellClass(const TerrainCellClass& obj);
	~TerrainCellClass();

	bool Initialize(ID3D11Device* pDevice, VERTEX* pTerrainModel, UINT nodeIndexX, UINT nodeIndexY,	UINT cellHeight, UINT cellWidth, UINT terrainWidth);
	void Shutdown();
	void Render(ID3D11DeviceContext* pDeviceContext);
	void RenderLineBuffers(ID3D11DeviceContext* pDeviceContext);

	UINT GetVertexCount() const;
	UINT GetIndexCount() const;
	UINT GetLineBuffersIndexCount() const;
	void GetCellDimentions(float&, float&, float&, float&, float&, float&);

private:
	bool InitializeBuffers(ID3D11Device* pDevice, UINT nodeIndexX, UINT nodeIndexY, UINT cellHeight, UINT cellWidth, UINT terrainWidth, VERTEX* pTerrainModel);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext* pDeviceContext);
	void CalculateCellDimensions();
	bool BuildLineBuffers(ID3D11Device* pDevice);
	void ShutdownLineBuffers();

public:
	DirectX::XMFLOAT3* pVertexList_ = nullptr;

private:
	ModelClass* pModel_ = nullptr;
	UINT lineIndexCount_ = 0;

	float maxWidth_ = 0.0f;
	float minWidth_ = 0.0f;
	float maxHeight_ = 0.0f;
	float minHeight_ = 0.0f;
	float maxDepth_ = 0.0f;
	float minDepth_ = 0.0f;
	DirectX::XMFLOAT3 position{ 0.0f, 0.0f, 0.0f };
};
