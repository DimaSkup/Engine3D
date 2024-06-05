#pragma once

#include <map>
#include <string>

enum RENDERING_SHADERS
{
	COLOR_SHADER,
	TEXTURE_SHADER,
	LIGHT_SHADER
};

static std::map<RENDERING_SHADERS, std::string> g_ShaderTypeToShaderName =
{
	{ RENDERING_SHADERS::COLOR_SHADER, "ColorShader" },
	{ RENDERING_SHADERS::TEXTURE_SHADER, "TextureShader" },
	{ RENDERING_SHADERS::LIGHT_SHADER, "LightShader" },
};

