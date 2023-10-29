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
		//std::list<Model*> modelsList_;   // list of pointers to models which will be rendered with this shader (by ptrToShader_)
	};

public:
	ModelToShaderMediator();
	~ModelToShaderMediator();


	// render all the models using HLSL shaders
	virtual void Render(ID3D11DeviceContext* pDeviceContext,
		ModelToShaderComponent* pModel) override;

	void AddShader(ShaderClass* pShader);

	//void AddModelForRenderingWithShader(const std::string & shaderName,
	//	ModelToShaderComponent* pModel);
	

private:
	std::map<std::string, std::unique_ptr<ShaderToModelsContainer>> shaderToModels_;
};