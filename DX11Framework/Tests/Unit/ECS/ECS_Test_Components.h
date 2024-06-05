#pragma once

#include <vector>
#include <DirectXMath.h>

using UINT = unsigned int;

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
		std::vector<DirectX::XMFLOAT3>& outPositions,
		std::vector<DirectX::XMFLOAT3>& outDirections,
		std::vector<DirectX::XMFLOAT3>& outScales);

	void PrepareRandomDataForMoveComponent(
		const size_t elemCount,
		std::vector<DirectX::XMFLOAT3>& outTranslations,
		std::vector<DirectX::XMFLOAT4>& outRotationQuats,
		std::vector<DirectX::XMFLOAT3>& outScaleChanges);
};