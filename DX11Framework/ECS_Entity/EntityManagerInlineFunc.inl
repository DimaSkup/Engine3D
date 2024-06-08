
#if 0
inline bool EntityManager::CheckEntityHasComponent(const entitiesName& entitiesName, const ComponentType componentType)
{
	return entityToComponent_.at(entitiesName).contains(componentType);
}

inline bool EntityManager::CheckEntityExist(const entitiesName& entitiesName) { return entityToComponent_.contains(entitiesName); }

inline const std::map<ComponentType, ComponentID>& EntityManager::GetPairsOfComponentTypeToName() { return componentTypeToName_; }
#endif