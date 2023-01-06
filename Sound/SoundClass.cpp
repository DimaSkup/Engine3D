////////////////////////////////////////////////////////////////////
// Filename:     SoundClass.cpp
// Description:  implementation of the SoundClass
// Created:      05.01.23
// Revised:      05.01.23
////////////////////////////////////////////////////////////////////
#include "SoundClass.h"



////////////////////////////////////////////////////////////////////
//
//                     PUBLIC FUNCTIONS
//
////////////////////////////////////////////////////////////////////


// Firstly we initialize the DirectSound API as well as the primary buffer.
// Once that is initialized then the LoadWaveFile() function can be called 
// which will load in the .wav audio file and initialize the secondary 
// buffer with the audio information from the .wav file.
// After loading is completed then PlayWaveFile() is called which then plays
// the .wav file once
bool SoundClass::Initialize(HWND hwnd)
{
	try
	{
		Log::Debug(THIS_FUNC_EMPTY);

		bool result = false;

		// initialize DirectSound and the primary sound buffer
		result = InitializeDirectSound(hwnd);
		COM_ERROR_IF_FALSE(result, "can't initialize DirectSound");

		// load a wave audio file onto a secondary buffer
		result = LoadWaveFile("../data/audio/sound01.wav", &pSecondaryBuffer1_);
		COM_ERROR_IF_FALSE(result, "can't load in a wave audio file");

		// play the wave file now that it has been loaded
		result = PlayWaveFile();
		COM_ERROR_IF_FALSE(result, "can't play the wave audio file");

	}
	catch (COMException& exception)
	{
		Log::Error(exception);
		return false;
	}

	return true;
} // Initialize()


// The Shutdown() function first releases the secondary buffer which held the .wav file
// audio data using the ShutdownWaveFile() function. Once that completes this function
// then calls ShutdownDirectSound() which releases the primary buffer and the DirectSound
// interface.
void SoundClass::Shutdown()
{
	// release the secondary buffer
	ShutdownWaveFile(&pSecondaryBuffer1_);

	// shutdown the Direct Sound API
	ShutdownDirectSound();

	Log::Debug(THIS_FUNC_EMPTY);

	return;
}


////////////////////////////////////////////////////////////////////
//
//                      PRIVATE FUNCTIONS
//
////////////////////////////////////////////////////////////////////

// InitializeDirectSound() handles getting an interface pointer to DirectSound and 
// the default primary sound buffer. Note that you can query the syster for all the sound
// devices and then grab the pointer to the primary sound buffer for a specific device.
bool SoundClass::InitializeDirectSound(HWND hwnd)
{
	HRESULT hr = S_OK;
	DSBUFFERDESC bufferDesc;
	WAVEFORMATEX waveFormat;

	// initialize the DirectSound interface pointer for the default sound device
	hr = DirectSoundCreate8(NULL, &pDirectSound_, NULL);
	COM_ERROR_IF_FAILED(hr, "can't initialize the DirectSound interface");

	// set the cooperative level to priority so the format of the primary sound buffer
	// can be modified
	hr = pDirectSound_->SetCooperativeLevel(hwnd, DSSCL_PRIORITY);
	COM_ERROR_IF_FAILED(hr, "can't set the cooperation level");

	// we have to setup the description of how we want to access the primary buffer. 
	// The dwFlags are the important part of this structure. In this case we just want 
	// to setup a primary buffer description with the capability of adjesting its volume.
	// There are other capabilities you can grab but we are keeping it simple for now.

	// setup the primary buffer description
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;
	bufferDesc.dwBufferBytes = 0;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = nullptr;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	// get control of the primary sound buffer on the default sound device
	hr = pDirectSound_->CreateSoundBuffer(&bufferDesc, &pPrimaryBuffer_, NULL);
	COM_ERROR_IF_FAILED(hr, "can't create a primary sound buffer");


	// now that we have control of the primary buffer on the default sound device we want 
	// to change its format to our desired audio file format. Now we have high quality
	// sound so we will set it to uncompressed CD audio quality.

	// setup the format of the primary sound buffer;
	// in this case it is a .WAV file recorded at 44,100 samples per second in 
	// 16-bit stereo (cd audio format)
	SetDefaultWaveFormat(waveFormat);

	// set the primery buffer to be the wave format specified
	hr = pPrimaryBuffer_->SetFormat(&waveFormat);
	COM_ERROR_IF_FAILED(hr, "can't set a format for the primary buffer");

	return true;
} // InitializeDirectSound()


// The ShutdownDirectSound() function handles releasing the primary buffer 
// and DirectSound interfaces.
void SoundClass::ShutdownDirectSound()
{
	_RELEASE(pPrimaryBuffer_);  // release the primary sound buffer pointer
	_RELEASE(pDirectSound_);    // release the direct sound interface pointer

	return;
}


// the LoadWaveFile() function is what handles loading in a .wav audio file and then 
// copies the data onto a new secondary buffer. If you are looking to do different 
// formats your would replace this function or write a similar one
bool SoundClass::LoadWaveFile(char* filename, IDirectSoundBuffer8** secondaryBuffer)
{
	bool result = false;
	errno_t error = 0;
	HRESULT hr = S_OK;
	FILE* filePtr = nullptr;
	UCHAR* waveData = nullptr;
	UCHAR* bufferPtr = nullptr;
	IDirectSoundBuffer* tempBuffer = nullptr;
	ULONG bufferSize = 0;
	size_t count = 0;
	WaveHeaderType waveFileHeader;
	WAVEFORMATEX waveFormat;
	DSBUFFERDESC bufferDesc;

	/*
		to start first open the .wav file and read in the header of the file. The header
		will contain all the information about the audio file so we can use that to
		create a secondary buffer to accommodate the audio data. The audio file header
		also tells us where the data begins and how big it is. You will notice I check for
		all the needed tags to ensure the audio file is not corrupt and is the proper
		wave file format containing RIFF, WAVE, fmt, data, and WAVE_FORMAT_PCM tags.
		I also do a couple other checks to ensure it is a 44.1KHz stereo 16bit audio file.
		If it is mono, 22.1KHz, 8bit, or anything else then it will fail ensuring we are
		only loading the exact format we want.	
	*/
	
	// open the wave file in binary
	error = fopen_s(&filePtr, filename, "rb");
	COM_ERROR_IF_FALSE(error == 0, "can't open the wave file in binary");

	// read in the wave file header
	count = fread(&waveFileHeader, sizeof(waveFileHeader), 1, filePtr);
	COM_ERROR_IF_FALSE(count == 1, "can't read in the wave file header");

	// verify the wave header file so we ensure everything is correct
	this->VerifyWaveHeaderFile(waveFileHeader);



	/*
		now that the wave header file has been verified we can setup the secondary buffer
		we will load the audio data onto. We have to first set wave format and buffer
		description of the secondary buffer similar to how we did for the primary buffer.
		There are some changes though since this is the secondary and not primary in 
		terms of the dwFlags and dwBufferBytes.
	*/

	// set the wave format of secondary buffer that this wave file will be loaded onto
	SetDefaultWaveFormat(waveFormat);

	// set the buffer description of the secondary sound buffer that the wave file will be loaded onto.
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME;
	bufferDesc.dwBufferBytes = waveFileHeader.dataSize;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = &waveFormat;
	bufferDesc.guid3DAlgorithm = GUID_NULL;


	/*
		Now the way to create a secondary buffer is fairly strange. First ............
	*/

	
	

	return true;
}



bool SoundClass::VerifyWaveHeaderFile(const WaveHeaderType& waveFileHeader)
{
	int isEqual = 0;

	// check that the chunk ID is the RIFF format
	isEqual = strcmp(waveFileHeader.chunkId, "RIFF");
	COM_ERROR_IF_FALSE(isEqual == 0, "chunk ID isn't the RIFF format");

	// check that the format is the WAVE format
	isEqual = strcmp(waveFileHeader.format, "WAVE");
	COM_ERROR_IF_FALSE(isEqual == 0, "the file format is not the WAVE format");

	// check that the sub chunk ID is the fmt format
	isEqual = strcmp(waveFileHeader.subChunkId, "fmt ");
	COM_ERROR_IF_FALSE(isEqual == 0, "the sub chunk Id is not the fmt format");

	// check that the audio format is WAVE_FORMAT_PCM
	COM_ERROR_IF_FALSE(waveFileHeader.audioFormat == WAVE_FORMAT_PCM, "the audio format is not WAVE_FORMAT_PCM");

	// check that the wave file was recorded in stereo format
	COM_ERROR_IF_FALSE(waveFileHeader.numChannels == 2, "the wave file wasn't recorded in stereo format");

	// check that the wave file was recorded at a sample rate of 44.1KHz
	COM_ERROR_IF_FALSE(waveFileHeader.sampleRate == 44100, "the wave file wasn't recorded at a sample rate of 44.1KHz");

	// ensure that the wave file was recorded in 16 bit format
	COM_ERROR_IF_FALSE(waveFileHeader.bitsPerSample == 16, "the wave file wasn't recorded in 16 bit format");

	// check for the data chunk header
	isEqual = strcmp(waveFileHeader.dataChunkId, "data");
	COM_ERROR_IF_FALSE(isEqual == 0, "wrong data chunk header");

	return true;
}





// set up the wave format with default parameters for the sound buffer
void SetDefaultWaveFormat(WAVEFORMATEX& waveFormat)
{
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 2;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	return;
}