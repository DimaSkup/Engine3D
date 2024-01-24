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

	try
	{
		// try to get a ptr to the shader class by such a name and
		// call this shader class for rendering the current model
		shaderToModels_.at(pModel->GetRenderShaderName())      // get pair ['shader_name', 'ptr_to_shader']
			->ptrToShader_
			->Render(pDeviceContext, pDataContainerForShaders_);
	}
	catch (std::out_of_range & e)
	{
		Log::Error(THIS_FUNC, e.what());
		COM_ERROR_IF_FALSE(false, "there is no shader class with such a name. CHECK if you add such a shader class into the shaders container during the initialization of all the shader classes");
	}
	catch (COMException & e)
	{
		Log::Error(e, false);

		std::string errorMsg{ "for some reason the model can't be rendered using its shader:  " };
		errorMsg += pModel->GetRenderShaderName();
		errorMsg += "\nset the rendering shader to the ColorShaderClass for this model";

		// try to change the rendering shader for this model to the color_shader_class
		// (maybe it can help us to render the model in any case)
		pModel->SetRenderShaderName("ColorShaderClass");

		COM_ERROR_IF_FALSE(false, errorMsg);
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