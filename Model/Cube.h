/////////////////////////////////////////////////////////////////////
// Filename:    Cube.h
// Description: this class is a representation of a cube model
// Revising:    14.02.23
/////////////////////////////////////////////////////////////////////
#pragma once


//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include "ModelDefault.h"


//////////////////////////////////
// Class name: Cube
//////////////////////////////////
class Cube : public ModelDefault
{
public:
	Cube();

	virtual bool Initialize(ID3D11Device* pDevice) override;

private:
	bool InitializeDefault(ID3D11Device* pDevice);
	bool InitializeNew(ID3D11Device* pDevice);

private:
	std::string modelType_{ "cube" };  // a type name of the current model
	static bool isDefaultInit_;        // defines whether we initialized the default cube or not
};