#include "modelconverterclass.h"


ModelConverterClass* ModelConverterClass::pInstance_ = nullptr;

ModelConverterClass::ModelConverterClass(void)
{
	if (!pInstance_)  // if there is no model converter yet
	{
		inputLine_ = new char[ModelConverterClass::INPUT_LINE_SIZE_];   // during execution of the getline() function we put here a one single text line
		
		pInstance_ = this;
	}
	else
	{
		COM_ERROR_IF_FALSE(false, "you can't have more than only one model converter instance!");
	}
	
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
	_DELETE(pModelType_);
	_DELETE(pPoint3D_);
	_DELETE(pTexCoord_);
	_DELETE(pNormal_);
}


ModelConverterClass* ModelConverterClass::Get()
{
	return pInstance_;
}


const std::string & ModelConverterClass::GetPathToModelDir() const
{
	return ModelConverterClass::MODEL_DIR_PATH_;
}


// converts a model of the ".obj" type into the internal model format
bool ModelConverterClass::ConvertFromObj(string objFilename)
{
	bool result = false;
	std::string inputModelFileFormat = ".obj";
	string outputModelFilename{ "" };
	string debugMsg{ "FILES:" };

	objFilename += inputModelFileFormat;

	GetOutputModelFilename(outputModelFilename, objFilename);

	// setup the debug message
	debugMsg += "\n\t\t\t\t";
	debugMsg += "in: " + objFilename;
	debugMsg += ";  ";
	debugMsg += "\n\t\t\t\t";
	debugMsg += "out: " + outputModelFilename;

	Log::Debug(THIS_FUNC, debugMsg.c_str());

	std::ifstream fin(objFilename, std::ios::in | std::ios::binary);	// input data file (.obj)
	std::ofstream fout(outputModelFilename, std::ios::out); // ouptput data file (.txt)
	

	// If it could not open the input file then exit
	result = fin.fail();
	COM_ERROR_IF_FALSE(!result, "can't open \"" + objFilename + "\" file");
	
	// if it could not open the output file then exit
	COM_ERROR_IF_FALSE(fout, "can't open the output file");

	// convert the model
	result = ConvertFromObjHelper(fin, fout); 
	COM_ERROR_IF_FALSE(result, "Model's data has been converted successfully");

	// close the .obj input file and the output file
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
	//constexpr int lineSize = 80;
	//char inputLine[lineSize];
	//char* inputLine = new char[INPUT_LINE_SIZE_];
	// Read up to the vertex values
	for (size_t i = 0; i < 4; i++)
	{
		fin.getline(inputLine_, INPUT_LINE_SIZE_, '\n');    // skip first four lines of the .obj file								
	}

	
	Log::Debug(THIS_FUNC, "START of the convertation process");
	
	ReadInVerticesData(fin); 
	ReadInTextureData(fin);
	ReadInNormalsData(fin);

	ReadInFacesData(fin);
	WriteIntoFileFacesData(fout); // write model data in an internal model format into the output data file
	ResetConverterState();        // after each convertation we MUST reset the state of the converter for proper later convertations

	Log::Print(THIS_FUNC, "Convertation is finished successfully!");

	return true;
}

bool ModelConverterClass::ReadInVerticesData(ifstream& fin)
{
	//Log::Debug(THIS_FUNC_EMPTY);

	char input;
	//constexpr int lineSize = 80;
	//char inputLine[lineSize];
	streampos posBeforeVerticesData;
	std::string firstReadingDataLine{ "" };
	std::string lastReadingDataLine{ "" };

	// ------- COUNT THE NUMBER OF VERTICES AND SAVE A POSITION BEFORE VERTICES DATA ----- //
	input = fin.get();		// now we are at the first 'v' symbol position
	posBeforeVerticesData = fin.tellg();	// later we'll return to this position so save it
											
	// we need to know how many vertices we have in this model so go through it and count
	fin.getline(inputLine_, INPUT_LINE_SIZE_);
	firstReadingDataLine = inputLine_;

	// calculate the number of vertices
	while (inputLine_[1] != 't')
	{
		verticesCount_++;
		fin.getline(inputLine_, INPUT_LINE_SIZE_);
	};

	lastReadingDataLine = inputLine_;

	
	this->PrintDebugData("vertices", verticesCount_, firstReadingDataLine, lastReadingDataLine);
	

	// return to the position before the vertices data 
	fin.seekg(posBeforeVerticesData);
	
	// allocate the memory for this count of vertices
	pPoint3D_ = new(std::nothrow) POINT3D[verticesCount_];
	COM_ERROR_IF_FALSE(pPoint3D_, "can't allocate the memory for vertices");

	// read in the vertices positions
	for (size_t i = 0; i < verticesCount_; i++)
	{
		fin >> pPoint3D_[i].x >> pPoint3D_[i].y >> pPoint3D_[i].z >> input;  // read in x, y, z vertex coordinates and the '\n' symbol
	}
	//inputLine = { '\0' };

	return true;
}

bool ModelConverterClass::ReadInTextureData(ifstream& fin)
{
	//constexpr int lineSize = 80;
	char input;
	//char inputLine[lineSize];
	size_t posBeforeTextureData = 0;
	std::string firstReadingDataLine{ "" };
	std::string lastReadingDataLine{ "" };

	fin.seekg(-1, ios::cur); // move at the position before the symbol "v"
	posBeforeTextureData = fin.tellg(); // later we'll return to this position to read in texture data


    // ----------- CALCULATE THE COUNT OF TEXTURE COORDINATES PAIRS ---------------- //

	fin.getline(inputLine_, INPUT_LINE_SIZE_, '\n');
	firstReadingDataLine = inputLine_;

	while (inputLine_[1] != 'n')   // while we don't get to the  data of normals
	{
		textureCoordsCount_++;
		fin.getline(inputLine_, INPUT_LINE_SIZE_, '\n');
		//fin.getline(inputLine, ModelConverterClass::INPUT_LINE_SIZE_);
	}

	lastReadingDataLine = inputLine_;


	this->PrintDebugData("textures", verticesCount_, firstReadingDataLine, lastReadingDataLine);


	// allocate the memory for this count of texture coordinates pairs
	pTexCoord_ = new(nothrow) TEXCOORD[textureCoordsCount_];
	COM_ERROR_IF_FALSE(pTexCoord_, "can't allocate memory for the texture coords data");

	// --------------------- READ IN TEXTURE COORDINATES DATA ---------------------- //

	// return back to the position before texture coordinates data
	fin.seekg(posBeforeTextureData); 

	//  reading in of each texture coordinates pair
	for (size_t i = 0; i < textureCoordsCount_; i++) 
	{
		fin.ignore(3); // ignore "vt " in the beginning of line
		fin >> pTexCoord_[i].tu >> pTexCoord_[i].tv; // read in the texture coordinates pair and a new line symbol
		
		//cout << '[' << i << "]:  " << pTexCoord_[i].tu << pTexCoord_[i].tv << endl;
	}

	// after reading of all the texture coords data we need to read in null-terminator as well
	fin >> input;

	return true;
}

bool ModelConverterClass::ReadInNormalsData(ifstream& fin)
{
	//Log::Debug(THIS_FUNC_EMPTY);

	//constexpr int lineSize = 80;
	char input;
	//char inputLine[lineSize];
	size_t posBeforeNormalsData = 0;
	std::string firstReadingDataLine{ "" };
	std::string lastReadingDataLine{ "" };


	fin.seekg(-1, ios::cur); // move at the position before the symbol "v"
	posBeforeNormalsData = fin.tellg(); // later we'll return to this position to read in normals data


	// ----------- CALCULATE THE COUNT OF NORMALS  ---------------- //
	fin.getline(inputLine_, INPUT_LINE_SIZE_, '\n');
	firstReadingDataLine = inputLine_;

	while (inputLine_[0] == 'v')   // while we don't get to the end of normals data
	{
		normalsCount_++;
		fin.getline(inputLine_, INPUT_LINE_SIZE_);
	}

	lastReadingDataLine = inputLine_;

	this->PrintDebugData("normal", verticesCount_, firstReadingDataLine, lastReadingDataLine);

		

	// allocate the memory for this count of normals
	pNormal_ = new(nothrow) NORMAL[normalsCount_];

	// --------------------- READ IN NORMALS DATA ---------------------- //
	fin.seekg(posBeforeNormalsData); // return back to the position before normals data

	for (size_t i = 0; i < normalsCount_; i++) //  reading in of each normal
	{
		fin.ignore(2); // ignore "vn " in the beginning of line
		fin >> pNormal_[i].nx >> pNormal_[i].ny >> pNormal_[i].nz >> input; // read in the normal data and a new line symbol
	}



	return true;
}

bool ModelConverterClass::ReadInFacesData(ifstream& fin)
{
	//Log::Debug(THIS_FUNC_EMPTY);
	inputLine_[0] = '\0';
	// skip data until we get to the 'f' symbol
	while (inputLine_[0] != 'f')
	{
		fin.getline(inputLine_, ModelConverterClass::INPUT_LINE_SIZE_);
	};

	size_t posBeforeFaceCommand = fin.tellg();
	posBeforeFaceCommand -= strlen(inputLine_) + 1; // come back at the beginning of line (size of the string + null character)
	fin.seekg(posBeforeFaceCommand);	// now we at the position before the beginning of polygonal face data

	// define how many faces we have
	fin.getline(inputLine_, ModelConverterClass::INPUT_LINE_SIZE_);
	while (!fin.eof())
	{
		facesCount_++;
		fin.getline(inputLine_, ModelConverterClass::INPUT_LINE_SIZE_);
	};

	if (ModelConverterClass::PRINT_CONVERT_PROCESS_MESSAGES_)
	{
		std::string debugMsg{ "FACES COUNT: " + std::to_string(facesCount_) };
		Log::Debug(THIS_FUNC, debugMsg.c_str());
	}

	// allocate the memory for such a count of faces
	//pModelType_ = new(nothrow) ModelType[facesCount_ * 3];

	fin.clear();
	fin.seekg(posBeforeFaceCommand);


	ReadInModelData(fin);


	return true;
}


bool ModelConverterClass::ReadInModelData(ifstream& fin)
{
	size_t vertexNum = 0, textureNum = 0, normalNum = 0;
	int curFaceIndex = 0;
	char symbol[2];
	char* inputLine = new char[ModelConverterClass::INPUT_LINE_SIZE_];

	while (curFaceIndex < facesCount_)
	{
		fin.getline(inputLine, ModelConverterClass::INPUT_LINE_SIZE_, '\n');

		// if we are at the correct face data line
		while (inputLine[0] != 'f')
		{
			if (fin.eof())
				return true;
			std::cout << "This line was skipped during the reading of the face data ";
			std::cout << curFaceIndex << ": ";
			std::cout << inputLine << std::endl;

			fin.getline(inputLine, INPUT_LINE_SIZE_, '\n');


		}
		
		// come back at the beginning of line (size of the string + null character)
		size_t posBeforeLastInputLine = fin.tellg();
		posBeforeLastInputLine -= strlen(inputLine) + 1;
		fin.seekg(posBeforeLastInputLine);	// now we at the position before the beginning of polygonal face data

		symbol[0] = fin.get(); // read the 'f' symbol
		symbol[1] = fin.get(); // read the ' ' symbol (space)

		


		// read in three bunches of vertex/texture/normal indices;
		// we go from 2 to 0 because vertices must be in the clockwise order
		for (int faceIndex = 2; faceIndex >= 0; faceIndex--)
		{
			
			// read in a bunch of a indices for vertex/texture/normal
			fin >> vertexNum;

			// if smth went wrong during reading of the faces
			while (fin.fail())
			{
				std::cout << "\n\nFACES ERROR!" << endl;
				std::cout << "symbols:     " << symbol[0] << "; and symbol: " << symbol[1] << std::endl;
				std::cout << "input line:  " << inputLine << endl;
				std::cout << "curFace:     " << curFaceIndex << endl;
				std::cout << "prevTexture: " << textureNum << endl;
				std::cout << "prevNormal:  " << normalNum << endl;
				return false;
			//	return false;
			}

			fin.ignore();  // ignore "/"
			fin >> textureNum;
			fin.ignore();  // ignore "/"
			fin >> normalNum;
			fin.get();     // read up the space (or '\n') after each set of v/vt/vn

			// change these values fox correct indexing
			vertexNum--;
			textureNum--;
			normalNum--;

			/*
			cout << '[' << faceIndex << "]:  vtn = " << vertexNum << "_" << textureNum << "_" << normalNum << " ==== ";
			cout << "RAW DATA:  ";
			cout << "V T: "
			<< setprecision(1)
			<< setw(2) << pPoint3D_[vertexNum].x << " "
			<< setw(2) << pPoint3D_[vertexNum].y << ' '
			<< setw(2) << pPoint3D_[vertexNum].z
			<< '\t'
			<< setw(2) << pTexCoord_[textureNum].tu << " "
			<< setw(2) << pTexCoord_[textureNum].tv << ' '
			<< endl;
			*/


			// write in vertices data
			vtnData[faceIndex].x = pPoint3D_[vertexNum].x;
			vtnData[faceIndex].y = pPoint3D_[vertexNum].y;
			vtnData[faceIndex].z = pPoint3D_[vertexNum].z * -1.0f;    // invert the value to use it in the left handed coordinate system

			// write in texture coordinates data
			vtnData[faceIndex].tu = pTexCoord_[textureNum].tu;
			vtnData[faceIndex].tv = 1.0f - pTexCoord_[textureNum].tv; // invert the value to use it in the left handed coordinate system

			// write in normals data
			vtnData[faceIndex].nx = pNormal_[normalNum].nx;
			vtnData[faceIndex].ny = pNormal_[normalNum].ny;
			vtnData[faceIndex].nz = pNormal_[normalNum].nz * -1.0f;   // invert the value to use it in the left handed coordinate system


			/*
			
			cout << '[' << faceIndex << "]:  vtn = " << vertexNum << "_" << textureNum << "_" << normalNum << " ==== ";
			cout << "CALCULATED V T: "
			<< setprecision(1)
			<< setw(2) << vtnData[faceIndex].x << " "
			<< setw(2) << vtnData[faceIndex].y << ' '
			<< setw(2) << vtnData[faceIndex].z
			<< '\t'
			<< setw(2) << vtnData[faceIndex].tu << " "
			<< setw(2) << vtnData[faceIndex].tv << ' '
			<< endl << endl;
			*/
		}


		// write the three bunches of vertex/texture/normal indices
		for (int faceIndex = 0; faceIndex <= 2; faceIndex++)
		{
			modelData.push_back(vtnData[faceIndex]);      // store the vertex/texture/normal values

		}

		curFaceIndex++;
	}

	return true;
}





// filling in the output text file with face data
// later we use this output file to render a model
bool ModelConverterClass::WriteIntoFileFacesData(ofstream& fout)
{
	string progressSymbols{ "|/-\\" };
	size_t progressSymbolsIndex = 0;
	size_t facesCount = modelData.size();


	fout << "Vertex Count: " << facesCount << "\n\n"; // to build a face we need 3 vertices
	fout << "Data:" << "\n\n";

	for (size_t i = 0; i < facesCount; i++)
	{

		// print information about the writing progress into the console
		if (ModelConverterClass::PRINT_CONVERT_PROCESS_MESSAGES_)
		{
			if (i % 2000 == 0 || (i == facesCount - 1))
			{
				float percentage = (float)(i + 1) / (float)facesCount * 100.0f;  // calculate the percentage of the writing progress

				std::cout << "\t\tWriting faces data into the file: ";
				std::cout << (int)percentage << "%  ";
				std::cout << progressSymbols[progressSymbolsIndex];
				std::cout << '\r';

				if (progressSymbolsIndex == progressSymbols.size())
					progressSymbolsIndex = 0;
				else
					progressSymbolsIndex++;
			}
		}


		fout.setf(ios::fixed, ios::floatfield);
		fout.precision(6);

		fout << setprecision(4)
			<< modelData[i].x << " "        // print a vertex coordinates
			<< modelData[i].y << " "
			<< modelData[i].z << " "

			<< modelData[i].tu << " "        // print into the file texture coordinates
			<< modelData[i].tv << " "

			<< modelData[i].nx << " "        // print a normal vector data
			<< modelData[i].ny << " "
			<< modelData[i].nz << " ";

		if (i < facesCount_ * 3 - 1)
			fout << "\n";

	}

	std::cout << endl;

	return true;
}


// after each convertation we MUST reset the state of the converter 
// for proper later convertations
bool ModelConverterClass::ResetConverterState()
{
	this->Shutdown();

	verticesCount_ = 0;
	textureCoordsCount_ = 0;
	normalsCount_ = 0;
	facesCount_ = 0;

	modelData.clear();

	return true;
}


// makes a final name for the file where we'll place model data
bool ModelConverterClass::GetOutputModelFilename(string& fullFilename, string& rawFilename)
{
	size_t pointPos = rawFilename.rfind('.');
	fullFilename = { rawFilename.substr(0, pointPos) + ModelConverterClass::MODEL_FILE_TYPE_ };
	return true;
}