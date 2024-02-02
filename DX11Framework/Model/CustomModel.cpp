////////////////////////////////////////////////////////////////////////////////////////////
// Filename:    Cube.cpp
// Description: an implementation of a cube model
// Revising:    14.02.23
////////////////////////////////////////////////////////////////////////////////////////////
#include "CustomModel.h"


// a default constructor
CustomModel::CustomModel(ID3D11Device* pDevice,	ID3D11DeviceContext* pDeviceContext)
	: Model(pDevice, pDeviceContext)
{
	this->modelType_ = "custom_model";
	this->gameObjType_ = GameObject::GAME_OBJ_RENDERABLE;

	// also we init the game object's ID with the name of the model's type;
	// NOTE: DON'T CHANGE ID after this game object was added into the game objects list;
	//
	// but if you really need it you have to change the game object's ID manually inside of the game object list
	// and here as well using the SetID() function.
	this->ID_ = this->modelType_;   // default ID
}

CustomModel::~CustomModel()
{
}