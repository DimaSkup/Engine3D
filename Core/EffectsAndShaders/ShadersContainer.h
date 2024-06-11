////////////////////////////////////////////////////////////////////
// Filename:        ShadersContainer.h
// Description:     
//
// Created:         16.03.23
////////////////////////////////////////////////////////////////////
#pragma once


//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include "shaderclass.h"   // basic shaders class

#include "../EffectsAndShaders/colorshaderclass.h"           // for rendering models with only colour but not textures
#include "../EffectsAndShaders/textureshaderclass.h"         // for texturing models
#include "../EffectsAndShaders/LightShaderClass.h"           // for light effect on models

namespace Shaders
{
	struct ShadersContainer
	{
		ColorShaderClass      colorShader_;
		TextureShaderClass    textureShader_;
		LightShaderClass      lightShader_;
	};
}