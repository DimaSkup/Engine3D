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


	ModelClass* CreateAndInitModel(ID3D11Device* pDevice, ShaderClass* pShader)
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

		// add this model to the list of models which will be rendered on the scene
		pModelList->AddModelForRendering(pModel, pModel->GetID());

		Log::Debug(THIS_FUNC, pModel->GetID().c_str());

		return pModel;
	}
};
