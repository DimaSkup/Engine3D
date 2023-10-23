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

	// creates a default (cube, sphere, etc.) model of particular type which will be 
	// used for creating other models of this type (for instance: we won't need to 
	// read model data from its data file each time when we create a model of this type, 
	// so we just copy data from this default model so the sake of speed);
	//
	// NOTE: this default model won't be rendered after creation;
	void CreateAndInitDefaultModel(ID3D11Device* pDevice,
		ModelInitializerInterface* pModelInitializer)
	{
		// check input params
		assert(pModelInitializer != nullptr);

		try
		{
			bool result = false;
			std::string modelID{ "" };

			// get pointers to the instance of a new model and the instance of models list
			Model* pModel = this->GetInstance(pModelInitializer);
			ModelListClass* pModelList = ModelListClass::Get();

			///////////////////////////////////////////////

			// initialize the model according to its type
			result = pModel->Initialize(pDevice);
			COM_ERROR_IF_FALSE(result, "can't initialize a model object");

			// add this model into the GLOBAL list of all models
			pModelList->AddModel(pModel, pModel->GetModelDataObj()->GetID());

			// set that this model is default
			pModelList->SetModelAsDefaultByID(pModel->GetModelDataObj()->GetID());   
		}
		catch (std::bad_alloc & e)
		{
			Log::Error(THIS_FUNC, e.what());
			Log::Error(THIS_FUNC, "can't create and init some default model");
		}
		
	}

	Model* CreateAndInitModel(ID3D11Device* pDevice, 
		ModelInitializerInterface* pModelInitializer)
	{
		// check input params
		assert(pModelInitializer != nullptr);

		bool result = false;
		std::string modelID{ "" };

		Model* pModel = this->GetInstance(pModelInitializer);
		ModelListClass* pModelList = ModelListClass::Get();
		
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

			// add this model for rendering on the scene
			pModelList->SetModelForRenderingByID(pModel->GetModelDataObj()->GetID());
		}
		else
		{
			// add a new zone element;
			// NOTE: zone elements are rendered separately so we don't have to add
			// each zone element into the rendering list
			pModelList->AddZoneElement(pModel, pModel->GetModelDataObj()->GetID());
		}

	

		std::string debugMsg{ pModel->GetModelDataObj()->GetID() + " is created" };
		Log::Debug(THIS_FUNC, debugMsg.c_str());

		return pModel;
	}
};
