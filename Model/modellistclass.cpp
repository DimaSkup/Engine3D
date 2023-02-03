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





// ---------------------------------------------------------------------------------- //
//                                                                                    //
//                           PUBLIC FUNCTIONS                                         //
//                                                                                    //
// ---------------------------------------------------------------------------------- //


// generates random color/position values for some kind of models
bool ModelListClass::GenerateDataForModels()
{
	float red = 0.0f, green = 0.0f, blue = 0.0f, alpha = 0.0f;
	float posX = 0.0f, posY = 0.0f, posZ = 0.0f;
	float posMultiplier = 50.0f;
	

	// seed the random generator with the current time
	srand(static_cast<unsigned int>(time(NULL)));

	for (auto& elem : modelsList_)
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
	for (auto& elem : modelsList_)
	{
		_SHUTDOWN(elem.second);
	}

	// clear the models list data
	if (!this->modelsList_.empty())
		this->modelsList_.clear();

	return;
}


// GetModelCount() returns the number of models that this class maintains information about
size_t ModelListClass::GetModelCount(void)
{
	return modelsList_.size();
}


// returns a pointer to the model by its id
ModelClass* ModelListClass::GetModelByID(const std::string& modelID) const
{
	return modelsList_.at(modelID);
}


// get data of the model
void ModelListClass::GetDataByID(const std::string& modelID, DirectX::XMFLOAT3& position, DirectX::XMFLOAT4& color)
{
	position = modelsList_[modelID]->GetPosition();
	color = modelsList_[modelID]->GetColor();

	return;
} // GetData()


// returns a reference to the map which contains the models data
const std::map<std::string, ModelClass*>& ModelListClass::GetModelsList()
{
	return this->modelsList_;
}


// adds a new model ptr to the list with a modelID name;
// 
// if we already have such an id of a model this function generates a new one for this copy
// and returns this new id
std::string ModelListClass::AddModel(ModelClass* pModel, const std::string& modelId)
{
	for (auto& elem : modelsList_)
	{
		if (elem.first == modelId) // if we already have the same modelId
		{
			std::string newModelID{ modelId + "_copy" };
			modelsList_.insert({ newModelID, pModel });
			return newModelID;
		}
	}


	modelsList_.insert({ modelId, pModel });
	return modelId;
}