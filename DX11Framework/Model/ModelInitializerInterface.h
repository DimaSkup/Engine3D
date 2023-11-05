////////////////////////////////////////////////////////////////////
// Filename:      ModelInitializerInterface.h
// Description:   an interface for concrete model initializers;
//                concrete model initializers will initialize a model
//                in a particular way
// 
// Created:       05.07.23
////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <d3d11.h>
#include <string>

#include "../Engine/Settings.h"
#include "../Model/ModelData.h"
#include "../Model/VertexBuffer.h"
#include "../Model/IndexBuffer.h"

#include "../Engine/UtilsForDLL.h"

//////////////////////////////////
// TYPEDEFS
//////////////////////////////////

// is needed for calling DLL ModelConverter's import function; here we store a pointer to the DLL's process;
typedef VOID(*DLLPROC) (const char* inputDataFile, const char* outputDataFile);    


//////////////////////////////////
// Class name: ModelInitializerInterface
//////////////////////////////////
class ModelInitializerInterface
{
public:
	//virtual bool Initialize(ID3D11Device* pDevice) = 0;

	// initialize a new model using data of the another model
	virtual bool InitializeCopyOf(ModelData* pNewModelData, 
		ModelData* pOriginModelData, 
		ID3D11Device* pDevice) = 0;

	// initialize a new model from the file of type .blend, .fbx, .3ds, .obj, etc.
	virtual bool InitializeFromFile(ID3D11Device* pDevice, 
		ModelData* pModelData,
		const std::string & modelFilename) = 0;

	// initialize a DEFAULT vertex and index buffer with model's data
	virtual bool InitializeDefaultBuffers(ID3D11Device* pDevice,
		VertexBuffer<VERTEX>* pVertexBuffer,
		IndexBuffer* pIndexBuffer,
		ModelData* pModelData) = 0;

	// initialize a DYNAMIC vertex and index buffer with model's data
	virtual bool InitializeDynamicBuffers(ID3D11Device* pDevice,
		VertexBuffer<VERTEX>* pVertexBuffer,
		IndexBuffer* pIndexBuffer,
		ModelData* pModelData) = 0;
};
