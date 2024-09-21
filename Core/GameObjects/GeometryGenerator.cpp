////////////////////////////////////////////////////////////////////////////////////////////
// Filename:    GeometryGenerator.cpp
// Description: implementation of the GeometryClass functional;
//
// Created:     13.03.24
////////////////////////////////////////////////////////////////////////////////////////////
#include "GeometryGenerator.h"

#include "../Engine/log.h"
#include "../Common/MathHelper.h"
#include "../Common/Convert.h"
#include "../Render/Color.h"
#include "../GameObjects/ModelMath.h"


#include "Waves.h"


typedef DirectX::PackedVector::XMCOLOR XMCOLOR;

GeometryGenerator::GeometryGenerator()
{
	//
}

//////////////////////////////////////////////////////////

void GeometryGenerator::GenerateCubeMesh(Mesh::MeshData& cubeMesh)
{
	// MANUALLY CREATE A CUBE

	const UINT vertexCount = 24;
	const UINT indexCount = 36;

	std::vector<DirectX::XMFLOAT2> texCoords(4);  // 4 coords for each corner of the texture
	std::vector<DirectX::XMFLOAT3> verticesPos;
	std::vector<DirectX::XMFLOAT3> facesNormals;

	// arrays for vertices/indices data
	cubeMesh.vertices.resize(vertexCount);
	cubeMesh.indices.resize(indexCount);

	SetupCubeVerticesPositions(verticesPos);
	SetupCubeFacesNormals(facesNormals);

	// ----------------------------------- 

	// compute the bounding box of the cube mesh
	XMVECTOR vMin{ FLT_MAX, FLT_MAX, FLT_MAX };
	XMVECTOR vMax{ FLT_MIN, FLT_MIN, FLT_MIN };

	for (const XMFLOAT3& pos : verticesPos)
	{
		XMVECTOR P = XMLoadFloat3(&pos);
		vMin = XMVectorMin(vMin, P);
		vMax = XMVectorMax(vMax, P);
	}

	// convert min/max representation to center and extents representation
	XMStoreFloat3(&cubeMesh.AABB.Center, 0.5f * (vMin + vMax));
	XMStoreFloat3(&cubeMesh.AABB.Extents, 0.5f * (vMax - vMin));


	// ----------------------------------- 
	// create vertices of the cube 

	std::vector<u32> verticesPosIdxs =
	{
		5,4,0,1,   // front
		3,2,6,7,   // back
		7,6,4,5,   // left
		1,0,2,3,   // right
		4,6,2,0,   // top
		7,5,1,3    // bottom
	};

	// setup position for each vertex
	for (u32 v_idx = 0; const u32 pos_idx : verticesPosIdxs)
		cubeMesh.vertices[v_idx++].position = verticesPos[pos_idx];


	// setup the texture coords of each cube's vertex
	texCoords[0] = { 0, 1 };
	texCoords[1] = { 0, 0 };
	texCoords[2] = { 1, 0 };
	texCoords[3] = { 1, 1 };

	for (UINT idx = 0; idx < vertexCount; idx += 4)
	{
		cubeMesh.vertices[idx + 0].texture = texCoords[0];
		cubeMesh.vertices[idx + 1].texture = texCoords[1];
		cubeMesh.vertices[idx + 2].texture = texCoords[2];
		cubeMesh.vertices[idx + 3].texture = texCoords[3];
	}

	// setup the normals for each vertex of each side of the cube
	for (UINT v_idx = 0, normal_idx = 0; v_idx < vertexCount; v_idx += 4, ++normal_idx)
	{
		cubeMesh.vertices[v_idx + 0].normal = facesNormals[normal_idx];
		cubeMesh.vertices[v_idx + 1].normal = facesNormals[normal_idx];
		cubeMesh.vertices[v_idx + 2].normal = facesNormals[normal_idx];
		cubeMesh.vertices[v_idx + 3].normal = facesNormals[normal_idx];
	}

	// generate a unique colour for each vertex of each side of the cube
	for (UINT idx = 0; idx < vertexCount; ++idx)
	{
		// stored as a 32-bit ARGB color vector
		const XMCOLOR color(MathHelper::RandF(), MathHelper::RandF(), MathHelper::RandF(), 1.0f);
		
		// ATTENTION: the color member has a DirectX::PackedVector::XMCOLOR type
		cubeMesh.vertices[idx].color = Convert::ArgbToAbgr(color);
	}

	// is used for calculations of the model's normal vector, binormal, etc.
	ModelMath modelMath;

	// after the model data has been loaded we now call the CalculateModelVectors() to
	// calculate the tangent and binormal. It also recalculates the normal vector;
	modelMath.CalculateModelVectors(cubeMesh.vertices, false);

	//
	// --- setup the indices of the cube --- //
	//
	cubeMesh.indices =
	{
		0,1,2,    0,2,3,    // front
		4,5,6,    4,6,7,    // back
		8,9,10,   8,10,11,  // left
		12,13,14, 12,14,15, // right			
		16,17,18, 16,18,19, // top			
		20,21,22, 20,22,23  // bottom
	};

	// setup default material for the mesh
	SetDefaultMaterial(cubeMesh.material);
	
}

///////////////////////////////////////////////////////////

void GeometryGenerator::GenerateAxisMesh(Mesh::MeshData & meshData)
{
	// create a mesh which contains axis data;
	// (axis are used for editor mode)

	const UINT axisVerticesCount = 6;

	const XMCOLOR & red   = Colors::Red;
	const XMCOLOR & green = Colors::Green;
	const XMCOLOR & blue  = Colors::Blue;


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
	meshData.indices = { 0,1,2,3,4,5 };
	
}

//////////////////////////////////////////////////////////

void GeometryGenerator::GeneratePlaneMesh(
	const float width,
	const float height,
	Mesh::MeshData& meshData)
{
	assert((width > 0) && (height > 0));

	// since each 2D sprite is just a plane it has 4 vertices and 6 indices
	meshData.vertices.resize(4);
	meshData.indices.resize(6);

	// ------------------------------------------------------- //
	// setup the vertices positions

	const float halfWidth = 0.5f * width;
	const float halfHeight = 0.5f * height;

	// top left / bottom right
	meshData.vertices[0].position = { -halfWidth, +halfHeight,  0 };
	meshData.vertices[1].position = { +halfWidth, -halfHeight,  0 };

	// bottom left / top right
	meshData.vertices[2].position = { -halfWidth, -halfHeight,  0 };
	meshData.vertices[3].position = { +halfWidth, +halfHeight,  0 };

	// setup the texture coords of each vertex
	meshData.vertices[0].texture = { 0, 0 };
	meshData.vertices[1].texture = { 1, 1 };
	meshData.vertices[2].texture = { 0, 1 };
	meshData.vertices[3].texture = { 1, 0 };

	// setup the indices
	meshData.indices = { 0, 1, 2, 0, 3, 1 };

	for (VERTEX& v : meshData.vertices)
		v.normal = { 0,0,-1 };

	// setup default material for the mesh
	SetDefaultMaterial(meshData.material);
}

//////////////////////////////////////////////////////////

void GeometryGenerator::GenerateFlatGridMesh(
	const float width,
	const float depth,
	const UINT verticesByX,
	const UINT verticesByZ,
	Mesh::MeshData & meshData)
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

	assert((UINT)width >= 1);
	assert((UINT)depth >= 1);

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
		//meshData.vertices.reserve(vertexCount);
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(e.what());
		throw EngineException("can't allocate memory for vertices of the grid");
	}


	// precompute X-coords (of position) and tu-component (of texture) for each quad 
	std::vector<float> quadsXCoords;
	std::vector<float> quadsTU;
	quadsXCoords.reserve(verticesByX);

	for (UINT j = 0; j < verticesByX; ++j)
		quadsXCoords.push_back(-halfWidth + j * dx);

	for (UINT j = 0; j < verticesByX; ++j)
		quadsTU.push_back(j * du);


	// make vertices for the grid
	for (UINT i = 0; i < verticesByZ; ++i)
	{
		const float z = halfDepth - i * dz;

		for (UINT j = 0; j < verticesByX; ++j)
		{
			VERTEX& vertex = meshData.vertices[i*verticesByZ + j];

			vertex.position = DirectX::XMFLOAT3(quadsXCoords[j], 0.0f, z);
			vertex.texture = DirectX::XMFLOAT2(quadsTU[j], i * dv);
			vertex.normal = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
			vertex.tangent = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
			vertex.binormal = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);
		}
	}

	// compute the bounding box of the mesh
	XMStoreFloat3(&meshData.AABB.Center, { 0,0,0 });
	XMStoreFloat3(&meshData.AABB.Extents, { width, 0.0f, depth });
	

	//
	// Create grid indices 
	// 
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

	// setup default material for the mesh
	SetDefaultMaterial(meshData.material);
}

//////////////////////////////////////////////////////////

void GeometryGenerator::GeneratePyramidMesh(
	const float height,                                // height of the pyramid
	const float baseWidth,                             // width (length by X) of one of the base side
	const float baseDepth,                             // depth (length by Z) of one of the base side
	Mesh::MeshData& meshData)
{
	// THIS FUNCTION constructs a pyramid by the input height, baseWidth, baseDepth,
	// and stores its vertices and indices into the meshData variable;

	meshData.vertices.clear();
	meshData.indices.clear();

	const UINT verticesOfSides = 12;
	const UINT verticesCount = 16;

	//const float halfHeight = 0.5f * height;
	const float halfBaseWidth = 0.5f * baseWidth;
	const float halfBaseDepth = 0.5f * baseDepth;

	const XMCOLOR tipColor = Convert::ArgbToAbgr(Colors::Red);
	const XMCOLOR baseColor = Convert::ArgbToAbgr(Colors::Green);

	// -------------------------------------------------- //

	// create a tip vertex 
	VERTEX tipVertex;

	tipVertex.position = { 0, height, 0 };
	tipVertex.texture = { 0.5f, 0 };   // upper center of texture
	tipVertex.color = tipColor;

	// -------------------------------------------------- //

	// create pyramid sides
	meshData.vertices.resize(verticesCount);

	const std::vector<DirectX::XMFLOAT3> basePositions =
	{
		{ -baseWidth, 0, +baseDepth },
		{ -baseWidth, 0, -baseDepth },
		{ +baseWidth, 0, -baseDepth },
		{ +baseWidth, 0, +baseDepth }
	};

	// first side
	meshData.vertices[0] = tipVertex;
	meshData.vertices[1].position = basePositions[0];
	meshData.vertices[2].position = basePositions[1];

	// second side
	meshData.vertices[3] = tipVertex;
	meshData.vertices[4].position = basePositions[1];
	meshData.vertices[5].position = basePositions[2];

	// third side
	meshData.vertices[6] = tipVertex;
	meshData.vertices[7].position = basePositions[2];
	meshData.vertices[8].position = basePositions[3];

	// fourth side
	meshData.vertices[9] = tipVertex;
	meshData.vertices[10].position = basePositions[3];
	meshData.vertices[11].position = basePositions[0];

	// -------------------------------------------------- //

	// set texture coords and colors for vertices of the pyramid's sides
	for (UINT idx = 0; idx < verticesOfSides; idx += 3)
	{
		meshData.vertices[idx + 1].texture = { 0, 1 };   // bottom left of texture
		meshData.vertices[idx + 1].color = baseColor;
		meshData.vertices[idx + 2].texture = { 1, 1 };   // bottom right of texture
		meshData.vertices[idx + 2].color = baseColor;
	}

	// bottom
	const std::vector<DirectX::XMFLOAT2> bottomTexCoords = { { 1, 0 },{ 1, 1 },{ 0, 1 },{ 0, 0 } };
	const DirectX::XMFLOAT3 bottomNormalVec{ 0, -1, 0 };

	for (UINT v_idx = verticesOfSides, data_idx = 0; v_idx < verticesCount; ++v_idx, ++data_idx)
	{
		meshData.vertices[v_idx].position = basePositions[data_idx];
		meshData.vertices[v_idx].texture = bottomTexCoords[data_idx];
		meshData.vertices[v_idx].normal = bottomNormalVec;
	}


	// -------------------------------------------------- //

	ModelMath modelMath;

	// compute normal vectors for the first face of the pyramid
	DirectX::XMVECTOR tangent;
	DirectX::XMVECTOR bitangent;
	DirectX::XMVECTOR normal;

	// for each side of the pyramid we compute a tangent, bitangent, and normal vector
	for (UINT v_idx = 0; v_idx < 12; v_idx += 3)
	{
		modelMath.CalculateTangentBinormal(
			meshData.vertices[v_idx + 0],
			meshData.vertices[v_idx + 1],
			meshData.vertices[v_idx + 2],
			tangent, bitangent);

		modelMath.CalculateNormal(tangent, bitangent, normal);

		// convert vectors of normal, tangent, and bitangent into XMFLOAT3
		DirectX::XMFLOAT3 normalFloat3;
		DirectX::XMFLOAT3 tangentFloat3;
		DirectX::XMFLOAT3 bitangentFloat3;
		DirectX::XMStoreFloat3(&normalFloat3, normal);
		DirectX::XMStoreFloat3(&tangentFloat3, tangent);
		DirectX::XMStoreFloat3(&bitangentFloat3, bitangent);

		// for each vertex of this face we store the normal, tangent, bitangent
		for (UINT idx = 0; idx < 3; ++idx)
		{
			const UINT index = v_idx + idx;
			meshData.vertices[index].normal = normalFloat3;
			meshData.vertices[index].tangent = tangentFloat3;
			meshData.vertices[index].binormal = bitangentFloat3;
		}
	}



	//
	// create indices data for the pyramid
	//
	meshData.indices = {

		// sides
		0, 2, 1,
		3, 5, 4,
		6, 8, 7,
		9, 11, 10,

		// bottom
		13, 14, 12,
		14, 15, 12,
	};

	// setup default material for the mesh
	SetDefaultMaterial(meshData.material);
}

//////////////////////////////////////////////////////////

void GeometryGenerator::GenerateWavesMesh(
	const UINT numRows,
	const UINT numColumns,
	const float spatialStep,
	const float timeStep,
	const float speed,
	const float damping,
	Waves & waves, 
	_Out_ Mesh::MeshData & wavesMesh)
{
	std::vector<DirectX::XMFLOAT3> positions;   // positions of each vertex of the wave
	std::vector<DirectX::XMFLOAT3> normals;     // normal vectors of each vertex of the wave
	const UINT numOfDisturbs = 100;

	// -----------------------------------------------------------------------------

	// generate positions/normals/tangentX for waves vertices
	waves.Init(numRows,
		numColumns,
		spatialStep,
		timeStep,
		speed,
		damping);
	
	// make disturbs of the wave
	for (UINT idx = 0; idx < numOfDisturbs; ++idx)
	{
		// generate random ijth indices of some wave's vertex which will be disturbed
		DWORD i = 5 + rand() % 190;
		DWORD j = 5 + rand() % 190;

		// random magnitude of the disturb
		float magnitude = MathHelper::RandF(1.0f, 2.0f);

		waves.Disturb(i, j, magnitude);
	}

	waves.Update(0.04f);

	// -----------------------------------------------------------------------------

	const UINT vertexCount = waves.GetVertexCount();
	positions = waves.GetPositions();
	normals = waves.GetNormals();
	const float wavesWidth_inv = 1.0f / waves.GetWidth();
	const float wavedDepth_inv = 1.0f / waves.GetDepth();



	wavesMesh.vertices.resize(vertexCount);
	wavesMesh.indices.resize(vertexCount);

	// setup vertices of the wave
	for (UINT idx = 0; idx < vertexCount; ++idx)
	{
		wavesMesh.vertices[idx].position = positions[idx];
		wavesMesh.vertices[idx].normal = normals[idx];

		// derive tex-coords in [0,1] from position.
		wavesMesh.vertices[idx].texture.x = 0.5f + wavesMesh.vertices[idx].position.x * wavesWidth_inv;
		wavesMesh.vertices[idx].texture.y = 0.5f - wavesMesh.vertices[idx].position.z * wavedDepth_inv;

	}

	// -----------------------------------------------------------------------------
	// Create the indices of the wave's vertices (3 indices per face)

	std::vector<UINT> indices(3 * waves.GetTriangleCount());

	// Iterate over each quad.
	UINT m = waves.GetRowCount();
	UINT n = waves.GetColumnCount();
	int k = 0;

	for (UINT i = 0; i < m - 1; ++i)
	{
		const UINT temp_idx_1 = i*n;
		const UINT temp_idx_2 = (i + 1)*n;

		for (DWORD j = 0; j < n - 1; ++j)
		{
			const UINT idx_1 = temp_idx_1 + j;
			const UINT idx_2 = temp_idx_2 + j;

			// first triangle
			indices[k + 0] = idx_1;
			indices[k + 1] = idx_1 + 1;
			indices[k + 2] = idx_2;

			// second triangle
			indices[k + 3] = idx_2;
			indices[k + 4] = idx_1 + 1;
			indices[k + 5] = idx_2 + 1;

			k += 6; // next quad
		}
	}

	// store indices of the wave
	wavesMesh.indices = indices;

	// setup default material for the mesh
	SetDefaultMaterial(wavesMesh.material);
}

//////////////////////////////////////////////////////////

void GeometryGenerator::GenerateCylinderMesh(
	const float bottomRadius,
	const float topRadius,
	const float height,
	const UINT sliceCount,
	const UINT stackCount,
	Mesh::MeshData & meshData)
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
	BuildCylinderStacks(
		bottomRadius,
		topRadius,
		height,
		sliceCount,
		stackCount,
		tu,
		thetaSinuses,
		theta—osines,
		meshData);

	BuildCylinderTopCap(
		topRadius, 
		height,
		sliceCount,
		thetaSinuses,
		theta—osines,
		meshData);

	BuildCylinderBottomCap(
		bottomRadius, 
		height,
		sliceCount, 
		thetaSinuses,
		theta—osines,
		meshData);


	// ----------------------------------- 

	// compute a bounding box of the mesh
	XMVECTOR vMin{ FLT_MAX, FLT_MAX, FLT_MAX };
	XMVECTOR vMax{ FLT_MIN, FLT_MIN, FLT_MIN };

	for (const VERTEX& v : meshData.vertices)
	{
		XMVECTOR P = XMLoadFloat3(&v.position);
		vMin = XMVectorMin(vMin, P);
		vMax = XMVectorMax(vMax, P);
	}

	// convert min/max representation to center and extents representation
	XMStoreFloat3(&meshData.AABB.Center, 0.5f * (vMin + vMax));
	XMStoreFloat3(&meshData.AABB.Extents, 0.5f * (vMax - vMin));


	// setup default material for the mesh
	SetDefaultMaterial(meshData.material);
}

///////////////////////////////////////////////////////////

void GeometryGenerator::GenerateSphereMesh(
	const float radius,
	const UINT sliceCount,
	const UINT stackCount,
	Mesh::MeshData & mesh)
{
	// THIS FUNCTION creates data for the sphere mesh by specifying its radius, and
	// the slice and stack count. The algorithm for generation the sphere is very similar to 
	// that of the cylinder, except that the radius per ring changes is a nonlinear way
	// based on trigonometric functions

	const float du = 1.0f / sliceCount;
	const float dv = 1.0f / stackCount;

	const float dTheta = DirectX::XM_2PI * du;   // horizontal ring angles delta
	const float dAlpha = DirectX::XM_PI  * dv;    // vertical angle delta

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


	// prepare enough amount of memory for vertices of the sphere
	mesh.vertices.reserve(stackCount * sliceCount);

	// build vertices
	for (UINT i = 0; i < stackCount; ++i)
	{
		// vertical angle from bottom to top
		const float curAlpha = -DirectX::XM_PIDIV2 + (float)(i + 1) * dAlpha;

		// radius and height of the current ring
		const float r = radius * cosf(curAlpha);
		const float y = radius * sinf(curAlpha);

		// Y coord of the texture
		const float tv = 1.0f - (float)(i + 1) * dv;

		// make vertices for this ring
		for (UINT j = 0; j <= sliceCount; ++j)
		{
			mesh.vertices.emplace_back(
				r*theta—osines[j], y, r*thetaSinuses[j],   // position
				tu[j], tv);                                // tex coords
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
			const UINT idx_3 = idx_2 + 1;
			const UINT idx_4 = idx_1 + 1;

			mesh.indices.insert(mesh.indices.end(),
			{
				idx_1, idx_2, idx_3, // first triangle of the face
				idx_1, idx_3, idx_4  // second triangle of the face
			});
		}
	}
	

	//
	// build bottom of the sphere
	//

	// make the lowest vertex of the sphere
	VERTEX vertex;

	vertex.position = { 0, -radius, 0 };
	vertex.texture  = { 0.5f, 1.0f };
	mesh.vertices.push_back(vertex);

	// store normalized normal vectors as XMFLOAT3
	for (VERTEX& v : mesh.vertices)
		DirectX::XMStoreFloat3(&v.normal, DirectX::XMVector3Normalize({ v.position.x, v.position.y, v.position.z }));

	// compute the bounding box of the mesh
	XMStoreFloat3(&mesh.AABB.Center, { 0,0,0 });
	XMStoreFloat3(&mesh.AABB.Extents, { radius, radius, radius });



	//
	// setup indices of the sphere
	//
	
	// index of center vertex
	const UINT centerIdx = (UINT)mesh.vertices.size() - 1;

	mesh.indices.reserve(sliceCount * 3);

	// build faces for bottom of the sphere
	for (UINT i = 0; i < sliceCount; ++i)
	{
		mesh.indices.push_back(centerIdx);
		mesh.indices.push_back(i);
		mesh.indices.push_back(i+1);
	}

	// setup default material for the mesh
	SetDefaultMaterial(mesh.material);
}

///////////////////////////////////////////////////////////

void GeometryGenerator::GenerateGeosphereMesh(
	const float radius,
	UINT numSubdivisions,
	Mesh::MeshData & meshData)
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

	// setup vertices positions
	for (size_t i = 0; i < numOfPos; ++i)
		meshData.vertices[i].position = pos[i];

	// setup indices data
	meshData.indices.insert(meshData.indices.end(), indicesData, indicesData + numOfIdx);

	// divide each triangle of sphere into smaller ones
	for (size_t i = 0; i < numSubdivisions; ++i)
		Subdivide(meshData);

	// project vertices onto the sphere and scale
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

	// setup default material for the mesh
	SetDefaultMaterial(meshData.material);
}



///////////////////////////////////////////////////////////////////////////////////////////
//                               PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////////////////////////////////////


void GeometryGenerator::SetupCubeVerticesPositions(
	std::vector<DirectX::XMFLOAT3>& verticesPos)
{
	// setup the vertices positions of a cube
	verticesPos =
	{
		// right side
		{ 1,  1, -1 },  // near top
		{ 1, -1, -1 },  // near bottom
		{ 1,  1,  1 },  // far top
		{ 1, -1,  1 },  // far bottom

						// left side
		{ -1,  1, -1 }, // near top
		{ -1, -1, -1 }, // near bottom
		{ -1,  1,  1 }, // far top 
		{ -1, -1,  1 }, // far bottom
	};
}

///////////////////////////////////////////////////////////

void GeometryGenerator::SetupCubeFacesNormals(
	std::vector<DirectX::XMFLOAT3>& facesNormals)
{
	// setup the faces normals of a cube
	facesNormals =
	{
		{ 0,  0, -1}, // front
		{ 0,  0, +1}, // back
		{-1,  0,  0}, // left
		{+1,  0,  0}, // right
		{ 0, +1,  0}, // top
		{ 0, -1,  0}  // bottom
	};
}

///////////////////////////////////////////////////////////

void GeometryGenerator::BuildCylinderStacks(
	const float bottomRadius,
	const float topRadius,
	const float height,
	const UINT sliceCount,
	const UINT stackCount,
	const std::vector<float> & tu,           // texture X coords
	const std::vector<float> & thetaSinuses,
	const std::vector<float> & thetaCosines,
	Mesh::MeshData & meshData)
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
	Mesh::MeshData & meshData)
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
	Mesh::MeshData & meshData)
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

void GeometryGenerator::Subdivide(Mesh::MeshData & outMeshData)
{
	// save copy of the input geometry
	Mesh::MeshData inputCopy = outMeshData;

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
		// vertex components in GenerateGeosphereMesh.

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

///////////////////////////////////////////////////////////

void GeometryGenerator::SetDefaultMaterial(Mesh::Material& mat)
{
	mat.ambient_  = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mat.diffuse_  = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mat.specular_ = XMFLOAT4(0.2f, 0.2f, 0.2f, 2.0f);
}