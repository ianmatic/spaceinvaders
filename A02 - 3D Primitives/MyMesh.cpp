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

	vector3 point0(-fValue, -fValue, fValue); //0
	vector3 point1(fValue, -fValue, fValue); //1
	vector3 point2(fValue, fValue, fValue); //2
	vector3 point3(-fValue, fValue, fValue); //3

	vector3 point4(-fValue, -fValue, -fValue); //4
	vector3 point5(fValue, -fValue, -fValue); //5
	vector3 point6(fValue, fValue, -fValue); //6
	vector3 point7(-fValue, fValue, -fValue); //7

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

	//get the total angle in a circle (PI * 2)
	const double twoPI = 3.1415926535898 * 2;

	float firstX;
	float firstZ;
	float secondX;
	float secondZ;

	//make subdivision number of triangles
	for (int i = 0; i < a_nSubdivisions; i++) {

		firstX = cosf(twoPI / a_nSubdivisions * i) * a_fRadius;
		firstZ = sinf(twoPI / a_nSubdivisions * i) * a_fRadius;

		//we add 1 to i here so that on the next iteration, this point aligns with firstX & firstY
		secondX = cosf(twoPI / a_nSubdivisions * (i + 1)) * a_fRadius;
		secondZ = sinf(twoPI / a_nSubdivisions * (i + 1)) * a_fRadius;


		//make the base triangles 
		//Bottom Left, Bottom Right, Top
		AddTri(vector3(firstX, -(a_fHeight / 2), firstZ), vector3(secondX, -(a_fHeight / 2), secondZ), vector3(0, -(a_fHeight / 2), 0));

		//make the top-vertex triangles
		//Top, Bottom Right, Bottom Left
		AddTri(vector3(0, a_fHeight / 2, 0), vector3(secondX, -(a_fHeight / 2), secondZ), vector3(firstX, -(a_fHeight / 2), firstZ));
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

	//get the total angle in a circle (PI * 2)
	const double twoPI = 3.1415926535898 * 2;

	float firstX;
	float firstZ;
	float secondX;
	float secondZ;

	//make subdivision number of triangles and quads
	for (int i = 0; i < a_nSubdivisions; i++) {

		firstX = cosf(twoPI / a_nSubdivisions * i) * a_fRadius;
		firstZ = sinf(twoPI / a_nSubdivisions * i) * a_fRadius;

		//we add 1 to i here so that on the next iteration, this point aligns with firstX & firstY
		secondX = cosf(twoPI / a_nSubdivisions * (i + 1)) * a_fRadius;
		secondZ = sinf(twoPI / a_nSubdivisions * (i + 1)) * a_fRadius;

		//make the base triangles
		//Bottom Left, Bottom Right, Top
		AddTri(vector3(firstX, -(a_fHeight / 2), firstZ), vector3(secondX, -(a_fHeight / 2), secondZ), vector3(0, -(a_fHeight / 2), 0));

		//draw in opposite order since we want it to face the opposite direction
		//make the top triangles
		//Top, Bottom Right, Bottom Left
		AddTri(vector3(0, a_fHeight / 2, 0), vector3(secondX, a_fHeight / 2, secondZ), vector3(firstX, a_fHeight / 2, firstZ));

		//make the vertical quads
		//Top Left, Top Right
		//Bottom Left, Bottom Right
		AddQuad(vector3(firstX, a_fHeight / 2, firstZ), vector3(secondX, a_fHeight / 2, secondZ),
			vector3(firstX, -(a_fHeight / 2), firstZ), vector3(secondX, -(a_fHeight / 2), secondZ));

	}
	// -------------------------------

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

	//get the total angle in a circle (PI * 2)
	const double twoPI = 3.1415926535898 * 2;

	float firstInnerX;
	float firstInnerZ;
	float secondInnerX;
	float secondInnerZ;

	float firstOuterX;
	float firstOuterZ;
	float secondOuterX;
	float secondOuterZ;

	//make subdivision number of triangles and quads
	for (int i = 0; i < a_nSubdivisions; i++) {

		//make the inner quads first
		firstInnerX = cosf(twoPI / a_nSubdivisions * i) * a_fInnerRadius;
		firstInnerZ = sinf(twoPI / a_nSubdivisions * i) * a_fInnerRadius;

		//we add 1 to i here so that on the next iteration, this point aligns with firstX & firstY
		secondInnerX = cosf(twoPI / a_nSubdivisions * (i + 1)) * a_fInnerRadius;
		secondInnerZ = sinf(twoPI / a_nSubdivisions * (i + 1)) * a_fInnerRadius;

		//make the vertical inner quads
		AddQuad(vector3(firstInnerX, -(a_fHeight / 2), firstInnerZ), vector3(secondInnerX, -(a_fHeight / 2), secondInnerZ),
			vector3(firstInnerX, a_fHeight / 2, firstInnerZ), vector3(secondInnerX, a_fHeight / 2, secondInnerZ));

		//make the outer quads next
		firstOuterX = cosf(twoPI / a_nSubdivisions * i) * a_fOuterRadius;
		firstOuterZ = sinf(twoPI / a_nSubdivisions * i) * a_fOuterRadius;

		//we add 1 to i here so that on the next iteration, this point aligns with firstX & firstY
		secondOuterX = cosf(twoPI / a_nSubdivisions * (i + 1)) * a_fOuterRadius;
		secondOuterZ = sinf(twoPI / a_nSubdivisions * (i + 1)) * a_fOuterRadius;

		//make the vertical outer quads
		//Top Left, Top Right, 
		//Bottom Left, Bottom Right
		AddQuad(vector3(firstOuterX, a_fHeight / 2, firstOuterZ), vector3(secondOuterX, a_fHeight / 2, secondOuterZ),
			vector3(firstOuterX, -(a_fHeight / 2), firstOuterZ), vector3(secondOuterX, -(a_fHeight / 2), secondOuterZ));


		//make the top quads
		//Top Left, Top Right,
		//Bottom Left, Bottom Right
		AddQuad(vector3(firstInnerX, a_fHeight / 2, firstInnerZ), vector3(secondInnerX, a_fHeight / 2, secondInnerZ),
			vector3(firstOuterX, a_fHeight / 2, firstOuterZ), vector3(secondOuterX, a_fHeight / 2, secondOuterZ));

		//make the base quads 
		//Bottom Left, Bottom Right
		//Top Left, Top Right
		AddQuad(vector3(firstOuterX, -(a_fHeight / 2), firstOuterZ), vector3(secondOuterX, -(a_fHeight / 2), secondOuterZ),
			vector3(firstInnerX, -(a_fHeight / 2), firstInnerZ), vector3(secondInnerX, -(a_fHeight / 2), secondInnerZ));
	}
	// -------------------------------

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

	//a_nSubdivisionsA is how round each piece of the torus is
	//a_nSubDivisionsB is how many pieces the torus has

	//get the total angle in a circle (PI * 2)
	const double twoPI = 3.1415926535898 * 2;

	
	//used for rotating quads
	float radius = (a_fOuterRadius - a_fInnerRadius) / 2;

	//used for angle of revolution
	float currentTheta;
	float nextTheta;

	//used for angle of rotation of tube sections
	float currentPhi;
	float nextPhi;

	//used to determine point distance from center
	float currentXZRadius;
	float nextXZRadius;

	//loop around the axis of revolution
	for (int i = 0; i < a_nSubdivisionsB; i++) {

		//calculate the amount we revolve for the next section
		currentTheta = (twoPI / a_nSubdivisionsB) * i;
		nextTheta = (twoPI / a_nSubdivisionsB) * (i + 1);

		//loop around the circle for 'this' section
		for (int j = 0; j < a_nSubdivisionsA; j++) {

			//calculate the amount we rotate about the tube of 'this' section
			currentPhi = (twoPI / a_nSubdivisionsA) * j;
			nextPhi = (twoPI / a_nSubdivisionsA) * (j + 1);

			//the horizontal distance to the points at current and next angles
			//around the section of the tube
			currentXZRadius = a_fInnerRadius + radius + (radius * cosf(currentPhi));
			nextXZRadius = a_fInnerRadius + radius + (radius * cosf(nextPhi));

			//multiply by cos to get x
			//multiply by sin to get z
			vector3 topLeft = vector3(
				currentXZRadius * cosf(currentTheta),
				//compute height for current point on section of tube
				radius * sinf(currentPhi),
				currentXZRadius * sinf(currentTheta)
			);

			//use nextTheta to go to next section of tube
			vector3 topRight = vector3(
				currentXZRadius * cosf(nextTheta),
				radius * sinf(currentPhi),
				currentXZRadius * sinf(nextTheta)
			);

			vector3 bottomLeft = vector3(
				nextXZRadius * cosf(currentTheta),
				radius * sinf(nextPhi),
				nextXZRadius * sinf(currentTheta)
			);

			vector3 bottomRight = vector3(
				nextXZRadius * cosf(nextTheta),
				radius * sinf(nextPhi),
				nextXZRadius * sinf(nextTheta)
			);

			AddQuad(bottomLeft, bottomRight, topLeft, topRight);
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
	//if (a_nSubdivisions > 6)
		//a_nSubdivisions = 6;

	Release();
	Init();

	//get the total angle in a circle (PI * 2)
	const double twoPI = 3.1415926535898 * 2;
	//used for vertical rotation
	const double halfPI = 3.1415926535898 / 2;

	float firstX;
	float firstZ;
	float secondX;
	float secondZ;

	//Start from the middle of the sphere and move towards the ends

	//make vertical loop
	for (int i = 0; i < a_nSubdivisions / 2; i++) {
		//make horizontal loop
		for (int j = 0; j < a_nSubdivisions; j++) {

			//shift horizontally
			firstX = cosf(twoPI / a_nSubdivisions * j);
			firstZ = sinf(twoPI / a_nSubdivisions * j);

			//we add 1 to i here so that on the next iteration, this point aligns with firstX & firstY
			secondX = cosf(twoPI / a_nSubdivisions * (j + 1));
			secondZ = sinf(twoPI / a_nSubdivisions * (j + 1));

			//quads towards the top
			//Top Left, Top Right,
			//Bottom Left, Bottom Right
			//apply the vertical rotation to the x and z coordinates, and apply positive y values
			vector3 topLeft =
				vector3(firstX * (a_fRadius * cosf(halfPI / (a_nSubdivisions / 2) * (i + 1))),
					a_fRadius * sinf(halfPI / (a_nSubdivisions / 2) * (i + 1)),
					firstZ * (a_fRadius * cosf(halfPI / (a_nSubdivisions / 2) * (i + 1))));
			vector3 topRight =
				vector3(secondX * (a_fRadius * cosf(halfPI / (a_nSubdivisions / 2) * (i + 1))),
					a_fRadius * sinf(halfPI / (a_nSubdivisions / 2) * (i + 1)),
					secondZ * (a_fRadius * cosf(halfPI / (a_nSubdivisions / 2) * (i + 1))));
			vector3 bottomLeft =
				vector3(firstX * (a_fRadius * cosf(halfPI / (a_nSubdivisions / 2) * i)),
					a_fRadius * sinf(halfPI / (a_nSubdivisions / 2) * i),
					firstZ * (a_fRadius * cosf(halfPI / (a_nSubdivisions / 2) * i)));
			vector3 bottomRight =
				vector3(secondX * (a_fRadius * cosf(halfPI / (a_nSubdivisions / 2) * i)),
					a_fRadius * sinf(halfPI / (a_nSubdivisions / 2) * i),
					secondZ * (a_fRadius * cosf(halfPI / (a_nSubdivisions / 2) * i)));

			AddQuad(topLeft, topRight, bottomLeft, bottomRight);

			//quads towards the bottom
			//Top Left, Top Right,
			//Bottom Left, Bottom Right
			//apply the vertical rotation to the x and z coordinates, and apply negative y values
			//the following points are reversed from the above i.e.: topLeft is bottomLeft, bottomRight is topRight, etc.
			bottomLeft =
				vector3(firstX * (a_fRadius * cosf(halfPI / (a_nSubdivisions / 2) * (i + 1))),
					-(a_fRadius * sinf(halfPI / (a_nSubdivisions / 2) * (i + 1))),
					firstZ * (a_fRadius * cosf(halfPI / (a_nSubdivisions / 2) * (i + 1))));
			bottomRight =
				vector3(secondX * (a_fRadius * cosf(halfPI / (a_nSubdivisions / 2) * (i + 1))),
					-(a_fRadius * sinf(halfPI / (a_nSubdivisions / 2) * (i + 1))),
					secondZ * (a_fRadius * cosf(halfPI / (a_nSubdivisions / 2) * (i + 1))));
			topLeft =
				vector3(firstX * (a_fRadius * cosf(halfPI / (a_nSubdivisions / 2) * i)),
					-(a_fRadius * sinf(halfPI / (a_nSubdivisions / 2) * i)),
					firstZ * (a_fRadius * cosf(halfPI / (a_nSubdivisions / 2) * i)));
			topRight =
				vector3(secondX * (a_fRadius * cosf(halfPI / (a_nSubdivisions / 2) * i)),
					-(a_fRadius * sinf(halfPI / (a_nSubdivisions / 2) * i)),
					secondZ * (a_fRadius * cosf(halfPI / (a_nSubdivisions / 2) * i)));

			AddQuad(topLeft, topRight, bottomLeft, bottomRight);
		}
	}

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}