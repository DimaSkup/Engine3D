/////////////////////////////////////////////////////////////////////
// Filename:    Cube.h
// Description: this class is a representation of a cube model
// Revising:    14.02.23
/////////////////////////////////////////////////////////////////////
#pragma once


//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include "../Model/Model.h"


//////////////////////////////////
// Class name: Cube
//////////////////////////////////
class Cube : public Model
{
public:
	Cube(ModelInitializerInterface* pModelInitializer);
	~Cube();

	virtual bool Initialize(const std::string & filePath,
		ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext) override;

private:
	std::string modelType_{ "cube" };  // a type name of the current model
	
};