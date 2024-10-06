// *********************************************************************************
// Filename:     RenderStatesSystem.cpp
// Description:  implementation of the RenderStatesSystem functional
// 
// Created:      29.08.24
// *********************************************************************************
#include "RenderStatesSystem.h"

#include "../Common/Assert.h"
#include "../Common/Utils.h"
#include "../Common/log.h"

namespace ECS
{

using namespace Utils;
	
RenderStatesSystem::RenderStatesSystem(RenderStates* pRenderStatesComponent)
{
	Assert::NotNullptr(pRenderStatesComponent, "a ptr to the component == nullptr");
	pRSComponent_ = pRenderStatesComponent;

	// 
	const std::vector<RENDER_STATES> defaultRenderStates =
	{
		FILL_SOLID,
		CULL_BACK,
		NO_BLENDING,
		NO_ALPHA_CLIPPING
	};

	const std::vector<RENDER_STATES> specRasterStates =
	{
		FILL_WIREFRAME,
		CULL_FRONT,
		CULL_NONE,
	};

	const std::vector<RENDER_STATES> blendingStates =
	{
		ALPHA_ENABLE,
		ADDING,
		SUBTRACTING,
		MULTIPLYING,
		TRANSPARENCY,
	};

	
	// a hash for two default render states (no blending, no alpha clipping, etc.)
	for (const RENDER_STATES rs : defaultRenderStates)
		defaultRenderStatesMask_ |= (1 << rs);

	// hash for all not default raster states
	for (const RENDER_STATES rs : specRasterStates)
		specRasterStates_ |= (1 << rs);

	// we'll use this hash to get entts which must be blended
	for (const RENDER_STATES bs : blendingStates)
		blendStatesMask_ |= (1 << bs);

	// by this hash we define if entt has an alpha clipping state
	alphaClippingMask_ |= (1 << ALPHA_CLIPPING);

	// define a hash which will be used to get entts with specific render states
	specRenderStates_ |= ~defaultRenderStatesMask_;

	// ---------------------------------------------
	
	// make a map of pairs ['rs_hash' => 'rs_state']
	//for (const RENDER_STATES rs : rasterStates)
	//	hashesToRS_.insert({ (1 << rs), rs });

	// make a map of pairs ['bs_hash' => 'bs_state']
	for (const RENDER_STATES bs : blendingStates)
		hashesToBS_.insert({ (1 << bs), bs });


	MakeDisablingMasks();
}

///////////////////////////////////////////////////////////

RenderStatesSystem::~RenderStatesSystem()
{
	pRSComponent_ = nullptr;
	hashesToRS_.clear();
	hashesToBS_.clear();
}

///////////////////////////////////////////////////////////

void RenderStatesSystem::AddOrUpdate(
	const std::vector<EntityID>& ids,
	const std::vector<std::set<RENDER_STATES>>& states)  // each entt has its own set of render states
{
	Assert::True(ids.size() == states.size(), "the number of IDs and states must be equal");
	
	RenderStates& comp = *pRSComponent_;
	std::vector<u32> statesHashes(ids.size(), 0);



	// if all input entts are new we just execute sorted insertion 
	// of new records into the component
	if (CheckEnttsAreNew(ids))
	{
		AddNewRecords(ids, states);
	}
	// some entts are new and some entts must be updates
	{
		std::vector<bool> isInComponent;
		std::vector<EntityID> enttsToAdd;
		std::vector<EntityID> enttsToUpdate;
		std::vector<RenderStatesSet> statesToAdd;
		std::vector<RenderStatesSet> statesToUpdate;

		// define which entts are already in the component and which are not
		for (const EntityID id : ids)
			isInComponent.push_back(BinarySearch(comp.ids_, id));

		// separate entts (some we will just add, and other we have to update)
		for (u32 idx = 0; idx < (u32)ids.size(); ++idx)
		{
			// if there is already such an id
			if (isInComponent[idx])
			{
				enttsToUpdate.push_back(ids[idx]);
				statesToUpdate.push_back(states[idx]);
			}
			else
			{
				enttsToAdd.push_back(ids[idx]);
				statesToAdd.push_back(states[idx]);
			}
		}

		AddNewRecords(enttsToAdd, statesToAdd);
		UpdateRecords(enttsToUpdate, statesToUpdate);
	}
}

///////////////////////////////////////////////////////////

void RenderStatesSystem::AddNewRecords(
	const std::vector<EntityID>& ids,
	const std::vector<RenderStatesSet>& states)
{
	// add new records with input entts ids and render states

	Assert::True(ids.size() == states.size(), "input arrays must be equal");
	std::vector<u32> hashes(std::ssize(ids), 0);

	// generate a hash for each states set of each input entt
	for (size idx = 0; idx < std::ssize(states); ++idx)
	{
		for (const RENDER_STATES state : states[idx])
			hashes[idx] |= (1 << state);
	}

	// add records
	for (u32 idx = 0; idx < (u32)ids.size(); ++idx)
	{
		RenderStates& comp = *pRSComponent_;
		const ptrdiff_t pos = GetPosForID(comp.ids_, ids[idx]);

		InsertAtPos(comp.ids_, pos, ids[idx]);
		InsertAtPos(comp.statesHashes_, pos, hashes[idx]);
	}
}

///////////////////////////////////////////////////////////

void RenderStatesSystem::UpdateRecords(
	const std::vector<EntityID>& ids,
	const std::vector<RenderStatesSet>& states)
{
	// update records by ids with new render states values

	Assert::True(ids.size() == states.size(), "input arrays must be equal");

	RenderStates& comp = *pRSComponent_;
	const size enttsCount = std::ssize(ids);
	std::vector<ptrdiff_t> idxs;
	std::vector<u32> rsHashes(enttsCount, 0);

	// get data idxs to input entts ids
	Utils::GetIdxsInSortedArr(comp.ids_, ids, idxs);

	// get render states hashes which are related to the input entts
	for (size i = 0; i < enttsCount; ++i)
		rsHashes[i] = comp.statesHashes_[idxs[i]];

	// compute new render states hashes
	UpdateRenderStatesForHashes(states, rsHashes);

	// store updated render states hashes by idxs
	for (size i = 0; i < enttsCount; ++i)
		comp.statesHashes_[idxs[i]] = rsHashes[i];
}

///////////////////////////////////////////////////////////

void RenderStatesSystem::UpdateRenderStatesForHashes(
	const std::vector<RenderStatesSet>& states,
	std::vector<u32>& hashes)
{
	// go through each input hash and update its values according
	// to related input render states

	for (size idx = 0; idx < std::ssize(hashes); ++idx)
	{
		u32& hash = hashes[idx];

		for (const RENDER_STATES state : states[idx])
		{
			switch (state)
			{
				case FILL_SOLID:
				case FILL_WIREFRAME:
				{
					ChangeRenderStateForHash(hash, state, disableAllFillModesMask_);
					break;
				}
				case CULL_BACK:
				case CULL_FRONT:
				case CULL_NONE:
				{
					ChangeRenderStateForHash(hash, state, disableAllCullModesMask_);
					break;
				}
				case NO_COLOR_WRITE:
				case NO_BLENDING:
				case ALPHA_ENABLE:
				case ADDING:
				case SUBTRACTING:
				case MULTIPLYING:
				case TRANSPARENCY:
				{
					ChangeRenderStateForHash(hash, state, disableAllBlendingMask_);
					break;
				}
				case NO_ALPHA_CLIPPING:
				case ALPHA_CLIPPING:
				{
					ChangeRenderStateForHash(hash, state, disableAllAlphaClippingMask_);
					break;
				}
				default:
				{
					Log::Error("unknown render state: " + std::to_string(state));
				}
			}
		}
	}
	
}

///////////////////////////////////////////////////////////

void RenderStatesSystem::GetRenderStates(
	const std::vector<EntityID>& ids,
	RenderStatesData& outData)
{
	const RenderStates& component = *pRSComponent_;

	std::vector<ptrdiff_t> idxs;
	std::vector<bool> isWithSpecRenderState;
	const size idsCount = std::ssize(ids);

	GetIdxsInSortedArr(component.ids_, ids, idxs);

	// check if entt by such idx has any specific render state
	isWithSpecRenderState.resize(idsCount);

	for (u32 i = 0; const ptrdiff_t idx : idxs)
		isWithSpecRenderState[i++] = (component.statesHashes_[idx] & specRenderStates_);

	

	// IS BRANCHLESS AND FASTER WAY? -- TODO: MEASUREMENT
#if 1
	std::vector<ptrdiff_t> idxsWithDefaultRS;  // RS -- render state
	std::vector<ptrdiff_t> idxsToEnttsWithSpecificRS;
	std::vector<std::vector<ptrdiff_t>*> ptrsToOutArrs =
	{ 
		&idxsWithDefaultRS,
		&idxsToEnttsWithSpecificRS,
	};

	for (ptrdiff_t idx = 0; idx < idsCount; ++idx)
		ptrsToOutArrs[isWithSpecRenderState[idx]]->push_back(idxs[idx]);
#else
	std::vector<ptrdiff_t> idxsWithDefaultRS;  // here RS -- render state
	std::vector<ptrdiff_t> idxsToEnttsWithSpecificRS;

	// separate idxs into two arrays; one of them is to entts 
	// that has default RS; the other is to entts with specific RS (blending, etc.)
	for (ptrdiff_t i = 0; i < idsCount; ++i)
	{
		if (isWithSpecRenderState[i])
			idxsToEnttsWithSpecificRS.push_back(idxs[i]);
		else
			idxsWithDefaultRS.push_back(idxs[i]);
	}
#endif

	// get ids of entts which have default render states
	GetIdsByIdxs(idxsWithDefaultRS, outData.enttsWithDefaultStates_);

	// get data for entts with specific render states
	GetRenderStatesByDataIdxs(idxsToEnttsWithSpecificRS, outData);
}

///////////////////////////////////////////////////////////

void RenderStatesSystem::GetRenderStatesByDataIdxs(
	const std::vector<ptrdiff_t>& idxsToEnttsWithSpecificRS,
	RenderStatesData& outData)
{
	// get ids of entts with specific render states and its render states;
	// (under specific render state means: any blending, enabled alpha clipping, etc.)

	const RenderStates& comp = *pRSComponent_;
	std::vector<ptrdiff_t> idxsToEntts;
	std::map<RENDER_STATES, std::vector<EntityID>> stateToEnttsIDs;


	// get entts aka tree leaves, bushes, grass
	GetEnttsByStates(
		idxsToEnttsWithSpecificRS,
		{ FILL_SOLID, CULL_NONE, NO_BLENDING, ALPHA_CLIPPING },
		outData.enttsAlphaClippingAndCullModelNone_,
		idxsToEntts);
	
	SeparateEnttsByBlendingStates(idxsToEntts, stateToEnttsIDs);

	// put into the output array the entts by blending states in order: 
	// 1. NO_BLENDING (no blending), 2. blending states,  3. TRANSPARENCY
	const size blendStatesCount = std::ssize(stateToEnttsIDs);

	outData.enttsWithBlending_.reserve(std::ssize(idxsToEntts));
	outData.instancesPerBlendingState_.reserve(blendStatesCount);
	outData.blendingStates_.reserve(blendStatesCount);

	// sort entts ids by blending states
	for (const auto& it : stateToEnttsIDs)
		Utils::AppendArray(outData.enttsWithBlending_, it.second);
		
	// store instances per blending state
	for (const auto& it : stateToEnttsIDs)
		outData.instancesPerBlendingState_.push_back((u32)std::ssize(it.second));

	// store blending states
	for (const auto& it : stateToEnttsIDs)
		outData.blendingStates_.push_back(it.first);
}

///////////////////////////////////////////////////////////

void RenderStatesSystem::GetEnttsByStates(
	const std::vector<ptrdiff_t>& idxsToEntts,
	const std::vector<RENDER_STATES>& states,
	std::vector<EntityID>& outEnttsWithStates,
	std::vector<ptrdiff_t>& outIdxsToOther)     // to other entts that don't fit to the input states
{
	// out: 1. arr of entts ids which have such set of states
	//      2. arr of idxs to entts which don't have such set of states


	// arrays of idxs to entts
	std::vector<ptrdiff_t> idxsToFit;
	std::vector<std::vector<ptrdiff_t>*> ptrsToOutArrs = { &outIdxsToOther, &idxsToFit };

	// make a hash mask by input states
	u32 hashMask = 0;

	for (const RENDER_STATES rs : states)
		hashMask |= (1 << rs);

	// go through each input idx and filter the ones that fit to the input set of states
	for (const ptrdiff_t idx : idxsToEntts)
	{
		// has == 0 -> idxsToOther
		// has == 1 -> idxsOnlyWithAlphaClipping
		bool fit = (pRSComponent_->statesHashes_[idx] == hashMask);
		ptrsToOutArrs[fit]->push_back(idx);
	}

	// get ids to entts which fit the input set of states
	GetIdsByIdxs(idxsToFit, outEnttsWithStates);
}

///////////////////////////////////////////////////////////

void RenderStatesSystem::SeparateEnttsByBlendingStates(
	const std::vector<ptrdiff_t>& idxsToEntts,
	std::map<RENDER_STATES, std::vector<EntityID>>& blendStateToId)
{
	for (const ptrdiff_t idx : idxsToEntts)
	{
		const RenderStates& comp = *pRSComponent_;
		u32 bsHash = (comp.statesHashes_[idx] & blendStatesMask_);

		// if hash == 0 it means that entt has no blending (NO_BLENDING state)
		RENDER_STATES bs = (bsHash) ? hashesToBS_[bsHash] : NO_BLENDING;
		const EntityID id = comp.ids_[idx];

		blendStateToId[bs].push_back(id);
	}
}

///////////////////////////////////////////////////////////

bool RenderStatesSystem::CheckEnttsAreNew(const std::vector<EntityID>& ids)
{
	// check if all input ids of entts don't exist in the component
	return !CheckValuesExistInArr(pRSComponent_->ids_, ids);
}

void RenderStatesSystem::MakeDisablingMasks()
{
	// these hash masks are used to reset render states 
	for (const RENDER_STATES fillMode : allFillModes_)
		disableAllFillModesMask_ &= ~(1 << fillMode);

	for (const RENDER_STATES cullMode : allCullModes_)
		disableAllCullModesMask_ &= ~(1 << cullMode);

	for (const RENDER_STATES alphaClippingState : allAlphaClipping_)
		disableAllAlphaClippingMask_ &= ~(1 << alphaClippingState);

	for (const RENDER_STATES blendingState : allBS_)
		disableAllBlendingMask_ &= ~(1 << blendingState);
}

}