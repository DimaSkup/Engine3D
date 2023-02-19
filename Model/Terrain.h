/////////////////////////////////////////////////////////////////////
// Filename:    Terrain.h
// Description: this class is a representation of a terrain model
//
// Created:     19.02.23
/////////////////////////////////////////////////////////////////////
#pragma once

#include "Plane.h"   // the terrarin is a particular kind of a plane model

class Terrain : public Plane
{
public:
	Terrain();

	virtual bool Initialize(ID3D11Device* pDevice) override;

private:
	static bool isInit_;
};





