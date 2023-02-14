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

	size_t GetModelCount(void);
	ModelClass* GetModelByID(const std::string& modelID) const;
	ModelClass* GetDefaultModelByID(const std::string& modelId) const;
	void GetDataByID(const std::string& modelID, DirectX::XMFLOAT3& position, DirectX::XMFLOAT4& color);

	const std::map<std::string, ModelClass*>& GetModelsList();
	std::string AddModelForRendering(ModelClass* pModel, const std::string& modelId);
	void AddDefaultModel(ModelClass* pModel, const std::string& modelId);

private:
	std::map<std::string, ModelClass*> modelsList_;        // contains a model_id and a pointer to the model object
	std::map<std::string, ModelClass*> defaultModelsList_; // contains a pointers to the default models objects
	static ModelListClass* pInstance_;                     // a pointer to the current instance of the models list class
};
