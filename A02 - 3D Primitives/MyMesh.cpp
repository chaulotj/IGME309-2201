#include "MyMesh.h"
void MyMesh::Init(void)
{
	m_bBinded = false;
	m_uVertexCount = 0;

	m_VAO = 0;
	m_VBO = 0;

	m_pShaderMngr = ShaderManager::GetInstance();
}
void MyMesh::Release(void)
{
	m_pShaderMngr = nullptr;

	if (m_VBO > 0)
		glDeleteBuffers(1, &m_VBO);

	if (m_VAO > 0)
		glDeleteVertexArrays(1, &m_VAO);

	m_lVertex.clear();
	m_lVertexPos.clear();
	m_lVertexCol.clear();
}
MyMesh::MyMesh()
{
	Init();
}
MyMesh::~MyMesh() { Release(); }
MyMesh::MyMesh(MyMesh& other)
{
	m_bBinded = other.m_bBinded;

	m_pShaderMngr = other.m_pShaderMngr;

	m_uVertexCount = other.m_uVertexCount;

	m_VAO = other.m_VAO;
	m_VBO = other.m_VBO;
}
MyMesh& MyMesh::operator=(MyMesh& other)
{
	if (this != &other)
	{
		Release();
		Init();
		MyMesh temp(other);
		Swap(temp);
	}
	return *this;
}
void MyMesh::Swap(MyMesh& other)
{
	std::swap(m_bBinded, other.m_bBinded);
	std::swap(m_uVertexCount, other.m_uVertexCount);

	std::swap(m_VAO, other.m_VAO);
	std::swap(m_VBO, other.m_VBO);

	std::swap(m_lVertex, other.m_lVertex);
	std::swap(m_lVertexPos, other.m_lVertexPos);
	std::swap(m_lVertexCol, other.m_lVertexCol);

	std::swap(m_pShaderMngr, other.m_pShaderMngr);
}
void MyMesh::CompleteMesh(vector3 a_v3Color)
{
	uint uColorCount = m_lVertexCol.size();
	for (uint i = uColorCount; i < m_uVertexCount; ++i)
	{
		m_lVertexCol.push_back(a_v3Color);
	}
}
void MyMesh::AddVertexPosition(vector3 a_v3Input)
{
	m_lVertexPos.push_back(a_v3Input);
	m_uVertexCount = m_lVertexPos.size();
}
void MyMesh::AddVertexColor(vector3 a_v3Input)
{
	m_lVertexCol.push_back(a_v3Input);
}
void MyMesh::CompileOpenGL3X(void)
{
	if (m_bBinded)
		return;

	if (m_uVertexCount == 0)
		return;

	CompleteMesh();

	for (uint i = 0; i < m_uVertexCount; i++)
	{
		//Position
		m_lVertex.push_back(m_lVertexPos[i]);
		//Color
		m_lVertex.push_back(m_lVertexCol[i]);
	}
	glGenVertexArrays(1, &m_VAO);//Generate vertex array object
	glGenBuffers(1, &m_VBO);//Generate Vertex Buffered Object

	glBindVertexArray(m_VAO);//Bind the VAO
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);//Bind the VBO
	glBufferData(GL_ARRAY_BUFFER, m_uVertexCount * 2 * sizeof(vector3), &m_lVertex[0], GL_STATIC_DRAW);//Generate space for the VBO

	// Position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)0);

	// Color attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)(1 * sizeof(vector3)));

	m_bBinded = true;

	glBindVertexArray(0); // Unbind VAO
}
void MyMesh::Render(matrix4 a_mProjection, matrix4 a_mView, matrix4 a_mModel)
{
	// Use the buffer and shader
	GLuint nShader = m_pShaderMngr->GetShaderID("Basic");
	glUseProgram(nShader); 

	//Bind the VAO of this object
	glBindVertexArray(m_VAO);

	// Get the GPU variables by their name and hook them to CPU variables
	GLuint MVP = glGetUniformLocation(nShader, "MVP");
	GLuint wire = glGetUniformLocation(nShader, "wire");

	//Final Projection of the Camera
	matrix4 m4MVP = a_mProjection * a_mView * a_mModel;
	glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(m4MVP));
	
	//Solid
	glUniform3f(wire, -1.0f, -1.0f, -1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);  

	//Wire
	glUniform3f(wire, 1.0f, 0.0f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_POLYGON_OFFSET_LINE);
	glPolygonOffset(-1.f, -1.f);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);
	glDisable(GL_POLYGON_OFFSET_LINE);

	glBindVertexArray(0);// Unbind VAO so it does not get in the way of other objects
}
void MyMesh::AddTri(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft)
{
	//C
	//| \
	//A--B
	//This will make the triangle A->B->C 
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);
}
void MyMesh::AddQuad(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft, vector3 a_vTopRight)
{
	//C--D
	//|  |
	//A--B
	//This will make the triangle A->B->C and then the triangle C->B->D
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);

	AddVertexPosition(a_vTopLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopRight);
}
void MyMesh::GenerateCube(float a_fSize, vector3 a_v3Color)
{
	if (a_fSize < 0.01f)
		a_fSize = 0.01f;

	Release();
	Init();

	float fValue = a_fSize * 0.5f;
	//3--2
	//|  |
	//0--1

	vector3 point0(-fValue,-fValue, fValue); //0
	vector3 point1( fValue,-fValue, fValue); //1
	vector3 point2( fValue, fValue, fValue); //2
	vector3 point3(-fValue, fValue, fValue); //3

	vector3 point4(-fValue,-fValue,-fValue); //4
	vector3 point5( fValue,-fValue,-fValue); //5
	vector3 point6( fValue, fValue,-fValue); //6
	vector3 point7(-fValue, fValue,-fValue); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCuboid(vector3 a_v3Dimensions, vector3 a_v3Color)
{
	Release();
	Init();

	vector3 v3Value = a_v3Dimensions * 0.5f;
	//3--2
	//|  |
	//0--1
	vector3 point0(-v3Value.x, -v3Value.y, v3Value.z); //0
	vector3 point1(v3Value.x, -v3Value.y, v3Value.z); //1
	vector3 point2(v3Value.x, v3Value.y, v3Value.z); //2
	vector3 point3(-v3Value.x, v3Value.y, v3Value.z); //3

	vector3 point4(-v3Value.x, -v3Value.y, -v3Value.z); //4
	vector3 point5(v3Value.x, -v3Value.y, -v3Value.z); //5
	vector3 point6(v3Value.x, v3Value.y, -v3Value.z); //6
	vector3 point7(-v3Value.x, v3Value.y, -v3Value.z); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCone(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();
	float angle = 0.0f;
	std::vector<vector3> vertices;
	for (int c = 0; c < a_nSubdivisions; c++) {
		//Filling my vector with points in a circle
		vector3 vertex = vector3(a_fRadius * cos(angle), a_fRadius * sin(angle), -a_fHeight/2);
		vertices.push_back(vertex);
		angle += 2 * PI / a_nSubdivisions;
	}
	for (int c = 0; c < a_nSubdivisions; c++) {
		//Adding triangles to make the circle and to connect the edges of the circle to the top of the cone
		AddTri(vector3(0, 0, -a_fHeight/2), vertices[c], vertices[(c + 1) % a_nSubdivisions]);
		AddTri(vector3(0, 0, a_fHeight/2), vertices[(c + 1) % a_nSubdivisions], vertices[c]);
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCylinder(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	float angle = 0.0f;
	std::vector<vector3> vertices;
	for (int c = 0; c < a_nSubdivisions; c++) {
		//Makes a bunch of points in a circle
		vector3 vertex = vector3(a_fRadius * cos(angle), a_fRadius * sin(angle), 0.0f);
		vertices.push_back(vertex);
		angle += 2 * PI / a_nSubdivisions;
	}
	for (int c = 0; c < a_nSubdivisions; c++) {
		int d = (c + 1) % a_nSubdivisions;
		//Places points above and below the circle, to keep things centered
		vector3 rightVector = vector3(vertices[d].x, vertices[d].y, vertices[d].z + a_fHeight/2);
		vector3 leftVector = vector3(vertices[c].x, vertices[c].y, vertices[c].z + a_fHeight/2);
		vector3 botRightVector = vector3(vertices[d].x, vertices[d].y, vertices[d].z - a_fHeight / 2);
		vector3 botLeftVector = vector3(vertices[c].x, vertices[c].y, vertices[c].z - a_fHeight / 2);
		//Makes the tris that connect the bottom and top triangles, and the quads that make up the sides
		AddTri(vector3(0, 0, -a_fHeight / 2), botRightVector, botLeftVector);
		AddTri(vector3(0, 0, a_fHeight/2), leftVector, rightVector);
		AddQuad(botLeftVector, botRightVector, leftVector, rightVector);
	}

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTube(float a_fOuterRadius, float a_fInnerRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	float angle = 0.0f;
	//Four vectors to hold all my points
	std::vector<vector3> topInVertices;
	std::vector<vector3> topOutVertices;
	std::vector<vector3> bottomInVertices;
	std::vector<vector3> bottomOutVertices;
	for (int c = 0; c < a_nSubdivisions; c++) {
		//Fills my vectors with the proper points
		bottomInVertices.push_back(vector3(a_fInnerRadius * cos(angle), a_fInnerRadius * sin(angle), -a_fHeight / 2));
		bottomOutVertices.push_back(vector3(a_fOuterRadius * cos(angle), a_fOuterRadius * sin(angle), -a_fHeight / 2));
		topInVertices.push_back(vector3(a_fInnerRadius * cos(angle), a_fInnerRadius * sin(angle), a_fHeight/2));
		topOutVertices.push_back(vector3(a_fOuterRadius * cos(angle), a_fOuterRadius * sin(angle), a_fHeight/2));
		angle += 2 * PI / a_nSubdivisions;
	}
	for (int c = 0; c < a_nSubdivisions; c++) {
		int d = (c + 1) % a_nSubdivisions;
		//Makes quads between the top, bottom, and both sides of the tube
		AddQuad(topInVertices[c], topInVertices[d], bottomInVertices[c], bottomInVertices[d]);
		AddQuad(bottomInVertices[c], bottomInVertices[d], bottomOutVertices[c], bottomOutVertices[d]);
		AddQuad(bottomOutVertices[c], bottomOutVertices[d], topOutVertices[c], topOutVertices[d]);
		AddQuad(topOutVertices[c], topOutVertices[d], topInVertices[c], topInVertices[d]);
	}

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTorus(float a_fOuterRadius, float a_fInnerRadius, int a_nSubdivisionsA, int a_nSubdivisionsB, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_nSubdivisionsA < 3)
		a_nSubdivisionsA = 3;
	if (a_nSubdivisionsA > 360)
		a_nSubdivisionsA = 360;

	if (a_nSubdivisionsB < 3)
		a_nSubdivisionsB = 3;
	if (a_nSubdivisionsB > 360)
		a_nSubdivisionsB = 360;

	Release();
	Init();
	float angleA = 0.0f;
	//A vector of vectors of points
	std::vector<std::vector<vector3>> vLists;
	for (int c = 0; c < a_nSubdivisionsB; c++) {
		//Fills the vector with other vectors
		std::vector<vector3> vec;
		vLists.push_back(vec);
	}
	//The radius of the outside circle in the torus
	float a = (a_fOuterRadius - a_fInnerRadius) / 2;
	//The center of the outside circle in the torus
	float b = (a_fOuterRadius + a_fInnerRadius) / 2;
	for (int c = 0; c < a_nSubdivisionsA; c++) {
		float angleB = 0.0f;
		for (int d = 0; d < a_nSubdivisionsB; d++) {
			//Uses the torus formula to place points properly
			vLists[d].push_back(vector3((b + (a * cos(angleA))) * cos(angleB), (b + (a * cos(angleA))) * sin(angleB), a*sin(angleA)));
			angleB += 2 * PI / a_nSubdivisionsB;
		}
		angleA += 2 * PI / a_nSubdivisionsA;
	}
	for (int c = 0; c < a_nSubdivisionsA; c++) {
		int d = (c + 1) % a_nSubdivisionsA;
		for (int e = 0; e < a_nSubdivisionsB; e++) {
			int f = (e + 1) % a_nSubdivisionsB;
			//Makes all the quads between all the adjacent points of the torus
			AddQuad(vLists[e][d], vLists[e][c], vLists[f][d], vLists[f][c]);
		}
	}
	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateSphere(float a_fRadius, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	//Sets minimum and maximum of subdivisions
	if (a_nSubdivisions < 1)
	{
		GenerateCube(a_fRadius * 2.0f, a_v3Color);
		return;
	}
	if (a_nSubdivisions > 6)
		a_nSubdivisions = 6;

	Release();
	Init();
	//Another vector of vectors
	std::vector<std::vector<vector3>> vLists;
	for (int c = 0; c < a_nSubdivisions; c++) {
		//Filling our vector with vectors
		std::vector<vector3> vec;
		vLists.push_back(vec);
	}
	float angleA = 0.0f;
	for (int c = 0; c < a_nSubdivisions; c++) {
		float angleB = PI / a_nSubdivisions;
		for (int d = 0; d < a_nSubdivisions - 1; d++) {
			//Makes all the points between the two ends based on the parametric formula for spheres
			float u = a_fRadius * cos(angleB);
			float v = sqrtf(pow(a_fRadius, 2) - pow(u, 2));
			vLists[c].push_back(vector3(v * cos(angleA), v * sin(angleA), u));
			angleB += PI / a_nSubdivisions;
		}
		angleA += 2 * PI / a_nSubdivisions;
	}
	for (int c = 0; c < a_nSubdivisions; c++) {
		//Adds triangles between the top and the first set of points
		int d = (c + 1) % a_nSubdivisions;
		AddTri(vector3(0, 0, a_fRadius), vLists[c][0], vLists[d][0]);
	}
	for (int c = 0; c < a_nSubdivisions - 2; c++) {
		//Makes quads between all the middle points
		for (int d = 0; d < a_nSubdivisions; d++) {
			int e = (d + 1) % a_nSubdivisions;
			AddQuad(vLists[d][c], vLists[d][c + 1], vLists[e][c], vLists[e][c + 1]);
		}
	}
	for (int c = 0; c < a_nSubdivisions; c++) {
		//Makes tris between the bottom and the last set of points
		int d = (c + 1) % a_nSubdivisions;
		AddTri(vector3(0, 0, -a_fRadius), vLists[d][a_nSubdivisions-2], vLists[c][a_nSubdivisions - 2]);
	}

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}