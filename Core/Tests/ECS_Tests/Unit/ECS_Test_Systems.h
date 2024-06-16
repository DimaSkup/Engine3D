// *********************************************************************************
// Filename:       ECS_Test_Systems.h
// Description:    tests for each system of the ECS;
// 
// Created:        13.06.24
// *********************************************************************************
#pragma once


class ECS_Test_Systems final
{
public:
	ECS_Test_Systems() {}

	void TestSerializationDeserialization();

private:
	// test serialization and deserialization
	void TestTransformSystemToSerialAndDeserial();   
	void TestMoveSystemToSerialAndDeserial();
	void TestMeshSystemToSerialAndDeserial();
	void TestRenderedSystemToSerialAndDeserial();
};