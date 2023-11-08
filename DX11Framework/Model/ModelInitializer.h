////////////////////////////////////////////////////////////////////
// Filename:      ModelInitializer.h
// Description:   a concrete implementation of the ModelInitializerInterface;
// 
// Created:       05.07.23
////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include "ModelInitializerInterface.h"


//////////////////////////////////
// Class name: ModelInitializer
//////////////////////////////////
class ModelInitializer final : public ModelInitializerInterface
{
public:
	// initialize a new model from the file of type .blend, .fbx, .3ds, .obj, etc.
	virtual bool InitializeFromFile(ID3D11Device* pDevice, 
		ModelData* pModelData,                             
		const std::string & modelFilename) override;

	virtual bool InitializeMesh(Mesh** ppMesh,
		ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext,
		const std::vector<VERTEX> & verticesData,
		const std::vector<UINT> & indicesData) override;

private:
	bool ConvertModelFromFile(const std::string & modelType, const std::string & modelFilename);
	bool LoadModelDataFromFile(ModelData* pModelData, const std::string & modelFilename);
	void ExecuteModelMathCalculations(ModelData* pModelData);
};