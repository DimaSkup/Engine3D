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

#include "colorshaderclass.h"           // for rendering models with only colour but not textures
#include "Textureshaderclass.h"         // for texturing models
#include "LightShaderClass.h"           // for light effect on models
#include "fontshaderclass.h"            // for rendering text onto the screen

namespace Render
{
	struct ShadersContainer
	{
		ColorShaderClass      colorShader_;
		TextureShaderClass    textureShader_;
		LightShaderClass      lightShader_;
		FontShaderClass       fontShader_;
	};
}