// *********************************************************************************
// Filename:      HelperTypes.h
// Description:   contains some data types which are helpful for ECS testing
// *********************************************************************************
#pragma once
#include <vector>
#include <DirectXMath.h>

struct TransformData
{
	std::vector<DirectX::XMFLOAT3> positions;
	std::vector<DirectX::XMFLOAT3> directions;
	std::vector<DirectX::XMFLOAT3> scales;
};

struct MoveData
{
	std::vector<DirectX::XMFLOAT3> translations;
	std::vector<DirectX::XMFLOAT4> rotQuats;
	std::vector<DirectX::XMFLOAT3> scaleChanges;
};
