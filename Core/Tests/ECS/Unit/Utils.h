// *********************************************************************************
// Filename:     Utils.h
// Description:  contains different utils for ECS components testing
// 
// Created:      26.05.24
// *********************************************************************************

#pragma once


#include <vector>
#include <algorithm>
#include <random>


#include "Entity/EntityManager.h"
#include "MathHelper.h"
#include "HelperTypes.h"

#include "../Engine/log.h"
#include "../Engine/EngineException.h"

#include <fstream>
#include <filesystem>



namespace Utils
{

// *******************************************************************************
// 
// 	                             COMMON UTILS
// 
// *******************************************************************************

namespace fs = std::filesystem;


static void RemoveFile(const std::string& filepath)
{
	// if a file by filepath exists we remove it from the OS filesystem
	if (fs::exists(filepath))
	{
		if (!fs::remove(fs::path(filepath)))
			Log::Error(LOG_MACRO, "can't remove a file by path: " + filepath);
	}
}

///////////////////////////////////////////////////////////

static void SysSerialDeserialHelper(
	const std::string filepath,
	ComponentType type,
	EntityManager& fromMgr,
	EntityManager& intoMgr)
{
	// in: filepath - path to the data file with the serialized data
	//     type     - handle data related to this component
	//     fromMgr  - serialize data from this entity manager
	//     intoMgr  - deserialize data into this entity manager

	u32 offset = 0;
	std::ofstream fout(filepath, std::ios::binary);
	std::ifstream fin;

	switch (type)
	{
		case TransformComponent:
		{
			fromMgr.transformSystem_.Serialize(fout, offset);
			fout.close();
			fin.open(filepath, std::ios::binary);
			intoMgr.transformSystem_.Deserialize(fin, offset);

			break;
		}
		case MoveComponent:
		{
			fromMgr.moveSystem_.Serialize(fout, offset);
			fout.close();
			fin.open(filepath, std::ios::binary);
			intoMgr.moveSystem_.Deserialize(fin, offset);

			break;
		}
		case NameComponent:
		{
			fromMgr.nameSystem_.Serialize(fout, offset);
			fout.close();
			fin.open(filepath, std::ios::binary);
			intoMgr.nameSystem_.Deserialize(fin, offset);
			
			break;
		}
		case MeshComp:
		{
			fromMgr.meshSystem_.Serialize(fout, offset);
			fout.close();
			fin.open(filepath, std::ios::binary);
			intoMgr.meshSystem_.Deserialize(fin, offset);

			break;
		}
		case RenderedComponent:
		{
			fromMgr.renderSystem_.Serialize(fout, offset);
			fout.close();
			fin.open(filepath, std::ios::binary);
			intoMgr.renderSystem_.Deserialize(fin, offset);
			break;
		}
		case TexturedComponent:
		{
			assert("TODO: implement is!" && 0);
			break;
		}
		case TextureTransformComponent:
		{
			assert("TODO: implement it!" && 0);
			break;
		}
	}

	fin.close();
	Utils::RemoveFile(filepath);
}

///////////////////////////////////////////////////////////

static bool CheckEnttsHaveComponent(
	EntityManager& entityMgr,
	const std::vector<EntityID>& ids,
	const ComponentType type)
{
	// define if input entts are set to have a component of type;
	// for it we using a bitflag and chech if some bit is set
	// which responsible to paricular component

	std::vector<ComponentFlagsType> componentFlags;
	entityMgr.GetComponentFlagsByIDs(ids, componentFlags);
	
	const u32 bitmask = (1 << type);
	u32 haveComponent = bitmask;

	for (const ComponentFlagsType flag : componentFlags)
		haveComponent &= (flag & bitmask);

	return haveComponent;
}

///////////////////////////////////////////////////////////

static std::string GetRandAlnumStr(u32 length)
{
	// return randomly generated string of length

	const std::string charset = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

	thread_local std::mt19937 rg{ std::random_device{}() };
	thread_local std::uniform_int_distribution<std::string::size_type> pick(0, charset.size() - 1);

	std::string str;
	str.reserve(length);

	while (length--) str += charset[pick(rg)];

	return str;
}

///////////////////////////////////////////////////////////

template<class T, class Pred = std::equal_to<>>
static bool ContainerCompare(
	const T& lhs,
	const T& rhs,
	Pred pred = {})
{
	// check two STL containers (vector, map, set, etc.) for complete equality
	// using the passed predicate (pred);
	// return: true -- if two containers are completely equal

	return (lhs.size() == rhs.size()) &&
		std::equal(lhs.begin(), lhs.end(), rhs.begin(), pred);
}

///////////////////////////////////////////////////////////

static void GetRandEnttsNames(
	const u32 enttsCount,
	const u32 nameLength,
	std::vector<EntityName>& outNames)
{
	// generate unique names for entities in quantity enttsCount
	// out: array of entities names;

	for (u32 idx = 0; idx < enttsCount; ++idx)
		outNames.emplace_back(GetRandAlnumStr(nameLength));
}

///////////////////////////////////////////////////////////

static void PrepareRandDataForXMArr(
	const u32 arrSize,
	std::vector<XMFLOAT3>& outArr)
{
	// go through each element of the array and generate for it random data
	outArr.reserve(arrSize);
	
	for (u32 idx = 0; idx < arrSize; ++idx)
		outArr.push_back(MathHelper::RandXMFLOAT3());
}

///////////////////////////////////////////////////////////

static void PrepareRandDataForXMArr(
	const u32 arrSize,
	std::vector<XMFLOAT4>& outArr)
{
	// go through each element of the array and generate for it random data
	outArr.reserve(arrSize);

	for (u32 idx = 0; idx < arrSize; ++idx)
		outArr.push_back(MathHelper::RandXMFLOAT4());
}

///////////////////////////////////////////////////////////

static void PrepareRandDataForXMArr(
	const u32 arrSize,
	std::vector<XMVECTOR>& outArr)
{
	// go through each element of the array and generate for it random data
	outArr.reserve(arrSize);

	for (u32 idx = 0; idx < arrSize; ++idx)
		outArr.emplace_back(MathHelper::RandXMVECTOR());
}

///////////////////////////////////////////////////////////

static void PrepareRandDataForFloatArr(
	const u32 elemCount,
	std::vector<float>& outArr)
{
	outArr.reserve(elemCount);

	for (u32 idx = 0; idx < elemCount; ++idx)
		outArr.push_back(MathHelper::RandF());
}

///////////////////////////////////////////////////////////

static XMMATRIX BuildMatrix(
	const XMFLOAT3& pos,
	const XMFLOAT3& dir,
	const XMFLOAT3& scale)
{
	return	
		DirectX::XMMatrixScalingFromVector(XMLoadFloat3(&scale)) *
		DirectX::XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&dir)) *
		DirectX::XMMatrixTranslationFromVector(XMLoadFloat3(&pos));
}




// *******************************************************************************
// 
// 	                     UTILS RELATED TO ECS COMPONENTS
// 
// *******************************************************************************

static void GetRandTransformData(
	const u32 elemCount,
	TransformData& outTransform)
{
	// generate random values for positions/directions/scales
	PrepareRandDataForXMArr(elemCount, outTransform.positions);
	PrepareRandDataForXMArr(elemCount, outTransform.dirQuats);
	PrepareRandDataForFloatArr(elemCount, outTransform.uniformScales);
}

///////////////////////////////////////////////////////////

static void GetRandMoveData(
	const u32 elemCount,
	MoveData& outMove)
{
	// generate random values for translations/rotations_quaternions/scale_changes
	PrepareRandDataForXMArr(elemCount, outMove.translations);
	PrepareRandDataForXMArr(elemCount, outMove.rotQuats);
	PrepareRandDataForFloatArr(elemCount, outMove.uniformScales);
}

///////////////////////////////////////////////////////////

static void GetRandRenderedData(
	const u32 elemCount,
	RenderedData& rendered)
{
	// generate pseudo random data for the Rendered component
	rendered.shaderTypes.resize(elemCount, ECS::RENDERING_SHADERS::COLOR_SHADER);
	rendered.primTopologyTypes.resize(elemCount, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

///////////////////////////////////////////////////////////

static void MergeIntoFloat4(
	const std::vector<XMFLOAT3>& arrOfFloat3,
	const std::vector<float>& arrOfFloat,
	std::vector<XMFLOAT4>& outArrOfFloat4)
{
	// merge input XMFLOAT3 and float into XMFLOAT4;
	// XMFLOAT3 is placed as (x,y,z) and float is placed as (w) components

	const ptrdiff_t dataCount = std::ssize(arrOfFloat3);
	ASSERT_TRUE(dataCount == std::ssize(arrOfFloat), "arr size of XMFLOAT3s must be equal to arr size of floats");

	outArrOfFloat4.clear();
	outArrOfFloat4.reserve(dataCount);

	for (ptrdiff_t idx = 0; idx < dataCount; ++idx)
	{
		const XMFLOAT3& vec3 = arrOfFloat3[idx];
		const float f = arrOfFloat[idx];

		outArrOfFloat4.emplace_back(vec3.x, vec3.y, vec3.z, f);
	}
}

///////////////////////////////////////////////////////////

static void GetArrOfNormQuaternions(
	const std::vector<XMVECTOR>& inQuats,
	std::vector<XMVECTOR>& outNormQuats)
{
	// in:   array of quaternions
	// out:  array of normalized quaternions

	outNormQuats.clear();
	outNormQuats.reserve(std::ssize(inQuats));

	for (const XMVECTOR& quat : inQuats)
		outNormQuats.emplace_back(DirectX::XMVector3Normalize(quat));
}

///////////////////////////////////////////////////////////

static void CompareTransformData(
	const Transform& transComp,
	const std::vector<EntityID>& ids,
	const TransformData& data)
{
	// here we check if input transform data is the same
	// as the data from the Transform component

	std::vector<XMFLOAT4> origPosAndUniScales;
	std::vector<XMVECTOR> origNormDirQuats;

	// convert origin data to the proper format 
	MergeIntoFloat4(data.positions, data.uniformScales, origPosAndUniScales);
	GetArrOfNormQuaternions(data.dirQuats, origNormDirQuats);

	// check if data from the Transform component is equal to the origin data
	const bool areIDsValid             = Utils::ContainerCompare(transComp.ids_, ids);
	const bool arePosAndScalesCorrect  = Utils::ContainerCompare(transComp.posAndUniformScale_, origPosAndUniScales);
	const bool areDirQuatsCorrect      = Utils::ContainerCompare(transComp.dirQuats_, origNormDirQuats, DirectX::CompareXMVECTOR());

	ASSERT_TRUE(areIDsValid,            "TEST: IDs data from the Transform component isn't valid");
	ASSERT_TRUE(arePosAndScalesCorrect, "TEST: positions and uniform scale data isn't correct");
	ASSERT_TRUE(areDirQuatsCorrect,     "TEST: direction quaternions data isn't correct");
}

///////////////////////////////////////////////////////////

static void CompareMoveData(
	const Movement& component,
	const std::vector<EntityID>& ids,
	const MoveData& data)
{
	// here we check if input movement data is the same
	// as the data from the Movement component

	std::vector<XMFLOAT4> origTransAndUniScales;  
	std::vector<XMVECTOR> origNormRotQuats;       

	// convert origin data to the proper format 
	MergeIntoFloat4(data.translations, data.uniformScales, origTransAndUniScales);
	GetArrOfNormQuaternions(data.rotQuats, origNormRotQuats);

	// check if data from the Movement component is equal to the origin (input) data
	const bool areIDsValid             = Utils::ContainerCompare(component.ids_, ids);
	const bool areTransAndScalesValid  = Utils::ContainerCompare(component.translationAndUniScales_, origTransAndUniScales);
	const bool areRotQuatsValid        = Utils::ContainerCompare(component.rotationQuats_, origNormRotQuats, DirectX::CompareXMVECTOR());

	ASSERT_TRUE(areIDsValid,            "TEST: deserialized IDs data isn't correct");
	ASSERT_TRUE(areTransAndScalesValid, "TEST: translations and uniform scale data isn't correct");
	ASSERT_TRUE(areRotQuatsValid,       "TEST: rotations quaternions data isn't correct");
}

///////////////////////////////////////////////////////////

static void CompareNameData(
	const Name& component,
	const std::vector<EntityID>& ids,
	const std::vector<EntityName>& names)
{
	// check if the input data is the same as the data from the Name component

	const bool areIDsValid    = ContainerCompare(ids, component.ids_);
	const bool areNamesValid  = ContainerCompare(names, component.names_);

	ASSERT_TRUE(areIDsValid,   "IDs data from the Name component isn't correct");
	ASSERT_TRUE(areNamesValid, "names data from the Name component isn't correct");
}

///////////////////////////////////////////////////////////

static void CompareMeshData(
	MeshSystem& system,
	const std::vector<EntityID>& enttsIDs,
	const std::vector<MeshID>& meshesIDs)
{
	// check if the input data is the same as the data from the Mesh component

	std::vector<EntityID> enttsIDsFromMeshComponent;
	std::vector<MeshID> meshesIDsFromMeshComponent;

	system.GetEnttsIDsFromMeshComponent(enttsIDsFromMeshComponent);
	system.GetAllMeshesIDsFromMeshComponent(meshesIDsFromMeshComponent);

	const bool areEnttsIDsCorrect   = ContainerCompare(enttsIDs, enttsIDsFromMeshComponent);
	const bool areMeshesIDsCorrect  = ContainerCompare(meshesIDs, meshesIDsFromMeshComponent);

	ASSERT_TRUE(areEnttsIDsCorrect,  "the Mesh component doesn't have a record about some entity");
	ASSERT_TRUE(areMeshesIDsCorrect, "the Mesh component doesn't have a record about some mesh");
}

///////////////////////////////////////////////////////////

static void CompareRenderedData(
	const Rendered& component,
	const std::vector<EntityID>& ids,
	const RenderedData& data)
{
	// check if the input data is the same as the data from the Rendered component

	const bool areIDsValid          = Utils::ContainerCompare(component.ids_, ids);
	const bool areShaderTypesValid  = Utils::ContainerCompare(component.shaderTypes_, data.shaderTypes);
	const bool areTopologiesValid   = Utils::ContainerCompare(component.primTopologies_, data.primTopologyTypes);

	ASSERT_TRUE(areIDsValid,         "TEST SYSTEMS: deserialized IDs data isn't correct");
	ASSERT_TRUE(areShaderTypesValid, "TEST SYSTEMS: deserialized shader types data isn't correct");
	ASSERT_TRUE(areTopologiesValid,  "TEST SYSTEMS: deserialized primitive topologies data isn't correct");
}

///////////////////////////////////////////////////////////

static void CompareTexTransformations(
	const TextureTransform& component,
	const std::vector<EntityID>& ids,
	const std::vector<XMMATRIX>& transform)
{
	// check if the input data is the same as the data 
	// from the TextureTransform component

	const bool areIDsValid            = ContainerCompare(component.ids_, ids);
	const bool areTexTransformsValid  = ContainerCompare(component.texTransforms_, transform);

	ASSERT_TRUE(areIDsValid,           "ids data isn't correct");
	ASSERT_TRUE(areTexTransformsValid, "textures transformation data isn't correct");
}

};  // namespace Utils
