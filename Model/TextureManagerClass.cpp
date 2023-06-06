////////////////////////////////////////////////////////////////////
// Filename:      TextureManagerClass.cpp
// Description:   a manager for work with textures: initialization of
//                ALL the textures, getting it and releasing;
// Created:       06.06.23
////////////////////////////////////////////////////////////////////
#include "TextureManagerClass.h"
#include <iostream>

TextureManagerClass::TextureManagerClass()
{

}

TextureManagerClass::~TextureManagerClass()
{

}

bool TextureManagerClass::Initialize()
{
	Log::Debug(THIS_FUNC_EMPTY);

	std::vector<std::wstring> filesNames = GetAllFilesNamesWithinFolder(TEXTURES_DIR_PATH);

	for (const auto & elem : filesNames)
	{
		std::cout << elem.c_str() << std::endl;
	}

	return true;
}

std::vector<std::wstring> TextureManagerClass::GetAllFilesNamesWithinFolder(std::string folderName)
{
	std::vector<std::wstring> filesNames;
	std::string searchPath{ folderName + "/*.dds" };   // search only for dx textures
	WIN32_FIND_DATA fd;
	const WCHAR* wSearchPath = StringConverter::StringToWide(searchPath).c_str();

	HANDLE hFind = ::FindFirstFile(wSearchPath, &fd);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				filesNames.push_back(fd.cFileName);
			}
		} while (::FindNextFile(hFind, &fd));

		::FindClose(hFind);
	}

	return filesNames;
}