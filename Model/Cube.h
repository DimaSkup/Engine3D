/////////////////////////////////////////////////////////////////////
// Filename:    Cube.h
// Description: this class is a representation of a cube model
// Revising:    14.02.23
/////////////////////////////////////////////////////////////////////
#pragma once


//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include "Model.h"


//////////////////////////////////
// Class name: Cube
//////////////////////////////////
class Cube : public Model
{
public:
	Cube();
	~Cube();

	virtual bool Initialize(ID3D11Device* pDevice) override;

private:
	bool InitializeDefault(ID3D11Device* pDevice);
	bool InitializeNew(ID3D11Device* pDevice);

private:
	Model* pModel_ = nullptr;          // for using all the model's functional
	std::string modelType_{ "cube" };  // a type name of the current model
	static Cube* pDefaultCube_;        // a static pointer to the default cube
};