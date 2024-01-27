///////////////////////////////////////////////////////////////////////////////////////////
// Filename:     ModelsToShaderMediator.h
// Description:  this class is a mediator between models (Model class)
//               and shaders (all of the shader classes)
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <string>
#include <map>
#include <list>
#include <memory>

#include "../ShaderClass/shaderclass.h"
#include "../ShaderClass/ModelToShaderMediatorInterface.h"


class ModelToShaderMediator : public ModelToShaderMediatorInterface
{
public:
	struct ShaderToModelsContainer
	{
		ShaderClass* ptrToShader_ = nullptr;
	};

public:
	ModelToShaderMediator();
	~ModelToShaderMediator();


	// render a model using HLSL shaders
	virtual void Render(ID3D11DeviceContext* pDeviceContext,
		ModelToShaderComponent* pModel) override;

	void AddShader(ShaderClass* pShader);

private:
	std::map<std::string, std::unique_ptr<ShaderToModelsContainer>> shaderToModels_;
};