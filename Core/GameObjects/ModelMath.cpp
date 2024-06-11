////////////////////////////////////////////////////////////////////
// Filename:     ModelMath.cpp
// Created:      06.02.23
////////////////////////////////////////////////////////////////////
#include "ModelMath.h"

#include <cmath>


void ModelMath::CalculateModelVectors(
	_Inout_ std::vector<VERTEX> & verticesArr,
	const bool isCalcNormals)
{
	// CalculateModelVectors() generates the tangent and binormal for the model as well as 
	// a recalculated normal vector. To start it calculates how many faces (triangles) are
	// in the model. Then for each of those triangles it gets the three vertices and uses
	// that to calculate the tangent, binormal, and normal. After calculating those three
	// normal vectors it then saves them back into the model structure.

	// Input:
	// 1. An array of vertices (verticesArr).
	// 2. A boolean flag (isCalcNormals) which defined either we compute normal vectors or not

	// check input params
	assert(verticesArr.size() >= 3);  // check if we have any vertices in the array (3 because we must have at least one face)

	DirectX::XMVECTOR tangent;
	DirectX::XMVECTOR binormal;
	DirectX::XMVECTOR normal;

	// calculate the number of faces in the model
	const size_t facesCount = verticesArr.size() / 3;

	// go throught all the faces and calculate the tangent, binormal, and normal vectors
	for (size_t face_idx = 0, data_idx = 0; face_idx < facesCount; ++face_idx)
	{
		// calculate the tangent and binormal of that face
		this->CalculateTangentBinormal(
			verticesArr[data_idx++],
			verticesArr[data_idx++],
			verticesArr[data_idx++],
			tangent,
			binormal);

		// for usual models we need to calculate normals but in case of the terrain
		// we don't have to do it here because it has been already done before so we use this flag
		if (isCalcNormals)
		{
			// calculate the new normal using the tangent and binormal
			//this->CalculateNormal(tangent, binormal, normal);

			normal = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(tangent, binormal));
			DirectX::XMFLOAT3 norm;
			DirectX::XMStoreFloat3(&norm, normal);
			
			// store these normal vectors into the vertices of the face
			verticesArr[data_idx - 3].normal = norm;
			verticesArr[data_idx - 2].normal = norm;
			verticesArr[data_idx - 1].normal = norm;
		}
	

		// store the tangent, and binormal for this face back in the vertex structure
		for (size_t backIndex = 3; backIndex > 0; backIndex--)
		{
			const size_t v_idx = data_idx - backIndex;  // index of the vertex

			DirectX::XMStoreFloat3(&verticesArr[v_idx].tangent, tangent);
			DirectX::XMStoreFloat3(&verticesArr[v_idx].binormal, binormal);
		}
	}


	return;
}

///////////////////////////////////////////////////////////

void ModelMath::CalculateTangentBinormal(
	const VERTEX & vertex1,
	const VERTEX & vertex2,
	const VERTEX & vertex3,
	_Out_ DirectX::XMVECTOR & tangent,
	_Out_ DirectX::XMVECTOR & binormal)
{
	// the CalculateTangentBinormal() takes in three vertices and then
	// calculates and returns the tangent and binormal of those three vertices

	float den = 0.0f;    // denominator of the tangent/binormal equation
	float tuVector[2]{ 0.0f };
	float tvVector[2]{ 0.0f };
	
	const DirectX::XMVECTOR vertex1_pos = DirectX::XMLoadFloat3(&vertex1.position);
	const DirectX::XMVECTOR vertex2_pos = DirectX::XMLoadFloat3(&vertex2.position);
	const DirectX::XMVECTOR vertex3_pos = DirectX::XMLoadFloat3(&vertex3.position);

	// calculate the two vectors of edges for this face
	DirectX::XMFLOAT3 edge1;
	DirectX::XMFLOAT3 edge2;
	DirectX::XMStoreFloat3(&edge1, DirectX::XMVectorSubtract(vertex2_pos, vertex1_pos));
	DirectX::XMStoreFloat3(&edge2, DirectX::XMVectorSubtract(vertex3_pos, vertex1_pos));
	
	// calculate the tu and tv texture space vectors
	tuVector[0] = vertex2.texture.x - vertex1.texture.x; 
	tvVector[0] = vertex2.texture.y - vertex1.texture.y;

	tuVector[1] = vertex3.texture.x - vertex1.texture.x;
	tvVector[1] = vertex3.texture.y - vertex1.texture.y;

	// calculate the denominator of the tangent/binormal equation
	den = 1.0f / (tuVector[0] * tvVector[1] - tuVector[1] * tvVector[0]);

#if 0
	if (isinf(den))
	{
		throw std::runtime_error("denominator of the tangent/binormal equation is infinite");
	}
#endif

	// calculate the cross products and multiply by the coefficient to get 
	// the tangent and binormal;
	const float tangent_x = (tvVector[1] * edge1.x - tvVector[0] * edge2.x) * den;
	const float tangent_y = (tvVector[1] * edge1.y - tvVector[0] * edge2.y) * den;
	const float tangent_z = (tvVector[1] * edge1.z - tvVector[0] * edge2.z) * den;

	const float binormal_x = (tuVector[0] * edge2.x - tuVector[1] * edge1.x) * den;
	const float binormal_y = (tuVector[0] * edge2.y - tuVector[1] * edge1.y) * den;
	const float binormal_z = (tuVector[0] * edge2.z - tuVector[1] * edge1.z) * den;

	// normalize the tangent components and then store it
	tangent = DirectX::XMVector3Normalize({ tangent_x, tangent_y, tangent_z });

	// normalize the binormal components and then store it
	binormal = DirectX::XMVector3Normalize({ binormal_x, binormal_y, binormal_z });

	return;

} // end CalculateTangentBinormal

///////////////////////////////////////////////////////////

void ModelMath::CalculateNormal(
	const DirectX::XMVECTOR & tangent,
	const DirectX::XMVECTOR & binormal,
	_Out_ DirectX::XMVECTOR & normal)
{
	// the CalculateNormal() takes in the tangent and binormal and does a cross product
	// to give back the normalized normal vector

	normal = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(tangent, binormal));

	return;
}