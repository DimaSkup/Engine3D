#include "BoundingSystem.h"

#include "../Common/Assert.h"
#include "../Common/Utils.h"
#include "Utils/SysUtils.h"

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
	const std::vector<BoundingData>& data,
	const std::vector<BoundingType>& types)
{
	Bounding& component = *pBoundingComponent_;

	// check if we can add each input entt ID
	bool canAddComponent = SysUtils::RecordsNotExist(component.ids_, ids);
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



} // namespace ECS