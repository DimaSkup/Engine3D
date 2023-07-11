#pragma once

#include "Model.h"
#include "ModelToShaderMediator.h"
#include "modellistclass.h"
#include "../ShaderClass/shaderclass.h"
#include "../ShaderClass/DataContainerForShadersClass.h"


class ModelCreator
{
public:
	virtual ~ModelCreator() {};
	virtual Model* GetInstance() = 0;  // get an instance of the model


	Model* CreateAndInitModel(ID3D11Device* pDevice, 
		ShaderClass* pShader,
		bool isRendered = false,
		bool isDefault = false)
	{
		assert(pDevice != nullptr);
		assert(pShader != nullptr);

		bool result = false;
		Model* pModel = this->GetInstance();
		ModelListClass* pModelList = ModelListClass::Get();
		std::string modelID{ "" };

		// initialize the model according to its type (the function GetInstance() 
		// creates an instance of Cube/Sphere/Plane/etc. and returns a pointer to it)
		result = pModel->Initialize(pDevice);
		COM_ERROR_IF_FALSE(result, "can't initialize a model object");

		// initialize a model to shader mediator object
		new ModelToShaderMediator(pModel, pShader, DataContainerForShadersClass::Get());

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
