#include "modelconverterclass.h"

#define INPUT_LINE_SIZE 80

ModelConverterClass::ModelType* ModelConverterClass::m_model = nullptr;
ModelConverterClass::POINT3D* ModelConverterClass::m_point = nullptr;
ModelConverterClass::TEXCOORD* ModelConverterClass::m_tex = nullptr;
ModelConverterClass::NORMAL* ModelConverterClass::m_normal = nullptr;

// converts .obj file model data into the internal model format
bool ModelConverterClass::ConvertFromObjIntoModel(char* objFilename)
{
	std::ifstream fin(objFilename, std::ios::in | std::ios::binary);	// input data file (.obj)
	std::ofstream fout("cube2.txt", std::ios::out); // ouptput data file (.txt)

	

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

	ConvertFromObjIntoModelHelper(fin, fout);

	// close the .obj file and the output file
	fin.close();
	fout.close();

	return true;
}

bool ModelConverterClass::ConvertFromObjIntoModelHelper(ifstream& fin, ofstream& fout)
{
	char input = '\0';
	char* inputPtr = nullptr;
	std::string line;
	double x, y, z;
	int index;
	char inputLine[INPUT_LINE_SIZE];

	// Read up to the vertex values
	for (size_t i = 0; i < 4; i++)
	{
		fin.getline(inputLine, INPUT_LINE_SIZE);		// skip first four lines of the .obj file
											//std::cout << inputLine << std::endl;
	}

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
	cout << "VERTICES COUNT: " << verticesCount << endl;

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
		cout.setf(ios::fixed, ios::floatfield);
		cout.precision(6);
		cout << "xyz: " << m_point[i].x << ' ' << m_point[i].y << ' ' << m_point[i].z << std::endl;
	}

	cout << __FUNCTION__ << "():    we've read the vertices data\n\n\n" << endl;

	return true;
}

bool ModelConverterClass::ReadInTextureData(ifstream& fin)
{
	int texturePairsCount = 0;
	char input;
	char inputLine[INPUT_LINE_SIZE];
	int posBeforeTextureData = 0;

	fin.seekg(-1, ios::cur); // move at the position before the symbol "v"
	posBeforeTextureData = fin.tellg(); // later we'll return to this position to read in texture data


    // ----------- CALCULATE THE COUNT OF TEXTURE COORDINATES PAIRS ---------------- //
	fin.getline(inputLine, INPUT_LINE_SIZE);
	while (inputLine[1] != 'n')   // while we don't get to the  data of normals
	{
		texturePairsCount++;
		fin.getline(inputLine, INPUT_LINE_SIZE);
	}
	cout << "COUNT OF TEXTURE COORDINATES PAIRS: " << texturePairsCount << endl;

	// allocate the memory for this count of texture coordinates pairs
	m_tex = new(nothrow) TEXCOORD[texturePairsCount];

	// --------------------- READ IN TEXTURE COORDINATES DATA ---------------------- //
	fin.seekg(posBeforeTextureData); // return back to the position before texture coordinates data
	
	for (size_t i = 0; i < texturePairsCount; i++) //  reading in of each texture coordinates pair
	{
		fin.ignore(3); // ignore "vt " in the beginning of line
		fin >> m_tex[i].tu >> m_tex[i].tv >> input; // read in the texture coordinates pair and a new line symbol
		cout << "vt[" << i << "]: " << m_tex[i].tu << " | " << m_tex[i].tv << endl;
	}
	
	cout << __FUNCTION__ << "():    we've read the texture data\n\n\n" << endl;
	

	return true;
}

bool ModelConverterClass::ReadInNormalsData(ifstream& fin)
{
	int normalsCount = 0;
	char input;
	char inputLine[INPUT_LINE_SIZE];
	int posBeforeNormalsData = 0;

	fin.seekg(-1, ios::cur); // move at the position before the symbol "v"
	posBeforeNormalsData = fin.tellg(); // later we'll return to this position to read in normals data


	// ----------- CALCULATE THE COUNT OF NORMALS  ---------------- //
	fin.getline(inputLine, INPUT_LINE_SIZE);
	while (inputLine[0] == 'v')   // while we don't get to the end of normals data
	{
		normalsCount++;
		fin.getline(inputLine, INPUT_LINE_SIZE);
	}
	cout << "COUNT OF NORMALS: " << normalsCount << endl;

	// allocate the memory for this count of normals
	m_normal = new(nothrow) NORMAL[normalsCount];

	// --------------------- READ IN NORMALS DATA ---------------------- //
	fin.seekg(posBeforeNormalsData); // return back to the position before normals data

	for (size_t i = 0; i < normalsCount; i++) //  reading in of each normal
	{
		fin.ignore(3); // ignore "vn " in the beginning of line
		fin >> m_normal[i].nx >> m_normal[i].ny >> m_normal[i].nz >> input; // read in the normal data and a new line symbol
		cout << "vn[" << i << "]: " << m_normal[i].nx << " | " << m_normal[i].ny << " | " << m_normal[i].nz << endl;
	}

	cout << __FUNCTION__ << "():    we've read the normals data\n\n\n" << endl;

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
	fin.seekg(posBeforeFaceCommand - 20);	// now we at the position before the beginning of polygonal face data

	// define how many faces we have
	int facesCount = 0;
	fin.getline(inputLine, INPUT_LINE_SIZE);
	while (!fin.eof())
	{
		facesCount++;
		fin.getline(inputLine, INPUT_LINE_SIZE);
	};
	//cout << "FACES COUNT: " << facesCount << endl;

	// allocate the memory for such a count of faces
	m_model = new(nothrow) ModelType[facesCount * 3];

	int i = 0;
	fin.clear();
	fin.seekg(posBeforeFaceCommand - 20);

	while (!fin.eof() && i < facesCount)
	{
		FillInVerticesDataByIndex(i * 3, fin);
		i++;
	}


	return true;
}

bool ModelConverterClass::WriteIntoFileFacesData(ofstream& fout)
{


	return true;
}

bool ModelConverterClass::FillInVerticesDataByIndex(int index, ifstream& fin)
{
	int vertexNum = 0, textureNum = 0, normalNum = 0;

	fin.ignore(2); // ignore "f " (f symbol and space) in the beginning of line

	// read in three times sets of vertices(v)/texture coordinates(vt)/normal vectors(vn)
	// this data will make a single polygon 
	for(size_t i = index; i < index + 3; i++)
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
		m_model[index].x = m_point[vertexNum].x;
		m_model[index].y = m_point[vertexNum].y;
		m_model[index].z = m_point[vertexNum].z;

		// write in texture coordinates data
		m_model[index].tu = m_tex[textureNum].tu;
		m_model[index].tv = m_tex[textureNum].tv;

		// write in normals data
		m_model[index].nx = m_normal[normalNum].nx;
		m_model[index].ny = m_normal[normalNum].ny;
		m_model[index].nz = m_normal[normalNum].nz;

		// print out data about face by index
		//cout << "FACE [" << index << "][" << i << "] xyz: " << m_model[index].x << '|' << m_model[index].y << '|' << m_model[index].z << endl;
		//cout << "FACE [" << index << "][" << i << "] vt: " << m_model[index].tu << '|' << m_model[index].tv << endl;
		//cout << "FACE [" << index << "][" << i << "] vn: " << m_model[index].nx << '|' << m_model[index].ny << '|' << m_model[index].nz << endl;
	}

	return true;
}