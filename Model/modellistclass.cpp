////////////////////////////////////////////////////////////////////
// Filename: modellistclass.cpp
// Revising: 17.09.22
////////////////////////////////////////////////////////////////////
#include "modellistclass.h"


ModelListClass* ModelListClass::pInstance_ = nullptr;


ModelListClass::ModelListClass() 
{
	if (pInstance_ != nullptr)   // we can have only one instance of this class
	{
		COM_ERROR_IF_FALSE(false, "there is already exists one instance of the models list class");
	}
	else
	{
		pInstance_ = this;
	}
}

ModelListClass::ModelListClass(const ModelListClass& copy) 
{
}

ModelListClass::~ModelListClass(void) 
{
	Log::Debug(THIS_FUNC_EMPTY);

	this->Shutdown();
	//pInstance_ = nullptr;
}






/////////////////////////////////////////////////////////////////////////////////////////
//
//                               PUBLIC FUNCTIONS
//
/////////////////////////////////////////////////////////////////////////////////////////


// generates random color/position values for the models on the scene
bool ModelListClass::GenerateDataForModels()
{
	float red = 0.0f, green = 0.0f, blue = 0.0f, alpha = 0.0f;
	float posX = 0.0f, posY = 0.0f, posZ = 0.0f;
	float posMultiplier = 50.0f;
	

	// seed the random generator with the current time
	srand(static_cast<unsigned int>(time(NULL)));

	for (auto & elem : modelsRenderingList_)
	{
		if (elem.first == "terrain" || elem.first == "sky_dome")
			continue;

		// generate a random colour for the model
		red = static_cast<float>(rand()) / RAND_MAX;
		green = static_cast<float>(rand()) / RAND_MAX;
		blue = static_cast<float>(rand()) / RAND_MAX;
		alpha = 1.0f;

		// generate a random position in from of the viewer for the mode
		posX = (static_cast<float>(rand()) / RAND_MAX) * posMultiplier;
		posY = (static_cast<float>(rand()) / RAND_MAX) * posMultiplier;
		posZ = (static_cast<float>(rand()) / RAND_MAX) * posMultiplier + 5.0f;


		elem.second->SetColor(red, green, blue, 1.0f);
		elem.second->SetPosition(posX, posY, posZ);
	}


	return true;
} // Initialize()


// Shutdown() function releases the model information list arrays
void ModelListClass::Shutdown(void)
{
	Log::Debug(THIS_FUNC_EMPTY);

	Log::Print("-------------------------------------------------");
	Log::Print("             MODELS FOR DESTROYMENT:             ");
	Log::Print("-------------------------------------------------");

	{
		for (auto & elem : modelsRenderingList_)
		{
			Log::Debug(THIS_FUNC, elem.second->GetID().c_str());
		}
	}

	// clear all the data in the models rendering list
	if (!modelsRenderingList_.empty())
	{
		for (auto & elem : modelsRenderingList_)
		{
			if (elem.second->GetID() == "terrain")
			{
				bool kek = false;
			}
			_DELETE(elem.second); // delete data by this model's pointer
		}

		Log::Print("ALL THE MODELS ARE DELETED");
		modelsRenderingList_.clear();
	}

	Log::Debug(THIS_FUNC, "the models rendering list is shutted down successfully");

	// remove all the default models
	if (!defaultModelsList_.empty())
	{
		for (auto & elem : defaultModelsList_)
		{
			_DELETE(elem.second); // delete data by this default model's pointer
		}

		modelsRenderingList_.clear();
	}

	Log::Debug(THIS_FUNC, "the default models list is shutted down successfully");

	Log::Debug(THIS_FUNC, "is shutted down successfully");

	return;
}


// GetModelCount() returns the number of models that this class maintains information about
size_t ModelListClass::GetModelCount(void)
{
	return modelsRenderingList_.size();
}


// returns a pointer to the model by its id
ModelClass* ModelListClass::GetModelByID(const std::string& modelId) const
{
	assert(!modelId.empty());

	return modelsRenderingList_.at(modelId);
}


// returns a pointer to the DEFAULT model by its id
ModelClass* ModelListClass::GetDefaultModelByID(const std::string& modelId) const
{
	assert(!modelId.empty());

	return defaultModelsList_.at(modelId);
}


// get data (generated position, color) of the model
void ModelListClass::GetDataByID(const std::string& modelId, DirectX::XMFLOAT3& position, DirectX::XMFLOAT4& color)
{
	assert(!modelId.empty());

	// check if we have such an id in the models list
	auto iterator = modelsRenderingList_.find(modelId);

	// if we found data by the key
	if (iterator != modelsRenderingList_.end())   
	{
		position = iterator->second->GetPosition();
		color = iterator->second->GetColor();
	}
	// we didn't found any data
	else   
	{
		std::string errorMsg{ "there is no model with such id: " + modelId };
		COM_ERROR_IF_FALSE(false, errorMsg.c_str());
	}

	return;
} // GetDataById()


// returns a reference to the map which contains the models data
const std::map<std::string, ModelClass*> & ModelListClass::GetModelsRenderingList()
{
	return this->modelsRenderingList_;
}


std::map<std::string, ModelClass*> & ModelListClass::GetDefaultModelsList()
{
	return this->defaultModelsList_;
}



// adds a new model ptr to the list and asigns it with a modelID name;
// (all these models will be rendered on the scene);
//
// if we already have such an id of a model this function generates a new one for this copy
// and returns this new id
std::string ModelListClass::AddModelForRendering(ModelClass* pModel, const std::string& modelId)
{
	assert(pModel != nullptr);
	assert(!modelId.empty());

	// try to insert a model pointer by such an id
	auto res = modelsRenderingList_.insert({ modelId, pModel });

	// check if the model was inserted 
	if (!res.second)
	{
		// we have a duplication by such a key so generate a new one (new ID for the model)
		std::string newModelId = this->GenerateNewKeyInMap(modelsRenderingList_, modelId);

		// insert a model pointer by the new id
		modelsRenderingList_.insert({ newModelId, pModel });
		pModel->SetID(newModelId);

		return newModelId;
	}

	return modelId;
}


// set that a model by this pointer must be the default one
void ModelListClass::AddDefaultModel(ModelClass* pModel, const std::string& modelId)
{
	assert(pModel != nullptr);
	assert(!modelId.empty());


	// try to insert a model pointer by such an id
	auto res = defaultModelsList_.insert({ modelId, pModel });

	// check if the model was inserted 
	if (!res.second)
	{
		std::string errorMsg{ "the list already has a model by such an id: " + modelId };
		COM_ERROR_IF_FALSE(false, errorMsg.c_str());
	}

	return;
}


// NOTIFICATION: we don't remove the model data;
// if we have a model by such modelId we set that we don't want to render it on the scene;
// but if we can't find such a model we throw an exception about it;
void ModelListClass::DontRenderModelById(const std::string& modelId)
{
	assert(!modelId.empty());

	// check if we have such an id in the models list
	auto iterator = modelsRenderingList_.find(modelId);

	if (iterator != modelsRenderingList_.end())   // if we found data by the key
	{
		std::string debugMsg{ "remove from rendering:  " + (*iterator).first };
		Log::Print(THIS_FUNC, debugMsg.c_str());
		modelsRenderingList_.erase(modelId);
	}
	else
	{
		std::string errorMsg{ "there is no model with such id: " + modelId };
		COM_ERROR_IF_FALSE(false, errorMsg.c_str());
	}

	return;
}


// if we have a model by such modelId we delete it from the models list
// but if we can't find such a model we throw an exception about it
void ModelListClass::RemoveFromRenderingListModelById(const std::string& modelId)
{
	assert(!modelId.empty());

	// check if we have such an id in the models list
	auto iterator = modelsRenderingList_.find(modelId);

	if (iterator != modelsRenderingList_.end())   // if we found data by the key
	{
		_DELETE(modelsRenderingList_[modelId]);
		modelsRenderingList_.erase(modelId);
	}
	else
	{
		std::string errorMsg{ "there is no model with such id: " + modelId };
		COM_ERROR_IF_FALSE(false, errorMsg.c_str());
	}

	return;
}






/////////////////////////////////////////////////////////////////////////////////////////
//
//                               PRIVATE FUNCTIONS
//
/////////////////////////////////////////////////////////////////////////////////////////
std::string ModelListClass::GenerateNewKeyInMap(std::map<std::string, ModelClass*> map, const std::string & key)
{
	assert(!key.empty());

	size_t copyIndex = 1;

	// try to make a new key which is based on the original one
	std::string newKey{ key + '(' + std::to_string(copyIndex) + ')'};

	// while we have the same key in the map we will generate a new
	while (map.find(newKey) != map.end())
	{
		++copyIndex;
		newKey = { key + '(' + std::to_string(copyIndex) + ')' }; // generate a new key
	}

	return newKey;
}