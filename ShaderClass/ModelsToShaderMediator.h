#pragma once

#include <string>
#include <map>
#include <list>
#include <memory>

#include "../ShaderClass/shaderclass.h"
#include "../Model/Model.h"


class ModelsToShaderMediator
{
public:
	struct ShaderToModelsContainer
	{
		ShaderClass* ptrToShader_ = nullptr;
		std::list<Model*> modelsList_;   // list of pointers to models which will be rendered with this shader (by ptrToShader_)
	};

public:
	ModelsToShaderMediator()
	{
		Log::Debug(THIS_FUNC_EMPTY);
	}

	~ModelsToShaderMediator()
	{

	}

	// render all the models using HLSL shaders
	void Render(ID3D11DeviceContext* pDeviceContext, Model* pModel)
	{
		// search for the shader in the map
		auto iterator = shaderToModels_.find(pModel->GetRenderShaderName());

		// there is a shader with such a name
		if (iterator != shaderToModels_.end())
		{
			std::string msg{ "rendering of the line using " + iterator->first };
			Log::Debug(THIS_FUNC, msg.c_str());

			iterator->second->ptrToShader_->Render(pDeviceContext);
		}
	}

	// render all the models which are related to some particular shader
	void RenderShader(const std::string & shaderName);

	bool CheckShaderExistsInMediator(const std::string & shaderName)
	{
		// return true if we found a shader by such a name or
		// return false in another case
		return (shaderToModels_.find(shaderName) != shaderToModels_.end()) ?
			true : false;
	}

	void AddShader(ShaderClass* pShader)
	{
		// check input params
		assert(pShader != nullptr);

		try
		{
			// create a shader_to_models container
			std::unique_ptr<ShaderToModelsContainer> pContainer = std::make_unique<ShaderToModelsContainer>();

			// make a node in the shader_to_models map [shader_name => shader_to_models_container]
			std::string shaderName = pShader->GetShaderName();
			shaderToModels_.insert_or_assign(shaderName, std::move(pContainer));

			// setup the node
			shaderToModels_[shaderName]->ptrToShader_ = pShader;
		}
		catch (std::bad_alloc & e)
		{
			Log::Error(THIS_FUNC, e.what());
			Log::Error(THIS_FUNC, "can't allocate memory for the shader_to_models container");
			COM_ERROR_IF_FALSE(false, "can't allocate memory for the shader_to_models container");
		}
	}

	void AddModelForRenderingWithShader(const std::string & shaderName, Model* pModel)
	{
		assert(shaderName.empty() != true);
		assert(pModel != nullptr);

		shaderToModels_[shaderName]->modelsList_.push_back(pModel);
	}

private:
	std::map<std::string, std::unique_ptr<ShaderToModelsContainer>> shaderToModels_;
};