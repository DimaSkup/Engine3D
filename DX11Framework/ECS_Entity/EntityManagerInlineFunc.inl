
#if 0
inline bool EntityManager::CheckEntityHasComponent(const EntityID& entityID, const ComponentType componentType)
{
	return entityToComponent_.at(entityID).contains(componentType);
}

inline bool EntityManager::CheckEntityExist(const EntityID& entityID) { return entityToComponent_.contains(entityID); }

inline const std::map<ComponentType, ComponentID>& EntityManager::GetPairsOfComponentTypeToName() { return componentTypeToName_; }
#endif