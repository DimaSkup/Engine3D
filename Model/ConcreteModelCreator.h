#pragma once

#include "ModelCreator.h"
#include "Cube.h"
#include "Sphere.h"
#include "Plane.h"
#include "Terrain.h"



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


class PlaneModelCreator : public ModelCreator
{
public:
	ModelClass* GetInstance() override
	{
		return new Plane();
	}
};

class TerrainModelCreator : public ModelCreator
{
public:
	ModelClass* GetInstance() override
	{
		return new Terrain();
	}
};