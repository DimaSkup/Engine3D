#include "modelconverterclass.h"


ModelConverterClass::ModelType* ModelConverterClass::m_model = nullptr;
ModelConverterClass::POINT3D* ModelConverterClass::m_point = nullptr;

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

	ConvertFromObjIntoModelHelper(fin);

	// close the .obj file
	fin.close();

	return true;
}

bool ModelConverterClass::ConvertFromObjIntoModelHelper(ifstream& fin)
{
	char input = '\0';
	char* inputPtr = nullptr;
	std::string line;
	double x, y, z;
	int verticesCount = 0;
	streampos posBeforeVerticesData;
	int index;
	char inputLine[80];
	wchar_t inputWLine[80];

	// Read up to the vertex values
	for (size_t i = 0; i < 4; i++)
	{
		fin.getline(inputLine, 100);		// skip first four lines of the .obj file
											//std::cout << inputLine << std::endl;
	}


	// ------- COUNT THE NUMBER OF VERTICES AND SAVE A POSITION BEFORE VERTICES DATA ----- //

	input = fin.get();		// now we are at the first 'v' symbol position
	posBeforeVerticesData = fin.tellg();	// later we'll return to this position so save it
	//std::cout << "before vertices data: " << posBeforeVerticesData << " symbol = " << input << std::endl; // 'v' pos

	// we need to know how many vertices we have in this model so go through it and count
	fin.getline(inputLine, ARRAYSIZE(inputLine));
	while (inputLine[1] != 't')
	{
		verticesCount++;
		fin.getline(inputLine, ARRAYSIZE(inputLine));
		cout << "input = " << inputLine << endl;
		
	};
	cout << "VERTICES COUNT: " << verticesCount << endl;



	// return to the position before the vertices data 
	fin.seekg(posBeforeVerticesData);
	//std::cout << "after reading the number of vertices: " << fin.tellg() << std::endl;
	
	
	// allocate the memory for this count of vertices
	m_point = new(nothrow) POINT3D[verticesCount];
	if (!m_point)
	{
		Log::Get()->Error(THIS_FUNC, "can't allocate the memory for vertices");
		return false;
	}

	// read in the vertices positions
	for (size_t i = 0; i < verticesCount; i++)
	{
		fin >> m_point[i].x >> m_point[i].y >> m_point[i].z >> input;
		cout.setf(ios::fixed, ios::floatfield);
		cout.precision(6);
		cout << "xyz: " << m_point[i].x << ' ' << m_point[i].y << ' ' << m_point[i].z << std::endl;
	}

	cout << "we've read the vertices data" << endl;
	cout << input << endl;

	// read up the 't' symbol 
	input = fin.get();
	cout << input << endl;


	
	// skip data until we get to the 'f' symbol
	while (inputLine[0] != 'f')
	{
		fin.getline(inputLine, ARRAYSIZE(inputLine));
	};
	int posBeforeFaceCommand = fin.tellg();
	cout << "position f: " << posBeforeFaceCommand << endl;
	fin.seekg(posBeforeFaceCommand - 20);	// now we at the position before the beginning of polygonal face data
	//posBeforeFaceCommand = fin.tellg();
	cout << "position f: " << posBeforeFaceCommand << endl;
	//cout << "f line: " << inputLine << endl;
	

	// define how many faces we have
	cout << "KOKOS == " << inputLine << endl;
	int facesCount = 0;
	fin.getline(inputLine, ARRAYSIZE(inputLine));
	while (!fin.eof())
	{
		facesCount++;
		fin.getline(inputLine, ARRAYSIZE(inputLine));
		cout << "face[" << facesCount << "]: " << inputLine << endl;

	};
	cout << "facesCount: " << facesCount << endl;

	// allocate the memory for such a count of faces
	m_model = new(nothrow) ModelType[facesCount * 3];

	
	
	int i = 0;
	/*
	input = fin.get();
	cout << "f: " << input << endl;
	input = fin.get();
	cout << "space: " << input << endl;
	*/
	fin.clear();
	fin.seekg(posBeforeFaceCommand - 20);
	int vertexNum = 0, textureNum = 0, normalNum = 0;
	//input = fin.get();
	//cout << "input = " << input << endl;
	while (!fin.eof() && i < 1)
	{
		
		FillInFaceDataByIndex(i, fin);


		

/*

fin >> vertexNum;
fin.ignore();  // ignore "/"
fin >> textureNum;
fin.ignore();  // ignore "/"
fin >> normalNum;
m_model[vertexNum].x = m_point[vertexNum].x;
m_model[vertexNum].y = m_point[vertexNum].y;
m_model[vertexNum].z = m_point[vertexNum].z;
cout << '[' << i << "]: " << vertexNum << '-' << textureNum << '-' << normalNum << endl;
input = fin.get();

fin >> vertexNum;
fin.ignore();  // ignore "/"
fin >> textureNum;
fin.ignore();  // ignore "/"
fin >> normalNum;
cout << '[' << i << "]: " << vertexNum << '-' << textureNum << '-' << normalNum << endl;
//fin.get();
*/
		i++;
	}


	return true;
}

bool ModelConverterClass::FillInFaceDataByIndex(int index, ifstream& fin)
{
	int vertexNum = 0, textureNum = 0, normalNum = 0;

	fin.ignore(2); // ignore "f "
	for(size_t i = index; i < index + 3; i++)
	{
		

		fin >> vertexNum;
		fin.ignore();  // ignore "/"
		fin >> textureNum;
		fin.ignore();  // ignore "/"
		fin >> normalNum;
		fin.get();
		cout << "vtn = " << vertexNum << "_" << textureNum << "_" << normalNum << endl;
		m_model[index].x = m_point[vertexNum].x;
		m_model[index].y = m_point[vertexNum].y;
		m_model[index].z = m_point[vertexNum].z;
		cout << '[' << index << "]: " << m_point[vertexNum].x << '-' << m_point[vertexNum].y << '-' << m_point[vertexNum].z << endl;
		
	}

	return true;
}