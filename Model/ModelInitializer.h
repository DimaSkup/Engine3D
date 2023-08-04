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

	// initialize a new model using data of the another model
	virtual bool InitializeCopyOf(ModelData* pNewModelData,
		ModelData* pOriginModelData,
		ID3D11Device* pDevice) override;

	// initialize a new model from the file of type .blend, .fbx, .3ds, .obj, etc.
	virtual bool InitializeFromFile(ID3D11Device* pDevice, 
		ModelData* pModelData,                             
		const std::string & modelFilename) override;

	// initialize a vertex and index buffers with model's data
	virtual bool InitializeBuffers(ID3D11Device* pDevice,
		VertexBuffer<VERTEX>* pVertexBuffer,
		IndexBuffer* pIndexBuffer,
		ModelData* pModelData) override;      

private:
	bool ConvertModelFromFile(const std::string & modelType, const std::string & modelFilename);
	bool LoadModelDataFromFile(ModelData* pModelData, const std::string & modelFilename);
	void ExecuteModelMathCalculations(ModelData* pModelData);
};