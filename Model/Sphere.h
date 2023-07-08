/////////////////////////////////////////////////////////////////////
// Filename:    Sphere.h
// Description: this class is a representation of a sphere model
// Revising:    01.02.23
/////////////////////////////////////////////////////////////////////
#pragma once


//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include "../Model/Model.h"


//////////////////////////////////
// Class name: Sphere
//////////////////////////////////
class Sphere : public Model
{
public:
	Sphere();
	~Sphere();

	virtual bool Initialize(ID3D11Device* pDevice) override;

private:
	bool InitializeDefault(ID3D11Device* pDevice);
	bool InitializeNew(ID3D11Device* pDevice);

private:
	// a static pointer to the DEFAULT sphere: we use it for not loading sphere data from
	// the data file each time when we create a new sphere model so we just use data of 
	// the DEFAULT sphere
	static Sphere* pDefaultSphere_; 

	Model* pModel_ = nullptr;                     // for using all the model's functional
	std::string modelType_{ "sphere" };           // a type name of the current model
	
};
