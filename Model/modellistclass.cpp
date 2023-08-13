////////////////////////////////////////////////////////////////////
// Filename: modellistclass.cpp
// Revising: 17.09.22
////////////////////////////////////////////////////////////////////
#include "modellistclass.h"


ModelListClass* ModelListClass::pInstance_ = nullptr;

// a constructor
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

// a destructor
ModelListClass::~ModelListClass(void) 
{
	Log::Print("-------------------------------------------------");
	Log::Print("               MODELS' DESTROYMENT:              ");
	Log::Print("-------------------------------------------------");
	Log::Debug(THIS_FUNC_EMPTY);

	this->Shutdown();
	pInstance_ = nullptr;
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
	float modelsStride = 20.0f;
	

	// seed the random generator with the current time
	srand(static_cast<unsigned int>(time(NULL)));

	for (auto & elem : modelsRenderingList_)
	{
		if (elem.first == "terrain" || elem.first == "sky_dome" || elem.first == "sky_plane")
			continue;

		// generate a random colour for the model
		red = static_cast<float>(rand()) / RAND_MAX;
		green = static_cast<float>(rand()) / RAND_MAX;
		blue = static_cast<float>(rand()) / RAND_MAX;
		alpha = 1.0f;

		// generate a random position in from of the viewer for the model
		posX = (static_cast<float>(rand()) / RAND_MAX) * posMultiplier + modelsStride;
		posY = (static_cast<float>(rand()) / RAND_MAX) * posMultiplier + 5.0f;
		posZ = (static_cast<float>(rand()) / RAND_MAX) * posMultiplier + modelsStride;


		elem.second->GetModelDataObj()->SetColor(red, green, blue, 1.0f);
		elem.second->GetModelDataObj()->SetPosition(posX, posY, posZ);
	}


	return true;
} // Initialize()


// Shutdown() function releases the model information list arrays
void ModelListClass::Shutdown(void)
{
	Log::Debug(THIS_FUNC_EMPTY);

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


// GetRenderedModelsCount() returns the number of models that this class maintains information about
size_t ModelListClass::GetRenderedModelsCount(void) const
{
	return modelsRenderingList_.size();
}


// returns a pointer to the model by its id
Model* ModelListClass::GetModelByID(const std::string& modelId)
{
	assert(!modelId.empty());

	auto it = GetIteratorByID(modelsGlobalList_, modelId);

	// if we found a model by this id
	if (it != modelsGlobalList_.end())
	{
		return it->second;  // return a pointer to the model
	}

	return nullptr;
}


// returns a pointer to the DEFAULT model by its id
Model* ModelListClass::GetDefaultModelByID(const std::string& modelId) const
{
	assert(!modelId.empty());

	return defaultModelsList_.at(modelId);
}


// get data (generated position, color) of the model
void ModelListClass::GetDataByID(const std::string& modelId, DirectX::XMFLOAT3& position, DirectX::XMFLOAT4& color)
{
	assert(!modelId.empty());

	// check if we have such an id in the models list
	auto iterator = GetIteratorByID(modelsGlobalList_, modelId);

	// if we found data by the key
	if (iterator != modelsGlobalList_.end())
	{
		position = iterator->second->GetModelDataObj()->GetPosition();
		color = iterator->second->GetModelDataObj()->GetColor();
	}

	return;
} // GetDataById()


const std::map<std::string, Model*> & ModelListClass::GetModelsGlobalList() const
{
	return modelsGlobalList_;
}

// returns a reference to the map which contains the models data
const std::map<std::string, Model*> & ModelListClass::GetModelsRenderingList()
{
	return this->modelsRenderingList_;
}


// returns a reference to the map which contains 2D sprite models for rendering
const std::map<std::string, Model*> & ModelListClass::GetSpritesRenderingList()
{
	return this->spritesRenderingList_;
}


std::map<std::string, Model*> & ModelListClass::GetDefaultModelsList()
{
	return this->defaultModelsList_;
}



// adds a model into the GLOBAL list by modelID;
// if we remove model from this list so we remove it from anywhere
std::string ModelListClass::AddModel(Model* pModel, const std::string & modelId)
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
		pModel->GetModelDataObj()->SetID(newModelId);
		modelsGlobalList_.insert({ newModelId, pModel });
		

		return newModelId;
	}

	return modelId;
}



// add a new 2D sprite (plane) into the sprites list for rendering onto the screen;
//
// if there is already ID in the list which is the same as the input one we 
// generate new ID for proper inserting into the sprites list and return this new sprite's ID
std::string ModelListClass::AddSprite(Model* pModel, const std::string& spriteID)
{
	assert(pModel != nullptr);
	assert(!spriteID.empty());

	// try to insert a sprite pointer by such an id
	auto res = spritesRenderingList_.insert({ spriteID, pModel });

	// if the model wasn't inserted
	if (!res.second)
	{
		// we have a duplication by such a key so generate a new one (new ID for the sprite)
		std::string newSpriteID = this->GenerateNewKeyInMap(spritesRenderingList_, spriteID);

		// insert a model pointer by the new id
		pModel->GetModelDataObj()->SetID(newSpriteID);    // rewrite sprite's ID with the generated new one
		spritesRenderingList_.insert({ newSpriteID, pModel });

		return newSpriteID;
	}

	return spriteID;
}


// adds a model ptr to the rendering list and asigns it with a modelID name;
// (all these models will be rendered on the scene);
// it gets a pointer to the model from the models GLOBAL list;
void ModelListClass::SetModelForRenderingByID(const std::string& modelId)
{
	assert(!modelId.empty());

	// try to find this model in the models GLOBAL list
	auto iterator = GetIteratorByID(modelsGlobalList_, modelId);

	// if we got a correct iterator
	if (iterator != modelsGlobalList_.end())
	{
		// add it into the rendering list
		auto res = modelsRenderingList_.insert({ iterator->first, iterator->second });

		if (!res.second)   // if the model wasn't inserted
		{
			std::string errorMsg{ "can't insert a model (" + modelId + ") into the models RENDERING list" };
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
	auto iterator = GetIteratorByID(modelsGlobalList_, modelId);

	// if we got a correct iterator
	if (iterator != modelsGlobalList_.end())
	{
		// add it into the default models list
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

	// if we got a correct iterator
	if (iterator != modelsGlobalList_.end())
	{
		modelsGlobalList_.erase(iterator->first);

		// if we had this model in the rendering list / default models list we also remove it from there
		modelsRenderingList_.erase(iterator->first);
		defaultModelsList_.erase(iterator->first);

		// delete the model object
		_DELETE(modelsGlobalList_[modelID]);
	}
	

	return;
}


// if we have a model by such modelId we delete it from the models rendering list
// but if we can't find such a model we throw an exception about it
void ModelListClass::RemoveFromRenderingListModelByID(const std::string& modelID)
{
	assert(!modelID.empty());

	// check if we have such an id in the models list
	auto iterator = GetIteratorByID(modelsRenderingList_, modelID);

	// if we got a correct iterator
	if (iterator != modelsRenderingList_.end())
	{
		modelsRenderingList_.erase(iterator->first);  // and remove it from the rendering list
	}


	return;
}






/////////////////////////////////////////////////////////////////////////////////////////
//
//                         PRIVATE FUNCTIONS (HELPERS)
//
/////////////////////////////////////////////////////////////////////////////////////////
std::string ModelListClass::GenerateNewKeyInMap(const std::map<std::string, Model*> & map, const std::string & key)
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
std::_Tree_const_iterator<std::_Tree_val<std::_Tree_simple_types<std::pair<const std::string, Model*>>>>  ModelListClass::GetIteratorByID(const std::map<std::string, Model*> & map,const std::string & modelID) const
{

	// check if we have such an id in the models list
	auto iterator = map.find(modelID);

	// if we didn't find any data by the key
	if (iterator == map.end())
	{
		std::string errorMsg{ "there is no model with such an id: " + modelID };
		Log::Error(new COMException(S_FALSE, errorMsg, __FILE__, __FUNCTION__, __LINE__), true);
	}


	return iterator;  // return the iterator in any case
}