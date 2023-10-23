#pragma once

#include "ModelCreator.h"

#include "Line3D.h"
#include "Triangle.h"
#include "Cube.h"
#include "Sphere.h"
#include "Plane.h"
#include "TreeModel.h"

#include "TerrainClass.h"
#include "SkyDomeClass.h"
#include "SkyPlaneClass.h"


class Line3DModelCreator : public ModelCreator
{
	// get an instance of the model (particular creator returns a pointer to respective model obj);
	virtual Model* GetInstance(ModelInitializerInterface* pModelInitializer) override
	{
		return new Line3D(pModelInitializer);
	}

	// define if this model is a usual model (line, cube, sphere, plane, etc.) 
	// in another case it is a Zone element (terrain, sky dome, sky plane, etc.)
	virtual bool IsUsualModel() const override
	{
		return true;
	}
};

class TriangleModelCreator : public ModelCreator
{
	// get an instance of the model (particular creator returns a pointer to respective model obj);
	virtual Model* GetInstance(ModelInitializerInterface* pModelInitializer) override
	{
		return new Triangle(pModelInitializer);
	}

	// define if this model is a usual model (cube, sphere, plane, etc.) 
	// in another case it is a Zone element (terrain, sky dome, sky plane, etc.)
	virtual bool IsUsualModel() const override
	{
		return true;
	}
};

class SphereModelCreator : public ModelCreator
{
public:
	virtual Model* GetInstance(ModelInitializerInterface* pModelInitializer) override
	{
		return new Sphere(pModelInitializer);
	}

	virtual bool IsUsualModel() const override
	{
		return true; 
	}
};


class CubeModelCreator : public ModelCreator
{
public:
	Model* GetInstance(ModelInitializerInterface* pModelInitializer) override
	{
		return new Cube(pModelInitializer);
	}

	virtual bool IsUsualModel() const override
	{
		return true;
	}
};


class PlaneModelCreator : public ModelCreator
{
public:
	Model* GetInstance(ModelInitializerInterface* pModelInitializer) override
	{
		return new Plane(pModelInitializer);
	}

	virtual bool IsUsualModel() const override
	{
		return true;
	}
};

class TreeModelCreator : public ModelCreator
{
public:
	Model* GetInstance(ModelInitializerInterface* pModelInitializer) override
	{
		return new TreeModel(pModelInitializer);
	}

	virtual bool IsUsualModel() const override
	{
		// this model is a Zone element
		return false;
	}
};

class TerrainModelCreator : public ModelCreator
{
public:
	Model* GetInstance(ModelInitializerInterface* pModelInitializer) override
	{
		return new TerrainClass(pModelInitializer);
	}

	virtual bool IsUsualModel() const override
	{
		// this model is a Zone element
		return false;
	}
};

class SkyDomeModelCreator : public ModelCreator
{
public:
	Model* GetInstance(ModelInitializerInterface* pModelInitializer) override
	{
		return new SkyDomeClass(pModelInitializer);
	}

	virtual bool IsUsualModel() const override
	{
		// this model is a Zone element
		return false;
	}
};

class SkyPlaneCreator : public ModelCreator
{
public:
	Model* GetInstance(ModelInitializerInterface* pModelInitializer) override
	{
		return new SkyPlaneClass(pModelInitializer);
	}

	virtual bool IsUsualModel() const override
	{
		// this model is a Zone element
		return false;
	}
};