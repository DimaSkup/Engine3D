////////////////////////////////////////////////////////////////////
// Filename:    ModelDefault.h
// Description: common functions for initialization of the default models
//              and making copies from these default models
//
// Created:     28.02.23
/////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include "modelclass.h"
#include "modellistclass.h"
#include "../Engine/log.h"


//////////////////////////////////
// Class name: ModelDefault
//////////////////////////////////
class ModelDefault : public ModelClass
{
public:
	// initialize a copy model which is based on the default model instance
	bool InitializeCopy(ModelClass* pModelCopy, ID3D11Device* pDevice, const std::string & modelType);


};
