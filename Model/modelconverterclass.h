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
#include "../Engine/Settings.h"   // here we get some settings for the convertation process

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

	// converts .obj file model data into the internal model format
	bool ConvertFromObj(const string & objFilename);

private:
	bool ConvertFromObjHelper(ifstream & fin, ofstream & fout);

	// input data file reading handlers  
	void SkipUntilVerticesData(ifstream & fin);
	void ReadCounts(ifstream & fin);   // read the count of vertices, textures coordinates and the count of faces as well
	bool ReadInVerticesData(ifstream & fin);
	bool ReadInTextureData(ifstream & fin);
	bool ReadInNormalsData(ifstream & fin);
	bool ReadInFacesData(ifstream & fin);

	bool ResetConverterState();                   // after each convertation we MUST reset the state of the converter for proper later convertations
	
	bool GetOutputModelFilename(string & outputFilename, const string & inputFilename);

	void PrintDebugFilenames(const std::string & inputFilename, const std::string & outputFilename) const;
	void PrintReadingDebugData(std::string dataType, int dataElemsCount, const std::string & firstLine, const std::string & lastLine);

	// output data file writing handlers 
	bool WriteDataIntoOutputFile(ofstream & fout);  // write model data in an internal model format into the output data file
	bool WriteIndicesIntoOutputFile(ofstream & fout);
	bool WriteVerticesIntoOutputFile(ofstream & fout);
	bool WriteTexturesIntoOutputFile(ofstream & fout);
private:

	struct POINT3D
	{
		POINT3D()
		{
			x = y = z = NULL;
		}

		float x, y, z;
	};

	struct TEXCOORD
	{
		TEXCOORD()
		{
			tu = tv = NULL;
		}

		float tu, tv;
	};

	struct NORMAL
	{
		NORMAL()
		{
			nx = ny = nz = NULL;
		}

		float nx, ny, nz;
	};


	POINT3D*   pPoint3D_ = nullptr;
	TEXCOORD*  pTexCoord_ = nullptr;
	NORMAL*    pNormal_ = nullptr;

	char* inputLineBuffer_ = nullptr;                   // during execution of the getline() function we put here a one single text line

	int verticesCount_ = 0;
	int textureCoordsCount_ = 0;
	int normalsCount_ = 0;
	int facesCount_ = 0;

	std::vector<UINT> vertexIndicesArray_;
	std::vector<UINT> textureIndicesArray_;

	bool isPrintConvertProcessMessages_ = false;  // defines whether to print or not in the console messages about the convertation process    

	// constants
	const int INPUT_LINE_SIZE_ = 80;              // how many symbols can read the getline() function as one sinle text line
};

