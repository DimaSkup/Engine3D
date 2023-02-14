#pragma once

#include "ModelClass.h"
#include "ModelToShaderMediator.h"
#include "../ShaderClass/shaderclass.h"
#include "../ShaderClass/DataContainerForShadersClass.h"


class ModelCreator
{
public:
	virtual ~ModelCreator() {};



	virtual ModelClass* GetInstance() = 0;

	ModelClass* CreateAndInitModel(ID3D11Device* pDevice

	ModelClass* CreateAndInitModel(ID3D11Device* pDevice, ShaderClass* pShader)
	{
		bool result = false;
		ModelClass* pModel = this->GetInstance();

		// initialize the model
		result = pModel->Initialize(pDevice);

		// initialize a model to shader mediator object
		new ModelToShaderMediator(pModel, pShader, DataContainerForShadersClass::Get());

		return pModel;
	}
};
