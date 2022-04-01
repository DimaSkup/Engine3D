/////////////////////////////////////////////////////////////////////
// Filename:    cameraclass.h
// Description: Lets DirectX 11 know from where and how we are 
//              viewving the scene. Will keep track of where the 
//              camera is and its current rotation. It will use the
//              position and rotation info to generate a view matrix.
// Revising:    01.04.22
/////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include "includes.h"

//////////////////////////////////
// Class name: CameraClass
//////////////////////////////////
class CameraClass
{
public:
	CameraClass();
	CameraClass(const CameraClass&);
	~CameraClass();

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);

	D3DXVECTOR3 GetPosition(void);
	D3DXVECTOR3 GetRotation(void);

	void Render(void); // uses the position and rotation of the camera to build and update the view matrix
	void GetViewMatrix(D3DXMATRIX&);

private:
	float m_posX, m_posY, m_posZ;
	float m_rotX, m_rotY, m_rotZ;
	D3DXMATRIX m_viewMatrix;
};