#pragma once

// models stuff
#include "Model.h"
#include "GameObjectsListClass.h"
#include "../Model/ModelInitializerInterface.h"

// shaders stuff
#include "../ShaderClass/shaderclass.h"
#include "../ShaderClass/ModelToShaderMediatorInterface.h"




class ModelCreator
{
public:
	virtual ~ModelCreator() {};

	// get an instance of the model
	virtual Model* GetInstance(ModelInitializerInterface* pModelInitializer) = 0; 

	// define if this model is a usual model (cube, sphere, plane, etc.) 
	// in another case it is a Zone element (terrain, sky dome, sky plane, etc.)
	virtual bool IsUsualModel() const = 0;

	bool ModelCreator::CreateAndInitDefaultModel(ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext,
		ModelInitializerInterface* pModelInitializer,
		ModelToShaderMediatorInterface* pModelToShaderMediator,
		const std::string & renderShaderName = "ColorShaderClass");

	Model* CreateAndInitModel(ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext,
		ModelInitializerInterface* pModelInitializer,
		ModelToShaderMediatorInterface* pModelToShaderMediator,
		const std::string & filePath,
		const std::string & renderShaderName = "ColorShaderClass");  // name of a shader which will be used for rendering a model

	Model* CreateCopyOfModel(Model* pOriginModel);

private:
	std::string TryToGetModelID_WhenException(Model* pModel);
};
