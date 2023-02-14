/////////////////////////////////////////////////////////////////////
// Filename:    Cube.h
// Description: this class is a representation of a cube model
// Revising:    14.02.23
/////////////////////////////////////////////////////////////////////
#pragma once


//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include "modelclass.h"
#include "modellistclass.h"


//////////////////////////////////
// Class name: Cube
//////////////////////////////////
class Cube : public ModelClass
{
public:
	Cube();

	virtual bool Initialize(ID3D11Device* pDevice) override;

private:
	bool InitializeDefault(ID3D11Device* pDevice);
	bool InitializeNewBasicCube(ID3D11Device* pDevice);

private:
	std::string modelType_{ "cube" };
	static bool isDefaultInit_;
	static size_t cubesCounter_;
};