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
#include "includes.h"
#include "log.h"
#include <fstream>
#include <iostream>
#include <iomanip>

using namespace std;

//////////////////////////////////
// Class name: ModelConverterClass
//////////////////////////////////
class ModelConverterClass
{
public:
	// converts .obj file model data into the internal model format
	static bool ConvertFromObjIntoModel(char* objFilename);

private:
	static bool ConvertFromObjIntoModelHelper(ifstream& fin);
	static bool FillInFaceDataByIndex(int index, ifstream& fin);
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

	static struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

	static ModelType* m_model;
	static POINT3D* m_point;
};

