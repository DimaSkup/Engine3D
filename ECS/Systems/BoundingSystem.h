// **********************************************************************************
// Filename:      BoundingSystem.h
// Description:   ECS system for handling bounding data of entts
// 
// Created:       26.09.24
// **********************************************************************************
#pragma once

#include "../Components/Bounding.h"

namespace ECS
{

class BoundingSystem final
{
public:
	BoundingSystem(Bounding* pBoundingComponent);
	~BoundingSystem() {}

	void Add(
		const std::vector<EntityID>& ids,
		const std::vector<DirectX::BoundingBox>& data,
		const std::vector<BoundingType>& types);

	DirectX::BoundingBox GetBoundingDataByID(const EntityID id);

	void GetBoundingDataByIDs(
		const std::vector<EntityID>& ids,
		std::vector<DirectX::BoundingBox>& outData);

	inline const std::vector<DirectX::BoundingBox>& GetBoundingData() const { return pBoundingComponent_->data_; }

private:
	Bounding* pBoundingComponent_ = nullptr;
};



} // namespace ECS