////////////////////////////////////////////////////////////////////////////////////////////
// Filename:    GeometryGenerator.cpp
// Description: implementation of the GeometryClass functional;
//
// Created:     13.03.24
////////////////////////////////////////////////////////////////////////////////////////////
#include "GeometryGenerator.h"
#include "../Engine/log.h"

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
	const UINT verticesByX,
	const UINT verticesByZ,
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

	assert((UINT)width > 2);
	assert((UINT)depth > 2);

	meshData.vertices.clear();
	meshData.indices.clear();

	const UINT quadsByX = verticesByX - 1;
	const UINT quadsByZ = verticesByZ - 1;
	const UINT faceCount = quadsByX * quadsByZ * 2;  // quad_num_by_X * quad_num_by_Z * 2_triangles_per_quad
	const UINT vertexCount = verticesByX * verticesByZ;
	
	//
	// Create grid vertices
	//

	const float halfWidth = 0.5f * width;
	const float halfDepth = 0.5f * depth;

	const float du = 1.0f / (float)quadsByX;
	const float dv = 1.0f / (float)quadsByZ;
	const float dx = width * du;      // how many quads we can put in such width
	const float dz = depth * dv;      // how many quads we can put in such depth

	try 
	{
		// allocate memory for vertices of the grid
		meshData.vertices.resize(vertexCount);
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(LOG_MACRO, e.what());
		COM_ERROR_IF_FALSE(false, "can't allocate memory for vertices of the grid");
	}



	// make data for vertices
	for (UINT i = 0; i < verticesByZ; ++i)
	{
		const float z = halfDepth - i * dz;

		for (UINT j = 0; j < verticesByX; ++j)
		{
			const float x = -halfWidth + j*dx;
			const UINT idx = i*verticesByZ + j;

			meshData.vertices[idx].position = DirectX::XMFLOAT3(x, 0.0f, z);

			// vertices data for texturing
			meshData.vertices[idx].texture = DirectX::XMFLOAT2(j*du, i*dv);

			// vertices data for lighting
			meshData.vertices[idx].normal = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
			meshData.vertices[idx].tangent = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);

			//meshData.vertices[idx].color = { 0.5f, 0.5f, 0.5f, 1.0f };
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
	const UINT m = verticesByX;
	const UINT n = verticesByZ;
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
	const UINT sliceCount,
	const UINT stackCount,
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

//////////////////////////////////////////////////////////

void GeometryGenerator::CreateSphere(
	const float radius,
	const UINT sliceCount,
	const UINT stackCount,
	MeshData & sphereMesh)
{
	// THIS FUNCTION creates data for the sphere mesh by specifying its radius, and
	// the slice and stack count. The algorithm for generation the sphere is very similar to 
	// that of the cylinder, except that the radius per ring changes is a nonlinear way
	// based on trigonometric functions

	const float y1 = sinf(-DirectX::XM_PIDIV2);
	const float y2 = sinf(0);
	const float y3 = sinf(DirectX::XM_PIDIV2);

	const float fSliceCount = (float)sliceCount;
	const float fStackCount = (float)stackCount;

	const float dTheta = DirectX::XM_2PI / sliceCount;
	const float dAlpha = DirectX::XM_PI / stackCount;

	const float du = 1.0f / sliceCount;
	const float dv = 1.0f / stackCount;
	const UINT ringCount = stackCount + 1;
	const float halfRadius = 0.5f * radius;



	// build upper half vertices for the sphere
	for (UINT i = 0; i < stackCount; ++i)
	{
		const float r = radius * cosf(-DirectX::XM_PIDIV2 + (float)(i+1) * dAlpha);
		const float y = radius * sinf(-DirectX::XM_PIDIV2 + (float)(i+1) * dAlpha);

		for (UINT j = 0; j <= sliceCount; ++j)
		{
			const float c = cosf(j * dTheta);
			const float s = sinf(j * dTheta);

			VERTEX vertex;

			vertex.position = { r*c, y, r*s };

			vertex.texture.x = (float)j / fSliceCount;
			vertex.texture.y = 1.0f - (float)(i+1) / fStackCount;

			sphereMesh.vertices.push_back(vertex);
		}
	}



	// build indices for the sphere
	const UINT ringVertexCount = sliceCount + 1;
	UINT k = 0;
	for (UINT i = 0; i < stackCount-1; ++i)
	{
		for (UINT j = 0; j < sliceCount; ++j)
		{
			const UINT idx_1 = i*ringVertexCount + j;
			const UINT idx_2 = (i+1)*ringVertexCount + j;
			const UINT idx_3 = (i+1)*ringVertexCount + j + 1;
			const UINT idx_4 = i*ringVertexCount + j + 1;

			sphereMesh.indices.insert(sphereMesh.indices.end(),
			{
				idx_1, idx_2, idx_3,
				idx_1, idx_3, idx_4
			});
		}
	}
	



	
	// build bottom of the sphere

	// make the lowest vertex of the sphere
	VERTEX vertex;
	vertex.position = { 0, -radius, 0 };
	vertex.texture = { 0.5f, 1.0f };

	sphereMesh.vertices.push_back(vertex);


	const float r = radius * cosf(-DirectX::XM_PIDIV2 + dAlpha);
	const float y = -radius + radius * sinf(-DirectX::XM_PIDIV2 + dAlpha);
	const float tv = 1.0f - 1.0f / fStackCount;
#if 0
	for (UINT j = 0; j <= sliceCount; ++j)
	{
		const float c = cosf(j * dTheta);
		const float s = sinf(j * dTheta);

		VERTEX vertex;

		vertex.position = { r*c, y, r*s };

		vertex.texture.x = (float)j / fSliceCount;
		vertex.texture.y = tv;

		sphereMesh.vertices.push_back(vertex);
	}
#endif
#if 1
	// index of center vertex
	const UINT centerIdx = (UINT)sphereMesh.vertices.size() - 1;

	for (UINT i = 0; i < sliceCount; ++i)
	{
		sphereMesh.indices.push_back(centerIdx);
		sphereMesh.indices.push_back(i);
		sphereMesh.indices.push_back(i+1);
	}
#endif
	
	return;
}



///////////////////////////////////////////////////////////////////////////////////////////
//                               PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////////////////////////////////////


void GeometryGenerator::CreateCylinderStacks(
	const float bottomRadius,
	const float topRadius,
	const float height,
	const UINT sliceCount,
	const UINT stackCount,
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
		const float y = halfHeight + i*stackHeight;   // Hi = -(h/2) + i*delta_h,
		const float r = bottomRadius + i*radiusStep;  // Ri = bottomRadius + i*delta_r

		// vertices of ring
		for (UINT j = 0; j <= sliceCount; ++j)
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

			//const float dr = bottomRadius - topRadius;
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

	// Add one because we duplicate the first and last vertex per ring
	// since the texture coordinates are different
	const UINT ringVertexCount = sliceCount + 1;

	//
	// Create cylinder stacks' indices 
	// (ring: i, slice: j)
	// ABC = (i*n + j,   (i+1)*n + j,    (i+1)*n + j+1)
	// ACD = (i*n + j,   (i+1)*n + j+1,  i*n + j+1)
	//
	//  B(i+1,j)  _______________ C(i+1,j+1)  
	//            |          /  |
	//            |        /    |
	//            |      /      | ij-th QUAD OF CYLINDER
	//            |    /        |
	//            |  /          |
	//   A(i,j)   |/____________| D(i,j+1)

	// compute indices for each stack
	for (UINT i = 0; i < stackCount; ++i)
	{
		for (UINT j = 0; j < sliceCount; ++j)
		{
			const UINT idx_1 = i*ringVertexCount + j;
			const UINT idx_2 = (i + 1)*ringVertexCount + j;
			const UINT idx_3 = (i + 1)*ringVertexCount + j + 1;
			const UINT idx_4 = i*ringVertexCount + j + 1;

			meshData.indices.insert(meshData.indices.end(),
			{
				idx_1, idx_2, idx_3, // ABC
				idx_1, idx_3, idx_4  // ACD
			});
		}
	}

	// and at last we build top and bottom cap of the cylinder
	//this->BuildCylinderTopCap(bottomRadius, topRadius, height, sliceCount, stackCount, meshData);
	//this->BuildCylinderBottomCap(bottomRadius, topRadius, height, sliceCount, stackCount, meshData);
}

///////////////////////////////////////////////////////////

void GeometryGenerator::BuildCylinderTopCap(
	const float bottomRadius,
	const float topRadius,
	const float height,
	const UINT sliceCount,
	const UINT stackCount,
	MeshData & meshData)
{
	// THIS FUNCTION generates the cylinder cap geometry amounts to generating the slice
	// triangles of the top/bottom rings to approximate a circle

	const UINT baseIndex = (UINT)meshData.vertices.size();
	const float halfHeight = 0.5f * height;
	const float dTheta = DirectX::XM_2PI / sliceCount;

	// duplicate cap ring vertices because the texture coordinates and normals differ
	for (UINT i = 0; i <= sliceCount; ++i)
	{
		const float x = topRadius*cosf(i*dTheta);
		const float z = topRadius*sinf(i*dTheta);

		// scale down by the height to try and make cap texture coord
		// area proportional to base
		const float u = x / halfHeight;
		const float v = z / halfHeight;

		// make a vertex of the cap
		VERTEX vertex;
		vertex.position = { x, halfHeight, z };
		vertex.texture = { u, v };
		vertex.normal = { 0, 1, 0 };
		vertex.tangent = { 1, 0, 0 };
		
		// store this vertex
		meshData.vertices.push_back(vertex);
	}

	// cap center vertex
	VERTEX centerVertex;
	centerVertex.position = { 0.0f, halfHeight, 0 };
	centerVertex.texture = { 0.5f, 0.5f };
	centerVertex.normal = { 0, 1, 0 };
	centerVertex.tangent = { 1, 0, 0 };

	meshData.vertices.push_back(centerVertex);

	// index of center vertex
	const UINT centerIndex = (UINT)meshData.vertices.size() - 1;

	for (UINT i = 0; i < sliceCount; ++i)
	{
		meshData.indices.push_back(centerIndex);
		meshData.indices.push_back(baseIndex + i + 1);
		meshData.indices.push_back(baseIndex + i);
	}
}

///////////////////////////////////////////////////////////

void GeometryGenerator::BuildCylinderBottomCap(
	const float bottomRadius,
	const float topRadius,
	const float height,
	const float sliceCount,
	const float stackCount,
	MeshData & meshData)
{
	return;
}