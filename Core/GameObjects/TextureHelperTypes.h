// ********************************************************************************
// Filename:    TextureHelperTypes.h
// Description: contains some common types for functional related to textures 
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
static const uint32_t TEXTURE_TYPE_COUNT = 22;            // the number of aiTextureType elements

using TexID   = uint32_t;                              // texture ID
using TexPath = std::string;                           // texture path
using TexName = std::string;                           // texture name

using TexIDsArr = std::vector<TexID>;                  // array of textures IDs
using TexPathsArr = std::vector<TexPath>;              // array of textures paths
using TexNamesArr = std::vector<TexName>;              // array of textures (semantic) names

//using TexPtrsArr = std::vector<TextureClass*>;         // array of pointers to the textures objects
using SRVsArr = std::vector<ID3D11ShaderResourceView*>;

