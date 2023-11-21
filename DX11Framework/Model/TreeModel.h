////////////////////////////////////////////////////////////////////
// Filename:     TreeModel.h
// Description:  a tree model
// 
// Created:      24.07.23
////////////////////////////////////////////////////////////////////
#pragma once


//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include "../Model/Model.h"


//////////////////////////////////
// Class name: TreeModel
//////////////////////////////////
class TreeModel : public Model
{
public:
	TreeModel(ModelInitializerInterface* pModelInitializer);
	~TreeModel();   
};