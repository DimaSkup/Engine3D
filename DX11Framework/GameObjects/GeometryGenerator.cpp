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

void GeometryGenerator::CreateAxis(MeshData & meshData)
{
	const UINT axisVerticesCount = 6;
	const UINT axisIndicesCount = 6;

	const DirectX::XMFLOAT4 red{ 1,0,0,1 };
	const DirectX::XMFLOAT4 green{ 0,1,0,1 };
	const DirectX::XMFLOAT4 blue{ 0,0,1,1 };


	//
	// create vertices data of axis 
	//

	meshData.vertices.resize(axisVerticesCount);

	// X-axis
	meshData.vertices[0].position = { -100, 0, 0 };  // negative X
	meshData.vertices[0].color = blue;
	
	meshData.vertices[1].position = { 100, 0, 0 };   // positive X
	meshData.vertices[1].color = blue;

	// Y-axis
	meshData.vertices[2].position = { 0, -100, 0 };  // negative Y
	meshData.vertices[2].color = green;

	meshData.vertices[3].position = { 0, 100, 0 };   // positive Y
	meshData.vertices[3].color = green;

	// Z-axis
	meshData.vertices[4].position = { 0, 0, -100 };  // negative Z
	meshData.vertices[4].color = red;

	meshData.vertices[5].position = { 0, 0, 100 };   // positive Z
	meshData.vertices[5].color = red;

	// 
	// create indices data of axis
	//
	meshData.indices.insert(meshData.indices.begin(), { 0,1,2,3,4,5 });
	
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

	meshData.vertices.clear();
	meshData.indices.clear();

	const UINT faceCount = (m - 1)*(n - 1) * 2;  // 2 triangles per quad
	const UINT vertexCount = m*n;
	
	//
	// Create grid vertices
	//

	const float halfWidth = 0.5f * width;
	const float halfDepth = 0.5f * depth;

	const float du = 1.0f / (n - 1);  
	const float dv = 1.0f / (m - 1);
	const float dx = width / (n-1);      // how many quads we can put in such width
	const float dz = depth / (m-1);      // how many quads we can put in such depth
	const float delta_color_x = 255.0f / n;
	const float delta_color_z = 255.0f / m;

	meshData.vertices.resize(vertexCount);



	for (UINT i = 0; i < m; ++i)
	{
		const float z = halfDepth - i * dz;

		for (UINT j = 0; j < n; ++j)
		{
			const float x = -halfWidth + j*dx;
			const UINT idx = i*n + j;

			meshData.vertices[idx].position = DirectX::XMFLOAT3(x, 0.0f, z);

			// vertices data for texturing
			meshData.vertices[idx].texture = DirectX::XMFLOAT2(j*du, i*dv);

			// vertices data for lighting
			meshData.vertices[idx].normal = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
			meshData.vertices[idx].tangent = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);

			meshData.vertices[idx].color = { du * i, 0.5f, dv * j, 1.0f };
		}
	}

	//
	// Create grid indices 
	// (row: i, column: j)
	// ABC = ((i*n) + j,    (i*n) + j+1,  (i+1)*n + j)
	// CBD = ((i+1)*n +j,   (i*n) + j+1,  (i+1)*n + j+1)
	//
	//  A(i,j)  _______________ B(i,j+1)  
	//          |          /  |
	//          |        /    |
	//          |      /      | ij-th QUAD
	//          |    /        |
	//          |  /          |
	// C(i+1,j) |/____________| D(i+1,j+1)
	//

	meshData.indices.resize(faceCount * 3);   // 3 indices per face

	// iterate over each quad and compute indices
	UINT k = 0;
	for (UINT i = 0; i < m-1; ++i)
	{
		for (UINT j = 0; j < n-1; ++j)
		{
			// first triangle
			meshData.indices[k] = i*n+j;
			meshData.indices[k + 1] = i*n+j+1;
			meshData.indices[k + 2] = (i+1)*n + j;

			// second triangle
			meshData.indices[k + 3] = meshData.indices[k + 2];     // (i+1)*n + j
			meshData.indices[k + 4] = meshData.indices[k + 1];     // i*n+j+1
			meshData.indices[k + 5] = meshData.indices[k + 3] + 1; // (i + 1)*n + j + 1;

			k += 6;  // next quad
		}
	}
}

//////////////////////////////////////////////////////////

void GeometryGenerator::CreateCylinder(
	const float bottomRadius,
	const float topRadius,
	const float height,
	const float sliceCount,
	const float stackCount,
	MeshData & meshData)
{
	// THIS FUNCTION generates a cylinder centered at the origin, parallel to the Y-axis.
	// All the vertices lie on the "rings" of the cylinder, where there are stackCount + 1
	// rings, and each ring has sliceCount unique vertices. The difference in radius between
	// consecutive rings is 
	//               delta_r = (toRadius - bottomRadius) / stackCount;
	//
	// If we start at the bottom ring with index 0, then the radius of the i-th ring is
	//                      Ri = bottomRadius + i*delta_r
	// and height of the i-th ring is
	//                      Hi = -(h/2) + i*delta_h,
	// where delta_h is the stack height and h is the cylinder height. So the basic idea is
	// to iterato over each ring and generate the vertices that lie on that ring.

	meshData.vertices.clear();
	meshData.indices.clear();

	this->CreateCylinderStacks(bottomRadius, topRadius, height, sliceCount, stackCount, meshData);
}



///////////////////////////////////////////////////////////////////////////////////////////
//                               PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////////////////////////////////////


void GeometryGenerator::CreateCylinderStacks(
	const float bottomRadius,
	const float topRadius,
	const float height,
	const float sliceCount,
	const float stackCount,
	MeshData & meshData)
{
	// 
	// BUILD CYLINDER STACKS
	//

	// (delta_h)
	const float stackHeight = height / stackCount;

	// (delta_r) amount to increment radius as we move up each stack level from bottom to top 
	const float dr = (topRadius - bottomRadius);
	const float radiusStep = dr / stackCount;

	const float halfHeight = -0.5f*height;
	const float dTheta = DirectX::XM_2PI / sliceCount;
	const UINT ringCount = stackCount + 1;


	// compute vertices for each stack ring starting at the bottom and moving up
	for (UINT i = 0; i < ringCount; ++i)
	{
		const float y = halfHeight + i*stackHeight;
		const float r = bottomRadius + i*radiusStep;

		// vertices of ring
		for (UINT j = 0; j < sliceCount; ++j)
		{
			VERTEX vertex;

			const float c = cosf(j*dTheta);
			const float s = sinf(j*dTheta);

			vertex.position = DirectX::XMFLOAT3(r*c, y, r*s);

			vertex.texture.x = (float)(j / sliceCount);
			vertex.texture.y = 1.0f - (float)(i / stackCount);

			// Cylinder can be parametrized as follows, where we introduce v parameter
			// that does in the same direction as the v tex-coord so that the bitangent
			// goes in the same direction as the v tex-coord
			//    Let r0 be the bottom radius and let r1 be the top radius.
			//    y(v) = h - hv for v in [0,1]
			//    r(v) = r1 + (r0-r1)*v
			//
			//    x(t, v) = r(v)*cos(t)
			//    y(t, v) = h - hv
			//    z(t, v) = r(v)*sin(t)
			//
			// dx/dt = -r(v)*sin(t)
			// dy/dt = 0
			// dz/dt = +r(v)*cos(t)
			//
			// dx/dv = (r0-r1)*cos(t)
			// dy/dv = -h
			// dz/dv = (r0-r1)*sin(t)

			// set tangent is unit length, and set the binormal
			vertex.tangent = DirectX::XMFLOAT3(-s, 0.0f, c);
			vertex.binormal = DirectX::XMFLOAT3(-dr*c, -height, -dr*s);

			// compute the normal vector
			const DirectX::XMVECTOR T = DirectX::XMLoadFloat3(&vertex.tangent);
			const DirectX::XMVECTOR B = DirectX::XMLoadFloat3(&vertex.binormal);
			const DirectX::XMVECTOR N = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(T, B));
			DirectX::XMStoreFloat3(&vertex.normal, N);

			// store this vertex
			meshData.vertices.push_back(vertex);
		}
	}
}