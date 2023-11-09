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

	virtual bool Initialize(const std::string & filePath,
		ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext) override;

private:
	const std::string modelType_{ "tree" };         // a type name of the current model
};