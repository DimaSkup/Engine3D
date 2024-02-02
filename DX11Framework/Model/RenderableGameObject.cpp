#include "RenderableGameObject.h"



RenderableGameObject::~RenderableGameObject()
{
}






////////////////////////////////////////////////////////////////////////////////////////////
//
//                                  PUBLIC FUNCTIONS
//
////////////////////////////////////////////////////////////////////////////////////////////



#if 0
///////////////////////////////////////////////////////////

void RenderableGameObject::RenderSprite()
{
	// ATTENTION: we don't set here a values for the world matrix because we use
	// the WVP (world * view * ortho) matrix which we've already set this value into the 
	// data_container_for_shaders for rendering of this 2D sprite onto the screen using
	// the SpriteShaderClass;

	// polymorph call of the SpriteClass's rendering function
	pModel_->Render();
}

///////////////////////////////////////////////////////////

Model* RenderableGameObject::GetModel() const
{
	// returns a pointer to the model of the game object

	COM_ERROR_IF_NULLPTR(this, "this == nullptr");
	return this->pModel_;
}

///////////////////////////////////////////////////////////

void RenderableGameObject::SetModel(Model* pModel)
{
	// set a model for this game object
	COM_ERROR_IF_NULLPTR(pModel, "the input ptr to model == nullptr");

	// if there was some another model we delete it
	_DELETE(pModel_);

	// set a new model for this game object
	this->pModel_ = pModel;
}
#endif
///////////////////////////////////////////////////////////

void RenderableGameObject::UpdateMatrix()
{
	// when we execute some manipulations with a renderable game object 
	// we have to compute a new world matrix for it
	worldMatrix_ = scalingMatrix_ * rotationMatrix_ * translationMatrix_;
}

///////////////////////////////////////////////////////////