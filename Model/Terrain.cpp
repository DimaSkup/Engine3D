/////////////////////////////////////////////////////////////////////
// Filename:    Terrain.cpp
// Description: this class is a representation of a terrain model
//
// Created:     19.02.23
/////////////////////////////////////////////////////////////////////
#pragma once

#include "Terrain.h"   

bool Terrain::isInit_ = false;  

Terrain::Terrain()
{
	if (Terrain::isInit_)
		COM_ERROR_IF_FALSE(false, "you can't have more than only one terrain instance");
}

bool Terrain::Initialize(ID3D11Device* pDevice)
{
	if (!IsDefaultPlaneInit()) // if the default plane isn't initialized yet we need to init it so later we will be able to create a terrain
	{
		Plane::Initialize(pDevice);
	}

	this->InitializeNew(pDevice, "terrain");

	return true;
}

