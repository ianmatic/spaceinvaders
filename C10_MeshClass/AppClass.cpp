#include "AppClass.h"
void Application::InitVariables(void)
{
	/*
	00100000100	2
	00010001000	2
	00111111100	7
	01101110110	7
	11111111111	11
	10111111101	9
	10100000101	4
	00011011000	4
	_____________
	11x9		46
	*/


	//Make MyMesh object
	map = {
	{ 0,0,1,0,0,0,0,0,1,0,0 },
	{ 0,0,0,1,0,0,0,1,0,0,0 },
	{ 0,0,1,1,1,1,1,1,1,0,0 },
	{ 0,1,1,0,1,1,1,0,1,1,0 },
	{ 1,1,1,1,1,1,1,1,1,1,1 },
	{ 1,0,1,1,1,1,1,1,1,0,1 },
	{ 1,0,1,0,0,0,0,0,1,0,1 },
	{ 0,0,0,1,1,0,1,1,0,0,0 }
	};


	//fill the myMesh 2d array
	for (int i = 0; i < map.size(); i++) {
		std::vector<MyMesh*> vec;
		for (int j = 0; j < map[i].size(); j++) {
			vec.push_back(new MyMesh());
			vec[j]->GenerateCube(1.0f, C_BLACK);
		}
		cubes.push_back(vec);
	}

	//get width and height
	width = cubes[0].size();
	height = cubes.size();

	//set the start position for the rendering
	startCoordinates = vector3(-width / 2, height / 2, 0.0f);

	//how much the cubes move per frame
	xIncrement = 0.05f;
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	vector3 cubeCoordinates = startCoordinates;

	for (int i = 0; i < cubes.size(); i++) {
		for (int j = 0; j < cubes[i].size(); j++) {

			//move right
			//only render the correct cubes
			if (map[i][j] == 1) {
				cubes[i][j]->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), glm::translate(cubeCoordinates));
			}
			cubeCoordinates.x += 1.0f;

		}
		//reset the x coordinate
		cubeCoordinates.x -= width;

		//move down
		cubeCoordinates.y -= 1.0f;
	}

	//toggle direction
	if (movingRight) {
		startCoordinates.x += xIncrement;
	}
	else {
		startCoordinates.x -= xIncrement;
	}

	//move in opposite direction
	if (startCoordinates.x >= 0 || startCoordinates.x <= -10) {
		movingRight = !movingRight;
	}

	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();

	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();

	//draw gui
	DrawGUI();

	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	for (int i = 0; i < cubes.size(); i++) {
		for (int j = 0; j < cubes[i].size(); j++) {
			SafeDelete(cubes[i][j]);
		}
	}
	cubes.clear();
	map.clear();
	//release GUI
	ShutdownGUI();
}