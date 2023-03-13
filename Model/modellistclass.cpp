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

// we don't use the copy constructor and destructor in this class
ModelListClass::ModelListClass(const ModelListClass& copy) {}
ModelListClass::~ModelListClass(void) {}






/////////////////////////////////////////////////////////////////////////////////////////
//
//                               PUBLIC FUNCTIONS
//
/////////////////////////////////////////////////////////////////////////////////////////


// generates random color/position values for some kind of models
bool ModelListClass::GenerateDataForModels()
{
	float red = 0.0f, green = 0.0f, blue = 0.0f, alpha = 0.0f;
	float posX = 0.0f, posY = 0.0f, posZ = 0.0f;
	float posMultiplier = 50.0f;
	

	// seed the random generator with the current time
	srand(static_cast<unsigned int>(time(NULL)));

	for (auto& elem : modelsRenderingList_)
	{
		// if this model is the terrain we don't generate data for it
		if (elem.first == "terrain")
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


// Shutdown() function releases the model information list array
void ModelListClass::Shutdown(void)
{
	// delete models objects
	for (auto& elem : modelsRenderingList_)
	{
		_SHUTDOWN(elem.second);
	}

	// clear the models list data
	if (!this->modelsRenderingList_.empty())
		this->modelsRenderingList_.clear();

	return;
}


// GetModelCount() returns the number of models that this class maintains information about
size_t ModelListClass::GetModelCount(void)
{
	return modelsRenderingList_.size();
}


// returns a pointer to the model by its id
ModelClass* ModelListClass::GetModelByID(const std::string& modelID) const
{
	return modelsRenderingList_.at(modelID);
}


// returns a pointer to the DEFAULT model by its id
ModelClass* ModelListClass::GetDefaultModelByID(const std::string& modelId) const
{
	return defaultModelsList_.at(modelId);
}


// get data (generated position, color) of the model
void ModelListClass::GetDataByID(const std::string& modelID, DirectX::XMFLOAT3& position, DirectX::XMFLOAT4& color)
{
	position = modelsRenderingList_[modelID]->GetPosition();
	color = modelsRenderingList_[modelID]->GetColor();

	return;
} // GetData()




// returns a reference to the map which contains the models data
std::map<std::string, ModelClass*>& ModelListClass::GetModelsRenderingList()
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
	// try to insert a model pointer by such an id
	auto res = modelsRenderingList_.insert({ modelId, pModel });

	// check if the model was inserted 
	if (!res.second)
	{
		// we have a duplication by such a key so generate a new one (new ID for the model)
		std::string newModelId = this->GenerateNewKeyInMap(modelsRenderingList_, modelId);
		//std::string debugMsg{ "DUPLICATION OF KEY: " + modelId + "; new key: " + newModelId };
		//Log::Error(THIS_FUNC, debugMsg.c_str());

		// insert a model pointer by the new id
		modelsRenderingList_.insert({ newModelId, pModel });
		pModel->SetID(newModelId);

		return newModelId;
	}

	return modelId;
}



void ModelListClass::AddDefaultModel(ModelClass* pModel, const std::string& modelId)
{
	assert(pModel != nullptr);
	defaultModelsList_.insert({ modelId, pModel });


	return;
}


void ModelListClass::RemoveFromRenderingListModelById(const std::string& modelId)
{
	// check if we have such an id in the models list
	auto iterator = modelsRenderingList_.find(modelId);

	if (iterator != modelsRenderingList_.end())   // if we found data by the key
	{
		Log::Error("KEK");
		_DELETE(modelsRenderingList_[modelId]);
		modelsRenderingList_.erase(modelId);
	}
}






/////////////////////////////////////////////////////////////////////////////////////////
//
//                               PRIVATE FUNCTIONS
//
/////////////////////////////////////////////////////////////////////////////////////////
std::string ModelListClass::GenerateNewKeyInMap(std::map<std::string, ModelClass*> map, const std::string & key)
{
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