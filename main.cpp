//////////////////////////////////
// Filename: main.cpp
//////////////////////////////////

//////////////////////////////////
// INCLUDE
//////////////////////////////////
#include <iostream>
#include <fstream>

//////////////////////////////////
// TYPEDEFS
//////////////////////////////////
typedef struct
{
	float x, y, z;
} VertexType;

typedef struct
{
	int vIndex1, vIndex2, vIndex3;
	int tIndex1, tIndex2, tIndex3;
	int nIndex1, nIndex2, nIndex3;
} FaceType;

//////////////////////////////////
// FUNCTION PROTOTYPES
//////////////////////////////////
void GetModelFilename(char*);
bool ReadFileCounts(char*, int&, int&, int&, int&);
bool LoadDataStructures(char*, int, int, int, int);

//////////////////////////////////
// MAIN PROGRAM
//////////////////////////////////
int main()
{
	bool	result;
	char	filename[256];
	int		vertexCount, textureCount, normalCount, faceCount;
	char	garbage;


	// Read in the name of the model file
	GetModelFilename(filename);

	// Read in the number of vertices, tex coords, normals, and faces so that the data
	// structures can be initialized with the exact sizes needed
	result = ReadFileCounts(filename, vertexCount, textureCount, normalCount, faceCount);
	if (!result)
	{
		return -1;
	}

	// Display the counts to the screen for information purpose
	cout << endl;
	cout << "Vertices: " << vertexCount << endl;
	cout << "UVs:      " << textureCount << endl;
	cout << "Normals:  " << normalCount << endl;
	cout << "Faces:    " << faceCount << endl;

	// Now read the data from the file into the data structures and then output
	// it in our model formal
	result = LoadDataStructures(filename, vertexCount, textureCount, normalCount, faceCount);
	if (!result)
	{
		return -1;
	}

	// Notify the user the model has been converted
	cout << "\nFile has been converted" << endl;
	cout << "\nDo you wish to exit (y/n)?";
	cin >> garbage;

	return 0;
}

void GetModelFilename(char* filename)
{
	bool done;
	ifstream fin;

	// Loop until we have a file name
	done = false;

	while (!done)
	{
		// Ask the user for the filename
		cout << "Enter model filename:";

		// Read in the filename
		cin >> filename;

		// Attempt to open the file
		fin.open(filename);

		if (fin.good())
		{
			// If the file exists and there are no problems 
			// then exit since we have the file name
			done = true;
		}
		else
		{
			// If the file does not exit or there was an issue opening
			fin.clear();
			cout << endl;
			cout << "File " << filename << " could not be opened" << endl << endl;
		}
	}

	return;
}

bool ReadFileCounts(char* filename, int& vertexCount, int& textureCount,
					int& normalCount, int& faceCount)
{
	ifstream fin;
	char input;

	// Initialize the counts
	vertexCount = 0;
	textureCount = 0;
	normalCount = 0;
	faceCount = 0;

	// Open the file
	fin.open(filename);

	// Check if it was successful in opening the file
	if (fin.fail() == true)
	{
		return false;
	}

	// Read from the file and continue to read until the end of the file is reached
	fin.get(input);
	while (!fin.eof())
	{
		// If the line starts with 'v' then count either the vertex, the texture
		// coordinates, or the normal vector
		if (input == 'v')
		{
			fin.get(input);
			if (input == ' ') { vertexCount++; }
			if (input == 't') { textureCount++; }
			if (input == 'n') { normalCount++; }
		}

		// If the line starts with 'f' then increment the face count
		if (input == 'f')
		{
			fin.get(input);
			if (input == ' ') { faceCount++; }
		}

		// Otherwise read in the remainder of the line
		while (input != '\n')
		{
			fin.get(input);
		}

		// Start reading the beginning of the next line.
		fin.get(input);
	}

	// Close the file
	fin.close();

	return true;
}

bool LoadDataStructures










#include "systemclass.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	SystemClass *System;
	bool result;


	// Create the system object
	System = new SystemClass;

	if (!System)
	{
		return 0;
	}

	// Initialize and run the system object
	result = System->Initialize();

	if (result)
	{
		System->Run();
	}

	// Shutdown and release the system object
	System->Shutdown();
	delete System;
	System = nullptr;

	return 0;
}