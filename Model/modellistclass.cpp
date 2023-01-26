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

// initialize the model list with random data about model's position/color
bool ModelListClass::Initialize(int numModels)
{
	float red, green, blue;

	// store the number of models
	modelCount_ = numModels;

	// create a list array of the model information
	pModelInfoList_ = new ModelInfoType[modelCount_];
	COM_ERROR_IF_FALSE(pModelInfoList_, "can't allocate the memory for the ModelInfoType list");

	// seed the random generator with the current time
	srand(static_cast<unsigned int>(time(NULL)));

	// go through all the models and randomly generate the model colour and position
	for (size_t i = 0; i < modelCount_; i++)
	{
		// generate a random colour for the model
		red = static_cast<float>(rand()) / RAND_MAX;
		green = static_cast<float>(rand()) / RAND_MAX;
		blue = static_cast<float>(rand()) / RAND_MAX;
		
		pModelInfoList_[i].color = { red, green, blue, 1.0f };

		// generate a random position in from of the viewer for the mode
		pModelInfoList_[i].posX = ((static_cast<float>(rand()) - static_cast<float>(rand())) / RAND_MAX) * 20.0f;
		pModelInfoList_[i].posY = ((static_cast<float>(rand()) - static_cast<float>(rand())) / RAND_MAX) * 20.0f;
		pModelInfoList_[i].posZ = ((static_cast<float>(rand()) - static_cast<float>(rand())) / RAND_MAX) * 20.0f + 5.0f;
	}

	return true;  // a list array of the model information was successfully initialized
} // Initialize()


// Shutdown() function releases the model information list array
void ModelListClass::Shutdown(void)
{
	// release the model information list
	_DELETE(pModelInfoList_);

	// delete models objects
	for (size_t i = 0; i < pModels_.size(); i++)
	{
		_SHUTDOWN(pModels_[i]);
	}

	if (!this->pModels_.empty())
		this->pModels_.clear();

	return;
}


// GetModelCount() returns the number of models that this class maintains information about
int ModelListClass::GetModelCount(void)
{
	return modelCount_;
}

// The GetData() function extracts 
void ModelListClass::GetData(int index, DirectX::XMFLOAT3& position, DirectX::XMVECTOR& color)
{
	position.x = pModelInfoList_[index].posX;
	position.y = pModelInfoList_[index].posY;
	position.z = pModelInfoList_[index].posZ;

	color = pModelInfoList_[index].color;

	return;
} // GetData()

const vector<ModelClass*>& ModelListClass::GetModels()
{
	return this->pModels_;
}

size_t ModelListClass::AddModel(ModelClass* pModel, std::string modelName)
{
	this->pModels_.push_back(pModel);

	std::string debugMsg = modelName + " was successfully added to the list";
	Log::Debug(THIS_FUNC, debugMsg.c_str());

	return this->pModels_.size() - 1;  // return an index of the last model
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