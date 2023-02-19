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
#include "modelclass.h"
#include "modellistclass.h"

//////////////////////////////////
// Class name: Plane
//////////////////////////////////
class Plane : public ModelClass
{
public:
	Plane();

	virtual bool Initialize(ID3D11Device* pDevice) override;

protected:
	bool InitializeDefault(ID3D11Device* pDevice);
	bool InitializeNew(ID3D11Device* pDevice, const std::string & modelId);

	bool IsDefaultPlaneInit() const;

private:
	std::string modelType_{ "plane" };
	static bool isDefaultInit_;
	static size_t planesCounter_;
};
