/////////////////////////////////////////////////////////////////////
// Filename:    TerrainCellLineBoxClass.cpp
// Description: line box model around a terrain cell
//
// Created:     11.07.23
/////////////////////////////////////////////////////////////////////
#include "../Model/TerrainCellLineBoxClass.h"



TerrainCellLineBoxClass::TerrainCellLineBoxClass(ModelInitializerInterface* pModelInitializer,
	ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext)
	: Model(pDevice, pDeviceContext)
{
	this->SetModelInitializer(pModelInitializer);
	this->modelType_ = "terrain_cell_line_box";
}


TerrainCellLineBoxClass::~TerrainCellLineBoxClass()
{
	std::string debugMsg{ "destroyment of the terrain cell" };
	Log::Debug(THIS_FUNC, debugMsg.c_str());
}