#pragma once

#include "ModelClass.h"
#include "ModelToShaderMediator.h"
#include "modellistclass.h"
#include "../ShaderClass/shaderclass.h"
#include "../ShaderClass/DataContainerForShadersClass.h"


class ModelCreator
{
public:
	virtual ~ModelCreator() {};
	virtual ModelClass* GetInstance() = 0;  // get an instance of the model class


	ModelClass* CreateAndInitModel(ID3D11Device* pDevice, 
		ShaderClass* pShader,
		bool isRendered = false,
		bool isDefault = false)
	{
		assert(pDevice != nullptr);
		assert(pShader != nullptr);

		bool result = false;
		ModelClass* pModel = this->GetInstance();
		ModelListClass* pModelList = ModelListClass::Get();

		// initialize the model
		result = pModel->Initialize(pDevice);
		COM_ERROR_IF_FALSE(result, "can't initialize a model object");

		// initialize a model to shader mediator object
		new ModelToShaderMediator(pModel, pShader, DataContainerForShadersClass::Get());


		if (pModel->GetID() == "sky_plane")
		{
			int i = 0;

		}

		// add this model to the list of models
		pModelList->AddModel(pModel, pModel->GetID());

		// set that the model must be rendered/default
		if (isRendered)
		{
			pModelList->SetModelForRenderingByID(pModel->GetID()); // add this model for rendering on the scene
		}
		else if (isDefault)
		{
			pModelList->SetModelAsDefaultByID(pModel->GetID());    // add this model to the list of the default models
		}


		std::string debugMsg{ pModel->GetID() + " is created" };
		Log::Debug(THIS_FUNC, debugMsg.c_str());

		return pModel;
	}
};
