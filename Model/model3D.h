#pragma once

#include "modelclass.h"


class Model3D : public ModelClass
{
public:
	~Model3D() {
		ReleaseModel();     // release the model data
	}

	bool Model3D::Initialize(ID3D11Device* pDevice, std::string modelName, WCHAR* textureFilename);

	
	

	// memory allocation (we need it because we use DirectX::XM-objects)
	void* operator new(size_t i);
	void operator delete(void* p);

private:
	// functions to handle loading and unloading the model data from the text data file
	bool LoadModel(std::string modelName);
	void ReleaseModel();

	bool InitializeBuffers(ID3D11Device* pDevice);

private:

	// internal representation of a model structure
	struct ModelType
	{
		float x, y, z;     // position coords
		float tu, tv;      // texture coords
		float nx, ny, nz;  // normals
	};


	ModelType* pModelType_ = nullptr;
};