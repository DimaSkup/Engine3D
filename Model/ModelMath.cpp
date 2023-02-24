////////////////////////////////////////////////////////////////////
// Filename:     ModelMath.cpp
// Created:      06.02.23
////////////////////////////////////////////////////////////////////

#include "ModelMath.h"


// CalculateModelVectors() generates the tangent and binormal for the model as well as 
// a recalculated normal vector. To start it calculates how many faces (triangles) are
// in the model. Then for each of those triangles it gets the three vertices and uses
// that to calculate the tangent, binormal, and normal. After calculating those three
// normal vectors it then saves them back into the model structure.
void ModelMath::CalculateModelVectors(void* pModelTypeData, int vertexCount)
{
	//Log::Debug(THIS_FUNC_EMPTY);
	ModelType* pModelType = static_cast<ModelType*>(pModelTypeData);

	int faceCount = 0;	// the number of faces in the model
	int index = 0;		// the index to the model data

	TempVertexType vertices[3];

	VectorType tangent;
	VectorType binormal;
	VectorType normal;


	// calculate the number of faces in the model
	faceCount = vertexCount / 3;  // ATTENTION: don't use "this->vertexBuffer_.GetBufferSize()" because at this point we haven't initialized the vertex buffer yet

										 // go throught all the faces and calculate the tangent, binormal, and normal vectors
	for (size_t i = 0; i < faceCount; i++)
	{
		// get the three vertices for this face from the model
		for (size_t vertexIndex = 0; vertexIndex < 3; vertexIndex++)
		{
			vertices[vertexIndex].x = pModelType[index].x;
			vertices[vertexIndex].y = pModelType[index].y;
			vertices[vertexIndex].z = pModelType[index].z;
			vertices[vertexIndex].tu = pModelType[index].tu;
			vertices[vertexIndex].tv = pModelType[index].tv;
			vertices[vertexIndex].nx = pModelType[index].nx;
			vertices[vertexIndex].ny = pModelType[index].ny;
			vertices[vertexIndex].nz = pModelType[index].nz;
			index++;
		}


		// calculate the tangent and binormal of that face
		this->CalculateTangentBinormal(vertices[0], vertices[1], vertices[2], tangent, binormal);

		// calculate the new normal using the tangent and binormal
		this->CalculateNormal(tangent, binormal, normal);

		// store the normal, tangent, and binormal for this face back in the model structure;

		for (size_t backIndex = 3; backIndex > 0; backIndex--)
		{
			pModelType[index - backIndex].nx = normal.x;
			pModelType[index - backIndex].ny = normal.y;
			pModelType[index - backIndex].nz = normal.z;
			pModelType[index - backIndex].tx = tangent.x;
			pModelType[index - backIndex].ty = tangent.y;
			pModelType[index - backIndex].tz = tangent.z;
			pModelType[index - backIndex].bx = binormal.x;
			pModelType[index - backIndex].by = binormal.y;
			pModelType[index - backIndex].bz = binormal.z;
		}
	}


	return;
} /* CalculateModelVectors() */


  // the CalculateTangentBinormal() takes in three vertices and then
  // calculates and returns the tangent and binormal of those three vertices
void ModelMath::CalculateTangentBinormal(TempVertexType vertex1,
	TempVertexType vertex2,
	TempVertexType vertex3,
	VectorType& tangent,
	VectorType& binormal)
{
	float den;    // denominator of the tangent/binormal equation
	float length; // length of the tangent/binormal
	float vector1[3], vector2[3];
	float tuVector[2], tvVector[2];
	

	// calculate the two vectors for this face
	vector1[0] = vertex2.x - vertex1.x;
	vector1[1] = vertex2.y - vertex1.y;
	vector1[2] = vertex2.z - vertex1.z;

	vector2[0] = vertex3.x - vertex1.x;
	vector2[1] = vertex3.y - vertex1.y;
	vector2[2] = vertex3.z - vertex1.z;

	// calculate the tu and tv texture space vectors
	tuVector[0] = vertex2.tu - vertex1.tu;
	tvVector[0] = vertex2.tv - vertex1.tv;

	tuVector[1] = vertex3.tu - vertex1.tu;
	tvVector[1] = vertex3.tv - vertex1.tv;

	// calculate the denominator of the tangent/binormal equation
	den = 1.0f / (tuVector[0] * tvVector[1] - tuVector[1] * tvVector[0]);

	// calculate the cross products and multiply by the coefficient to get 
	// the tangent and binormal;
	tangent.x = (tvVector[1] * vector1[0] - tvVector[0] * vector2[0]) * den;
	tangent.y = (tvVector[1] * vector1[1] - tvVector[0] * vector2[1]) * den;
	tangent.z = (tvVector[1] * vector1[2] - tvVector[0] * vector2[2]) * den;


	binormal.x = (tuVector[0] * vector2[0] - tuVector[1] * vector1[0]) * den;
	binormal.y = (tuVector[0] * vector2[1] - tuVector[1] * vector1[1]) * den;
	binormal.z = (tuVector[0] * vector2[2] - tuVector[1] * vector1[2]) * den;

	// calculate the length of the tangent
	length = sqrt((tangent.x * tangent.x) + (tangent.y * tangent.y) + (tangent.z * tangent.z));

	// normalize the tangent components and then store it
	tangent.x = tangent.x / length;
	tangent.y = tangent.y / length;
	tangent.z = tangent.z / length;


	// calculate the length of the binormal
	length = sqrt((binormal.x * binormal.x) + (binormal.y * binormal.y) + (binormal.z * binormal.z));

	// normalize the binormal components and then store it
	binormal.x = binormal.x / length;
	binormal.y = binormal.y / length;
	binormal.z = binormal.z / length;

	return;
} // CalculateTangentBinormal() 


// the CalculateNormal() takes in the tangent and binormal and the does a cross product
// to give back the normal vector
void ModelMath::CalculateNormal(VectorType tangent,
	VectorType binormal,
	VectorType& normal)
{
	// the length of the normal vector
	float length = 0.0f;  

	// calculate the cross product of the tangent and binormal which will give
	// the normal vector
	normal.x = (tangent.y * binormal.z) - (tangent.z * binormal.y);
	normal.y = (tangent.z * binormal.x) - (tangent.x * binormal.z);
	normal.z = (tangent.x * binormal.y) - (tangent.y * binormal.x);

	// calculate the length of the normal
	length = sqrt((normal.x * normal.x) + (normal.y * normal.y) + (normal.z * normal.z));

	// normalize the normal
	normal.x = normal.x / length;
	normal.y = normal.y / length;
	normal.z = normal.z / length;

	return;
}