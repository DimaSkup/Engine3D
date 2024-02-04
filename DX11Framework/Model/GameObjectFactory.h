#pragma once

// models stuff
#include "Model.h"
#include "GameObjectsListClass.h"
#include "../Model/ModelInitializerInterface.h"

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

// shaders stuff
#include "../ShaderClass/shaderclass.h"
#include "../ShaderClass/ModelToShaderMediatorInterface.h"



class GameObjectFactory
{
public:
	virtual ~GameObjectFactory() {};

	virtual Model* GetInstanceOfModel() const = 0;

	virtual GameObject* Create() = 0;                                 // create a new game object
	virtual GameObject* CreateCopyOf(GameObject* pOriginGameObj) = 0; // create a copy of the origin game obj

protected:
	GameObjectsListClass*           pGameObjectsList_ = nullptr;
	ID3D11Device*                   pDevice_ = nullptr;
	ID3D11DeviceContext*            pDeviceContext_ = nullptr;
};


////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////










///////////////////////////////////////////////////////////////////////////////////////////
//                     A HELPER CLASS FOR THE GAME OBJECT CREATOR CLASS
///////////////////////////////////////////////////////////////////////////////////////////


class GameObjectCreatorHelper
{
public:
	std::string GetPathToDataFile(Model* pModel)
	{
		// generate a path to a model's data file by its type

		COM_ERROR_IF_NULLPTR(pModel, "the input ptr to model == nullptr");

		
	}
};


