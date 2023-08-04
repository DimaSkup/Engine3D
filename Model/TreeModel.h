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

	virtual bool Initialize(ID3D11Device* pDevice) override;

private:
	void InitializeDefault(ID3D11Device* pDevice);  // initialize a basic default tree so we can use its data for faster initialization of the other tree models (other TreeModel instances)
	void InitializeNew(ID3D11Device* pDevice);      // initialize a new instance of the TreeModel class (using data of the basic default tree model)

private:
	// a static pointer to the DEFAULT tree model: we use it for not loading tree data from
	// the data file each time when we create a new tree model so we just use data of 
	// the DEFAULT tree
	static TreeModel* pDefaultTree_;

	std::string modelType_{ "tree" };         // a type name of the current model
};