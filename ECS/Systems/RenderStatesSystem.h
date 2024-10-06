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
	using RenderStatesSet = std::set<RENDER_STATES>;

public:
	struct RenderStatesData
	{
		// is used to get entts render states data for rendering

		std::vector<EntityID>      enttsWithDefaultStates_;               // default: fill solid, cull back, no blending, no alpha clipping

		std::vector<EntityID>      enttsAlphaClippingAndCullModelNone_;   // for instance: fooliage, bushes, tree leaves (but no blending)

		std::vector<EntityID>      enttsWithBlending_;
		std::vector<u32>           instancesPerBlendingState_;
		std::vector<RENDER_STATES> blendingStates_;
		std::vector<bool>          alphaClippingForBlendedEntts_;
	};


private:
	// all possible fill/cull/alpha clipping states
	std::vector<RENDER_STATES> allFillModes_ = { FILL_SOLID, FILL_WIREFRAME };
	std::vector<RENDER_STATES> allCullModes_ = { CULL_BACK, CULL_FRONT, CULL_NONE, };
	std::vector<RENDER_STATES> allAlphaClipping_ = { NO_ALPHA_CLIPPING,	ALPHA_CLIPPING };

	// all possible blending states (BS)
	std::vector<RENDER_STATES> allBS_ =
	{
		NO_COLOR_WRITE,
		NO_BLENDING,
		ALPHA_ENABLE,
		ADDING,
		SUBTRACTING,
		MULTIPLYING,
		TRANSPARENCY,
	};

	

public:
	RenderStatesSystem(RenderStates* pRenderStatesComponent);
	~RenderStatesSystem();

	// restrict a copying of this class instance 
	RenderStatesSystem(const RenderStatesSystem& obj) = delete;
	RenderStatesSystem& operator=(const RenderStatesSystem& obj) = delete;

	void AddOrUpdate(
		const std::vector<EntityID>& ids,
		const std::vector<std::set<RENDER_STATES>>& states);

	void GetRenderStates(
		const std::vector<EntityID>& ids,
		RenderStatesData& outData);


	inline void ChangeRenderStateForHash(u32& hash, const RENDER_STATES newState, const u32 disablingMask)
	{
		hash &= disablingMask;    // disable all the other related render states (for instance: disable all blending states)
		hash |= (1 << newState);  // enable some render state
	}


	inline const std::vector<EntityID>& GetAllEnttsIds() const { return pRSComponent_->ids_; }

private:
	void MakeDisablingMasks();

	void GetIdsByIdxs(const std::vector<ptrdiff_t>& idxs, std::vector<EntityID>& outIds)
	{
		// get entts ids from the component by input idxs
		outIds.reserve(std::ssize(idxs));

		for (const ptrdiff_t idx : idxs)
			outIds.push_back(pRSComponent_->ids_[idx]);
	}

	bool CheckEnttsAreNew(const std::vector<EntityID>& ids);

	void AddNewRecords(const std::vector<EntityID>& ids, const std::vector<RenderStatesSet>& states);
	void UpdateRecords(const std::vector<EntityID>& ids, const std::vector<RenderStatesSet>& states);

	void UpdateRenderStatesForHashes(
		const std::vector<RenderStatesSet>& states,
		std::vector<u32>& hashes);


	void GetEnttsByStates(
		const std::vector<ptrdiff_t>& idxsToEntts,
		const std::vector<RENDER_STATES>& states,
		std::vector<EntityID>& outEnttsWithStates,
		std::vector<ptrdiff_t>& outIdxsToEnttsWithoutInputStates);

	void SeparateEnttsByBlendingStates(
		const std::vector<ptrdiff_t>& idxsToEntts,
		std::map<RENDER_STATES, std::vector<EntityID>>& blendStateToId);

	void GetRenderStatesByDataIdxs(
		const std::vector<ptrdiff_t>& idxsToEnttsWithSpecificRS,
		RenderStatesData& outData);

private:


	RenderStates* pRSComponent_ = nullptr;

	// using these hash masks we will get different 
	// render states of entities from responsible hashes
	u32 defaultRenderStatesMask_ = 0;

	// a specific hash to define if entt has any not default render state
	u32 specRenderStates_ = 0;

	u32 specRasterStates_ = 0;                  // to check if entt has any specific raster state except of fill_solid / cull_back
	u32 blendStatesMask_ = 0;                   // to check if entt has any blend state
	u32 alphaClippingMask_ = 0;                 // to check if entt has alpha clipping

	u32 disableAllFillModesMask_ = UINT32_MAX;
	u32 disableAllCullModesMask_ = UINT32_MAX;
	u32 disableAllAlphaClippingMask_ = UINT32_MAX;
	u32 disableAllBlendingMask_ = UINT32_MAX;
	
	
	std::map<u32, RENDER_STATES> hashesToRS_;   // hashes to RASTER states
	std::map<u32, RENDER_STATES> hashesToBS_;   // hashes to BLENDING states
};


}  // namespace ECS