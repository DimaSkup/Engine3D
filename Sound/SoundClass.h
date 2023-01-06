////////////////////////////////////////////////////////////////////
// Filename:     SoundClass.h
// Description:  this class ecapsulates the DirectSound functionality
//               as well as the .wav audio loading and playing capabilities.
// Created:      05.01.23
// Revised:      05.01.23
////////////////////////////////////////////////////////////////////
#pragma once


//////////////////////////////////
// LINKING
//////////////////////////////////
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <windows.h>
#include <mmsystem.h>
#include <dsound.h>
#include <stdio.h>

#include "../Engine/log.h"

//////////////////////////////////
// Class name: SoundClass
//////////////////////////////////
class SoundClass
{
private:
	// this structure is used here for the .wav file format;
	// its necessary to read in the header to determine the required
	// information for loading in the .wav audio data;
	struct WaveHeaderType
	{
		char  chunkId[4];
		ULONG chunkSize;
		char  format[4];
		char  subChunkId[4];
		ULONG  subChunkSize;
		USHORT audioFormat;
		USHORT numChannels;
		ULONG  sampleRate;
		ULONG  bytesPerSecond;
		USHORT blockAlign;
		USHORT bitsPerSample;
		char   dataChunkId[4];
		ULONG  dataSize;
	};


public:
	SoundClass() {};
	SoundClass(const SoundClass& o) {};
	~SoundClass() {};

	bool Initialize(HWND hwnd); // will initialize DirectSound and load in the audio file and then play it once.
	void Shutdown();            // will release the audio file and shutdown DirectSound

private:
	bool InitializeDirectSound(HWND hwnd);
	void ShutdownDirectSound();

	bool LoadWaveFile(char* filename, IDirectSoundBuffer8** ppSoundBuffer);
	void ShutdownWaveFile(IDirectSoundBuffer8** ppSoundBuffer);

	bool PlayWaveFile();


	bool VerifyWaveHeaderFile(const WaveHeaderType& waveFileHeader);
	void SetDefaultWaveFormat(WAVEFORMATEX& waveFormat);

private:
	IDirectSound8* pDirectSound_ = nullptr;
	IDirectSoundBuffer* pPrimaryBuffer_ = nullptr;
	IDirectSoundBuffer8* pSecondaryBuffer1_ = nullptr;  // for each sound we need a separate secondary buffer
};