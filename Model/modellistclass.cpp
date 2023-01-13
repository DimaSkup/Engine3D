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
	m_modelCount = numModels;

	// create a list array of the model information
	m_pModelInfoList = new ModelInfoType[m_modelCount];
	COM_ERROR_IF_FALSE(m_pModelInfoList, "can't allocate the memory for the ModelInfoType list");

	// seed the random generator with the current time
	srand(static_cast<unsigned int>(time(NULL)));

	// go through all the models and randomly generate the model colour and position
	for (size_t i = 0; i < m_modelCount; i++)
	{
		// generate a random colour for the model
		red = static_cast<float>(rand()) / RAND_MAX;
		green = static_cast<float>(rand()) / RAND_MAX;
		blue = static_cast<float>(rand()) / RAND_MAX;
		
		m_pModelInfoList[i].color = { red, green, blue, 1.0f };

		// generate a random position in from of the viewer for the mode
		m_pModelInfoList[i].posX = ((static_cast<float>(rand()) - static_cast<float>(rand())) / RAND_MAX) * 20.0f;
		m_pModelInfoList[i].posY = ((static_cast<float>(rand()) - static_cast<float>(rand())) / RAND_MAX) * 20.0f;
		m_pModelInfoList[i].posZ = ((static_cast<float>(rand()) - static_cast<float>(rand())) / RAND_MAX) * 20.0f + 5.0f;
	}

	return true;  // a list array of the model information was successfully initialized
} // Initialize()


// Shutdown() function releases the model information list array
void ModelListClass::Shutdown(void)
{
	// release the model information list
	_DELETE(m_pModelInfoList);

	return;
}


// GetModelCount() returns the number of models that this class maintains information about
int ModelListClass::GetModelCount(void)
{
	return m_modelCount;
}

// The GetData() function extracts 
void ModelListClass::GetData(int index, DirectX::XMFLOAT3& position, DirectX::XMVECTOR& color)
{
	position.x = m_pModelInfoList[index].posX;
	position.y = m_pModelInfoList[index].posY;
	position.z = m_pModelInfoList[index].posZ;

	color = m_pModelInfoList[index].color;

	return;
} // GetData()


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