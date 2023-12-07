#include "PNG_Reader.h"


PNG_Reader::PNG_Reader()
{
}

PNG_Reader::~PNG_Reader()
{
}



bool PNG_Reader::ReadImage(const std::string & filePath, const std::vector<BYTE> & imageData)
{
	FILE* pFile = nullptr;
	errno_t error = -1;

	try
	{
		// check input path
		COM_ERROR_IF_FALSE(!filePath.empty(), "the input file path is empty");

		
		// ---------------------------------------------------- //

		// open the file for reading in a binary mode
		error = fopen_s(&pFile, filePath.c_str(), "rb");
		COM_ERROR_IF_FALSE(error == 0, "can't open the file for reading in binary: " + filePath);

		std::string debugMsg{ "the file " + filePath + " is opened" };
		Log::Debug(THIS_FUNC, debugMsg.c_str());

		// read in the file signature and confirm that there is a true png image
		this->CheckFileSignature(pFile);



#if 0
		// return to the beginning and read the contents of the file
		long length = end - begin;
		BYTE* pBuffer = new BYTE[length];
#endif




		Log::Debug(THIS_FUNC, "this is a PNG FILE");

		// close the file 
		error = fclose(pFile);
		COM_ERROR_IF_FALSE(error == 0, "can't close the file: " + filePath);
	}
	catch (COMException & e)
	{
		fclose(pFile);

		Log::Error(e, false);
		Log::Error(THIS_FUNC, "can't read a file as PNG");
		return false;
	}
	// as we no longer need a buffer with image data we delete it
	//_DELETE_ARR(pBuffer);

	return true;
}




void PNG_Reader::CheckFileSignature(FILE* pFile)
{
	// this function reads first eight bytes of a file to the if it is a PNG file;
	// a PNG file always contain the following (decimal) values:
	//
	//    137 80 78 71 13 10 26 10

	UINT bytesCount = 8;
	uint8_t signatureBytes[] = { 137, 80, 78, 71, 13, 10, 26, 10 };
	BYTE buffer[8];

	// read in first eight bytes
	size_t count = fread(buffer, sizeof(BYTE) * bytesCount, 1, pFile);
	COM_ERROR_IF_FALSE(count == 1, "can't read in the file signature");

	// go through each signature byte and compare it with expected value
	for (UINT i = 0; i < bytesCount; i++)
	{
		COM_ERROR_IF_FALSE(buffer[i] == signatureBytes[i], "the file is not a PNG");
	}

	Log::Debug(THIS_FUNC, "a signature of PNG is ok");

} // end ReadFileSignature
