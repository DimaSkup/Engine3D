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
};