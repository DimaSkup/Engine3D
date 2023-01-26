////////////////////////////////////////////////////////////////////
// Filename:     modellistclass.h
// Description:  this class is a class for maintaining information
//               about all the models in the scene.
//
// Revising:     17.09.22
////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include "../Engine/Log.h"
#include "../Engine/macros.h"
#include "../Model/modelclass.h"

#include <DirectXMath.h>
#include <cstdlib>
#include <ctime>  // is needed for random generator
#include <vector> // will contains all the models data

//////////////////////////////////
// Class name: ModelListClass
//////////////////////////////////
class ModelListClass
{
private:
	struct ModelInfoType
	{
		DirectX::XMVECTOR color;
		float posX, posY, posZ;
	};

public:
	ModelListClass(void);
	ModelListClass(const ModelListClass& copy);
	~ModelListClass(void);

	bool Initialize(int numModels);
	void Shutdown(void);

	int GetModelCount(void);
	void GetData(int index, DirectX::XMFLOAT3& position, DirectX::XMVECTOR& color);

	const vector<ModelClass*>& GetModels();
	size_t AddModel(ModelClass* pModel, std::string modelName);

	// memory allocation
	void* operator new(size_t size);
	void operator delete(void* ptr);

private:
	int modelCount_ = 0;                       // contains the count of all the models in the scene
	ModelInfoType* pModelInfoList_ = nullptr;  // a pointer to the list with information about all the models in the scene
	vector<ModelClass*> pModels_;              // contains pointers to the models objects
};
