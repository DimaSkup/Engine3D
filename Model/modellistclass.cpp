////////////////////////////////////////////////////////////////////
// Filename: modellistclass.cpp
// Revising: 17.09.22
////////////////////////////////////////////////////////////////////
#include "modellistclass.h"




ModelListClass::ModelListClass(void) {}

// we don't use the copy constructor and destructor in this class
ModelListClass::ModelListClass(const ModelListClass& copy) {}
ModelListClass::~ModelListClass(void) {}

// ---------------------------------------------------------------------------------- //
//                                                                                    //
//                           PUBLIC FUNCTIONS                                         //
//                                                                                    //
// ---------------------------------------------------------------------------------- //

bool ModelListClass::GenerateDataForModelByID(const std::string& modelID)
{
	float red = 0.0f, green = 0.0f, blue = 0.0f, alpha = 0.0f;
	float posX = 0.0f, posY = 0.0f, posZ = 0.0f;
	size_t modelsCount = modelsList_.size();
	

	// seed the random generator with the current time
	srand(static_cast<unsigned int>(time(NULL)));

	// generate a random colour for the model
	red = static_cast<float>(rand()) / RAND_MAX;
	green = static_cast<float>(rand()) / RAND_MAX;
	blue = static_cast<float>(rand()) / RAND_MAX;
	alpha = 1.0f;
		
	// generate a random position in from of the viewer for the mode
	posX = ((static_cast<float>(rand()) - static_cast<float>(rand())) / RAND_MAX) * 20.0f;
	posY = ((static_cast<float>(rand()) - static_cast<float>(rand())) / RAND_MAX) * 20.0f;
	posZ = ((static_cast<float>(rand()) - static_cast<float>(rand())) / RAND_MAX) * 20.0f + 5.0f;


	modelsList_[modelID]->SetColor(red, green, blue, 1.0f);
	modelsList_[modelID]->SetPosition(posX, posY, posZ);

	Log::Error("color: %f %f %f;     pos: %f %f %f", red, green, blue, )

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

void ModelListClass::AddModel(ModelClass* pModel, const std::string& modelId)
{

	modelsList_.insert({ modelId, pModel });
}





// memory allocation
void* ModelListClass::operator new(size_t size)
{
	void* ptr = _aligned_malloc(size, 16);

	COM_ERROR_IF_FALSE(ptr, "can't allocate the memory for the ModelListClass object");

	return ptr;
}

void ModelListClass::operator delete(void* ptr)
{
	_aligned_free(ptr);
}