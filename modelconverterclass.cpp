#include "modelconverterclass.h"

// defines to print or not in the console messages about the convertation process 
#define PRINT_CONVERT_PROCESS_MESSAGES true   


ModelConverterClass::ModelConverterClass(void)
{
	m_model = nullptr;
	m_point = nullptr;
	m_tex = nullptr;
	m_normal = nullptr;
}

ModelConverterClass::ModelConverterClass(const ModelConverterClass& other)
{
}

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
	_DELETE(m_model);
	_DELETE(m_point);
	_DELETE(m_tex);
	_DELETE(m_normal);
}


// converts .obj file model data into the internal model format
bool ModelConverterClass::ConvertFromObjIntoModel(string objFilename)
{
	string fullFilename;
	GetFinalModelFilename(fullFilename, objFilename);

	std::ifstream fin(objFilename, std::ios::in | std::ios::binary);	// input data file (.obj)
	std::ofstream fout(fullFilename, std::ios::out); // ouptput data file (.txt)
	

	// If it could not open the file then exit
	if (fin.fail())
	{
		Log::Get()->Error(THIS_FUNC, "can't open such an .obj file");
		return false;
	}

	if (!fout)
	{
		Log::Get()->Error(THIS_FUNC, "can't open the output file");
		return false;
	}

	if (ConvertFromObjIntoModelHelper(fin, fout)) // convert the model
	{
		// Notify the use the model has been converted
		cout << "File has been coverted successfully\n\n" << endl;
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
bool ModelConverterClass::ConvertFromObjIntoModelHelper(ifstream& fin, ofstream& fout)
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
	m_point = new(nothrow) POINT3D[verticesCount];
	if (!m_point)
	{
		Log::Get()->Error(THIS_FUNC, "can't allocate the memory for vertices");
		return false;
	}

	// ---------------------- READ IN THE VERTICES POSITIONS ------------------------- //
	for (size_t i = 0; i < verticesCount; i++)
	{
		fin >> m_point[i].x >> m_point[i].y >> m_point[i].z >> input;
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
	m_tex = new(nothrow) TEXCOORD[texturePairsCount];

	// --------------------- READ IN TEXTURE COORDINATES DATA ---------------------- //
	fin.seekg(posBeforeTextureData); // return back to the position before texture coordinates data
	
	for (size_t i = 0; i < texturePairsCount; i++) //  reading in of each texture coordinates pair
	{
		fin.ignore(3); // ignore "vt " in the beginning of line
		fin >> m_tex[i].tu >> m_tex[i].tv >> input; // read in the texture coordinates pair and a new line symbol
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
	m_normal = new(nothrow) NORMAL[normalsCount];

	// --------------------- READ IN NORMALS DATA ---------------------- //
	fin.seekg(posBeforeNormalsData); // return back to the position before normals data

	for (size_t i = 0; i < normalsCount; i++) //  reading in of each normal
	{
		fin.ignore(2); // ignore "vn " in the beginning of line
		fin >> m_normal[i].nx >> m_normal[i].ny >> m_normal[i].nz >> input; // read in the normal data and a new line symbol
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

	int posBeforeFaceCommand = fin.tellg();
	posBeforeFaceCommand -= strlen(inputLine) + 1; // come back at the beginning of lige (size of the string + null character)
	fin.seekg(posBeforeFaceCommand);	// now we at the position before the beginning of polygonal face data

	// define how many faces we have
	facesCount = 0;
	fin.getline(inputLine, INPUT_LINE_SIZE);
	while (!fin.eof())
	{
		facesCount++;
		fin.getline(inputLine, INPUT_LINE_SIZE);
	};
	cout << "FACES COUNT: " << facesCount << endl;

	// allocate the memory for such a count of faces
	m_model = new(nothrow) ModelType[facesCount * 3];

	int i = 0;
	fin.clear();
	fin.seekg(posBeforeFaceCommand);

	while (!fin.eof() && i < facesCount)
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
	fout << "Vertex Count: " << facesCount * 3 << "\n\n"; // to build a face we need 3 vertices
	fout << "Data:" << "\n\n";

	for (size_t i = 0; i < facesCount * 3; i++)
	{
		fout.setf(ios::fixed, ios::floatfield);
		fout.precision(6);

		fout << m_model[i].x << " "        // print a vertex coordinates
			 << m_model[i].y << " "
			 << m_model[i].z << " "
			
			 << m_model[i].tu << " "        // print into the file texture coordinates
			 << m_model[i].tv << " "   
			 
			 << setprecision(4)
			 << m_model[i].nx << " "        // print a normal vector data
			 << m_model[i].ny << " "
			 << m_model[i].nz << " ";

		if (i < facesCount * 3 - 1)
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
		m_model[i].x = m_point[vertexNum].x;
		m_model[i].y = m_point[vertexNum].y;
		m_model[i].z = m_point[vertexNum].z * -1.0f; // invert the value to use it in the left handed coordinate system

		// write in texture coordinates data
		m_model[i].tu = m_tex[textureNum].tu;
		m_model[i].tv = 1.0f - m_tex[textureNum].tv; // invert the value to use it in the left handed coordinate system

		// write in normals data
		m_model[i].nx = m_normal[normalNum].nx;
		m_model[i].ny = m_normal[normalNum].ny;
		m_model[i].nz = m_normal[normalNum].nz * -1.0f; // invert the value to use it in the left handed coordinate system
		if (m_model[i].nz == 0.0f)
			m_model[i].nz = 0.0f;

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