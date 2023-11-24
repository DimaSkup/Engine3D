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

	virtual T* GetInstance(ModelInitializerInterface* pModelInitializer)
	{
		try
		{
			return new T(pModelInitializer);
		}
		catch (std::bad_alloc & e)
		{
			Log::Error(THIS_FUNC, e.what());
			COM_ERROR_IF_FALSE(false, "can't allocate memory for new model object");
		}
	}
};
