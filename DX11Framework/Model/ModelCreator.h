#pragma once

#include "GameObjectCreator.h"

// basic models
#include "CustomModel.h"
#include "Line3D.h"
#include "Triangle.h"
#include "Cube.h"
#include "Sphere.h"
#include "Plane.h"
#include "TreeModel.h"

// 2d sprite
#include "../2D/SpriteClass.h"

// terrain elements
#include "TerrainClass.h"
#include "SkyDomeClass.h"
#include "SkyPlaneClass.h"


class CustomModelCreator : public GameObjectCreator
{
public:
	CustomModelCreator(GameObjectsListClass* pGameObjList)
		: GameObjectCreator(pGameObjList) {}

	// get an instance of the model (particular creator returns a pointer to respective model obj);
	virtual Model* GetInstance(ModelInitializerInterface* pModelInitializer) override
	{
		return new CustomModel(pModelInitializer);
	}

	// define if this model is a basic model (custom model, line, cube, sphere, plane, etc.) 
	// in another case it is a Zone element (terrain, sky dome, sky plane, etc.)
	virtual bool IsBasicModel() const override
	{
		return true;
	}
};

class Line3DModelCreator : public GameObjectCreator
{
public:
	Line3DModelCreator(GameObjectsListClass* pGameObjList)
		: GameObjectCreator(pGameObjList) {}

	// get an instance of the model (particular creator returns a pointer to respective model obj);
	virtual Model* GetInstance(ModelInitializerInterface* pModelInitializer) override
	{
		return new Line3D(pModelInitializer);
	}

	// define if this model is a basic model (line, cube, sphere, plane, etc.) 
	// in another case it is a Zone element (terrain, sky dome, sky plane, etc.)
	virtual bool IsBasicModel() const override
	{
		return true;
	}
};

///////////////////////////////////////////////////////////

class TriangleModelCreator : public GameObjectCreator
{
public:
	TriangleModelCreator(GameObjectsListClass* pGameObjList)
		: GameObjectCreator(pGameObjList) {}

	// get an instance of the model (particular creator returns a pointer to respective model obj);
	virtual Model* GetInstance(ModelInitializerInterface* pModelInitializer) override
	{
		return new Triangle(pModelInitializer);
	}

	// define if this model is a basic model (cube, sphere, plane, etc.) 
	// in another case it is a Zone element (terrain, sky dome, sky plane, etc.)
	virtual bool IsBasicModel() const override
	{
		return true;
	}
};

///////////////////////////////////////////////////////////

class SphereModelCreator : public GameObjectCreator
{
public:
	SphereModelCreator(GameObjectsListClass* pGameObjList)
		: GameObjectCreator(pGameObjList) {}

	virtual Model* GetInstance(ModelInitializerInterface* pModelInitializer) override
	{
		return new Sphere(pModelInitializer);
	}

	virtual bool IsBasicModel() const override
	{
		return true; 
	}
};

///////////////////////////////////////////////////////////

class CubeModelCreator : public GameObjectCreator
{
public:
	CubeModelCreator(GameObjectsListClass* pGameObjList)
		: GameObjectCreator(pGameObjList) {}

	Model* GetInstance(ModelInitializerInterface* pModelInitializer) override
	{
		return new Cube(pModelInitializer);
	}

	virtual bool IsBasicModel() const override
	{
		return true;
	}
};

///////////////////////////////////////////////////////////

class PlaneModelCreator : public GameObjectCreator
{
public:
	PlaneModelCreator(GameObjectsListClass* pGameObjList)
		: GameObjectCreator(pGameObjList) {}

	Model* GetInstance(ModelInitializerInterface* pModelInitializer) override
	{
		return new Plane(pModelInitializer);
	}

	virtual bool IsBasicModel() const override
	{
		return true;
	}
};

///////////////////////////////////////////////////////////

class TreeModelCreator : public GameObjectCreator
{
public:
	TreeModelCreator(GameObjectsListClass* pGameObjList)
		: GameObjectCreator(pGameObjList) {}

	Model* GetInstance(ModelInitializerInterface* pModelInitializer) override
	{
		return new TreeModel(pModelInitializer);
	}

	virtual bool IsBasicModel() const override
	{
		// this model is a Zone element
		return false;
	}
};

///////////////////////////////////////////////////////////

class TerrainModelCreator : public GameObjectCreator
{
public:
	TerrainModelCreator(GameObjectsListClass* pGameObjList)
		: GameObjectCreator(pGameObjList) {}

	Model* GetInstance(ModelInitializerInterface* pModelInitializer) override
	{
		return new TerrainClass(pModelInitializer);
	}

	virtual bool IsBasicModel() const override
	{
		// this model is a Zone element
		return false;
	}
};

///////////////////////////////////////////////////////////

class SkyDomeModelCreator : public GameObjectCreator
{
public:
	SkyDomeModelCreator(GameObjectsListClass* pGameObjList)
		: GameObjectCreator(pGameObjList) {}

	Model* GetInstance(ModelInitializerInterface* pModelInitializer) override
	{
		return new SkyDomeClass(pModelInitializer);
	}

	virtual bool IsBasicModel() const override
	{
		// this model is a Zone element
		return false;
	}
};

///////////////////////////////////////////////////////////

class SkyPlaneCreator : public GameObjectCreator
{
public:
	SkyPlaneCreator(GameObjectsListClass* pGameObjList)
		: GameObjectCreator(pGameObjList) {}

	Model* GetInstance(ModelInitializerInterface* pModelInitializer) override
	{
		return new SkyPlaneClass(pModelInitializer);
	}

	virtual bool IsBasicModel() const override
	{
		// this model is a Zone element
		return false;
	}
};

///////////////////////////////////////////////////////////

class Sprite2DCreator : public GameObjectCreator
{
public:
	Sprite2DCreator(GameObjectsListClass* pGameObjList)
		: GameObjectCreator(pGameObjList) {}

	Model* GetInstance(ModelInitializerInterface* pModelInitializer) override
	{
		return new SpriteClass(pModelInitializer);
	}

	virtual bool IsBasicModel() const override
	{
		return true;
	}
};