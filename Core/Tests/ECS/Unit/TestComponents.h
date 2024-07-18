// *********************************************************************************
// Filename:       ECS_Test_Components.h
// Description:    unit-tests for each component of the ECS;
// *********************************************************************************
#pragma once

class TestComponents
{
public:
	TestComponents() {};

	void Run();

	void TestTransformComponent();
	void TestNameComponent();
	void TestMoveComponent();
	void TestMeshComponent();

	void TestRenderComponent();
	void TestTexTransformComponent();

private:
	void TestAddingTransformComponent();
	void TestTransformComponentData();
};