/////////////////////////////////////////////////////////////////////
// Filename:      ModelLoader.cpp
// Description:   contains a functional for loading model data (internal model type)
//                from a data file into a model object
//
// Created:       27.02.23
/////////////////////////////////////////////////////////////////////
#include "ModelLoader.h"



ModelLoader::ModelLoader()
{
}


ModelLoader::~ModelLoader()
{
	_DELETE(pVertexIndicesData_);
	_DELETE(pTextureIndicesData_);

	_DELETE(pVerticesData_);
	_DELETE(pTexturesData_);
}



/////////////////////////////////////////////////////////////////////////////////////////
//
//                               PUBLIC FUNCTIONS
// 
/////////////////////////////////////////////////////////////////////////////////////////


// load model data from a data file
bool ModelLoader::Load(std::string modelName, VERTEX** ppModelData, UINT** ppIndicesData)
{
	std::string modelFilename = { SETTINGS::GetSettings()->MODEL_DIR_PATH + modelName + ".txt" }; // prepare the path to a model data file
	std::ifstream fin(modelFilename, std::ios::in);

	// If it could not open the file then exit
	if (fin.fail())
	{
		std::string errorMsg = "can't open the text file \"" + modelFilename + "\" with model data";
		COM_ERROR_IF_FALSE(false, errorMsg);
	}

	// read the vertices, indices, and textures count
	this->LoadModelVITCount(fin);

	// read the vertices, indices, adn textures data
	this->LoadModelIndexData(fin);
	this->LoadModelVertexData(fin);
	this->LoadModelTextureData(fin);

	// initialize an internal model data structure
	this->InitializeInternalModelDataType(ppModelData, ppIndicesData);

	// Close the model file
	fin.close();

	return true;
}


UINT ModelLoader::GetIndexCount() const
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


	// print debug data: the number of vertices, indices, and texture coords
	if (ModelLoader::PRINT_DEBUG_DATA_)
	{
		Log::Debug("VERTEX COUNT: %d", vertexCount_);
		Log::Debug("INDEX COUNT:  %d", indexCount_);
		Log::Debug("TEXTURE COUNT: %d", texturesCount_);
	}

	return true;
}


// read in the vertices data from the data file
bool ModelLoader::LoadModelVertexData(ifstream & fin)
{
	char input = ' ';

	// Create the model using the vertex count that was read in
	try
	{
		pVerticesData_ = new DirectX::XMFLOAT3[vertexCount_];
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(THIS_FUNC, e.what());
		COM_ERROR_IF_FALSE(false, "can't create the model using the vertex count");
	}
	

	// Read up to the beginning of the vertices data
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin.ignore(2);


	// Read in the vertex data
	for (size_t i = 0; i < vertexCount_; i++)
	{
		fin >> pVerticesData_[i].x >> pVerticesData_[i].y >> pVerticesData_[i].z;
	}


	// print the debug vertices data
	if (ModelLoader::PRINT_DEBUG_DATA_)
	{
		Log::Debug("VERTEX DATA: ");
		for (size_t i = 0; i < vertexCount_; i++)
		{
			cout << setiosflags(ios::fixed | ios::showpoint);
			cout << setprecision(4);
			cout << setw(2) << " ";
			cout << setw(2) << pVerticesData_[i].x << ' '
				 << setw(2) << pVerticesData_[i].y << ' '
				 << setw(2) << pVerticesData_[i].z;
			cout << endl;
		}
	}

	return true;
}


// read in the indices data from the data file
bool ModelLoader::LoadModelIndexData(ifstream & fin)
{
	char input = ' ';

	try
	{
		pVertexIndicesData_ = new size_t[indexCount_];  // allocate the memory for the VERTEX INDICES data
		pTextureIndicesData_ = new size_t[indexCount_]; // allocate the memory for the TEXTURE INDICES data
	}
	catch (std::bad_alloc & e)
	{
		_DELETE_ARR(pVertexIndicesData_);
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

	// Read in the VERTEX indices data
	for (size_t i = 0; i < indexCount_; i++)
	{
		fin >> pVertexIndicesData_[i];
	}


	// Read up to the TEXTURE indices data
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	// Read in the TEXTURE indices data
	for (size_t i = 0; i < indexCount_; i++)
	{
		fin >> pTextureIndicesData_[i];
	}


	if (ModelLoader::PRINT_DEBUG_DATA_)
	{
		Log::Debug(THIS_FUNC, "VERTEX INDICES DATA:");
		for (size_t i = 0; i < indexCount_; i++)
		{
			cout << pVertexIndicesData_[i] << ' ';
		}
		cout << endl;

		Log::Debug(THIS_FUNC, "TEXTURE INDICES DATA:");
		for (size_t i = 0; i < indexCount_; i++)
		{
			cout << pTextureIndicesData_[i] << ' ';
		}
		cout << endl;
	}


	return true;
}


// read in the textures data from the data file
bool ModelLoader::LoadModelTextureData(ifstream & fin)
{
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
	

	// print textures debug data
	if (ModelLoader::PRINT_DEBUG_DATA_)
	{
		Log::Debug(THIS_FUNC, "TEXTURES DATA FROM FILE:");
		for (size_t i = 0; i < texturesCount_; i++)
		{
			cout << pTexturesData_[i].x << ' ' << pTexturesData_[i].y << endl;
		}
		cout << endl;
	}

	return true;
}


// initialize an internal model data structure
bool ModelLoader::InitializeInternalModelDataType(VERTEX** ppModelData, UINT** ppIndicesData)
{
	try
	{
		*ppModelData = new VERTEX[indexCount_];
		*ppIndicesData = new UINT[indexCount_];
	}
	catch (std::bad_alloc & e)
	{
		_DELETE_ARR(*ppModelData);
		_DELETE_ARR(*ppIndicesData);

		Log::Error(THIS_FUNC, e.what());
		COM_ERROR_IF_FALSE(false, "can't allocate memory for the vertices/indices array");
	}

	size_t vertexIndex = 0;
	size_t textureIndex = 0;

	// ----------------------------------------------------------------------- // 
	//             PREPARE DATA OF VERTICES AND INDICES                        //
	// ----------------------------------------------------------------------- //

	// Load up the vertex array with data
	for (size_t i = 0; i < indexCount_; i++)
	{
		vertexIndex = pVertexIndicesData_[i];
		textureIndex = pTextureIndicesData_[i];

		(*ppModelData)[i].position = { pVerticesData_[vertexIndex].x, pVerticesData_[vertexIndex].y, pVerticesData_[vertexIndex].z };
		(*ppModelData)[i].texture = { pTexturesData_[textureIndex].x, pTexturesData_[textureIndex].y };

		//pVertices[i].normal   = { pModelType_[i].nx, pModelType_[i].ny, pModelType_[i].nz };
		//pVertices[i].tangent  = { pModelType_[i].tx, pModelType_[i].ty, pModelType_[i].tz };
		//pVertices[i].binormal = { pModelType_[i].bx, pModelType_[i].by, pModelType_[i].bz };
		//pVertices[i].color    = { pModelType_[i].cr, pModelType_[i].cg, pModelType_[i].cb, pModelType_[i].ca };
	}


	// Load up the index array with data
	for (size_t i = 0; i < indexCount_; i++)
	{
		(*ppIndicesData)[i] = static_cast<UINT>(i);
	}


	// print data structure for debugging
	if (ModelLoader::PRINT_DEBUG_DATA_)
	{
		Log::Error(THIS_FUNC, "FINAL INTERNAL MODEL DATA STRUCTURE");
		for (size_t i = 0; i < indexCount_; i++)
		{
			cout.setf(ios::fixed | ios::showpoint);
			cout << '\t';
			cout << '[' << i << "]: ";
			cout << setprecision(4);
			cout << setw(6) << (*ppModelData)[i].position.x << ' '
				<< setw(6) << (*ppModelData)[i].position.y << ' '
				<< setw(6) << (*ppModelData)[i].position.z << '\t'
				<< setw(6) << (*ppModelData)[i].texture.x << ' '
				<< setw(6) << (*ppModelData)[i].texture.y << endl;
		}
		cout << endl << endl;

		Log::Error(THIS_FUNC, "INDICES: ");
		cout << '\t';
		for (size_t i = 0; i < indexCount_; i++)
			cout << (*ppIndicesData)[i] << ' ';
		cout << endl << endl << endl;
	}

	return true;
}
