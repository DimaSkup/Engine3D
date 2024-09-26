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
		const std::vector<BoundingData>& data,
		const std::vector<BoundingType>& types);

	inline const std::vector<BoundingData>& GetBoundingData() const { return pBoundingComponent_->data_; }

private:
	Bounding* pBoundingComponent_ = nullptr;
};



} // namespace ECS