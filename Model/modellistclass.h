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
#include <map>




//////////////////////////////////
// Class name: ModelListClass
//////////////////////////////////
class ModelListClass
{

public:
	ModelListClass(void);
	ModelListClass(const ModelListClass& copy);
	~ModelListClass(void);

	bool GenerateDataForModelByID(const std::string& modelID);
	void Shutdown(void);

	size_t GetModelCount(void);
	ModelClass* GetModelByID(const std::string& modelID) const;
	void GetDataByID(const std::string& modelID, DirectX::XMFLOAT3& position, DirectX::XMFLOAT4& color);

	const std::map<std::string, ModelClass*>& GetModelsList();
	void AddModel(ModelClass* pModel, const std::string& modelId);
	//void SetShaderForModelById(const std::string& modelId, const ShaderClass* pShader);

	// memory allocation
	void* operator new(size_t size);
	void operator delete(void* ptr);

private:
	//ModelInfoType* pModelInfoList_ = nullptr;               // a pointer to the list with information about all the models in the scene
	std::map<std::string, ModelClass*> modelsList_;      // contains a model_id and a pointer to the model object
};
