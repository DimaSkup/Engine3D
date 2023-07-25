#pragma once

#include "ModelCreator.h"
#include "Cube.h"
#include "Sphere.h"
#include "Plane.h"
#include "TreeModel.h"

#include "TerrainClass.h"
#include "SkyDomeClass.h"
#include "SkyPlaneClass.h"



class SphereModelCreator : public ModelCreator
{
public:
	Model* GetInstance() override
	{
		return new Sphere();
	}
};


class CubeModelCreator : public ModelCreator
{
public:
	Model* GetInstance() override
	{
		return new Cube();
	}
};


class PlaneModelCreator : public ModelCreator
{
public:
	Model* GetInstance() override
	{
		return new Plane();
	}
};

class TreeModelCreator : public ModelCreator
{
public:
	Model* GetInstance() override
	{
		return new TreeModel();
	}
};

class TerrainModelCreator : public ModelCreator
{
public:
	Model* GetInstance() override
	{
		return new TerrainClass();
	}
};

class SkyDomeModelCreator : public ModelCreator
{
public:
	Model* GetInstance() override
	{
		return new SkyDomeClass();
	}
};

class SkyPlaneCreator : public ModelCreator
{
public:
	Model* GetInstance() override
	{
		return new SkyPlaneClass();
	}
};