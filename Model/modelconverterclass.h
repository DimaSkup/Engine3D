/////////////////////////////////////////////////////////////////////
// Filename:     modelconverterclass.h
// Description:  this class is used to convert models data from others
//               formats into the internal model data format
//
// Revising:     25.04.22
/////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include "../Engine/Log.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <vector>


#define INPUT_LINE_SIZE 80
#define MODEL_FILE_TYPE ".txt"

using namespace std;

//////////////////////////////////
// Class name: ModelConverterClass
//////////////////////////////////
class ModelConverterClass
{
public:
	ModelConverterClass(void);
	ModelConverterClass(const ModelConverterClass& other);
	~ModelConverterClass(void);

	void Shutdown(void);	// releases the memory

	// converts .obj file model data into the internal model format
	bool ConvertFromObj(string objFilename);

private:
	bool ConvertFromObjHelper(ifstream& fin, ofstream& fout);

	bool ReadInVerticesData(ifstream& fin);
	bool ReadInTextureData(ifstream& fin);
	bool ReadInNormalsData(ifstream& fin);
	bool ReadInFacesData(ifstream& fin);
	bool WriteIntoFileFacesData(ofstream& fout);
	
	bool ReadInModelData(ifstream& fin);

	bool GetFinalModelFilename(string& fullFilename, string& rawFilename);

private:
	struct POINT3D
	{
		float x, y, z;
	};

	struct TEXCOORD
	{
		float tu, tv;
	};

	struct NORMAL
	{
		float nx, ny, nz;
	};

	struct ModelType
	{
		ModelType()
		{
			// by default we set a purple colour for each vertex
			cr = 1.0f;
			cg = 0.0f;
			cb = 1.0f;
			ca = 1.0f;  // alpha
		}

		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
		float cr, cg, cb, ca;  // colours (cr - red, cg - green, cb - blue, ca - alpha)
	};

	ModelType* pModelType_ = nullptr;
	POINT3D* pPoint3D_     = nullptr;
	TEXCOORD* pTexCoord_   = nullptr;
	NORMAL* pNormal_       = nullptr;

	int verticesCount_ = 0;
	int textureCoordsCount_ = 0;
	int normalsCount_ = 0;
	int facesCount_ = 0;

	ModelType vtnData[3];   // vertex/texture/normal data for a single vertex
	//ModelType* pModelData = nullptr;
	std::vector<ModelType> modelData;
};

