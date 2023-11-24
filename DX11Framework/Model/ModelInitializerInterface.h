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
#include "../Model/Mesh.h"
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
	// initialize a new model from the file of type .blend, .fbx, .3ds, .obj, etc.
	virtual bool InitializeFromFile(ID3D11Device* pDevice, 
		std::vector<Mesh*> & meshesArr,       // an array of meshes which have vertices/indices buffers that will be filled with vertices/indices data
		const std::string & modelFilename) = 0;

};
