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
	void TestNameComponent();
	void TestMoveComponent();
	void TestMeshComponent();
	void TestRenderComponent();

private:
	void TestAddingTransformComponent();
	void TestTransformComponentData();
};