/////////////////////////////////////////////////////////////////////
// Filename:     modelclass.h
// Description:  the ModelClass is responsible for encapsulating 
//               the geometry for 3DModels, converting model data,
//               texturing;
//
// Revising:     09.01.23
/////////////////////////////////////////////////////////////////////
#pragma once


//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <d3d11.h>
//#include <d3dx10math.h>
#include <fstream>
#include <DirectXMath.h>
#include <memory>                // for using unique_ptr

#include "../Engine/macros.h"    // for some macros utils
#include "../Engine/Log.h"       // for using a logger

#include "modelconverterclass.h" // for converting a model data from other types (obj, etc.) into our internal model type
#include "ModelLoader.h"
#include "Vertex.h"

#include "ModelMath.h"



//////////////////////////////////
// Class name: ModelClass
//////////////////////////////////
class ModelClass :
{
public:

	void AllocateVerticesAndIndicesArrays(UINT vertexCount, UINT indexCount);
};

