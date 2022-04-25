#include "modelconverterclass.h"


ModelConverterClass::ModelType* ModelConverterClass::m_model = nullptr;

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
	int posBeforeVerticesData = 0;
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
			
	
	do(fin.getline(inputLine, 100))
	{
		fin.getline(inputLine, 100);
		cout << "input = " << inputLine << endl;
	} while (inputLine[1] != 't');

	// we need to know how many vertices we have in this model so go through it and count
	while (input != 't')
	{
		input = fin.get();
		if (input == '\n')
			verticesCount++;
	}

	cout << "VERTICES COUNT: " << verticesCount << endl;

	// return to the position before the vertices data 
	fin.seekg(posBeforeVerticesData);
	//std::cout << "after reading the number of vertices: " << fin.tellg() << std::endl;

	/*
	
	// allocate the memory for this count of vertices
	m_model = new(nothrow) ModelType[verticesCount];
	if (!m_model)
	{
		Log::Get()->Error(THIS_FUNC, "can't allocate the memory for vertices");
		return false;
	}

	// read in the vertices positions
	for (size_t i = 0; i < verticesCount; i++)
	{
		fin >> m_model[i].x >> m_model[i].y >> m_model[i].z >> input;
		cout.setf(ios::fixed, ios::floatfield);
		cout.precision(6);
		cout << "xyz: " << m_model[i].x << ' ' << m_model[i].y << ' ' << m_model[i].z << std::endl;
	}

	cout << "we've read the vertices data" << endl;
	cout << input << endl;

	// read up the 't' symbol 
	input = fin.get();
	cout << input << endl;

	*/

	// skip data until we get to the 'f' symbol
	while (input != 'f')
	{
		input = fin.get();
	}

	int vertexNum, textureNum, normalNum;
	input = fin.get();
	input = fin.get();

	input = fin.get();
	fin >> vertexNum >> input >> textureNum >> input >> normalNum;
	//index = atoi(&input);

	cout << "vtn = " << vertexNum << input
		<< textureNum << input
		<< normalNum << endl;


	fin >> index;
	fin >> index;
	fin >> index;
	cout << "index = " << index << endl;



	return true;
}