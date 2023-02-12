#pragma once

#include "ModelCreator.h"
#include "Sphere.h"
//#include "Cube.h"


class SphereModelCreator : public ModelCreator
{
public:
	ModelClass* GetInstance() override
	{
		return new Sphere();
	}
};
