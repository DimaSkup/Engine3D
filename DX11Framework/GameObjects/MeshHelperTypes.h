#pragma once

#include <vector>
#include <map>
#include <string>
#include "Vertex.h"
#include "textureclass.h"

typedef unsigned int UINT;


struct RawMesh
{
	std::string name {"some_mesh"};
	std::vector<VERTEX> vertices;
	std::vector<UINT> indices;
	std::map<aiTextureType, TextureClass*> textures;
};
