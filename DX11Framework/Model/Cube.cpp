////////////////////////////////////////////////////////////////////
// Filename:    Cube.cpp
// Description: an implementation of a cube model
// Revising:    14.02.23
/////////////////////////////////////////////////////////////////////
#include "Cube.h"


Cube::Cube(ModelInitializerInterface* pModelInitializer)
{
	this->SetModelInitializer(pModelInitializer);

	// allocate memory for the model's common elements
	this->AllocateMemoryForElements();

	// set that this model has a cube type
	this->modelType_ = "cube";
}

Cube::~Cube()
{
}