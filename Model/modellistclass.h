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

#include <DirectXMath.h>
#include <cstdlib>
#include <ctime>  // is needed for random generator

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

	// memory allocation
	void* operator new(size_t size);
	void operator delete(void* ptr);

private:
	int m_modelCount = 0;                       // contains the count of all the models in the scene
	ModelInfoType* m_pModelInfoList = nullptr;  // a pointer to the list with information about all the models in the scene
};
