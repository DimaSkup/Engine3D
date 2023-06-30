#pragma once

#include "ModelCreator.h"
#include "Cube.h"
#include "Sphere.h"
#include "Plane.h"
//#include "Terrain.h"
#include "TerrainClass.h"
#include "SkyDomeClass.h"
#include "SkyPlaneClass.h"


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
		return new TerrainClass();
	}
};

class SkyDomeModelCreator : public ModelCreator
{
public:
	ModelClass* GetInstance() override
	{
		return new SkyDomeClass();
	}
};

class SkyPlaneCreator : public ModelCreator
{
public:
	ModelClass* GetInstance() override
	{
		return new SkyPlaneClass();
	}
};