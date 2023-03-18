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
#include "shaderclass.h"
#include <map>

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

private:
	static ShadersContainer* pInstance_;   // a static pointer to the class instance

	std::map<std::string, ShaderClass*> shadersMap_;  // contains a shader name and a pointer to the relative shader class
};