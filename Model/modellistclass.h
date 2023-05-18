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

	static ModelListClass* Get() { return pInstance_; }

	bool GenerateDataForModels();
	void Shutdown(void);

	// getters
	size_t GetAllModelsCount(void);
	ModelClass* GetModelByID(const std::string& modelID) const;
	ModelClass* GetDefaultModelByID(const std::string& modelId) const;
	void GetDataByID(const std::string& modelID, DirectX::XMFLOAT3& position, DirectX::XMFLOAT4& color);

	const std::map<std::string, ModelClass*> & GetModelsGlobalList() const;
	const std::map<std::string, ModelClass*> & GetModelsRenderingList();  // get a map of all the models on the scene 
	std::map<std::string, ModelClass*> & ModelListClass::GetDefaultModelsList();


	// setters / adders
	std::string AddModel(ModelClass* pModel, const std::string& modelId);
	void SetModelForRenderingByID(const std::string& modelId);
	void SetModelAsDefaultByID(const std::string& modelId);

	// deleters
	//void DontRenderModelById(const std::string& modelId);
	void RemoveFromRenderingListModelById(const std::string& modelId);


private:
	// generates a new key which is based on the passed one
	std::string GenerateNewKeyInMap(std::map<std::string, ModelClass*> map, const std::string & key);

private:
	std::map<std::string, ModelClass*> modelsGlobalList_;      // all the models of the project
	std::map<std::string, ModelClass*> modelsRenderingList_;   // contains a model_id and a pointer to the model object
	std::map<std::string, ModelClass*> defaultModelsList_;     // contains a pointers to the default models objects
	static ModelListClass* pInstance_;                         // a pointer to the current instance of the models list class
};
