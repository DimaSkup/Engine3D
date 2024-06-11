/////////////////////////////////////////////////////////////////////
// Filename:    TerrainCellLineBoxClass.cpp
// Description: line box model around a terrain cell
//
// Created:     11.07.23
/////////////////////////////////////////////////////////////////////
#include "../Model/TerrainCellLineBoxClass.h"

#if 0

TerrainCellLineBoxClass::TerrainCellLineBoxClass(ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext)
	: Model(pDevice, pDeviceContext)
{
	this->modelType_ = "terrain_cell_line_box";
	this->gameObjType_ = GameObject::GAME_OBJ_ZONE_ELEMENT;

	// also we init the game object's ID with the name of the model's type;
	// NOTE: DON'T CHANGE ID after this game object was added into the game objects list;
	//
	// but if you really need it you have to change the game object's ID manually inside of the game object list
	// and here as well using the SetID() function.
	this->ID_ = this->modelType_;   // default ID
}


TerrainCellLineBoxClass::~TerrainCellLineBoxClass()
{
	std::string debugMsg{ "destroyment of the terrain cell: " + this->ID_ };
	Log::Debug(LOG_MACRO, debugMsg.c_str());
}


#endif