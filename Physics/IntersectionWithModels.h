////////////////////////////////////////////////////////////////////////////////////////////
// Filename:     IntersectionWithModels.h
//
// Description:  contains functional for computing intersection between a direction vector
//               and a bounding box;
//               
//               Example of using: in most 3D applications the user will need to click
//               on the screen with the mouse to select or interact with one of the 3D
//               objects in the scene. This process is usually referred to as selection
//               or picking. 
//               The process of picking involved translating a 2D mouse coordinate position
//               into a vector that is in world space. That vector is then used for 
//               intersection checks with all the visible 3D objects. Once the 3D object
//               is determinated the test can be further refined to determine exactly
//               which polygon was selected on that 3D object
//
// Created:      01.10.23
////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include "../Model/Model.h"
#include "../Camera/cameraclass.h"

using namespace DirectX;


//////////////////////////////////
// Class: IntersectionWithModels
//////////////////////////////////
class IntersectionWithModels
{
public:
	IntersectionWithModels();
	~IntersectionWithModels();

	// functions for the picking: 
	Model*  TestIntersectionWithModel(const int mouseX, const int mouseY,
		const POINT & windowDimensions,
		const std::map<std::string, Model*> & modelsList,
		const CameraClass* pCamera,
		const DirectX::XMMATRIX & worldMatrix);

	bool RaySphereIntersect(const DirectX::XMVECTOR & rayOrigin,
		const DirectX::XMVECTOR & rayDirection,
		const float radius);

	bool RayTriangleIntersect(const DirectX::XMVECTOR & rayOrigin,
		const DirectX::XMVECTOR & rayDirection,
		const DirectX::XMVECTOR & vertice0,
		const DirectX::XMVECTOR & vertice1,
		const DirectX::XMVECTOR & vertice2);
};
