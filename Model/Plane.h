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
#include "ModelDefault.h"

//////////////////////////////////
// Class name: Plane
//////////////////////////////////
class Plane : public ModelDefault
{
public:
	Plane();

	virtual bool Initialize(ID3D11Device* pDevice) override;

protected:
	bool InitializeDefault(ID3D11Device* pDevice);
	bool InitializeNew(ID3D11Device* pDevice, const std::string & modelId);
	bool IsDefaultPlaneInit() const;
	std::string GetID();                // generate an id for the model

private:
	std::string modelType_{ "plane" };  // a type name of the current model
	static bool isDefaultInit_;         // defines whether we initialized the default plane or not
	static size_t planesCounter_;       // this variable is necessary because for each copy of the default plane we must have a unique ID
};
