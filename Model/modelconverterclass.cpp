#include "modelconverterclass.h"

// defines to print or not in the console messages about the convertation process 
#define PRINT_CONVERT_PROCESS_MESSAGES true   


ModelConverterClass::ModelConverterClass(void)
{
}

ModelConverterClass::ModelConverterClass(const ModelConverterClass& other) {}

ModelConverterClass::~ModelConverterClass(void)
{
	Shutdown();
}


// ----------------------------------------------------------------------------------- //
//
//                          PUBLIC METHODS
//
// ----------------------------------------------------------------------------------- //

void ModelConverterClass::Shutdown(void)
{
	_DELETE(pModelType_);
	_DELETE(pPoint3D_);
	_DELETE(pTexCoord_);
	_DELETE(pNormal_);
}


// converts .obj file model data into the internal model format
bool ModelConverterClass::ConvertFromObj(string objFilename)
{
	string fullFilename;
	GetFinalModelFilename(fullFilename, objFilename);

	std::ifstream fin(objFilename, std::ios::in | std::ios::binary);	// input data file (.obj)
	std::ofstream fout(fullFilename, std::ios::out); // ouptput data file (.txt)
	

	// If it could not open the file then exit
	if (fin.fail())
	{
		Log::Error(THIS_FUNC, "can't open such an .obj file");
		return false;
	}

	if (!fout)
	{
		Log::Error(THIS_FUNC, "can't open the output file");
		return false;
	}

	if (ConvertFromObjHelper(fin, fout)) // convert the model
	{
		Log::Debug(THIS_FUNC, "Model's data has been coverted successfully");
	}

	// close the .obj file and the output file
	fin.close();
	fout.close();

	return true;
}

// ----------------------------------------------------------------------------------- //
//
//                          PRIVATE METHODS / HELPERS
//
// ----------------------------------------------------------------------------------- //

// help us to convert .obj file model data into the internal model format
bool ModelConverterClass::ConvertFromObjHelper(ifstream& fin, ofstream& fout)
{
	char inputLine[INPUT_LINE_SIZE];

	// Read up to the vertex values
	for (size_t i = 0; i < 4; i++)
	{
		fin.getline(inputLine, INPUT_LINE_SIZE);    // skip first four lines of the .obj file								
	}

	cout << endl;

	ReadInVerticesData(fin); 
	ReadInTextureData(fin);
	ReadInNormalsData(fin);

	ReadInFacesData(fin);
	WriteIntoFileFacesData(fout);

	return true;
}

bool ModelConverterClass::ReadInVerticesData(ifstream& fin)
{
	char input;
	char inputLine[INPUT_LINE_SIZE];
	streampos posBeforeVerticesData;
	int verticesCount = 0;

	// ------- COUNT THE NUMBER OF VERTICES AND SAVE A POSITION BEFORE VERTICES DATA ----- //
	input = fin.get();		// now we are at the first 'v' symbol position
	posBeforeVerticesData = fin.tellg();	// later we'll return to this position so save it
											
	// we need to know how many vertices we have in this model so go through it and count
	fin.getline(inputLine, INPUT_LINE_SIZE);
	while (inputLine[1] != 't')
	{
		verticesCount++;
		fin.getline(inputLine, INPUT_LINE_SIZE);

	};
	cout << "VERTICES: " << verticesCount << endl;

	// return to the position before the vertices data 
	fin.seekg(posBeforeVerticesData);
	
	// allocate the memory for this count of vertices
	pPoint3D_ = new(nothrow) POINT3D[verticesCount];
	if (!pPoint3D_)
	{
		Log::Get()->Error(THIS_FUNC, "can't allocate the memory for vertices");
		return false;
	}

	// ---------------------- READ IN THE VERTICES POSITIONS ------------------------- //
	for (size_t i = 0; i < verticesCount; i++)
	{
		fin >> pPoint3D_[i].x >> pPoint3D_[i].y >> pPoint3D_[i].z >> input;
	}


	return true;
}

bool ModelConverterClass::ReadInTextureData(ifstream& fin)
{
	int texturePairsCount = 0;
	char input;
	char inputLine[INPUT_LINE_SIZE];
	size_t posBeforeTextureData = 0;

	fin.seekg(-1, ios::cur); // move at the position before the symbol "v"
	posBeforeTextureData = fin.tellg(); // later we'll return to this position to read in texture data


    // ----------- CALCULATE THE COUNT OF TEXTURE COORDINATES PAIRS ---------------- //
	fin.getline(inputLine, INPUT_LINE_SIZE);
	while (inputLine[1] != 'n')   // while we don't get to the  data of normals
	{
		texturePairsCount++;
		fin.getline(inputLine, INPUT_LINE_SIZE);
	}
	cout << "UVs:      " << texturePairsCount << endl;

	// allocate the memory for this count of texture coordinates pairs
	pTexCoord_ = new(nothrow) TEXCOORD[texturePairsCount];

	// --------------------- READ IN TEXTURE COORDINATES DATA ---------------------- //
	fin.seekg(posBeforeTextureData); // return back to the position before texture coordinates data
	
	for (size_t i = 0; i < texturePairsCount; i++) //  reading in of each texture coordinates pair
	{
		fin.ignore(3); // ignore "vt " in the beginning of line
		fin >> pTexCoord_[i].tu >> pTexCoord_[i].tv >> input; // read in the texture coordinates pair and a new line symbol
		//cout << "vt[" << i << "]: " << m_tex[i].tu << " | " << m_tex[i].tv << endl;
	}

	return true;
}

bool ModelConverterClass::ReadInNormalsData(ifstream& fin)
{
	size_t normalsCount = 0;
	char input;
	char inputLine[INPUT_LINE_SIZE];
	size_t posBeforeNormalsData = 0;

	fin.seekg(-1, ios::cur); // move at the position before the symbol "v"
	posBeforeNormalsData = fin.tellg(); // later we'll return to this position to read in normals data


	// ----------- CALCULATE THE COUNT OF NORMALS  ---------------- //
	fin.getline(inputLine, INPUT_LINE_SIZE);
	while (inputLine[0] == 'v')   // while we don't get to the end of normals data
	{
		normalsCount++;
		fin.getline(inputLine, INPUT_LINE_SIZE);
	}
	cout << "NORMALS:  " << normalsCount << endl;

	// allocate the memory for this count of normals
	pNormal_ = new(nothrow) NORMAL[normalsCount];

	// --------------------- READ IN NORMALS DATA ---------------------- //
	fin.seekg(posBeforeNormalsData); // return back to the position before normals data

	for (size_t i = 0; i < normalsCount; i++) //  reading in of each normal
	{
		fin.ignore(2); // ignore "vn " in the beginning of line
		fin >> pNormal_[i].nx >> pNormal_[i].ny >> pNormal_[i].nz >> input; // read in the normal data and a new line symbol
		//cout << "vn[" << i << "]: " << m_normal[i].nx << " | " << m_normal[i].ny << " | " << m_normal[i].nz << endl;
	}



	return true;
}

bool ModelConverterClass::ReadInFacesData(ifstream& fin)
{
	char inputLine[INPUT_LINE_SIZE];

	// skip data until we get to the 'f' symbol
	while (inputLine[0] != 'f')
	{
		fin.getline(inputLine, INPUT_LINE_SIZE);
	};

	size_t posBeforeFaceCommand = fin.tellg();
	posBeforeFaceCommand -= strlen(inputLine) + 1; // come back at the beginning of lige (size of the string + null character)
	fin.seekg(posBeforeFaceCommand);	// now we at the position before the beginning of polygonal face data

	// define how many faces we have
	facesCount_ = 0;
	fin.getline(inputLine, INPUT_LINE_SIZE);
	while (!fin.eof())
	{
		facesCount_++;
		fin.getline(inputLine, INPUT_LINE_SIZE);
	};
	cout << "FACES COUNT: " << facesCount_ << endl;

	// allocate the memory for such a count of faces
	pModelType_ = new(nothrow) ModelType[facesCount_ * 3];

	int i = 0;
	fin.clear();
	fin.seekg(posBeforeFaceCommand);

	while (!fin.eof() && i < facesCount_)
	{
		FillInVerticesDataByIndex(i * 3, fin);
		i++;
	}


	return true;
}

// filling in the output text file with face data
// later we use this output file to render a model
bool ModelConverterClass::WriteIntoFileFacesData(ofstream& fout)
{
	fout << "Vertex Count: " << facesCount_ * 3 << "\n\n"; // to build a face we need 3 vertices
	fout << "Data:" << "\n\n";

	for (size_t i = 0; i < facesCount_ * 3; i++)
	{
		fout.setf(ios::fixed, ios::floatfield);
		fout.precision(6);

		fout << pModelType_[i].x << " "        // print a vertex coordinates
			 << pModelType_[i].y << " "
			 << pModelType_[i].z << " "
			
			 << pModelType_[i].tu << " "        // print into the file texture coordinates
			 << pModelType_[i].tv << " "   
			 
			 << setprecision(4)
			 << pModelType_[i].nx << " "        // print a normal vector data
			 << pModelType_[i].ny << " "
			 << pModelType_[i].nz << " ";

		if (i < facesCount_ * 3 - 1)
			fout << "\n";
			 
	}

	return true;
}

bool ModelConverterClass::FillInVerticesDataByIndex(int index, ifstream& fin)
{
	int vertexNum = 0, textureNum = 0, normalNum = 0;

	fin.ignore(2); // ignore "f " (f symbol and space) in the beginning of line

	// read in three times sets of vertices(v)/texture coordinates(vt)/normal vectors(vn)
	// this data will make a single polygon 
	for(int i = index + 2; i >= index; i--)
	{
		fin >> vertexNum;
		fin.ignore();  // ignore "/"
		fin >> textureNum;
		fin.ignore();  // ignore "/"
		fin >> normalNum;
		fin.get();     // read up the space (or '\n') after each set of v/vt/vn
		//cout << "vtn = " << vertexNum << "_" << textureNum << "_" << normalNum << endl;

		// change these values for correct getting of data
		vertexNum--;
		textureNum--;
		normalNum--;

		// write in vertices data
		pModelType_[i].x = pPoint3D_[vertexNum].x;
		pModelType_[i].y = pPoint3D_[vertexNum].y;
		pModelType_[i].z = pPoint3D_[vertexNum].z * -1.0f; // invert the value to use it in the left handed coordinate system

		// write in texture coordinates data
		pModelType_[i].tu = pTexCoord_[textureNum].tu;
		pModelType_[i].tv = 1.0f - pTexCoord_[textureNum].tv; // invert the value to use it in the left handed coordinate system

		// write in normals data
		pModelType_[i].nx = pNormal_[normalNum].nx;
		pModelType_[i].ny = pNormal_[normalNum].ny;
		pModelType_[i].nz = pNormal_[normalNum].nz * -1.0f; // invert the value to use it in the left handed coordinate system
		if (pModelType_[i].nz == 0.0f)
			pModelType_[i].nz = 0.0f;

		// print out data about face by index
		//cout << "FACE [" << index << "][" << i << "] xyz: " << m_model[i].x << '|' << m_model[i].y << '|' << m_model[i].z << "   (" << vertexNum << ")" << endl;
		//cout << "FACE [" << index << "][" << i << "] vt:  " << m_model[i].tu << '|' << m_model[i].tv << endl;
		//cout << "FACE [" << index << "][" << i << "] vn:  " << m_model[i].nx << '|' << m_model[i].ny << '|' << m_model[i].nz << endl;
	}

	

	return true;
}

// makes a final name for the file where we'll place model data
bool ModelConverterClass::GetFinalModelFilename(string& fullFilename, string& rawFilename)
{
	size_t pointPos = rawFilename.rfind('.');
	fullFilename = rawFilename.substr(0, pointPos) + MODEL_FILE_TYPE;	
	return true;
}