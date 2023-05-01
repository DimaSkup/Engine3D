////////////////////////////////////////////////////////////////////
// Filename:     TerrainCellClass.cpp
// Description:  implementation of the TerrainCellClass functional
//
// Created:      02.05.23
////////////////////////////////////////////////////////////////////
#include "TerrainCellClass.h"

TerrainCellClass::TerrainCellClass()
{
}

TerrainCellClass::TerrainCellClass(const TerrainCellClass& obj)
{

}

TerrainCellClass::~TerrainCellClass()
{

}



////////////////////////////////////////////////////////////////////
//
//                       PUBLIC FUNCTIONS
//
////////////////////////////////////////////////////////////////////
bool TerrainCellClass::Initialize(ID3D11Device* pDevice, void*, int, int, int, int, int)
{
	return true;
}

void TerrainCellClass::Shutdown()
{
	return;
}

void TerrainCellClass::Render(ID3D11DeviceContext*)
{
	return;
}

void TerrainCellClass::RenderLineBuffers(ID3D11DeviceContext*)
{
	return;
}

UINT GetVertexCount() const;
UINT GetIndexCount() const;
UINT GetLineBuffersIndexCount() const;
void GetCellDimentions(float&, float&, float&, float&, float&, float&);




////////////////////////////////////////////////////////////////////
//
//                      PRIVATE FUNCTIONS
//
////////////////////////////////////////////////////////////////////