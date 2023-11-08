////////////////////////////////////////////////////////////////////////////////////////////
// Filename:    CustomModel.h
// Description: this class is a representation of a model which is IMPORTED
//              from some outer data file of 3D models format (.obj, .fbx, 3ds, etc.)
//
// Revising:    04.11.23
////////////////////////////////////////////////////////////////////////////////////////////
#pragma once


//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include "../Model/Model.h"


//////////////////////////////////
// Class name: CustomModel
//////////////////////////////////
class CustomModel : public Model
{
public:
	CustomModel(ModelInitializerInterface* pModelInitializer);
	~CustomModel();

	virtual bool Initialize(const std::string & filePath,
		ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext) override;

private:
	std::string modelType_{ "custom_model" };   // a type name of the current model
};