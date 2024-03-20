////////////////////////////////////////////////////////////////////////////////////////////
// Filename:    GeometryGenerator.cpp
// Description: implementation of the GeometryClass functional;
//
// Created:     13.03.24
////////////////////////////////////////////////////////////////////////////////////////////
#include "GeometryGenerator.h"

#include "../Engine/log.h"
#include "../Common/MathHelper.h"

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

	// Add one because we duplicate the first and last vertex per ring
	// since the texture coordinates are different
	const UINT ringVertexCount = sliceCount + 1;

	const float du = 1.0f / sliceCount;
	const float dTheta = DirectX::XM_2PI * du;        // delta theta

	std::vector<float> tu(ringVertexCount);           // texture X coords
	std::vector<float> thetaSinuses(ringVertexCount);
	std::vector<float> theta—osines(ringVertexCount);

	// precompute trigonometry for each Theta angle since these values are the same
	// for each ring vertex of the cylinder; also precompute texture coordinates by X
	for (UINT j = 0; j <= sliceCount; ++j)
	{
		// texture X coord
		tu[j] = (float)j * du;

		// sin/cos of Theta
		const float curTheta = j * dTheta;
		thetaSinuses[j] = sinf(curTheta);
		theta—osines[j] = cosf(curTheta);
	}

	//
	// create 3 main parts of cylinder: side, top cap, bottom cap
	//
	this->CreateCylinderStacks(
		bottomRadius,
		topRadius,
		height,
		sliceCount,
		stackCount,
		tu,
		thetaSinuses,
		theta—osines,
		meshData);

	this->BuildCylinderTopCap(
		topRadius, 
		height,
		sliceCount,
		thetaSinuses,
		theta—osines,
		meshData);

	this->BuildCylinderBottomCap(
		bottomRadius, 
		height,
		sliceCount, 
		thetaSinuses,
		theta—osines,
		meshData);
}

///////////////////////////////////////////////////////////

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

	const float dTheta = DirectX::XM_2PI / sliceCount;   // horizontal ring angles delta
	const float dAlpha = DirectX::XM_PI / stackCount;    // vertical angle

	const float du = 1.0f / sliceCount;
	const float dv = 1.0f / stackCount;
	//const float halfRadius = 0.5f * radius;

	const UINT ringCount = stackCount + 1;

	std::vector<float> tu(sliceCount + 1);           // texture X coords
	std::vector<float> thetaSinuses(sliceCount+1);
	std::vector<float> theta—osines(sliceCount+1);

	// precompute trigonometry for each Theta angle since these values are the same
	// for each ring of the sphere; also precompute texture coordinates by X
	for (UINT j = 0; j <= sliceCount; ++j)
	{
		// texture X coord
		tu[j] = (float)j * du;

		// sin/cos of Theta
		const float curTheta = j * dTheta;
		theta—osines[j] = cosf(curTheta);
		thetaSinuses[j] = sinf(curTheta);
	}


	for (UINT i = 0; i < stackCount; ++i)
	{
		const float curAlpha = -DirectX::XM_PIDIV2 + (float)(i + 1) * dAlpha;

		// radius of the current ring
		const float r = radius * cosf(curAlpha);

		// height of the current ring
		const float y = radius * sinf(curAlpha);

		// Y coord of the texture
		const float tv = 1.0f - (float)(i + 1) * dv;

		// make vertices for this ring
		for (UINT j = 0; j <= sliceCount; ++j)
		{
			VERTEX vertex;

			vertex.position = { r*theta—osines[j], y, r*thetaSinuses[j] };
			vertex.texture.x = tu[j];
			vertex.texture.y = tv;

			sphereMesh.vertices.push_back(vertex);
		}
	}


	//
	// build indices for the sphere
	//

	// Add one because we duplicate the first and last vertex per ring
	// since the texture coordinates are different
	const UINT ringVertexCount = sliceCount + 1;

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
	

	//
	// build bottom of the sphere
	//

	// make the lowest vertex of the sphere
	VERTEX vertex;

	vertex.position = { 0, -radius, 0 };
	vertex.texture = { 0.5f, 1.0f };
	sphereMesh.vertices.push_back(vertex);


	// index of center vertex
	const UINT centerIdx = (UINT)sphereMesh.vertices.size() - 1;

	// build faces for bottom of the sphere
	for (UINT i = 0; i < sliceCount; ++i)
	{
		sphereMesh.indices.push_back(centerIdx);
		sphereMesh.indices.push_back(i);
		sphereMesh.indices.push_back(i+1);
	}
	
	return;
}

///////////////////////////////////////////////////////////

void GeometryGenerator::CreateGeosphere(
	const float radius,
	UINT numSubdivisions,
	MeshData & meshData)
{
	// THIS FUNCTION creates a geosphere. A geosphere approximates a sphere using 
	// triangles with almost equal areas as well as equal side length.
	// To generate a geosphere, we start with an icosahedron, subdivide the triangles, and
	// then project the new vertices onto the sphere with the given radius. We can repeat
	// this process to improve the tessellation.
	// The new vertices are found just by taking the midpoints along the edges of the 
	// original triangle. The new vertices can then be projected onto a sphere of radius R
	// by projection the vertices onto the unit sphere an then scalar multiplying by R:
	//                          v' = R * normalize(v)

	// put a cap on the number of subdivisition
	numSubdivisions = min(numSubdivisions, 5u);

	// approximate a sphere by tesselating an icosahedron
	const float X = 0.525731f;
	const float Z = 0.850651f;

	const UINT numOfPos = 12;
	const UINT numOfIdx = 60;

	DirectX::XMFLOAT3 pos[numOfPos] =
	{
		{-X, 0, Z},  {X, 0, Z},
		{-X, 0, -Z}, {X, 0, -Z},
		{0, Z, X},   {0, Z, -X},
		{0, -Z, X},  {0, -Z, -X},
		{Z, X, 0},   {-Z, X, 0},
		{Z, -X, 0},  {-Z, -X, 0}
	};

	DWORD indicesData[numOfIdx] =
	{
		1,4,0,   4,9,0,   4,5,9,  8,5,4,   1,8,4,
		1,10,8,  10,3,8,  8,3,5,  3,2,5,   3,7,2,
		3,10,7,  10,6,7,  6,11,7, 6,0,11,  6,1,0,
		10,1,6,  11,0,9,  2,11,9, 5,2,9,   11,2,7
	};

	
	// allocate memory for the sphere's mesh data
	meshData.vertices.resize(numOfPos);
	meshData.indices.resize(numOfIdx);

	// make vertices
	for (size_t i = 0; i < numOfPos; ++i)
		meshData.vertices[i].position = pos[i];

	// make indices
	meshData.indices.insert(meshData.indices.end(), indicesData, indicesData + numOfIdx);

	for (size_t i = 0; i < numSubdivisions; ++i)
		Subdivide(meshData);

	// project vertices onto sphere and scale
	for (UINT i = 0; i < meshData.vertices.size(); ++i)
	{
		// project onto unit sphere
		const DirectX::XMVECTOR N = DirectX::XMLoadFloat3(&meshData.vertices[i].position);
		const DirectX::XMVECTOR n = DirectX::XMVector3Normalize(N);

		// store the normal vector
		DirectX::XMStoreFloat3(&meshData.vertices[i].normal, n);

		// compute and store position of vertex
		DirectX::XMStoreFloat3(&meshData.vertices[i].position, DirectX::XMVectorScale(n, radius));

		// derive texture coordinates from spherical coordinates
		const float theta = MathHelper::AngleFromXY(
			meshData.vertices[i].position.x,
			meshData.vertices[i].position.z);

		const float phi = acosf(meshData.vertices[i].position.y / radius);

		meshData.vertices[i].texture.x = theta / DirectX::XM_2PI;
		meshData.vertices[i].texture.y = phi / DirectX::XM_PI;

		// partial derivative of P with respect to theta
		meshData.vertices[i].tangent.x = -radius * sinf(phi) * sinf(theta);
		meshData.vertices[i].tangent.y = 0.0f;
		meshData.vertices[i].tangent.z = +radius * sinf(phi) * cosf(theta);

		// normalize the tangent
		const DirectX::XMVECTOR T = DirectX::XMLoadFloat3(&meshData.vertices[i].tangent);
		DirectX::XMStoreFloat3(&meshData.vertices[i].tangent, DirectX::XMVector3Normalize(T));
	}
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
	const std::vector<float> & tu,           // texture X coords
	const std::vector<float> & thetaSinuses,
	const std::vector<float> & thetaCosines,
	MeshData & meshData)
{
	// 
	// BUILD CYLINDER STACKS
	//

	const float dv = 1.0f / stackCount;

	// (delta_h)
	const float stackHeight = height * dv;

	// (delta_r) amount to increment radius as we move up each stack level from bottom to top 
	const float dr = (topRadius - bottomRadius);
	const float radiusStep = dr * dv;

	const float halfHeight = -0.5f * height;
	
	const UINT ringCount = stackCount + 1;

	// Add one because we duplicate the first and last vertex per ring
	// since the texture coordinates are different
	const UINT ringVertexCount = sliceCount + 1;





	// compute vertices for each stack ring starting at the bottom and moving up
	for (UINT i = 0; i < ringCount; ++i)
	{
		const float y = halfHeight + i*stackHeight;      // Hi = -(h/2) + i*delta_h,
		const float r = bottomRadius + i*radiusStep;     // Ri = bottomRadius + i*delta_r
		const float tv = 1.0f - (float)(i * dv);         // Y (vertical) coord of the texture

		// vertices of ring
		for (UINT j = 0; j <= sliceCount; ++j)
		{
			VERTEX vertex;

			const float c = thetaCosines[j];
			const float s = thetaSinuses[j];

			vertex.position = DirectX::XMFLOAT3(r*c, y, r*s);

			vertex.texture.x = tu[j];
			vertex.texture.y = tv;

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
}

///////////////////////////////////////////////////////////

void GeometryGenerator::BuildCylinderTopCap(
	const float topRadius,
	const float height,
	const UINT sliceCount,
	const std::vector<float> & thetaSinuses,
	const std::vector<float> & thetaCosines,
	MeshData & meshData)
{
	// THIS FUNCTION generates the cylinder cap geometry amounts to generating the slice
	// triangles of the top/bottom rings to approximate a circle

	const UINT baseIndex = (UINT)meshData.vertices.size();
	const float inv_height = 1.0f / height;
	const float y = 0.5f * height;

	// duplicate cap ring vertices because the texture coordinates and normals differ
	for (UINT i = 0; i <= sliceCount; ++i)
	{
		const float x = topRadius * thetaCosines[i];
		const float z = topRadius * thetaSinuses[i];

		// scale down by the height to try and make cap texture coord
		// area proportional to base
		const float u = x * inv_height + 0.5f;
		const float v = z * inv_height + 0.5f;

		// make a vertex of the cap
		VERTEX vertex;
		vertex.position = { x, y, z };
		vertex.texture = { u, v };
		vertex.normal = { 0, 1, 0 };
		vertex.tangent = { 1, 0, 0 };
		
		// store this vertex
		meshData.vertices.push_back(vertex);
	}

	// cap center vertex
	VERTEX centerVertex;
	centerVertex.position = { 0, y, 0 };
	centerVertex.texture = { 0.5f, 0.5f };
	centerVertex.normal = { 0, 1, 0 };
	centerVertex.tangent = { 1, 0, 0 };

	meshData.vertices.push_back(centerVertex);

	// index of center vertex
	const UINT centerIndex = (UINT)meshData.vertices.size() - 1;

	// make top cap faces
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
	const float height,
	const UINT sliceCount,
	const std::vector<float> & thetaSinuses,
	const std::vector<float> & thetaCosines,
	MeshData & meshData)
{
	// THIS FUNCTION generates the cylinder cap geometry amounts to generating the slice
	// triangles of the top/bottom rings to approximate a circle

	const UINT baseIndex = (UINT)meshData.vertices.size();
	const float y = -0.5f * height;
	const float inv_height = 1.0f / height;

	// duplicate bottom cap ring vertices because the texture coordinates and normals differ
	for (UINT i = 0; i <= sliceCount; ++i)
	{
		const float x = bottomRadius * thetaCosines[i];
		const float z = bottomRadius * thetaSinuses[i];

		// scale down by the height to try and make cap texture coord
		// area proportional to base
		const float u = x * inv_height + 0.5f;
		const float v = z * inv_height + 0.5f;

		// make a vertex of the cap
		VERTEX vertex;
		vertex.position = { x, y, z };
		vertex.texture = { u, v };
		vertex.normal = { 0, -1, 0 };
		vertex.tangent = { 1, 0, 0 };

		// store this vertex
		meshData.vertices.push_back(vertex);
	}

	// bottom cap center vertex
	VERTEX centerVertex;
	centerVertex.position = { 0.0f, y, 0 };
	centerVertex.texture = { 0.5f, 0.5f };
	centerVertex.normal = { 0, -1, 0 };
	centerVertex.tangent = { 1, 0, 0 };

	meshData.vertices.push_back(centerVertex);

	// index of center vertex
	const UINT centerIndex = (UINT)meshData.vertices.size() - 1;

	// build bottom cap faces
	for (UINT i = 0; i < sliceCount; ++i)
	{
		meshData.indices.push_back(baseIndex + i);
		meshData.indices.push_back(baseIndex + i + 1);
		meshData.indices.push_back(centerIndex);
	}

	return;
}

///////////////////////////////////////////////////////////

void GeometryGenerator::Subdivide(MeshData & outMeshData)
{
	// save copy of the input geometry
	MeshData inputCopy = outMeshData;

	outMeshData.vertices.resize(0);
	outMeshData.indices.resize(0);

	//       v1
	//       *
	//      / \
	//     /   \
	//  m0*-----*m1
	//   / \   / \
	//  /   \ /   \
	// *-----*-----*
	// v0    m2     v2

	const UINT numTris = (UINT)inputCopy.indices.size() / 3;

	for (UINT i = 0; i < numTris; ++i)
	{
		const VERTEX v0 = inputCopy.vertices[ inputCopy.indices[i*3+0] ];
		const VERTEX v1 = inputCopy.vertices[ inputCopy.indices[i*3+1] ];
		const VERTEX v2 = inputCopy.vertices[ inputCopy.indices[i*3+2] ];

		//
		// generate midpoints
		//

		VERTEX m0, m1, m2;

		// For subdivision, we just care about the position component. We derive the other
		// vertex components in CreateGeosphere.

		m0.position = DirectX::XMFLOAT3(
			0.5f * (v0.position.x + v1.position.x),
			0.5f * (v0.position.y + v1.position.y),
			0.5f * (v0.position.z + v1.position.z));

		m1.position = DirectX::XMFLOAT3(
			0.5f * (v1.position.x + v2.position.x),
			0.5f * (v1.position.y + v2.position.y),
			0.5f * (v1.position.z + v2.position.z));

		m2.position = DirectX::XMFLOAT3(
			0.5f * (v0.position.x + v2.position.x),
			0.5f * (v0.position.y + v2.position.y),
			0.5f * (v0.position.z + v2.position.z));

		//
		// add new geometry
		//

		// make vertices of subdivided triangle
		outMeshData.vertices.insert(outMeshData.vertices.end(),
		{
			v0, v1, v2,
			m0, m1, m2,
		});

		const UINT new_geo_idx = i * 6;

		// make indices of subdivided triangle
		outMeshData.indices.insert(outMeshData.indices.end(), 
		{
			new_geo_idx + 0,
			new_geo_idx + 3,
			new_geo_idx + 5,

			new_geo_idx + 3,
			new_geo_idx + 4,
			new_geo_idx + 5,

			new_geo_idx + 5,
			new_geo_idx + 4,
			new_geo_idx + 2,

			new_geo_idx + 3,
			new_geo_idx + 1,
			new_geo_idx + 4,
		});
	}
}