#include "modelconverterclass.h"



ModelConverterClass::ModelConverterClass(void)
{
	inputLine_ = new char[ModelConverterClass::INPUT_LINE_SIZE_];   // during execution of the getline() function we put here a one single text line
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
	_DELETE(pPoint3D_);
	_DELETE(pTexCoord_);
	_DELETE(pNormal_);
	_DELETE(inputLine_);
}


// converts a model of the ".obj" type into the internal model format
bool ModelConverterClass::ConvertFromObj(const string & inputFilename)
{
	bool result = false;
	std::string outputFilename{ "" };

	this->GetOutputModelFilename(outputFilename, inputFilename);
	this->PrintDebugFilenames(inputFilename, outputFilename);
	
	// open the input file and create an output file
	std::ifstream fin(inputFilename, std::ios::in | std::ios::binary);	// input data file (.obj)
	std::ofstream fout(outputFilename, std::ios::out); // ouptput data file (.txt)
	

	// If it could not open the input file then exit
	result = fin.fail();
	COM_ERROR_IF_FALSE(!result, "can't open \"" + inputFilename + "\" file");
	
	// if it could not open the output file then exit
	COM_ERROR_IF_FALSE(fout, "can't open the output file");

	// convert the model
	result = this->ConvertFromObjHelper(fin, fout); 
	COM_ERROR_IF_FALSE(result, "Model's data has been converted successfully");

	// close the .obj input file and the output file
	fin.close();
	fout.close();

	return true;
}

void ModelConverterClass::PrintDebugFilenames(const std::string & inputFilename, const std::string & outputFilename) const
{
	// setup the debug message
	string debugMsg{ "FILES:" };
	debugMsg += "\n\t\t\t\t";
	debugMsg += "in: " + inputFilename;
	debugMsg += ";  ";
	debugMsg += "\n\t\t\t\t";
	debugMsg += "out: " + outputFilename;

	Log::Debug(THIS_FUNC, debugMsg.c_str());
}

// ----------------------------------------------------------------------------------- //
//
//                          PRIVATE METHODS / HELPERS
//
// ----------------------------------------------------------------------------------- //

// help us to convert .obj file model data into the internal model format
bool ModelConverterClass::ConvertFromObjHelper(ifstream& fin, ofstream& fout)
{
	// Read up to the vertex values
	for (size_t i = 0; i < 4; i++)
	{
		fin.getline(inputLine_, ModelConverterClass::INPUT_LINE_SIZE_, '\n');    // skip first four lines of the .obj file								
	}

	
	Log::Debug(THIS_FUNC, "START of the convertation process");
	
	ReadInVerticesData(fin); 
	ReadInTextureData(fin);
	ReadInNormalsData(fin);

	ReadInFacesData(fin);
	WriteDataIntoOutputFile(fout); // write model data in an internal model format into the output data file
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

	
	this->PrintReadingDebugData("vertices", verticesCount_, firstReadingDataLine, lastReadingDataLine);
	

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
	char input;
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


	this->PrintReadingDebugData("textures", verticesCount_, firstReadingDataLine, lastReadingDataLine);


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

	this->PrintReadingDebugData("normal", normalsCount_, firstReadingDataLine, lastReadingDataLine);

		

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




bool ModelConverterClass::ReadInFacesData(ifstream & fin)
{
	Log::Print(THIS_FUNC_EMPTY);

	int vertexIndex = 0;
	int textureIndex = 0;
	int normalIndex = 0;

	inputLine_[0] = '\0';
	//char input[2];


	// skip data until we get to the 'f' and ' ' (space) symbols
	while (inputLine_[0] != 'f' && inputLine_[1] != ' ')
	{
		fin.getline(inputLine_, ModelConverterClass::INPUT_LINE_SIZE_);
	};



	// store the file pointer position
	size_t posBeforeFaceCommand = fin.tellg();
	//posBeforeFaceCommand -= strlen(inputLine_) + 1; // come back at the beginning of line (size of the string + null character)
	fin.seekg(posBeforeFaceCommand);	// now we at the position before the beginning of polygonal face data

	Log::Debug("HOW MANY FACES:");
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


	fin.clear();
	fin.seekg(posBeforeFaceCommand);

	// go through each face
	for (size_t faceIndex = 0; faceIndex < facesCount_; faceIndex++)
	{
		/*
		
		while (!fin.eof())
		{
			cout << (char)fin.get();
		}
		exit(0);
		*/

		fin.ignore(2);  // skip the 'f' and ' ' (space) symbols at the beginning of the line

		// go through each vertex of the current face
		for (size_t faceVertex = 1; faceVertex <= 3; faceVertex++)
		{
			// read in a vertex index
			fin >> vertexIndex;
			COM_ERROR_IF_FALSE(!fin.bad(), "error about reading of the vertex index");
			fin.ignore();  // ignore "/"

			// read in a texture index
			fin >> textureIndex;
			COM_ERROR_IF_FALSE(!fin.bad(), "error about reading of the texture index");
			fin.ignore();  // ignore "/"

			// read in an index of the normal vector
			fin >> normalIndex;
			COM_ERROR_IF_FALSE(!fin.bad(), "error about reading of the normal index");
			fin.get();     // read up the space (or '\n') after each set of v/vt/vn

			//cout << "vtn[" << faceIndex << "][" << faceVertex << "]: " << vertexIndex - 1 << " " << textureIndex - 1 << " " << normalIndex - 1 << endl;

			// write point/texture/normal data into the vertexArray
			vertexIndex--;
			textureIndex--;
			normalIndex--;
			//std::string debugMsg{ "add to vector this vertex index: " + std::to_string(vertexIndex) };
			//Log::Error(THIS_FUNC, debugMsg.c_str());
				
			vertexIndicesArray_.push_back(vertexIndex);   // write the index of a vertex coord
			textureIndicesArray_.push_back(textureIndex);  // write the index of a texture coord
		}
	}
	return true;

	//fin.clear();
	//fin.seekg(posBeforeFaceCommand);
}


// here we write into the output model file the model data
bool ModelConverterClass::WriteDataIntoOutputFile(ofstream & fout)
{
	fout << "Vertex Count: " << verticesCount_ << "\n";
	fout << "Indices Count: " << vertexIndicesArray_.size() << "\n";
	fout << "Textures Count: " << textureCoordsCount_ << "\n\n";


	this->WriteIndicesIntoOutputFile(fout);
	this->WriteVerticesIntoOutputFile(fout);
	this->WriteTexturesIntoOutputFile(fout);


	return true;
}

bool ModelConverterClass::WriteIndicesIntoOutputFile(ofstream & fout)
{
	// VERTEX INDICES WRITING
	fout << "Vertex Indices Data:" << "\n\n";

	for (size_t it = 0; it < vertexIndicesArray_.size() - 2; it += 3)
	{
		fout << vertexIndicesArray_[it + 2] << ' ';
		fout << vertexIndicesArray_[it + 1] << ' ';
		fout << vertexIndicesArray_[it] << endl;
	}
	fout.seekp(-1, ios::cur);
	fout << "\n\n";


	// TEXTURE INDICES WRITING
	fout << "Texture Indices Data:" << "\n\n";

	for (size_t it = 0; it < textureIndicesArray_.size() - 2; it += 3)
	{
		fout << textureIndicesArray_[it + 2] << ' ';
		fout << textureIndicesArray_[it + 1] << ' ';
		fout << textureIndicesArray_[it] << endl;
	}

	// PRINT DEBUG INDICES DATA
	if (ModelConverterClass::PRINT_CONVERT_PROCESS_MESSAGES_)
	{
		Log::Debug(THIS_FUNC, "VERTEX INDICES DATA: ");
		for (auto it = vertexIndicesArray_.begin(); it != vertexIndicesArray_.end(); ++it)
		{
			cout << *it << " ";
			
		}
		cout << "\n\n";

		Log::Debug(THIS_FUNC, "TEXTURE INDICES DATA: ");
		for (auto it = textureIndicesArray_.begin(); it != textureIndicesArray_.end(); ++it)
		{
			cout << *it << " ";
		}
		cout << "\n\n";
	}

	return true;
}


// VERTICES DATA WRITING
bool ModelConverterClass::WriteVerticesIntoOutputFile(ofstream & fout)
{
	fout << "Vertices Data:" << "\n\n";

	for (size_t index = 0; index < verticesCount_; index++)
	{
		fout.setf(ios::fixed, ios::floatfield);
		fout.precision(6);

		fout << setprecision(4)
			<< pPoint3D_[index].x << " "           // print a vertex coordinates
			<< pPoint3D_[index].y << " "
			<< pPoint3D_[index].z * -1.0f << " ";  // invert the value to use it in the left handed coordinate system

		fout << "\n";
	}
	fout << "\n\n";

	return true;
}


// TEXTURES DATA WRITING
bool ModelConverterClass::WriteTexturesIntoOutputFile(ofstream & fout)
{
	fout << "Textures Data:" << "\n\n";

	for (size_t index = 0; index < textureCoordsCount_; index++)
	{
		fout.setf(ios::fixed, ios::floatfield);
		fout.precision(6);

		fout << setprecision(4)
			<< pTexCoord_[index].tu << " "
			<< 1.0f - pTexCoord_[index].tv << " ";  // invert the value to use it in the left handed coordinate system

		fout << "\n";
	}

	return true;
}









// after each convertation we MUST reset the state of the converter 
// for proper later convertations
bool ModelConverterClass::ResetConverterState()
{
	_DELETE(pPoint3D_);
	_DELETE(pTexCoord_);
	_DELETE(pNormal_);

	verticesCount_ = 0;
	textureCoordsCount_ = 0;
	normalsCount_ = 0;
	facesCount_ = 0;

	//modelData.clear();
	vertexIndicesArray_.clear();
	textureIndicesArray_.clear();
	inputLine_[0] = '\0';

	return true;
}


// makes a final name for the file where we'll place model data
bool ModelConverterClass::GetOutputModelFilename(string & fullFilename, const string & rawFilename)
{
	size_t pointPos = rawFilename.rfind('.');
	fullFilename = { rawFilename.substr(0, pointPos) + Settings::Get()->GetSettingStrByKey("MODEL_FILE_TYPE") };

	return true;
}


void ModelConverterClass::PrintReadingDebugData(std::string dataType, int dataElemsCount, const std::string & firstLine, const std::string & lastLine)
{
	if (ModelConverterClass::PRINT_CONVERT_PROCESS_MESSAGES_)
	{
		std::string debugMsg{ dataType + " DEBUG DATA:" };
		Log::Debug(THIS_FUNC, debugMsg.c_str());

		debugMsg = { "first line of the " + dataType + " data: " + firstLine };
		Log::Debug("\t\t%s", debugMsg.c_str());

		debugMsg = { "line after the " + dataType + " data: " + lastLine };
		Log::Debug("\t\t%s", debugMsg.c_str());

		debugMsg = { dataType + " count: " + std::to_string(dataElemsCount) };
		Log::Debug("\t\t%s", debugMsg.c_str());
	}
}







/*
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
*/