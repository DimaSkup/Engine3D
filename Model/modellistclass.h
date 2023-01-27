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
		DirectX::XMFLOAT4 color;
		float posX, posY, posZ;
	};

public:
	ModelListClass(void);
	ModelListClass(const ModelListClass& copy);
	~ModelListClass(void);

	bool GenerateDataForModels();
	void Shutdown(void);

	size_t GetModelCount(void);
	void GetData(int index, DirectX::XMFLOAT3& position, DirectX::XMFLOAT4& color);

	const vector<ModelClass*>& GetModels();
	size_t AddModel(ModelClass* pModel, const std::string& modelId);
	//void SetShaderForModelById(const std::string& modelId, const ShaderClass* pShader);

	// memory allocation
	void* operator new(size_t size);
	void operator delete(void* ptr);

private:
	ModelInfoType* pModelInfoList_ = nullptr;  // a pointer to the list with information about all the models in the scene
	vector<ModelClass*> modelsArray_;              // contains pointers to the models objects
};
