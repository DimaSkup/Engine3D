// *********************************************************************************
// Filename:       ECS_Test_Components.h
// Description:    unit-tests for each component of the ECS;
// *********************************************************************************
#pragma once

#include "ECS_Common/ECS_Types.h"
#include <vector>
#include <DirectXMath.h>

class ECS_Test_Components
{
public:
	ECS_Test_Components() {};

	void TestTransformComponent();
	void TestMovementComponent();
	void TestMeshComponent();
	void TestRenderComponent();

private:
	void PrepareRandomDataForTransformComponent(
		const size_t elemCount,
		std::vector<XMFLOAT3>& outPositions,
		std::vector<XMFLOAT3>& outDirections,
		std::vector<XMFLOAT3>& outScales);

	void PrepareRandomDataForMoveComponent(
		const size_t elemCount,
		std::vector<XMFLOAT3>& outTranslations,
		std::vector<XMFLOAT4>& outRotationQuats,
		std::vector<XMFLOAT3>& outScaleChanges);

	void TestAddingTransformComponent();
	void TestTransformComponentData();
};