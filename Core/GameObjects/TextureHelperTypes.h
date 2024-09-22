// ********************************************************************************
// Filename:    TextureHelperTypes.h
// Description: contains some common types and data 
//              for functional related to textures 
// 
// Created:     05.06.24
// ********************************************************************************
#pragma once

#include <cstdint>
#include <string>
#include <vector>


//
// TEXTURES TYPEDEFS
//
using TexID   = uint32_t;                              // texture ID
using TexPath = std::string;                           // texture path
using TexName = std::string;                           // texture name

using TexIDsArr = std::vector<TexID>;                  // array of textures IDs
using TexPathsArr = std::vector<TexPath>;              // array of textures paths
using TexNamesArr = std::vector<TexName>;              // array of textures (semantic) names

//using TexPtrsArr = std::vector<TextureClass*>;         // array of pointers to the textures objects
using SRVsArr = std::vector<ID3D11ShaderResourceView*>;


static const uint32_t TEXTURE_TYPE_COUNT = 22;            // the number of aiTextureType elements
static const TexID INVALID_TEXTURE_ID = 0;           


static const std::vector<std::string> namesOfTexTypes =
{
	// to understand wtf you need to look at the enum aiTextureType 
	// inside of material.h of the ASSIMP
	"NONE",
	"DIFFUSE",
	"SPECULAR",
	"AMBIENT",
	"EMISSIVE",
	"HEIGHT",
	"NORMALS",
	"SHININESS",
	"OPACITY",
	"DISPLACEMENT",
	"LIGHTMAP",
	"REFLECTION",
	"BASE_COLOR",
	"NORMAL_CAMERA",
	"EMISSION_COLOR",
	"METALNESS",
	"DIFFUSE_ROUGHNESS",
	"AMBIENT_OCCLUSION",
	"UNKNOWN",
	"SHEEN",
	"CLEARCOAT",
	"TRANSMISSION",
};