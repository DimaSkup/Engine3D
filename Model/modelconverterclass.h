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
#include <memory>


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

	static ModelConverterClass* Get();
	const std::string & GetPathToModelDir() const;

	// converts .obj file model data into the internal model format
	bool ConvertFromObj(string objFilename);

private:
	bool ConvertFromObjHelper(ifstream& fin, ofstream& fout);

	bool ReadInVerticesData(ifstream& fin);
	bool ReadInTextureData(ifstream& fin);
	bool ReadInNormalsData(ifstream& fin);
	bool ReadInFacesData(ifstream& fin);
	bool WriteIntoFileFacesData(ofstream& fout);  // write model data in an internal model format into the output data file
	bool ResetConverterState();                   // after each convertation we MUST reset the state of the converter for proper later convertations
	
	bool ReadInModelData(ifstream& fin);
	bool GetOutputModelFilename(string& fullFilename, string& rawFilename);

	void PrintDebugData(std::string dataType, int dataElemsCount, const std::string & firstLine, const std::string & lastLine)
	{
		if (ModelConverterClass::PRINT_CONVERT_PROCESS_MESSAGES_)
		{
			std::string debugMsg{ dataType + " DEBUG DATA:" };
			Log::Debug(THIS_FUNC, debugMsg.c_str());

			debugMsg = { "first line of the " + dataType + " data: " + firstLine };
			Log::Debug("\t\t%s", debugMsg.c_str());

			debugMsg = { "line after the " + dataType + " data: " + lastLine };
			Log::Debug("\t\t%s", debugMsg.c_str());

			debugMsg = { dataType + " count: " + std::to_string(dataElemsCount) };
			Log::Debug("\t\t%s", debugMsg.c_str());
		}
	}

private:
	static ModelConverterClass* pInstance_;

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
			x = y = z = 0.0f;
			tu = tv = 0.0f;
			nx = ny = nz = 0.0f;
			cr = cb = ca = 1.0f;
		}

		float x, y, z;         // vertex coords
		float tu, tv;          // texture coords
		float nx, ny, nz;      // normal
		float cr, cg, cb, ca;  // colours (RGBA)
	};

	char* inputLine_ = nullptr;

	// constants
	const int INPUT_LINE_SIZE_ = 80;                    // how many symbols can read the getline() function as one sinle text line
	const std::string MODEL_FILE_TYPE_ { ".txt" };         // internal model data file will have this format
	const std::string MODEL_DIR_PATH_ { "data/models/" };  // the path to the directory with models
	const bool PRINT_CONVERT_PROCESS_MESSAGES_ = true;     // defines whether to print or not in the console messages about the convertation process 

	ModelType* pModelType_ = nullptr;
	POINT3D*   pPoint3D_   = nullptr;
	TEXCOORD*  pTexCoord_  = nullptr;
	NORMAL*    pNormal_    = nullptr;

	int verticesCount_ = 0;
	int textureCoordsCount_ = 0;
	int normalsCount_ = 0;
	int facesCount_ = 0;

	ModelType vtnData[3];   // vertex/texture/normal data for a single vertex
	std::vector<ModelType> modelData;

	//char* inputLine = nullptr;    // during execution of the getline() function we put here a one single text line
};

