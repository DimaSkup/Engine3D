#pragma once

#include "BasicGameObjectCreator.h"


template<class T>
class GameObjectCreator : public BasicGameObjectCreator
{
public:
	GameObjectCreator(GameObjectsListClass* pGameObjList)
		: BasicGameObjectCreator(pGameObjList)
	{
	}

	virtual T* GetInstance(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	{
		try
		{
			return new T(pDevice, pDeviceContext);
		}
		catch (std::bad_alloc & e)
		{
			Log::Error(LOG_MACRO, e.what());
			COM_ERROR_IF_FALSE(false, "can't allocate memory for new model object");
		}
	}
};
