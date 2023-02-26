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






bool ModelLoader::Load(std::string modelName, VERTEX** ppModelData, UINT** ppIndicesData)
{
	//Log::Debug(THIS_FUNC_EMPTY);

	std::string modelFilename = { ModelConverterClass::Get()->GetPathToModelDir() + modelName + ".txt" }; // prepare the path to a model data file
	std::ifstream fin(modelFilename, std::ios::in);
	char input = ' ';

	// If it could not open the file then exit
	if (fin.fail())
	{
		std::string errorMsg = "can't open the text file \"" + modelFilename + "\" with model data";
		Log::Error(THIS_FUNC, errorMsg.c_str());
		return false;
	}

	// read the vertices, indices, and textures count
	this->LoadModelVITCount(fin);

	this->LoadModelIndexData(fin);
	this->LoadModelVertexData(fin);
	this->LoadModelTextureData(fin);

	this->InitializeInternalModelDataType(ppModelData, ppIndicesData);



	// Close the model file
	fin.close();

	return true;
}


size_t ModelLoader::GetIndexCount() const
{
	return indexCount_;
}




/////////////////////////////////////////////////////////////////////////////////////////



bool ModelLoader::LoadModelVITCount(ifstream & fin)
{
	Log::Debug(THIS_FUNC_EMPTY);

	char input = ' ';

	// Read up to the value of vertex count
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	// Read in the vertex count
	fin >> vertexCount_;



	// Read up to the value of index count
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	// Read in the index count
	fin >> indexCount_;


	// Read up to the value of textures count
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	// Read in the textures count
	fin >> texturesCount_;


	// print debug data: the number of vertices, indices, and texture coords
	if (ModelLoader::PRINT_DEBUG_DATA_)
	{
		Log::Debug("VERTEX COUNT: %d", vertexCount_);
		Log::Debug("INDEX COUNT:  %d", indexCount_);
		Log::Debug("TEXTURE COUNT: %d", texturesCount_);
	}

	return true;
}


bool ModelLoader::LoadModelVertexData(ifstream & fin)
{
	char input = ' ';

	// Create the model using the vertex count that was read in
	pVerticesData_ = new DirectX::XMFLOAT3[vertexCount_];
	COM_ERROR_IF_FALSE(pVerticesData_, "can't create the model using the vertex count");

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



bool ModelLoader::LoadModelIndexData(ifstream & fin)
{
	char input = ' ';
	pVertexIndicesData_ = new size_t[indexCount_];  // allocate the memory for the VERTEX INDICES data
	pTextureIndicesData_ = new size_t[indexCount_]; // allocate the memory for the TEXTURE INDICES data

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


bool ModelLoader::LoadModelTextureData(ifstream & fin)
{
	char input = ' ';

	// Read up to the textures data
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}


	// allocate the memory for the textures data
	pTexturesData_ = new DirectX::XMFLOAT2[texturesCount_];

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


bool ModelLoader::InitializeInternalModelDataType(VERTEX** ppModelData, UINT** ppIndicesData)
{
	*ppModelData = new VERTEX[indexCount_];
	*ppIndicesData = new UINT[indexCount_];

	//std::unique_ptr<VERTEX[]> pVertices = std::make_unique<VERTEX[]>(vertexCount_);
	//std::unique_ptr<UINT[]>  pIndices = std::make_unique<UINT[]>(indexCount_);
	size_t vertexIndex = 0;
	size_t textureIndex = 0;

	// ----------------------------------------------------------------------- // 
	//             PREPARE DATA OF VERTICES AND INDICES                        //
	// ----------------------------------------------------------------------- //

	// Load the vertex array and index array with data
	for (size_t i = 0; i < indexCount_; i++)
	{
		vertexIndex = pVertexIndicesData_[i];
		textureIndex = pTextureIndicesData_[i];

		(*ppModelData)[i].position = { pVerticesData_[vertexIndex].x, pVerticesData_[vertexIndex].y, pVerticesData_[vertexIndex].z };
		(*ppModelData)[i].texture = { pTexturesData_[textureIndex].x, pTexturesData_[textureIndex].y };
		//pVertices[i].texture  = { pModelType_[i].tu, pModelType_[i].tv };
		//pVertices[i].normal   = { pModelType_[i].nx, pModelType_[i].ny, pModelType_[i].nz };
		//pVertices[i].tangent  = { pModelType_[i].tx, pModelType_[i].ty, pModelType_[i].tz };
		//pVertices[i].binormal = { pModelType_[i].bx, pModelType_[i].by, pModelType_[i].bz };
		//pVertices[i].color    = { pModelType_[i].cr, pModelType_[i].cg, pModelType_[i].cb, pModelType_[i].ca };


	}


	for (size_t i = 0; i < indexCount_; i++)
	{
		//vertexIndex = pVertexIndicesData_[i];

		(*ppIndicesData)[i] = static_cast<UINT>(i);
	}
	cout << endl;




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
