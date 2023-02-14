#pragma once

#include "ModelCreator.h"
#include "Cube.h"
#include "Sphere.h"



class SphereModelCreator : public ModelCreator
{
public:
	ModelClass* GetInstance() override
	{
		return new Sphere();
	}
};


class CubeModelCreator : public ModelCreator
{
public:
	ModelClass* GetInstance() override
	{
		return new Cube();
	}
};