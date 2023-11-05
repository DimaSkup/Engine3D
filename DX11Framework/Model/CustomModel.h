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

	virtual bool Initialize(ID3D11Device* pDevice) override;

private:
	// a static pointer to the DEFAULT custom model: we use it for not loading model data 
	// from the data file each time when we create a new model (copy) so we just use data of 
	// the DEFAULT model of this model type
	//static CustomModel* pBasicCustomModel_;

	std::string modelType_{};  // a type name of the current model

};