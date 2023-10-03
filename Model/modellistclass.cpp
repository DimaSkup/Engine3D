////////////////////////////////////////////////////////////////////
// Filename: modellistclass.cpp
// Revising: 17.09.22
////////////////////////////////////////////////////////////////////
#include "modellistclass.h"


// initialize static elements
ModelListClass* ModelListClass::pInstance_ = nullptr;



ModelListClass::ModelListClass() 
{
	if (pInstance_ != nullptr)   // we can have only one instance of this class
	{
		COM_ERROR_IF_FALSE(false, "there is already exists one instance of the models list class");
	}
	else
	{
		Log::Debug(THIS_FUNC_EMPTY);
		pInstance_ = this;
	}
}

ModelListClass::~ModelListClass(void) 
{
	Log::Print("-------------------------------------------------");
	Log::Print("               MODELS' DESTROYMENT:              ");
	Log::Print("-------------------------------------------------");
	Log::Debug(THIS_FUNC_EMPTY);

	this->Shutdown();
	pInstance_ = nullptr;
}






////////////////////////////////////////////////////////////////////////////////////////////
//
//                               PUBLIC FUNCTIONS
//
////////////////////////////////////////////////////////////////////////////////////////////



bool ModelListClass::GenerateDataForModels()
{
	// this function generates random color/position values for the models on the scene

	float red = 1.0f, green = 1.0f, blue = 1.0f, alpha = 1.0f;
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
} // end GenerateDataForModels

/////////////////////////////////////////////////

void ModelListClass::Shutdown(void)
{
	// this function releases the model information list arrays

	Log::Debug(THIS_FUNC, "start of execution");

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
	if (!modelsRenderingList_.empty())  modelsRenderingList_.clear();

	// clear all the data from the zone elements list
	if (!zoneModelsList_.empty()) zoneModelsList_.clear();

	// clear all the data from the default models list
	if (!defaultModelsList_.empty()) modelsRenderingList_.clear();

	// clear all the data from the sprites rendering list
	if (!spritesRenderingList_.empty()) spritesRenderingList_.clear();

	Log::Debug(THIS_FUNC, "is shutted down successfully");

	return;

} // end Shutdown





////////////////////////////////////////////////////////////////////////////////////////////
//                                 PUBLIC GETTERS
////////////////////////////////////////////////////////////////////////////////////////////

size_t ModelListClass::GetRenderedModelsCount() const
{
	// this function returns the number of usual models (custom models, cubes, etc.)
	// that should be rendered onto the scene
	return modelsRenderingList_.size();
}

/////////////////////////////////////////////////

Model* ModelListClass::GetModelByID(const std::string & modelId)
{
	// this function returns a pointer to the usual model by its id

	assert(!modelId.empty());

	auto it = GetIteratorByID(modelsGlobalList_, modelId);

	// if we found a model by this ID we return a pointer to it or nullptr in another case;
	return (it != modelsGlobalList_.end()) ? it->second : nullptr;

} // end GetModelByID

/////////////////////////////////////////////////

Model* ModelListClass::GetZoneModelByID(const std::string & modelID)
{
	// this function returns a pointer to the zone's model by its id

	assert(!modelID.empty());

	auto it = GetIteratorByID(zoneModelsList_, modelID);

	// if we found a model by this ID we return a pointer to it or nullptr in another case;
	return (it != zoneModelsList_.end()) ? it->second : nullptr;

} // end GetZoneModelByID

/////////////////////////////////////////////////

Model* ModelListClass::GetDefaultModelByID(const std::string& modelId) const
{
	// this function returns a pointer to the DEFAULT model by its id

	assert(!modelId.empty());

	return defaultModelsList_.at(modelId);
}

/////////////////////////////////////////////////

void ModelListClass::GetDataByID(const std::string & modelId,
	DirectX::XMFLOAT3 & position, 
	DirectX::XMFLOAT4 & color)
{
	// get data (generated position, color) of the model

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

} // end GetDataById()

/////////////////////////////////////////////////

const std::map<std::string, Model*> & ModelListClass::GetModelsGlobalList() const
{
	// return a reference to the GLOBAL models list;
	// this list contains pointers to all the models on the scene;
	return modelsGlobalList_;
}

/////////////////////////////////////////////////

const std::map<std::string, Model*> & ModelListClass::GetModelsRenderingList()
{
	// this function returns a reference to the map which contains
	// the models for rendering onto the scene
	return this->modelsRenderingList_;
}

/////////////////////////////////////////////////

const std::map<std::string, Model*> & ModelListClass::GetSpritesRenderingList()
{
	// this function returns a reference to the map which contains 
	// 2D sprite models for rendering
	return this->spritesRenderingList_;
}

/////////////////////////////////////////////////

std::map<std::string, Model*> & ModelListClass::GetDefaultModelsList()
{
	// this function returns a reference to the map which contains 
	// pointers to DEFAULT models
	return this->defaultModelsList_;
}






////////////////////////////////////////////////////////////////////////////////////////////
//                              PUBLIC SETTERS/ADDERS
////////////////////////////////////////////////////////////////////////////////////////////

std::string ModelListClass::AddModel(Model* pModel, const std::string & modelId)
{
	// this function adds a model into the GLOBAL list by modelID;
	// if we remove model from this list so we remove it from anywhere

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

} // end AddModel

/////////////////////////////////////////////////

std::string ModelListClass::AddZoneElement(Model* pModel, const std::string & modelID)
{
	// this function adds [model_name, model_ptr] pairs of zone elements 
	// (models, for instance: terrain, sky dome, clouds, etc.) into the list

	assert(pModel != nullptr);
	assert(!modelID.empty());

	// first of all we add a model into the models global list
	this->AddModel(pModel, modelID);

	// try to insert a model pointer by such an id into the zone elements list
	auto res = zoneModelsList_.insert({ modelID, pModel });

	// if the model wasn't inserted
	if (!res.second)
	{
		// we have a duplication by such a key so generate a new one (new ID for the model)
		std::string newModelId = this->GenerateNewKeyInMap(zoneModelsList_, modelID);

		// insert a model pointer by the new id
		pModel->GetModelDataObj()->SetID(newModelId);
		zoneModelsList_.insert({ newModelId, pModel });


		return newModelId;
	}

	return modelID;

} // end AddZoneElement

/////////////////////////////////////////////////

std::string ModelListClass::AddSprite(Model* pModel, const std::string& spriteID)
{

	// this function adds a new 2D sprite (plane) into the sprites list 
	// for rendering onto the screen;
	//
	// if there is already ID in the list which is the same as the input one we 
	// generate new ID for proper inserting into the sprites list and return this new sprite's ID

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
} // add AddSprite

/////////////////////////////////////////////////

void ModelListClass::SetModelForRenderingByID(const std::string& modelId)
{
	// adds a model ptr to the rendering list and asigns it with a modelID name;
	// (all these models will be rendered on the scene);
	// it gets a pointer to the model from the models GLOBAL list;

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

} // end SetModelForRenderingByID

/////////////////////////////////////////////////

void ModelListClass::SetModelAsDefaultByID(const std::string& modelId)
{
	// set that a model by this ID must be the default one;
	// it gets a pointer to the model from the models GLOBAL list;

	assert(!modelId.empty());

	if (modelId == "tree")
	{
		int kek = 0;
		kek++;
	}

	try
	{
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
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(THIS_FUNC, "can't set a model as default");
	}

	return;

} // end SetModelAsDefaultByID






////////////////////////////////////////////////////////////////////////////////////////////
//                            PUBLIC REMOVERS/DELETERS
////////////////////////////////////////////////////////////////////////////////////////////


void ModelListClass::RemoveModelByID(const std::string& modelID)
{
	// this function removes a model by its id at all (deletes from the memory)

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
} // end RemoveModelByID

/////////////////////////////////////////////////

void ModelListClass::RemoveFromRenderingListModelByID(const std::string& modelID)
{
	// if we have a model by such modelId we delete it from the models rendering list
	// but if we can't find such a model we throw an exception about it

	assert(!modelID.empty());

	// check if we have such an id in the models list
	auto iterator = GetIteratorByID(modelsRenderingList_, modelID);

	// if we got a correct iterator
	if (iterator != modelsRenderingList_.end())
	{
		modelsRenderingList_.erase(iterator->first);  // and remove it from the rendering list
	}

	return;

} // end RemoveFromRenderingListModelByID







////////////////////////////////////////////////////////////////////////////////////////////
//
//                         PRIVATE FUNCTIONS (HELPERS)
//
////////////////////////////////////////////////////////////////////////////////////////////

std::string ModelListClass::GenerateNewKeyInMap(const std::map<std::string, Model*> & map, const std::string & key)
{
	// this function generates a new key (ID) for a model in the map;
	//
	// INPUT: takes as input a map and an old key (which is already exists in this map
	// so that we have to generate a new one)
	//
	// RETURN: a new generated key 

	assert(!key.empty());

	// an index for concatenation it with the origin key ( for example: old_key(1) )
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

} // end GenerateNewKeyInMap

/////////////////////////////////////////////////

// GetIteratorByID(map, modelID)
std::_Tree_const_iterator<std::_Tree_val<std::_Tree_simple_types<std::pair<const std::string, Model*>>>>  ModelListClass::GetIteratorByID(const std::map<std::string, Model*> & map,const std::string & modelID) const
{
	// this function searches a model in the map and returns an iterator to it;

	// try to find a model by its ID in the map
	auto iterator = map.find(modelID);

	// if we didn't find any data by the key (ID)
	if (iterator == map.end())
	{
		std::string errorMsg{ "there is no model with such an id: " + modelID };
		Log::Error(THIS_FUNC, errorMsg.c_str());
	}

	// return the iterator in any case
	return iterator;  

} // end GetIteratorByID