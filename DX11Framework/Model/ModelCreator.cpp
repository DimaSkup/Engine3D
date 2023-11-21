#include "../Model/ModelCreator.h"









bool ModelCreator::CreateAndInitDefaultModel(ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,
	ModelInitializerInterface* pModelInitializer,
	ModelToShaderMediatorInterface* pModelToShaderMediator,
	const std::string & renderingShaderName)
{
	// creates a default (cube, sphere, etc.) model of particular type which will be 
	// used for creating other models of this type (for instance: we won't need to 
	// read model data from its data file each time when we create a model of this type, 
	// so we just copy data from this default model so the sake of speed);
	//
	// NOTE: this default model won't be rendered after creation;

	// check input params
	assert(pModelInitializer != nullptr);
	assert(pModelToShaderMediator != nullptr);
	assert(renderingShaderName.empty() != true);

	Model* pModel = nullptr;

	try
	{
		// get pointers to the instance of a new model and the instance of models list
		pModel = this->GetInstance(pModelInitializer);

		///////////////////////////////////////////////

		// as this model type is default we have to get a path to the 
		// default models directory to get a data file
		const std::string defaultModelsDirPath{ Settings::Get()->GetSettingStrByKey("DEFAULT_MODELS_DIR_PATH") };
		const std::string defaultModelsExt{ ".obj" };
		const std::string filePath{ defaultModelsDirPath + pModel->GetModelType() + defaultModelsExt };


		// initialize the model according to its type loading data from the data file
		bool result = pModel->Initialize(filePath, pDevice, pDeviceContext);
		COM_ERROR_IF_FALSE(result, "can't initialize a default model object");

		// make a relation between the model and some shader which will be used for
		// rendering this model (by default the rendering shader is a color shader)
		pModel->SetModelToShaderMediator(pModelToShaderMediator);
		pModel->SetRenderShaderName(renderingShaderName);

		// set that this model is default
		//pModelList->SetModelAsDefaultByID(pModel->GetModelDataObj()->GetID());

	}
	catch (std::bad_alloc & e)
	{
		std::string exceptionMsg{ "can't allocate memory for some default model" };
		exceptionMsg += TryToGetModelType_WhenException(pModel);

		// print error messages
		Log::Error(THIS_FUNC, e.what());
		Log::Error(THIS_FUNC, exceptionMsg.c_str());

		COM_ERROR_IF_FALSE(false, exceptionMsg);
	}
	catch (COMException & e)
	{
		std::string exceptionMsg{ "can't create and init some default model" };
		exceptionMsg += TryToGetModelType_WhenException(pModel);

		// print error messages
		Log::Error(e, true);
		Log::Error(THIS_FUNC, exceptionMsg.c_str());

		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////

Model* ModelCreator::CreateAndInitModel(ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,
	ModelInitializerInterface* pModelInitializer,
	ModelToShaderMediatorInterface* pModelToShaderMediator,
	const std::string & filePath,                               // path to model's data file which is used for importing this model   
	const std::string & renderShaderName)  // name of a shader which will be used for rendering a model
{
	// check input params
	assert(pModelInitializer != nullptr);
	assert(pModelToShaderMediator != nullptr);
	assert(renderShaderName.empty() != true);
	assert(filePath.empty() != true);

	Model* pModel = nullptr;

	try
	{
		pModel = this->GetInstance(pModelInitializer);

		///////////////////////////////////////////////

		// make a relation between the model and some shader which will be used for
		// rendering this model (by default the rendering shader is a color shader)
		pModel->SetModelToShaderMediator(pModelToShaderMediator);
		pModel->SetRenderShaderName(renderShaderName);

		// set path to model's data file for importing this model
		//pModel->GetModelDataObj()->SetPathToDataFile(filePath);

		// initialize the model according to its type
		bool result = pModel->Initialize(filePath, pDevice, pDeviceContext);
		COM_ERROR_IF_FALSE(result, "can't initialize a model object");


		std::string debugMsg{ " model of type: '" + pModel->GetModelType() + "' is created" };
		Log::Debug(THIS_FUNC, debugMsg.c_str());
	}
	catch (std::bad_alloc & e)
	{
		std::string exceptionMsg{ "can't allocate memory for a model" };
		exceptionMsg += TryToGetModelType_WhenException(pModel);

		// print error messages
		Log::Error(THIS_FUNC, e.what());
		Log::Error(THIS_FUNC, exceptionMsg.c_str());

		COM_ERROR_IF_FALSE(false, exceptionMsg);
	}
	catch (COMException & e)
	{
		std::string exceptionMsg{ "can't create and init some model" };
		exceptionMsg += TryToGetModelType_WhenException(pModel);

		// print error messages
		Log::Error(e, true);
		Log::Error(THIS_FUNC, exceptionMsg.c_str());

		COM_ERROR_IF_FALSE(false, exceptionMsg);
	}

	return pModel;

} // CreateAndInitModel

///////////////////////////////////////////////////////////

Model* ModelCreator::CreateCopyOfModel(Model* pOriginModel)
{
	// check input params
	COM_ERROR_IF_FALSE(pOriginModel, "input model is empty");

	Model* pModel = nullptr;    // a ptr to a new model

	try
	{
		// create a model object
		pModel = this->GetInstance(pOriginModel->GetModelInitializer());

		///////////////////////////////////////////////

		// copy data from the origin model into the new one
		// (copying of the vertex/index buffers, and other data as well)
		*pModel = *pOriginModel;

	}
	catch (std::bad_alloc & e)
	{
		std::string exceptionMsg{ "can't allocate memory for a model" };
		exceptionMsg += TryToGetModelType_WhenException(pModel);

		// print error messages
		Log::Error(THIS_FUNC, e.what());
		Log::Error(THIS_FUNC, exceptionMsg.c_str());

		COM_ERROR_IF_FALSE(false, exceptionMsg);
	}
	catch (COMException & e)
	{
		std::string exceptionMsg{ "can't create and init some model" };
		exceptionMsg += TryToGetModelType_WhenException(pModel);

		// print error messages
		Log::Error(e, true);
		Log::Error(THIS_FUNC, exceptionMsg.c_str());

		COM_ERROR_IF_FALSE(false, exceptionMsg);
	}

	// return a pointer to the new model (copy of the origin)
	return pModel;

} // end CreateCopyOfModel





///////////////////////////////////////////////////////////////////////////////////////////
//                                   PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////////////////////////////////////

std::string ModelCreator::TryToGetModelType_WhenException(Model* pModel)
{
	// try to get an ID of the model where an exception happened
	if (pModel != nullptr)
	{
		return pModel->GetModelType();
	}

	return "can't get a model type";
}