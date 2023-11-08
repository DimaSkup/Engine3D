/////////////////////////////////////////////////////////////////////
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
/////////////////////////////////////////////////////////////////////
#include "ModelLoader.h"



ModelLoader::ModelLoader()
{
}


ModelLoader::~ModelLoader()
{
//	_DELETE_ARR(pVertexIndicesData_);
	_DELETE_ARR(pTextureIndicesData_);

	//_DELETE_ARR(pVerticesData_);
	_DELETE_ARR(pTexturesData_);
}





/////////////////////////////////////////////////////////////////////////////////////////
//
//                               PUBLIC FUNCTIONS
// 
/////////////////////////////////////////////////////////////////////////////////////////


// load model data from a data file
bool ModelLoader::Load(const std::string & filePath,
	std::vector<VERTEX> & verticesArr,
	std::vector<UINT> & indicesArr)
{
	std::ifstream fin(filePath, std::ios::in);

	// If it could not open the file then exit
	if (fin.fail())
	{
		std::string errorMsg = "can't open the text file \"" + filePath + "\" with model data";
		COM_ERROR_IF_FALSE(false, errorMsg);
	}

	// read the vertices, indices, and textures count
	this->LoadModelVITCount(fin);

	// allocate memory for the vertices/indices data
	verticesArr.reserve(this->vertexCount_);
	indicesArr.reserve(this->indexCount_);

	// read the vertices, indices, adn textures data
	this->LoadModelVertexData(fin, verticesArr);
	this->LoadModelTextureData(fin);
	this->LoadModelIndexData(fin, indicesArr);
	
	

	// initialize an internal model data structure
	//this->InitializeInternalModelDataType(ppModelData, ppIndicesData);

	// Close the model file
	fin.close();

	return true;
}


//
// GETTERS
//
UINT ModelLoader::GetVertexCount() const _NOEXCEPT
{
	return vertexCount_;
}

UINT ModelLoader::GetIndexCount() const _NOEXCEPT
{
	return indexCount_;
}






/////////////////////////////////////////////////////////////////////////////////////////
//
//                             PRIVATE FUNCTIONS
//
/////////////////////////////////////////////////////////////////////////////////////////


// read the vertices, indices, and textures count from a model data file
bool ModelLoader::LoadModelVITCount(ifstream & fin)
{
	//Log::Debug(THIS_FUNC_EMPTY);

	char input = ' ';

	// Read up to the value of vertex count
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin >> vertexCount_;  // Read in the vertex count


	// Read up to the value of index count
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin >> indexCount_;  // Read in the index count


	// Read up to the value of textures count
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin >> texturesCount_;  // Read in the textures count

	return true;
}


// read in the vertices data from the data file
bool ModelLoader::LoadModelVertexData(ifstream & fin, 
	std::vector<VERTEX> & verticesArr)
{
	char input = ' ';

	// Read up to the beginning of the vertices data
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin.ignore(1);


	// Read in the vertex data
	for (size_t i = 0; i < vertexCount_; i++)
	{
		fin >> verticesArr[i].position.x  
			>> verticesArr[i].position.y
			>> verticesArr[i].position.z;
	}

	return true;
}

///////////////////////////////////////////////////////////

bool ModelLoader::LoadModelIndexData(ifstream & fin, 
	std::vector<UINT> & indicesArr)
{
	// read in the indices data from the data file

	char input = ' ';

	try
	{
		pTextureIndicesData_ = new UINT[indexCount_]; // allocate the memory for the TEXTURE INDICES data
	}
	catch (std::bad_alloc & e)
	{
		_DELETE_ARR(pTextureIndicesData_);

		Log::Error(THIS_FUNC, e.what());
		COM_ERROR_IF_FALSE(false, "can't allocate memory for the vertices/textures index array");
	}
	
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



	// Read up to the TEXTURE indices data
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	// Read in the indices data (of textures)
	for (size_t i = 0; i < indexCount_; i++)
	{
		fin >> pTextureIndicesData_[i];
	}

	return true;
}

///////////////////////////////////////////////////////////


bool ModelLoader::LoadModelTextureData(ifstream & fin)
{
	// read in the textures data from the data file

	char input = ' ';

	try
	{
		// allocate the memory for the textures data
		pTexturesData_ = new DirectX::XMFLOAT2[texturesCount_];
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(THIS_FUNC, e.what());
		COM_ERROR_IF_FALSE(false, "can't allocate memory for the textures data array");
	}

	// Read up to the textures data
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	// Read in the indices data
	for (size_t i = 0; i < texturesCount_; i++)
	{
		fin >> pTexturesData_[i].x >> pTexturesData_[i].y;
	}


	return true;
}

///////////////////////////////////////////////////////////

bool ModelLoader::InitializeInternalModelDataType(VERTEX** ppModelData, UINT** ppIndicesData)
{
	// initialize an internal model data structure

	try
	{
	}
	catch (std::bad_alloc & e)
	{
		_DELETE_ARR(*ppModelData);
		_DELETE_ARR(*ppIndicesData);

		Log::Error(THIS_FUNC, e.what());
		COM_ERROR_IF_FALSE(false, "can't allocate memory for the vertices/indices array");
	}

	UINT vertexIndex = 0;
	UINT textureIndex = 0;

	// ----------------------------------------------------------------------- // 
	//             PREPARE DATA OF VERTICES AND INDICES                        //
	// ----------------------------------------------------------------------- //

	// Load up the vertex array with data
	for (size_t i = 0; i < indexCount_; i++)
	{
		if (vertexIndex > vertexCount_)
			continue;
		vertexIndex = ind[i];
		textureIndex = pTextureIndicesData_[i];

		

		(*ppModelData)[i].position = { pVerticesData_[vertexIndex].x, pVerticesData_[vertexIndex].y, pVerticesData_[vertexIndex].z };
		(*ppModelData)[i].texture = { pTexturesData_[textureIndex].x, pTexturesData_[textureIndex].y };

		//pVertices[i].normal   = { pModelType_[i].nx, pModelType_[i].ny, pModelType_[i].nz };
		//pVertices[i].tangent  = { pModelType_[i].tx, pModelType_[i].ty, pModelType_[i].tz };
		//pVertices[i].binormal = { pModelType_[i].bx, pModelType_[i].by, pModelType_[i].bz };
		//pVertices[i].color    = { pModelType_[i].cr, pModelType_[i].cg, pModelType_[i].cb, pModelType_[i].ca };
	}


	// Load up the index array with data
	for (UINT i = 0; i < indexCount_; i++)
	{
		(*ppIndicesData)[i] = i;
	}

	return true;
}
