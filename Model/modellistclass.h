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
#include "../Model/Model.h"

#include <DirectXMath.h>
#include <cstdlib>
#include <ctime>  // is needed for random generator
#include <map>




//////////////////////////////////
// Class name: ModelListClass
//////////////////////////////////
class ModelListClass final
{

public:
	ModelListClass(void);
	~ModelListClass(void);

	static ModelListClass* Get() { return pInstance_; }

	bool GenerateDataForModels();
	void Shutdown(void);

	//
	// getters
	//
	size_t GetRenderedModelsCount(void) const;
	Model* GetModelByID(const std::string& modelID);
	Model* GetZoneModelByID(const std::string& modelID);
	Model* GetDefaultModelByID(const std::string& modelId) const;
	void GetDataByID(const std::string& modelID, DirectX::XMFLOAT3& position, DirectX::XMFLOAT4& color);

	const std::map<std::string, Model*> & GetModelsGlobalList() const;
	const std::map<std::string, Model*> & GetModelsRenderingList();  // get a map of all the models on the scene 
	const std::map<std::string, Model*> & GetSpritesRenderingList();
	std::map<std::string, Model*> & ModelListClass::GetDefaultModelsList();


	// setters / adders
	std::string AddModel(Model* pModel, const std::string & modelID);
	std::string AddZoneElement(Model* pModel, const std::string & modelID);
	std::string AddSprite(Model* pModel, const std::string & spriteID);
	void SetModelForRenderingByID(const std::string& modelID);
	void SetModelAsDefaultByID(const std::string& modelID);

	// deleters
	void RemoveModelByID(const std::string& modelID);                  // delete a model by id at all
	void RemoveFromRenderingListModelByID(const std::string& modelID); // set that we don't want to render a model by this id


private:  // restrict a copying of this class instance
	ModelListClass(const ModelListClass & obj);
	ModelListClass & operator=(const ModelListClass & obj);

private:
	std::string GenerateNewKeyInMap(const std::map<std::string, Model*> & map, const std::string & key);  // generates a new key which is based on the passed one
	std::_Tree_const_iterator<std::_Tree_val<std::_Tree_simple_types<std::pair<const std::string, Model*>>>> GetIteratorByID(const std::map<std::string, Model*> & map, const std::string & modelID) const;         // searches a model in the map and returns an iterator to it;

private:
	std::map<std::string, Model*> modelsGlobalList_;      // all the models of the project
	std::map<std::string, Model*> modelsRenderingList_;   // contains a model_id and a pointer to the model object (custom models, spheres, cubes, planes, etc.)
	std::map<std::string, Model*> zoneModelsList_;        // [model_name, model_ptr] pairs of zone elements (models, for instance: terrain, sky dome, clouds, etc.)
	std::map<std::string, Model*> defaultModelsList_;     // contains a pointers to the default models objects
	std::map<std::string, Model*> spritesRenderingList_;

	static ModelListClass* pInstance_;                    // a pointer to the current instance of the models list class
};
