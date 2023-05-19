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
	Log::Print("               MODELS' DESTROYMENT:              ");
	Log::Print("-------------------------------------------------");

	// delete all the models
	if (!modelsGlobalList_.empty())
	{
		for (auto & elem : modelsGlobalList_)
		{
			_DELETE(elem.second); // delete data by this model's pointer
		}

		modelsGlobalList_.clear();
	}

	// clear all the data from the models rendering list
	if (!modelsRenderingList_.empty())
	{
		modelsRenderingList_.clear();
	}

	// clear all the data from the default models list
	if (!defaultModelsList_.empty())
	{
		modelsRenderingList_.clear();
	}

	Log::Debug(THIS_FUNC, "is shutted down successfully");

	return;
}


// GetAllModelsCount() returns the number of models that this class maintains information about
size_t ModelListClass::GetAllModelsCount(void)
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


const std::map<std::string, ModelClass*> & ModelListClass::GetModelsGlobalList() const
{
	return modelsGlobalList_;
}

// returns a reference to the map which contains the models data
const std::map<std::string, ModelClass*> & ModelListClass::GetModelsRenderingList()
{
	return this->modelsRenderingList_;
}


std::map<std::string, ModelClass*> & ModelListClass::GetDefaultModelsList()
{
	return this->defaultModelsList_;
}



// adds a model into the GLOBAL list by modelID;
// if we remove model from this list so we remove it from anywhere
std::string ModelListClass::AddModel(ModelClass* pModel, const std::string& modelId)
{
	assert(pModel != nullptr);
	assert(!modelId.empty());

	// try to insert a model pointer by such an id
	auto res = modelsGlobalList_.insert({ modelId, pModel });

	// if the model wasn't inserted
	if (!res.second)
	{
		// we have a duplication by such a key so generate a new one (new ID for the model)
		std::string newModelId = this->GenerateNewKeyInMap(modelsGlobalList_, modelId);

		// insert a model pointer by the new id
		pModel->SetID(newModelId);
		modelsGlobalList_.insert({ newModelId, pModel });
		

		return newModelId;
	}

	return modelId;
}


// adds a model ptr to the rendering list and asigns it with a modelID name;
// (all these models will be rendered on the scene);
// it gets a pointer to the model from the models GLOBAL list;
void ModelListClass::SetModelForRenderingByID(const std::string& modelId)
{
	assert(!modelId.empty());

	// try to find this model in the models GLOBAL list
	auto iterator = modelsGlobalList_.find(modelId); 

	// if we haven't got any such model in the list
	if (iterator == modelsGlobalList_.end())
	{
		std::string errorMsg{ "there is no such model (" + modelId + ") in the models GLOBAL list" };
		COM_ERROR_IF_FALSE(false, errorMsg);
	}
	else
	{
		// add it into the rendering list
		auto res = modelsRenderingList_.insert({ iterator->first, iterator->second });

		if (!res.second)   // if the model wasn't inserted
		{
			std::string errorMsg{ "can't insert a model (" + modelId + ") into the models rendering list" };
			COM_ERROR_IF_FALSE(false, errorMsg);
		}
	}

	return;
}


// set that a model by this ID must be the default one;
// it gets a pointer to the model from the models GLOBAL list;
void ModelListClass::SetModelAsDefaultByID(const std::string& modelId)
{
	assert(!modelId.empty());


	// try to find this model in the models GLOBAL list
	auto iterator = modelsGlobalList_.find(modelId);

	// if we haven't got any such model in the list
	if (iterator == modelsGlobalList_.end())
	{
		std::string errorMsg{ "there is no such model (" + modelId + ") in the models GLOBAL list" };
		COM_ERROR_IF_FALSE(false, errorMsg);
	}
	else
	{
		// add it into the rendering list
		auto res = defaultModelsList_.insert({ iterator->first, iterator->second });

		if (!res.second)   // if the model wasn't inserted
		{
			std::string errorMsg{ "can't insert a model (" + modelId + ") into the DEFAULT models list" };
			COM_ERROR_IF_FALSE(false, errorMsg);
		}
	}

	return;
}


// delete a model by id at all
void ModelListClass::RemoveModelByID(const std::string& modelID)
{
	assert(!modelID.empty());

	// check if we have such an id in the models list
	auto iterator = GetIteratorByID(modelsGlobalList_, modelID);

	modelsGlobalList_.erase(iterator->first);

	// if we had this model in the rendering list / default models list we also remove it from there
	modelsRenderingList_.erase(iterator->first);
	defaultModelsList_.erase(iterator->first);

	// delete the model object
	_DELETE(modelsGlobalList_[modelID]);

	return;
}


// if we have a model by such modelId we delete it from the models rendering list
// but if we can't find such a model we throw an exception about it
void ModelListClass::RemoveFromRenderingListModelByID(const std::string& modelID)
{
	assert(!modelID.empty());

	// check if we have such an id in the models list
	auto iterator = GetIteratorByID(modelsRenderingList_, modelID);

	modelsRenderingList_.erase(iterator->first);  // and remove it from the rendering list


	return;
}






/////////////////////////////////////////////////////////////////////////////////////////
//
//                         PRIVATE FUNCTIONS (HELPERS)
//
/////////////////////////////////////////////////////////////////////////////////////////
std::string ModelListClass::GenerateNewKeyInMap(const std::map<std::string, ModelClass*> & map, const std::string & key)
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



// searches a model in the map and returns an iterator to it;
// if there is no such a model it throws an exception;
std::_Tree_const_iterator<std::_Tree_val<std::_Tree_simple_types<std::pair<const std::string, ModelClass*>>>>  ModelListClass::GetIteratorByID(const std::map<std::string, ModelClass*> & map,const std::string & modelID)
{
	// check if we have such an id in the models list
	auto iterator = map.find(modelID);

	// if we found data by the key
	if (iterator != map.end())
	{
		return iterator;  // return it
	}
	else // else throw an exception
	{
		std::string errorMsg{ "there is no model with such an id: " + modelID };
		COM_ERROR_IF_FALSE(false, errorMsg.c_str());
	}
}