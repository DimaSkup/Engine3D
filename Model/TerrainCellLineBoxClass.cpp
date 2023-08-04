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

	// allocate memory for the model's common elements
	this->AllocateMemoryForElements();
}


TerrainCellLineBoxClass::~TerrainCellLineBoxClass()
{
	std::string debugMsg{ "destroyment of the " + this->GetModelDataObj()->GetID() };
	Log::Debug(THIS_FUNC, debugMsg.c_str());
}