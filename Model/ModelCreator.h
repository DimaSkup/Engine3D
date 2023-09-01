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
	virtual Model* GetInstance(ModelInitializerInterface* pModelInitializer) = 0;  // get an instance of the model


	Model* CreateAndInitModel(ID3D11Device* pDevice, 
		ShaderClass* pShader,
		ModelInitializerInterface* pModelInitializer,
		bool isRendered = false,
		bool isDefault = false)
	{
		assert(pDevice != nullptr);
		assert(pShader != nullptr);
		assert(pModelInitializer != nullptr);

		bool result = false;
		Model* pModel = this->GetInstance(pModelInitializer); // create an instance of Cube/Sphere/Plane/etc. and return a pointer to it
		ModelListClass* pModelList = ModelListClass::Get();
		std::string modelID{ "" };

		// initialize the model according to its type
		result = pModel->Initialize(pDevice);
		COM_ERROR_IF_FALSE(result, "can't initialize a model object");

		// add this model to the list of models
		pModelList->AddModel(pModel, pModel->GetModelDataObj()->GetID());

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
