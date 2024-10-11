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

//
// TEXTURES TYPEDEFS
//
using TexID   = uint32_t;                              // texture ID
using TexPath = std::string;                           // texture path
using TexName = std::string;                           // texture name

using SRV = ID3D11ShaderResourceView;

const uint32_t TEXTURE_TYPE_COUNT = 22;            // the number of aiTextureType elements
//const TexID    INVALID_TEXTURE_ID = 0;           


static const std::string namesOfTexTypes[] =
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