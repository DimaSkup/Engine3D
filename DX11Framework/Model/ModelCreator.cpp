#include "../Model/ModelCreator.h"









void ModelCreator::CreateAndInitDefaultModel(ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,
	ModelInitializerInterface* pModelInitializer)
{
	// creates a default (cube, sphere, etc.) model of particular type which will be 
	// used for creating other models of this type (for instance: we won't need to 
	// read model data from its data file each time when we create a model of this type, 
	// so we just copy data from this default model so the sake of speed);
	//
	// NOTE: this default model won't be rendered after creation;

	// check input params
	assert(pModelInitializer != nullptr);

	Model* pModel = nullptr;

	try
	{
		// get pointers to the instance of a new model and the instance of models list
		pModel = this->GetInstance(pModelInitializer);
		ModelListClass* pModelList = ModelListClass::Get();

		///////////////////////////////////////////////

		// since we initialize default models so each default model type knows by itself
		// its own path to the data file 
		// (this path generates inside the Initialize function)
		std::string filePath{ "no_path" };

		// initialize the model according to its type
		bool result = pModel->Initialize(filePath, pDevice, pDeviceContext);
		COM_ERROR_IF_FALSE(result, "can't initialize a model object");

		// add this model into the GLOBAL list of all models
		pModelList->AddModel(pModel, pModel->GetModelDataObj()->GetID());

		// set that this model is default
		pModelList->SetModelAsDefaultByID(pModel->GetModelDataObj()->GetID());
	}
	catch (std::bad_alloc & e)
	{
		std::string exceptionMsg{ "can't create and init some default model" };
		exceptionMsg += TryToGetModelID_WhenException(pModel);

		// print error messages
		Log::Error(THIS_FUNC, e.what());
		Log::Error(THIS_FUNC, exceptionMsg.c_str());

		COM_ERROR_IF_FALSE(false, exceptionMsg);
	}
	catch (COMException & e)
	{
		std::string exceptionMsg{ "can't create and init some model" };
		exceptionMsg += TryToGetModelID_WhenException(pModel);

		// print error messages
		Log::Error(e, true);
		Log::Error(THIS_FUNC, exceptionMsg.c_str());

		COM_ERROR_IF_FALSE(false, exceptionMsg);
	}

	return;
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
		ModelListClass* pModelList = ModelListClass::Get();

		///////////////////////////////////////////////

		// make a relation between the model and some shader which will be used for
		// rendering this model (by default the rendering shader is a color shader)
		pModel->SetModelToShaderMediator(pModelToShaderMediator);
		pModel->SetRenderShaderName(renderShaderName);

		// set path to model's data file for importing this model
		pModel->GetModelDataObj()->SetPathToDataFile(filePath);

		// initialize the model according to its type
		bool result = pModel->Initialize(filePath, pDevice, pDeviceContext);
		COM_ERROR_IF_FALSE(result, "can't initialize a model object");

		// we have two types of models: 
		// 1. usual models (cubes, spheres, planes, etc.)
		// 2. zone elements (terrain, sky plane, sky dome, trees, etc.)
		//
		// so that we have to put these types into separate models lists
		if (this->IsUsualModel() == true)
		{
			// add this model to the list of usual models
			pModelList->AddModel(pModel, pModel->GetModelDataObj()->GetID());

			// add this model for rendering on the scene
			pModelList->SetModelForRenderingByID(pModel->GetModelDataObj()->GetID());
		}
		else
		{
			// add a new zone element;
			// NOTE: zone elements are rendered separately so we don't have to add
			// each zone element into the rendering list
			pModelList->AddZoneElement(pModel, pModel->GetModelDataObj()->GetID());
		}

		std::string debugMsg{ pModel->GetModelDataObj()->GetID() + " is created" };
		Log::Debug(THIS_FUNC, debugMsg.c_str());
	}
	catch (std::bad_alloc & e)
	{
		std::string exceptionMsg{ "can't allocate memory for a model" };
		exceptionMsg += TryToGetModelID_WhenException(pModel);

		// print error messages
		Log::Error(THIS_FUNC, e.what());
		Log::Error(THIS_FUNC, exceptionMsg.c_str());

		COM_ERROR_IF_FALSE(false, exceptionMsg);
	}
	catch (COMException & e)
	{
		std::string exceptionMsg{ "can't create and init some model" };
		exceptionMsg += TryToGetModelID_WhenException(pModel);

		// print error messages
		Log::Error(e, true);
		Log::Error(THIS_FUNC, exceptionMsg.c_str());

		COM_ERROR_IF_FALSE(false, exceptionMsg);
	}

	return pModel;
}

///////////////////////////////////////////////////////////







///////////////////////////////////////////////////////////////////////////////////////////
//                                   PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////////////////////////////////////

std::string ModelCreator::TryToGetModelID_WhenException(Model* pModel)
{
	// try to get an ID of the model where an exception happened
	if (pModel != nullptr)
	{
		if (pModel->GetModelDataObj() != nullptr)
		{
			return pModel->GetModelDataObj()->GetID();
		}
	}

	return "";
}