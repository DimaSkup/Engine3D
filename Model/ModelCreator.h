#pragma once

#include "ModelClass.h"



class ModelCreator
{
public:
	virtual ~ModelCreator() {};



	virtual ModelClass* GetInstance() = 0;

	ModelClass* CreateAndInitModel(ID3D11Device* pDevice)
	{
		bool result = false;
		ModelClass* pModel = this->GetInstance();

		// initialize the model
		result = pModel->Initialize(pDevice);

		return pModel;
	}
};
