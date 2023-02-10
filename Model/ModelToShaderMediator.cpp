////////////////////////////////////////////////////////////////////
// Filename:    ModelToShaderMediator.cpp
// Description: this class is a mediator between the ModelClass and
//              the shaders
//
// Created:     06.02.23
////////////////////////////////////////////////////////////////////

#include "ModelToShaderMediator.h"


ModelToShaderMediator::ModelToShaderMediator(ModelClass* pModel, 
	ShaderClass* pShader,
	DataContainerForShadersClass* pDataForShader)
	:
	pModel_(pModel),
	pShader_(pShader)
{
	this->pModel_->SetMediator(this);
	this->pShader_->SetMediator(this);
	this->pDataForShader_ = pDataForShader;
}


// by particular shader name we define which shader we will use for rendering of the model
void ModelToShaderMediator::Render(ID3D11DeviceContext* pDeviceContext,
	std::string shaderName,
	GraphicsComponent* pModel)
{
	bool result = false;

	if (shaderName == "ColorShaderClass")
	{
		result = pShader_->Render(pDeviceContext,
			pModel_->GetIndexCount(),
			pModel_->GetWorldMatrix(),
			pDataForShader_->GetViewMatrix(),
			pDataForShader_->GetProjectionMatrix(),
			pModel_->GetTextureArray());
		
		COM_ERROR_IF_FALSE(result, "can't render a model using the colour shader");
	}
	else if (shaderName == "TextureShaderClass")
	{
		Log::Error("TextureShaderClass");

		result = pShader_->Render(pDeviceContext,
			pModel_->GetIndexCount(),
			pModel_->GetWorldMatrix(),
			pDataForShader_->GetViewMatrix(),
			pDataForShader_->GetProjectionMatrix(),
			pModel_->GetTextureArray());

		COM_ERROR_IF_FALSE(result, "can't render a model using the texture shader");
	}
	else if (shaderName == "LightShaderClass")
	{
		Log::Error("LightShaderClass");
	}
	else if (shaderName == "CombinedShaderClass")
	{
		Log::Error("CombinedShaderClass");
	}
	else
		COM_ERROR_IF_FALSE(false, "there is not such a shader");

	return;
}