////////////////////////////////////////////////////////////////////////////////////////////
// Filename:    Plane.h
// Description: this class is a representation of a plane model
//
// Created:     19.02.23
////////////////////////////////////////////////////////////////////////////////////////////
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
	Plane();
	Plane(ModelInitializerInterface* pModelInitializer);
	~Plane();

	virtual bool Initialize(const std::string & filePath,
		ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext);

protected:
	std::string modelType_{ "plane" };  // a type name of the current model
};
