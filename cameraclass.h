/////////////////////////////////////////////////////////////////////
// Filename:     cameraclass.h
// Description:  CameraClass tells the DirectX from where we are 
//               looking at the scene. Initializes the view matrix
//               which we use to render image
// Revising:     07.04.22
/////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include "includes.h"   // some system headers, DirectX headers, macroses, etc.
#include "log.h"        // log system


//////////////////////////////////
// Class name: CameraClass
//////////////////////////////////
class CameraClass 
{
public:
	CameraClass(void);
	CameraClass(const CameraClass&);
	~CameraClass(void);

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);

	D3DXVECTOR3 GetPosition(void);
	D3DXVECTOR3 GetRotation(void);

	void Render(void);	// calculate camera position and generate the view matrix
	void GetViewMatrix(D3DXMATRIX&);

private:
	float m_posX, m_posY, m_posZ;
	float m_rotX, m_rotY, m_rotZ;
	D3DXMATRIX m_viewMatrix;
};