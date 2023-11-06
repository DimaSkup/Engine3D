#include "ModelsToShaderMediator.h"



ModelToShaderMediator::ModelToShaderMediator()
{
	Log::Debug(THIS_FUNC_EMPTY);

	try
	{
		// create a data container which will be used by different shaders to get necessary data for rendering
		pDataContainerForShaders_ = new DataContainerForShaders();
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(THIS_FUNC, e.what());
		Log::Error(THIS_FUNC, "can't allocate memory for ModelToShaderMediator members");
		COM_ERROR_IF_FALSE(false, "can't allocate memory for ModelToShaderMediator members");
	}

}

ModelToShaderMediator::~ModelToShaderMediator()
{
	_DELETE(pDataContainerForShaders_);
}

///////////////////////////////////////////////////////////

void ModelToShaderMediator::Render(ID3D11DeviceContext* pDeviceContext,
	ModelToShaderComponent* pModel)
{
	// this function renders the model using HLSL shaders

	// search for the shader in the map
	auto iterator = shaderToModels_.find(pModel->GetRenderShaderName());

	// there is a shader with such a name
	if (iterator != shaderToModels_.end())
	{
		//std::string msg{ "rendering of the line using " + iterator->first };
		//Log::Debug(THIS_FUNC, msg.c_str());

		// call the rendering function of the shader class
		iterator->second->ptrToShader_->Render(pDeviceContext, pDataContainerForShaders_);
	}
}



  ///////////////////////////////////////////////////////////

void ModelToShaderMediator::AddShader(ShaderClass* pShader)
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

///////////////////////////////////////////////////////////
#if 0


void ModelToShaderMediator::AddModelForRenderingWithShader(const std::string & shaderName,
	ModelToShaderComponent* pModel)
{
	assert(shaderName.empty() != true);
	assert(pModel != nullptr);

	//shaderToModels_[shaderName]->modelsList_.push_back(pModel);
}

#endif