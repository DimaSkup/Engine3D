/////////////////////////////////////////////////////////////////////
// Filename:    TerrainCellLineBoxClass.cpp
// Description: line box model around a terrain cell
//
// Created:     11.07.23
/////////////////////////////////////////////////////////////////////
#include "../Model/TerrainCellLineBoxClass.h"



TerrainCellLineBoxClass::TerrainCellLineBoxClass(ModelInitializerInterface* pModelInitializer)
{
	this->SetModelInitializer(pModelInitializer);
	this->AllocateMemoryForElements();
	this->modelType_ = "terrain_cell_line_box";
}


TerrainCellLineBoxClass::~TerrainCellLineBoxClass()
{
	std::string debugMsg{ "destroyment of the terrain cell" };
	Log::Debug(THIS_FUNC, debugMsg.c_str());
}