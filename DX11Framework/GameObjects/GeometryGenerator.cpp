////////////////////////////////////////////////////////////////////////////////////////////
// Filename:    GeometryGenerator.cpp
// Description: implementation of the GeometryClass functional;
//
// Created:     13.03.24
////////////////////////////////////////////////////////////////////////////////////////////
#include "GeometryGenerator.h"


GeometryGenerator::GeometryGenerator()
{

}

//////////////////////////////////////////////////////////

void GeometryGenerator::CreateGrid(
	const float width,
	const float depth,
	const UINT m,
	const UINT n,
	MeshData & meshData)
{
	// THIS FUNCTION builds the grid in the XZ-plane. A grid of (m * n) vertices includes
	// (m - 1) * (n - 1) quads (or cells). Each cell will be covered by two triangles, 
	// so there is total of 2 * (m - 1) * (n - 1) triangles. 
	//
	// If the grid has width w and depth d, 
	// the cell spacing along the x-asis is dx = w/(n - 1) and 
	// the cell spacing along the z-axis is dz = d/(m - 1).
	// To generate vertices, we start at the upper left corner and incrementally compute
	// the vertex coordinates row-by-row.
	//
	// The coordinates of the ij-th grid vertex in the xz-plane are given by:
	//              Vij = [-0.5*width + j*dx, 0.0, 0.5*depth - i*dz];


	const UINT faceCount = (m - 1)*(n - 1) * 2;  // since each quad has 2 triangles
	const UINT vertexCount = m*n;
	
	//
	// Create vertices
	//

	const float halfWidth = 0.5f * width;
	const float halfDepth = 0.5f * depth;

	const float du = 1.0f / (n - 1);
	const float dv = 1.0f / (m - 1);
	const float dx = width * du;
	const float dz = depth * dv;

	meshData.vertices.resize(vertexCount);

	for (UINT i = 0; i < m; ++i)
	{
		const float z = halfDepth - i * dz;

		for (UINT j = 0; j < n; ++j)
		{
			const float x = -halfWidth * j*dx;
			const UINT idx = i*n + j;

			meshData.vertices[idx].position = DirectX::XMFLOAT3(x, 0.0f, z);

			// vertices data for texturing
			meshData.vertices[idx].texture = DirectX::XMFLOAT2(j*du, i*dv);

			// vertices data for lighting
			meshData.vertices[idx].normal = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
			meshData.vertices[idx].tangent = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
		}
	}
}

//////////////////////////////////////////////////////////