////////////////////////////////////////////////////////////////////
// Filename:    Sphere.cpp
// Description: an implementation of a sphere model
// Revising:    01.02.23
/////////////////////////////////////////////////////////////////////
#include "Sphere.h"


Sphere::Sphere(ModelInitializerInterface* pModelInitializer)
{
	try
	{
		this->SetModelInitializer(pModelInitializer);

		// allocate memory for the model's common elements
		this->AllocateMemoryForElements();

		// set that this model has a sphere type
		this->modelType_ = "sphere";
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(THIS_FUNC, "can't create a sphere model");
		COM_ERROR_IF_FALSE(false, "can't create a sphere model");
	}
}

Sphere::~Sphere()
{
}