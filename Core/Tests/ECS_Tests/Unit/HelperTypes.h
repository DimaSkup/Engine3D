// *********************************************************************************
// Filename:      HelperTypes.h
// Description:   contains some data types which are helpful for ECS testing
// *********************************************************************************
#pragma once
#include <vector>
#include "ECS_Common/ECS_Types.h"

struct TransformData
{
	std::vector<XMFLOAT3> positions;
	std::vector<XMFLOAT3> directions;
	std::vector<XMFLOAT3> scales;
};

struct MoveData
{
	std::vector<XMFLOAT3> translations;
	std::vector<XMFLOAT4> rotQuats;
	std::vector<XMFLOAT3> scaleChanges;

};
