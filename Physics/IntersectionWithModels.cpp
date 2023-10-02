////////////////////////////////////////////////////////////////////////////////////////////
// Filename:     IntersectionWithModels.cpp
//
// Description:  contains an implementation of functional for intersection with models
//               a.k.a picking;
//
// Created:      01.10.23
////////////////////////////////////////////////////////////////////////////////////////////
#include "IntersectionWithModels.h"


IntersectionWithModels::IntersectionWithModels()
{
}

IntersectionWithModels::~IntersectionWithModels()
{
}




////////////////////////////////////////////////////////////////////////////////////////////
//                                PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////

Model* IntersectionWithModels::TestIntersectionWithModel(const int mouseX, const int mouseY,
	const POINT & windowDimensions,
	const std::map<std::string, Model*> & modelsList,
	const CameraClass* pCamera,
	const DirectX::XMMATRIX & worldMatrix)
{
	// there is general intersection check that
	// forms the vector for checking the intersection and then calls the specific type
	// of intersection check required. 
	// If an intersection occured with some model we return a pointer to this model;

	// DETAILED DESCRIPTION:
	/*

	This function takes as input the 2D mouse coordinates and then forms a vector in 3D
	space which it uses to then check for an intersection with the sphere (or some another
	shape). That vector is called the picking ray. The picking ray has the origin and
	a direction. With the 3D coordinate (origin) and 3D vector/normal (direction) we can
	create a line in 3D space and find out what it collides with.

	Usually we are very used to a vertex shader that takes a 3D point (vertice) and moves
	it from 3D space onto the 2D screen so it can be rendered as the pixel. Well now we are
	doing the exact opposite and moving a 2D point from the screen to view to projection
	to make a 2D point, we will now instead take a 2D point and go from projection to view
	to world and turn it into a 3D point.

	To do the reverse process we first start by taking the mouse coordinates and moving
	them into the -1 to +1 range on both axis. When we have that we then divide by the
	screen aspect using the projection matrix. With that value we can then multiply it by
	the inverse view matrix (inverse because we are going in reverse direction) to get the
	direction vector in view space. We can set the origin of the vector in view space
	to just be the location of the camera.

	With the direction vector and origin in view space we can now complete the final process
	of moving it into 3D world space. To do so we first need to get the world matrix and
	translate it by the position of the sphere. With the updated world matrix we once again
	need to invert it (since the process is going in the opposite direction) and then we
	can multiply the origin and direction by the inverted world matrix. We also normalize
	the direction after the multiplication. This gives us the origin and direction of the
	vector in 3D world space so that we can do tests with other objects that are also
	in 3D world space.

	Now that we have the origin of the vector and the direction of the vector we can
	perform an interstection test. For instance it can be a ray-sphere intersection test,
	but you could perform any kind of intersection test that you have the vector in 3D
	world space

	*/
	

	// a position of some model on the scene
	DirectX::XMFLOAT3 modelPosition{ 0.0f, 0.0f, 0.0f };
	DirectX::XMMATRIX inverseViewMatrix;
	DirectX::XMMATRIX inverseWorldMatrix;
	DirectX::XMMATRIX translateMatrix;
	DirectX::XMMATRIX tempWorldMatrix;     // we translate the world matrix for each model separately
	DirectX::XMMATRIX projMatrix{ pCamera->GetProjectionMatrix() };
	DirectX::XMVECTOR direction;
	DirectX::XMVECTOR rayOrigin;
	DirectX::XMVECTOR rayDirection;
	DirectX::XMFLOAT4X4 fInvViewMatrix;

	float pointX = 0.0f;
	float pointY = 0.0f;
	bool intersect = false;

	//////////////////////////////////////////////////////////

	// move the mouse cursor coordinates into the -1 to +1 range
	pointX = (2.0f * static_cast<float>(windowDimensions.x / 2.0f) / static_cast<float>(windowDimensions.x)) - 1.0f;
	pointY = ((2.0f * static_cast<float>(windowDimensions.y / 2.0f) / static_cast<float>(windowDimensions.y)) - 1.0f) * -1.0f;

	// adjust the points using the projection matrix to account for the aspect ration of the viewport;
	pointX = pointX / (DirectX::XMVectorGetX(projMatrix.r[0]));
	pointY = pointY / (DirectX::XMVectorGetY(projMatrix.r[1]));

	// get the inverse of the view matrix
	inverseViewMatrix = DirectX::XMMatrixInverse(nullptr, pCamera->GetViewMatrix());

	// convert the inverse of the view matrix into a 4x4 float type
	DirectX::XMStoreFloat4x4(&fInvViewMatrix, inverseViewMatrix);

	// calculate the direction of the picking ray in view space
	direction.m128_f32[0] = (pointX * fInvViewMatrix._11) + (pointY * fInvViewMatrix._21) + fInvViewMatrix._31;
	direction.m128_f32[1] = (pointX * fInvViewMatrix._12) + (pointY * fInvViewMatrix._22) + fInvViewMatrix._32;
	direction.m128_f32[2] = (pointX * fInvViewMatrix._13) + (pointY * fInvViewMatrix._23) + fInvViewMatrix._33;


	// check intersection with each model on the scene (custom models, cubes, spheres, etc.)
	for (const auto & elem : modelsList)
	{
		modelPosition = elem.second->GetModelDataObj()->GetPosition();

		// get the world matrix and translate it to the location of the model
		
		translateMatrix = DirectX::XMMatrixTranslation(modelPosition.x, modelPosition.y, modelPosition.z);
		tempWorldMatrix = worldMatrix * translateMatrix;

		// now get the inverse of the translated world matrix
		inverseWorldMatrix = DirectX::XMMatrixInverse(nullptr, tempWorldMatrix);

		// now transform the ray origin and the ray direction from view space to world space
		rayOrigin = DirectX::XMVector3TransformCoord(pCamera->GetPositionVector(), inverseWorldMatrix);
		rayDirection = DirectX::XMVector3TransformNormal(direction, inverseWorldMatrix);

		// normalize the ray direction
		rayDirection = DirectX::XMVector3Normalize(rayDirection);

		// now perform the ray-sphere intersection test;
		// if we have an intersection with some model we return a pointer to it
		RayTriangleIntersect(rayOrigin, rayDirection,
		{ 0.0f, 0.0f, 0.0 },
		{ 1.0f, 0.0f, 0.0f },
		{ 0.0f, 0.0f, 1.0f });

		//if (RaySphereIntersect(rayOrigin, rayDirection, 1.0f))
		//{
		//	return elem.second;
		//}

	} // end for

	return nullptr;
	
} // end TestIntersectionWithModel

/////////////////////////////////////////////////

bool IntersectionWithModels::RaySphereIntersect(const DirectX::XMVECTOR & rayOrigin,
	const DirectX::XMVECTOR & rayDirection,
	const float radius)
{
	// this function executes the ray-sphere intersection check; 
	// For other intersection tests such as ray-triangle,
	// ray-rectangle, and so forth you would add them here


	// this function performs the math of a basic ray-sphere intersection test

	float a = 0.0f;
	float b = 0.0f;
	float c = 0.0f;
	float discriminant = 0.0f;

	DirectX::XMFLOAT3 fRayOrigin;
	DirectX::XMFLOAT3 fRayDirection;

	DirectX::XMStoreFloat3(&fRayOrigin, rayOrigin);
	DirectX::XMStoreFloat3(&fRayDirection, rayDirection);


	// calculate the a, b and c coefficients
	a = (fRayDirection.x * fRayDirection.x) + (fRayDirection.y * fRayDirection.y) + (fRayDirection.z * fRayDirection.z);
	b = ((fRayDirection.x * fRayOrigin.x) + (fRayDirection.y * fRayOrigin.y) + (fRayDirection.z * fRayOrigin.z)) * 2.0f;
	c = (fRayOrigin.x * fRayOrigin.x) + (fRayOrigin.y * fRayOrigin.y) + (fRayOrigin.z * fRayOrigin.z) - (radius * radius);

	// find the discriminant
	discriminant = (b * b) - (4 * a * c);

	// if discriminant is negative the picking ray missed the sphere, otherwise it intersected the sphere
	return (discriminant > 0.0f);

} // end RaySphereIntersect

/////////////////////////////////////////////////

bool IntersectionWithModels::RayTriangleIntersect(const DirectX::XMVECTOR & rayOrigin,
	const DirectX::XMVECTOR & rayDirection,
	const DirectX::XMFLOAT3 & v0,
	const DirectX::XMFLOAT3 & v1,
	const DirectX::XMFLOAT3 & v2)
{
	// find a cross product of the plane so that we will have its normal vector
	//DirectX::XMVECTOR v{ v1.x - v0.x, v1.y - v0.y, v1.z - v0.z };
	//DirectX::XMVECTOR u{ v2.x - v0.x, v2.y - v0.y, v2.z - v0.z };

	const XMVECTOR vecV0 = XMLoadFloat3(&v0);
	const XMVECTOR vecV1 = XMLoadFloat3(&v1);
	const XMVECTOR vecV2 = XMLoadFloat3(&v2);

	XMVECTOR vecV = vecV1 - vecV0;
	XMVECTOR vecU = vecV2 - vecV0;
	XMVECTOR planeNormal = XMVector3Cross(vecV, vecU);
	
	XMVECTOR vecP0V0 = vecV0 - rayOrigin;          // a vector from the rayOrigin to v0
	XMVECTOR vecP0P1 = rayDirection - rayOrigin;   // a vector which is our intersection ray

	// get a dot product between the plane's normal and the ray
	float denominator = XMVector3Dot(planeNormal, vecP0P1).m128_f32[0];  

	// if our denominator is equal or less that 0 we the ray doesn't intersect the plane
	if (denominator <= 0)
		return false;

	// find a numerator for the equation: ri = vecN * (vecP0V0) / vecN * (vecP0P1);
	float numerator = XMVector3Dot(planeNormal, vecP0V0).m128_f32[0];

	// find the parameter value 
	float ri = numerator / denominator;

	// if the parameter > 0 the ray intersects a plane so that we return true
	return (ri > 0) ? true : false;

} // RayTriangleIntersect
