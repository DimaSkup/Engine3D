#include "BoundingSystem.h"

#include "../Common/Assert.h"
#include "../Common/Utils.h"

namespace ECS
{


BoundingSystem::BoundingSystem(Bounding* pBoundingComponent) :
	pBoundingComponent_(pBoundingComponent)
{
	Assert::NotNullptr(pBoundingComponent, "ptr to the bounding component == nullptr");
}

///////////////////////////////////////////////////////////

void BoundingSystem::Add(
	const std::vector<EntityID>& ids,
	const std::vector<DirectX::BoundingBox>& data,
	const std::vector<BoundingType>& types)
{
	Bounding& component = *pBoundingComponent_;

	// check if we can add each input entt ID
	bool canAddComponent = !Utils::CheckValuesExistInSortedArr(component.ids_, ids);
	Assert::True(canAddComponent, "can't add component: there is already a record with some entity id");

	// execute sorted insertion of the data
	for (size idx = 0; idx < std::ssize(ids); ++idx)
	{
		const ptrdiff_t insertAt = Utils::GetPosForID(component.ids_, ids[idx]);

		Utils::InsertAtPos(component.ids_, insertAt, ids[idx]);
		Utils::InsertAtPos(component.data_, insertAt, data[idx]);
		Utils::InsertAtPos(component.types_, insertAt, types[idx]);
	}
}

///////////////////////////////////////////////////////////

DirectX::BoundingBox BoundingSystem::GetBoundingDataByID(const EntityID id)
{
	const Bounding& component = *pBoundingComponent_;

	// if entt by input ID doesn't have AABB we just return the default one
	if (!Utils::BinarySearch(component.ids_, id))
		return DirectX::BoundingBox();

	ptrdiff_t idx = Utils::GetIdxInSortedArr(component.ids_, id);
	return component.data_[idx];
}

///////////////////////////////////////////////////////////

void BoundingSystem::GetBoundingDataByIDs(
	const std::vector<EntityID>& ids,
	std::vector<DirectX::BoundingBox>& outData)
{
	// get an arr of AABB data by input entts IDs

	const Bounding& component = *pBoundingComponent_;
	std::vector<bool> flags;
	std::vector<ptrdiff_t> idxs;

	Utils::GetExistingFlags(component.ids_, ids, flags);
	Utils::GetIdxsInSortedArr(component.ids_, ids, idxs);

	const size enttsCount = std::ssize(ids);
	outData.reserve(enttsCount);

	// if entt by i has an AABB we get it from the component or set default AABB in another case
	for (size i = 0; i < enttsCount; ++i)
		outData.emplace_back((flags[i]) ? component.data_[idxs[i]] : DirectX::BoundingBox());
}


} // namespace ECS