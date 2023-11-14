///////////////////////////////////////////////////////////////////////////////////////////
// Filename:      ModelLoader.cpp
// Description:   contains a functional for loading model data (internal model type)
//                from a data file into a model object;
//
// model data file structure:
//                1. vertices coordinates count
//                2. indices count
//                3. textures coordinates count
//
//                4. vertices coordinates data
//                5. textures coordinates data
//
//                6. indices data (of vertices)
//                7. texture indices data
//

//
// Created:       27.02.23
///////////////////////////////////////////////////////////////////////////////////////////
#include "ModelLoader.h"



ModelLoader::ModelLoader()
{
}


ModelLoader::~ModelLoader()
{
	this->Shutdown();
}





///////////////////////////////////////////////////////////////////////////////////////////
//
//                               PUBLIC FUNCTIONS
// 
///////////////////////////////////////////////////////////////////////////////////////////


void ModelLoader::Shutdown()
{
	verticesCoordsArr_.clear();
	texturesCoordsArr_.clear();
	textureIndicesArr_.clear();
}

///////////////////////////////////////////////////////////

bool ModelLoader::Load(const std::string & filePath,
	std::vector<VERTEX> & verticesArr,
	std::vector<UINT> & indicesArr)
{
	// load model data from a data file

	try
	{
		std::ifstream fin(filePath, std::ios::in);

		// If it could not open the file then exit
		if (fin.fail())
		{
			std::string errorMsg = "can't open the text file \"" + filePath + "\" with model data";
			COM_ERROR_IF_FALSE(false, errorMsg);
		}

		// read in the COUNT of vertices, indices, and textures
		this->LoadModelVITCount(fin);

		// now we can allocate memory for the vertices/textures/indices data
		verticesCoordsArr_.resize(this->indexCount_);
		texturesCoordsArr_.resize(this->texturesCount_);
		indicesArr.resize(this->indexCount_);

		// read in the vertices, indices, and textures data into input arrays
		this->LoadModelVertexData(fin);
		this->LoadModelTextureData(fin);
		this->LoadModelIndexData(fin, indicesArr);



		// initialize an internal model data structure
		this->InitializeInternalModelDataType(verticesArr, indicesArr);

		// Close the model file
		fin.close();
	}
	catch (COMException & e)
	{
		this->Shutdown();

		Log::Error(e, true);
		Log::Error(THIS_FUNC, "can't load model data from the data file: " + filePath);
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////

UINT ModelLoader::GetVertexCount() const
{
	// returns how many vertices coords we have in a data file
	return vertexCount_;
}

///////////////////////////////////////////////////////////

UINT ModelLoader::GetIndexCount() const
{
	// returns how many indices we have in a data file
	return indexCount_;
}






////////////////////////////////////////////////////////////////////////////////////////////
//
//                             PRIVATE FUNCTIONS
//
////////////////////////////////////////////////////////////////////////////////////////////



bool ModelLoader::LoadModelVITCount(ifstream & fin)
{
	// this function reads in the vertices, indices, and textures counts
	// from a model data file

	char input = ' ';

	// Read up to the value of vertex count
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	// Read in the vertex count
	fin >> vertexCount_;  

	//////////////////////////////////////

	// Read up to the value of index count
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	// Read in the index count
	fin >> indexCount_;  


	//////////////////////////////////////

	// Read up to the value of textures count
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	// Read in the textures count
	fin >> texturesCount_;  

	//////////////////////////////////////

	// check readed counts
	assert(vertexCount_ > 0);
	assert(indexCount_ > 0);
	assert(texturesCount_ > 0);

	return true;
}

///////////////////////////////////////////////////////////

bool ModelLoader::LoadModelIndexData(ifstream & fin, 
	std::vector<UINT> & indicesArr)
{
	// this function reads in the indices data 
	// (for vertices and textures) from the data file;

	// assert that the input array has enough space for the indices data
	assert(indexCount_ == indicesArr.size());

	char input = ' ';

	//////////////////////////////////////
	
	// Read up to the VERTEX indices data
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	// Read in the indices data (of vertices)
	for (size_t i = 0; i < indexCount_; i++)
	{
		fin >> indicesArr[i];
	}


	//////////////////////////////////////

	// Read up to the TEXTURE indices data
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	textureIndicesArr_.resize(indexCount_);
	// Read in the indices data (of textures)
	for (size_t i = 0; i < indexCount_; i++)
	{
		fin >> textureIndicesArr_[i];
	}

	return true;

} // end LoadModelIndexData

///////////////////////////////////////////////////////////

bool ModelLoader::LoadModelVertexData(ifstream & fin)
{
	// this function reads in the vertices data from the data file 
	// and stores this data into the vertices temporal array

	// temporal character
	char input = ' ';

	// Read up to the beginning of the vertices data
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin.ignore(1);


	// Read in the vertices data
	for (size_t i = 0; i < vertexCount_; i++)
	{
		fin >> verticesCoordsArr_[i].x
			>> verticesCoordsArr_[i].y
			>> verticesCoordsArr_[i].z;
	}

	return true;

} // end LoadModelVertexData

///////////////////////////////////////////////////////////

bool ModelLoader::LoadModelTextureData(ifstream & fin)
{
	// this function reads in the texture coordinates data from the data file

	char input = ' ';

	// Read up to the textures data
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	// Read in the texture coordinates data
	for (size_t i = 0; i < texturesCount_; i++)
	{
		fin >> texturesCoordsArr_[i].x >> texturesCoordsArr_[i].y;
	}


	return true;

} // end LoadModelTextureData

///////////////////////////////////////////////////////////

bool ModelLoader::InitializeInternalModelDataType(std::vector<VERTEX> & verticesArr,
	std::vector<UINT> & indicesArr)
{
	// initialize an internal model data structure

	UINT vertexIndex = 0;
	UINT textureIndex = 0;

	// resize the input verticesArr to the necessary size
	verticesArr.resize(indicesArr.size());

	// write texture coordinates data into relative vertices;
	// note: vertices positions and another data has been already written before;
	for (size_t i = 0; i < indexCount_; i++)
	{
		vertexIndex = indicesArr[i];
		textureIndex = textureIndicesArr_[i];

		// copy vertex coordinates into the vertex
		verticesArr[i].position = verticesCoordsArr_[vertexIndex];

		// copy texture coordinates into the vertex
		verticesArr[i].texture = texturesCoordsArr_[textureIndex];

		//pVertices[i].normal   = { pModelType_[i].nx, pModelType_[i].ny, pModelType_[i].nz };
		//pVertices[i].tangent  = { pModelType_[i].tx, pModelType_[i].ty, pModelType_[i].tz };
		//pVertices[i].binormal = { pModelType_[i].bx, pModelType_[i].by, pModelType_[i].bz };
		//pVertices[i].color    = { pModelType_[i].cr, pModelType_[i].cg, pModelType_[i].cb, pModelType_[i].ca };
	}


	// Load up the index array with data
	for (UINT i = 0; i < indexCount_; i++)
	{
		indicesArr[i] = i;
	}

	return true;

} // end InitializeInternalModelDataType
