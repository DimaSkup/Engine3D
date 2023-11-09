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

	// for initialization line boxes of terrain cells we don't use this function;
	virtual bool Initialize(const std::string & filePath,
		ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext) override { return false; }
};