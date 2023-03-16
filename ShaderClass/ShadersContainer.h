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

	static ShadersContainer* Get();

	std::map<std::string, ShaderClass*> ShadersContainer::GetShadersList() const;
	ShaderClass* GetShaderByName(const std::string& shaderName) const;
	
	void SetShaderByName(const std::string& shaderName, ShaderClass* pShader);

private:
	static ShadersContainer* pInstance_;

	std::map<std::string, ShaderClass*> shadersMap_;
};