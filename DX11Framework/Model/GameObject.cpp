#include "GameObject.h"



GameObject::GameObject(Model* pModel)
{
	try
	{
		// check input params
		COM_ERROR_IF_NULLPTR(pModel, "the input model == nullptr");

		// create a GameObjectData object which contains common data
		// of the current game object;
		// it is automatically setup with default values;
		this->pGameObjData_ = std::make_unique<GameObjectData>();

		// set a model for this game object
		this->pModel_ = pModel;

		// also we init the game object's ID with the name of the model's type;
		// NOTE: don't do this after this game object was added into the game objects list
		// instead of it you have to rename the game object manually inside of the game object list
		this->ID_ = pModel->GetModelType();
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
	// each game object have to setup the data container for shaders with its own data
	// so we do it here
	DataContainerForShaders* pDataContainer = pModel_->GetDataContainerForShaders();

	pDataContainer->world = this->GetData()->GetWorldMatrix();
	pDataContainer->modelColor = this->GetData()->GetColor();

	pModel_->Render(topologyType);

	return;
}

///////////////////////////////////////////////////////////

void GameObject::SetModel(Model* pModel)
{
	// set a model for this game object
	COM_ERROR_IF_NULLPTR(pModel, "the input ptr to model == nullptr");

	// if there was some another model we delete it
	_DELETE(pModel_);

	// set a new model for this game object
	this->pModel_ = pModel;
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


