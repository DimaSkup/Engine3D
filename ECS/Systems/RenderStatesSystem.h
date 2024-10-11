// *********************************************************************************
// Filename:     RenderStatesSystem.h
// Description:  an ECS system which is responsible for
//               handling blending of the entities;
//
// Created:      28.08.24
// *********************************************************************************
#pragma once

#include "../Components/RenderStates.h"
#include <set>
#include <map>

namespace ECS
{



class RenderStatesSystem final
{
	using RenderStatesTypesSet = std::set<RenderStatesTypes>;

public:

	struct EnttsDefaultState
	{
		std::vector<EntityID>      ids_;                              // default: fill solid, cull back, no blending, no alpha clipping
		const std::vector<RenderStatesTypes> states_ =
		{
			FILL_SOLID ,
			CULL_BACK,
			FRONT_COUNTER_CLOCKWISE,
			NO_BLENDING,
			NO_ALPHA_CLIPPING
		};

		void Clear() { ids_.clear(); }
	};

	// ------------------------------------------------

	struct EnttsAlphaClipping
	{
		std::vector<EntityID>      ids_;                              // for instance: fooliage, bushes, tree leaves (but no blending)
		const std::vector<RenderStatesTypes> states_ = { CULL_NONE, ALPHA_CLIPPING };

		void Clear() { ids_.clear(); }
	};

	// ------------------------------------------------

	struct EnttsBlended
	{
		std::vector<EntityID>      ids_;
		std::vector<u32>           instancesPerBlendingState_;
		std::vector<RenderStatesTypes> states_;                   // each instances set has its own blending state

		void Clear() 
		{ 
			ids_.clear();
			instancesPerBlendingState_.clear();
			states_.clear();
		}
	};

	// ------------------------------------------------

	struct EnttsReflection
	{
		std::vector<EntityID>      ids_;                              // reflection planes
		RenderStatesTypes          states_ = TRANSPARENCY;  // we need to see through reflection planes

		void Clear() { ids_.clear(); }
	};

	// ------------------------------------------------


	struct EnttsRenderStatesData
	{
		// is used to get entts render states data for rendering

		EnttsDefaultState  enttsDefault_;
		EnttsAlphaClipping enttsAlphaClipping_;
		EnttsBlended       enttsBlended_;
		EnttsReflection    enttsReflection_;
		
		void Clear()
		{
			enttsDefault_.Clear();
			enttsAlphaClipping_.Clear();
			enttsBlended_.Clear();
			enttsReflection_.Clear();
		}
	};


public:
	RenderStatesSystem(RenderStates* pRenderStatesComponent);
	~RenderStatesSystem();

	// restrict a copying of this class instance 
	RenderStatesSystem(const RenderStatesSystem& obj) = delete;
	RenderStatesSystem& operator=(const RenderStatesSystem& obj) = delete;

	void AddOrUpdate(
		const std::vector<EntityID>& ids,
		const std::vector<std::set<RenderStatesTypes>>& states);

	void GetRenderStates(
		const std::vector<EntityID>& ids,
		EnttsRenderStatesData& outData);

	inline void ChangeRenderStateForHash(u32& hash, const RenderStatesTypes newState, const u32 disablingMask)
	{
		hash &= disablingMask;    // disable all the other related render states (for instance: disable all blending states)
		hash |= (1 << newState);  // enable some render state
	}


	inline const std::vector<EntityID>& GetAllEnttsIds() const { return pRSComponent_->ids_; }

private:
	void MakeDisablingMasks();
	bool CheckEnttsAreNew(const std::vector<EntityID>& ids);

	void AddNewRecords(const std::vector<EntityID>& ids, const std::vector<RenderStatesTypesSet>& states);
	void UpdateRecords(const std::vector<EntityID>& ids, const std::vector<RenderStatesTypesSet>& states);

	void UpdateRenderStatesTypesForHashes(
		const std::vector<RenderStatesTypesSet>& states,
		std::vector<u32>& hashes);

	void GetEnttsByStates(
		const std::vector<ptrdiff_t>& idxsToEntts,
		const std::vector<RenderStatesTypes>& states,
		std::vector<EntityID>& outEnttsWithStates,
		std::vector<ptrdiff_t>& outIdxsToEnttsWithoutInputStates);

	void SeparateEnttsByBlendingStates(
		const std::vector<ptrdiff_t>& idxsToEntts,
		std::map<RenderStatesTypes, std::vector<EntityID>>& blendStateToId);

	void GetRenderStatesTypesByDataIdxs(
		const std::vector<ptrdiff_t>& idxsToEnttsWithSpecificRS,
		EnttsRenderStatesData& outData);

	void GetIdsByIdxs(
		const std::vector<ptrdiff_t>& idxs,
		std::vector<EntityID>& outIds);


private:


	RenderStates* pRSComponent_ = nullptr;

	// all possible fill/cull/alpha clipping states
	std::vector<RenderStatesTypes> allFillModes_ = { FILL_SOLID, FILL_WIREFRAME };
	std::vector<RenderStatesTypes> allCullModes_ = { CULL_BACK, CULL_FRONT, CULL_NONE, };
	std::vector<RenderStatesTypes> allAlphaClipping_ = { NO_ALPHA_CLIPPING,	ALPHA_CLIPPING };

	// all possible blending states (BS)
	std::vector<RenderStatesTypes> allBS_ =
	{
		NO_COLOR_WRITE,
		NO_BLENDING,
		ALPHA_ENABLE,
		ADDING,
		SUBTRACTING,
		MULTIPLYING,
		TRANSPARENCY,
	};

	// using these hash masks we will get different 
	// render states of entities from responsible hashes
	u32 defaultRenderStatesTypesMask_ = 0;

	// a specific hash to define if entt has any not default render state
	u32 specRenderStatesTypes_ = 0;

	u32 specRasterStates_ = 0;                  // to check if entt has any specific raster state except of fill_solid / cull_back
	u32 blendStatesMask_ = 0;                   // to check if entt has any blend state
	u32 alphaClippingMask_ = 0;                 // to check if entt has alpha clipping

	u32 disableAllFillModesMask_     = UINT32_MAX;
	u32 disableAllCullModesMask_     = UINT32_MAX;
	u32 disableAllAlphaClippingMask_ = UINT32_MAX;
	u32 disableAllBlendingMask_      = UINT32_MAX;
	
	
	std::map<u32, RenderStatesTypes> hashesToRS_;   // hashes to RASTER states
	std::map<u32, RenderStatesTypes> hashesToBS_;   // hashes to BLENDING states
};


}  // namespace ECS