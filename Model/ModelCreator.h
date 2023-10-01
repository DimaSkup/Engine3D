#pragma once

#include "Model.h"
#include "modellistclass.h"
#include "../ShaderClass/shaderclass.h"
#include "../Model/ModelInitializerInterface.h"
//#include "../Model/ModelInitializer.h"


class ModelCreator
{
public:
	virtual ~ModelCreator() {};

	// get an instance of the model
	virtual Model* GetInstance(ModelInitializerInterface* pModelInitializer) = 0; 

	// define if this model is a usual model (cube, sphere, plane, etc.) 
	// in another case it is a Zone element (terrain, sky dome, sky plane, etc.)
	virtual bool IsUsualModel() const = 0;


	Model* CreateAndInitModel(ID3D11Device* pDevice, 
		ModelInitializerInterface* pModelInitializer,
		bool isRendered = false,
		bool isDefault = false)
	{
		// check input params
		assert(pModelInitializer != nullptr);

		bool result = false;
		Model* pModel = this->GetInstance(pModelInitializer); // create an instance of Cube/Sphere/Plane/etc. and return a pointer to it
		ModelListClass* pModelList = ModelListClass::Get();
		std::string modelID{ "" };

		///////////////////////////////////////////////

		// initialize the model according to its type
		result = pModel->Initialize(pDevice);
		COM_ERROR_IF_FALSE(result, "can't initialize a model object");

		// we have two types of models: 
		// 1. usual models (cubes, spheres, planes, etc.)
		// 2. zone elements (terrain, sky plane, sky dome, trees, etc.)
		//
		// so that we have to put these types into separate models lists
		if (this->IsUsualModel() == true)
		{
			// add this model to the list of usual models
			pModelList->AddModel(pModel, pModel->GetModelDataObj()->GetID());
		}
		else
		{
			// add a new zone element
			pModelList->AddZoneElement(pModel, pModel->GetModelDataObj()->GetID());
		}

		// set that the model must be rendered/default
		if (isRendered)
		{
			pModelList->SetModelForRenderingByID(pModel->GetModelDataObj()->GetID()); // add this model for rendering on the scene
		}
		else if (isDefault)
		{
			pModelList->SetModelAsDefaultByID(pModel->GetModelDataObj()->GetID());    // add this model to the list of the default models
		}


		std::string debugMsg{ pModel->GetModelDataObj()->GetID() + " is created" };
		Log::Debug(THIS_FUNC, debugMsg.c_str());

		return pModel;
	}
};
