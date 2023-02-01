////////////////////////////////////////////////////////////////////
// Filename:    Square.cpp
// Description: realization of a 2D square model functional
// Revising:    07.12.22
////////////////////////////////////////////////////////////////////
#include "Square.h"

Square::Square()
{
}

// initialize a 2D Square
bool Square::Initialize(ID3D11Device* pDevice, 
	                    const std::string& modelId)
{
	Log::Debug(THIS_FUNC_EMPTY);

	// set what kind of model we want to init
	this->SetModel("internal/square");

	bool result = ModelClass::Initialize(pDevice, modelId);
	COM_ERROR_IF_FALSE(result, "can't initialize a 2D square object");
	
	/*
	string debugMsg = modelId + " is initialized successfully";
	Log::Debug(THIS_FUNC, debugMsg.c_str());
	*/
	
	return true;
}
