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
	Model* GetInstance(ModelInitializerInterface* pModelInitializer) override
	{
		return new Sphere(pModelInitializer);
	}
};


class CubeModelCreator : public ModelCreator
{
public:
	Model* GetInstance(ModelInitializerInterface* pModelInitializer) override
	{
		return new Cube(pModelInitializer);
	}
};


class PlaneModelCreator : public ModelCreator
{
public:
	Model* GetInstance(ModelInitializerInterface* pModelInitializer) override
	{
		return new Plane(pModelInitializer);
	}
};

class TreeModelCreator : public ModelCreator
{
public:
	Model* GetInstance(ModelInitializerInterface* pModelInitializer) override
	{
		return new TreeModel(pModelInitializer);
	}
};

class TerrainModelCreator : public ModelCreator
{
public:
	Model* GetInstance(ModelInitializerInterface* pModelInitializer) override
	{
		return new TerrainClass(pModelInitializer);
	}
};

class SkyDomeModelCreator : public ModelCreator
{
public:
	Model* GetInstance(ModelInitializerInterface* pModelInitializer) override
	{
		return new SkyDomeClass(pModelInitializer);
	}
};

class SkyPlaneCreator : public ModelCreator
{
public:
	Model* GetInstance(ModelInitializerInterface* pModelInitializer) override
	{
		return new SkyPlaneClass(pModelInitializer);
	}
};