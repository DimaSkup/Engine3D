/////////////////////////////////////////////////////////////////////
// Filename:    TerrainCellLineBoxClass.h
// Description: line box model around a terrain cell
//
// Created:     11.07.23
/////////////////////////////////////////////////////////////////////
#pragma once


//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include "../Model/Model.h"


//////////////////////////////////
// Class name: TerrainCellLineBox
//////////////////////////////////
class TerrainCellLineBoxClass : public Model
{
public:
	TerrainCellLineBoxClass(ModelInitializerInterface* pModelInitializer);
	~TerrainCellLineBoxClass();

	virtual bool Initialize(ID3D11Device* pDevice) override { return true; }
};