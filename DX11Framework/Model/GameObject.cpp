#include "GameObject.h"



GameObject::GameObject()
{
	try
	{
		// create a GameObjectData object which contains common data
		// of the current game object;
		// it is automatically setup with default values;
		pGameObjData_ = std::make_unique<GameObjectData>();
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(THIS_FUNC, e.what());
		COM_ERROR_IF_FALSE(false, "can't allocate memory for a game object members");
	}
}





////////////////////////////////////////////////////////////////////////////////////////////
//
//                                PUBLIC FUNCTIONS
//
////////////////////////////////////////////////////////////////////////////////////////////



bool GameObject::Initialize(ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext) 
{


	return true;
}

///////////////////////////////////////////////////////////

void GameObject::Shutdown() 
{
	_DELETE(this->pModel_);

	return;
}

///////////////////////////////////////////////////////////

void GameObject::Render(D3D_PRIMITIVE_TOPOLOGY topologyType)
{
	pModel_->Render(topologyType);

	return;
}

///////////////////////////////////////////////////////////

void GameObject::SetModel(Model* pModel)
{
	// set a model for this game object
	COM_ERROR_IF_NULLPTR(pModel, "input ptr to model == nullptr");
	this->pModel_ = pModel;

	// also we init the game object's ID with the name of the model's type
	this->ID_ = pModel->GetModelType();
}

///////////////////////////////////////////////////////////

const std::string & GameObject::GetID() const
{
	// returns a model's ID
	return this->ID_;
}

///////////////////////////////////////////////////////////

void GameObject::SetID(const std::string & newID)
{
	// set an identifier of the game object;
	//
	// NOTE: the ID of the game object can be changed when we add this game object
	//       into the GameObjectsList because there can't be the same IDs;
	COM_ERROR_IF_FALSE(newID.empty() == false, "wrong ID");
	ID_ = newID;
}


