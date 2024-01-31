/////////////////////////////////////////////////////////////////////
// Filename:    TerrainCellLineBoxClass.cpp
// Description: line box model around a terrain cell
//
// Created:     11.07.23
/////////////////////////////////////////////////////////////////////
#include "../Model/TerrainCellLineBoxClass.h"



TerrainCellLineBoxClass::TerrainCellLineBoxClass(ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext)
	: Model(pDevice, pDeviceContext)
{
	this->modelType_ = "terrain_cell_line_box";
}


TerrainCellLineBoxClass::~TerrainCellLineBoxClass()
{
	std::string debugMsg{ "destroyment of the terrain cell" };
	Log::Debug(LOG_MACRO, debugMsg.c_str());
}