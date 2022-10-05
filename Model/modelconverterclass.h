/////////////////////////////////////////////////////////////////////
// Filename:     modelconverterclass.h
// Description:  this class is used to convert model data from others
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
	bool ConvertFromObjIntoModel(string objFilename);

private:
	bool ConvertFromObjIntoModelHelper(ifstream& fin, ofstream& fout);

	bool ReadInVerticesData(ifstream& fin);
	bool ReadInTextureData(ifstream& fin);
	bool ReadInNormalsData(ifstream& fin);
	bool ReadInFacesData(ifstream& fin);
	bool WriteIntoFileFacesData(ofstream& fout);
	
	bool FillInVerticesDataByIndex(int index, ifstream& fin);

	bool GetFinalModelFilename(string& fullFilename, string& rawFilename);
public:
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
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

	ModelType* m_model;
	POINT3D* m_point;
	TEXCOORD* m_tex;
	NORMAL* m_normal;
	int facesCount;
};

