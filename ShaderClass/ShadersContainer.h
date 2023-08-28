////////////////////////////////////////////////////////////////////
// Filename:        ShadersContainer.h
// Description:     this class has an associative array ['shaderName' => shader_ptr]
//                  and some function for work with it
//
// Created:         16.03.23
////////////////////////////////////////////////////////////////////
#pragma once


//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <map>


#include "shaderclass.h"   // basic shaders class

// the list of all the internal shaders
#include "../ShaderClass/colorshaderclass.h"           // for rendering models with only colour but not textures
#include "../ShaderClass/textureshaderclass.h"         // for texturing models
#include "../ShaderClass/LightShaderClass.h"           // for light effect on models
#include "../ShaderClass/TerrainShaderClass.h"         // for rendering the terrain 
#include "../ShaderClass/SpecularLightShaderClass.h"   // for light effect with specular
#include "../ShaderClass/MultiTextureShaderClass.h"    // for multitexturing
#include "../ShaderClass/LightMapShaderClass.h"        // for light mapping
#include "../ShaderClass/AlphaMapShaderClass.h"        // for alpha mapping
#include "../ShaderClass/BumpMapShaderClass.h"         // for bump mapping
#include "../ShaderClass/CombinedShaderClass.h"        // for different shader effects at once (multitexturing, lighting, alpha mapping, etc.)
#include "../ShaderClass/SkyDomeShaderClass.h"         // for rendering the sky dome
#include "../ShaderClass/SkyPlaneShaderClass.h"        // for rendering the sky plane
#include "../ShaderClass/DepthShaderClass.h"           // for coloring objects according to its depth position
#include "../ShaderClass/PointLightShaderClass.h"      // for point lighting


//////////////////////////////////
//////////////////////////////////
class ShadersContainer
{
public:
	ShadersContainer();
	~ShadersContainer();

	// getters
	static ShadersContainer* Get();
	std::map<std::string, ShaderClass*> ShadersContainer::GetShadersList() const;
	ShaderClass* GetShaderByName(const std::string& shaderName) const;
	
	// setters
	void SetShaderByName(const std::string& shaderName, ShaderClass* pShader);


private:  // restrict a copying of this class instance
	ShadersContainer(const ShadersContainer & obj); 
	ShadersContainer & operator=(const ShadersContainer & obj);

private:
	static ShadersContainer* pInstance_;              // a static pointer to the class instance
	std::map<std::string, ShaderClass*> shadersMap_;  // contains a shader name and a pointer to the relative shader class
};