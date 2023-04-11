////////////////////////////////////////////////////////////////////
// Filename:        ShadersContainer.cpp
// Description:     this class has an associative array ['shaderName' => shader_ptr]
//                  and some function for work with it
//
// Created:         16.03.23
////////////////////////////////////////////////////////////////////
#include "ShadersContainer.h"


// a static pointer to the instance of the class
ShadersContainer* ShadersContainer::pInstance_ = nullptr;  

// the constructor
ShadersContainer::ShadersContainer()
{
	if (pInstance_)
	{
		COM_ERROR_IF_FALSE(false, "you can't have more than only one shader container");
		exit(-1);
	}
	else
	{
		pInstance_ = this;
		Log::Debug(THIS_FUNC_EMPTY);
	}
}


ShadersContainer::~ShadersContainer()
{
	// release shaders
	if (!shadersMap_.empty())
	{
		for (auto & elem : shadersMap_)  // delete each shader object from the memory
		{
			_DELETE(elem.second);
		}

		shadersMap_.clear();
	}
}


// a static function to get a pointer to the instance of the class
ShadersContainer* ShadersContainer::Get()
{
	return ShadersContainer::pInstance_;
}

std::map<std::string, ShaderClass*> ShadersContainer::GetShadersList() const
{
	return this->shadersMap_;
}


ShaderClass* ShadersContainer::GetShaderByName(const std::string& shaderName) const
{
	assert(shaderName.empty() != true);

	auto it = shadersMap_.find(shaderName);
	if (it == shadersMap_.end())
	{
		std::string debugMsg{ "There is no shaders with such a name: \"" + shaderName + "\"" };
		COM_ERROR_IF_FALSE(false, debugMsg.c_str());
	}

	return shadersMap_.at(shaderName);
}


void ShadersContainer::SetShaderByName(const std::string& shaderName, ShaderClass* pShader)
{
	assert(shaderName.empty() != true);
	assert(pShader);

	Log::Debug(THIS_FUNC, shaderName.c_str());

	shadersMap_.insert({ shaderName, pShader });
}