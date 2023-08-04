/////////////////////////////////////////////////////////////////////
// Filename:    Plane.h
// Description: this class is a representation of a plane model
//
// Created:     19.02.23
/////////////////////////////////////////////////////////////////////
#pragma once


//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include "../Model/Model.h"

//////////////////////////////////
// Class name: Plane
//////////////////////////////////
class Plane : public Model
{
public:
	Plane(ModelInitializerInterface* pModelInitializer);
	~Plane();

	virtual bool Initialize(ID3D11Device* pDevice) override;

protected:
	bool InitializeDefault(ID3D11Device* pDevice);
	bool InitializeNew(ID3D11Device* pDevice, const std::string & modelId);

private:
	// a static pointer to the DEFAULT plane: we use it for not loading plane data from
	// the data file each time when we create a new plane model so we just use data of 
	// the DEFAULT plane
	static Plane* pDefaultPlane_;

	//Model* pModel_ = nullptr;          // for using all the model's functional
	std::string modelType_{ "plane" };  // a type name of the current model
};
