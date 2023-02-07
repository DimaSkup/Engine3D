////////////////////////////////////////////////////////////////////
// Filename:    ModelToShaderMediator.cpp
// Description: this class is a mediator between the ModelClass and
//              the shaders
//
// Created:     06.02.23
////////////////////////////////////////////////////////////////////

#include "ModelToShaderMediator.h"


ModelToShaderMediator::ModelToShaderMediator(ModelClass* pModel, ShaderClass* pShader) :
	pModel_(pModel),
	pShader_(pShader)
{
	this->pModel_->SetMediator(this);
	this->pShader_->SetMediator(this);
}


// by particular shader name we define which shader we will use for rendering of the model
void ModelToShaderMediator::Render(std::string shaderName, GraphicsComponent* pModel)
{
	bool result = false;

	if (shaderName == "ColorShaderClass")
	{
		Log::Error("ColorShaderClass");

		//pShader->Render()
	}
	else if (shaderName == "TextureShaderClass")
	{
		Log::Error("TextureShaderClass");
		/*
		result = pGraphics_->GetShaderByName("textureShaderClass")->Render(
			pGraphics_->GetD3DClass()->GetDeviceContext(),
			pModel_->GetIndexCount(),
			pModel_->GetWorldMatrix(),
			pGraphics_->GetViewMatrix(),
			pGraphics_->GetProjectionMatrix(),
			pModel_->GetTextureArray(),
			1.0f);
		*/
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
	
	exit(-1);
	return;
}