// *********************************************************************************
// Filename:     MemHelpers.h
// Description:  operations with memory
// 
// Created:      13.08.24
// *********************************************************************************
#pragma once


inline void SafeDelete(void* p)
{
	if (p) { delete(p); p = nullptr; }
}

///////////////////////////////////////////////////////////

inline void SafeDeleteArr(void* p)
{
	if (p) { delete[](p); p = nullptr; }
}

///////////////////////////////////////////////////////////

template<class T>
inline void SafeShutdown(T** ppT)
{
	if (*ppT) { (*ppT)->Shutdown(); delete(ppT); *ppT = nullptr; }
}

///////////////////////////////////////////////////////////

template<class T>
inline void SafeRelease(T** ppT)
{
	if (*ppT) { (*ppT)->Release(); *ppT = nullptr; }
}