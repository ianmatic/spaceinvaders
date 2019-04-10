#include "AppClass.h"

#include "MyOctant.h"

using namespace Simplex;
void Application::InitVariables(void)
{

	//Set the position and target of the camera
	m_pCameraMngr->SetPositionTargetAndUpward(
		vector3(0.0f, 0.0f, 100.0f), //Position
		vector3(0.0f, 0.0f, 99.0f),	//Target
		AXIS_Y);					//Up

	m_pLightMngr->SetPosition(vector3(0.0f, 3.0f, 13.0f), 1); //set the position of first light (0 is reserved for ambient light)

#ifdef DEBUG
	uint uInstances = 900;
#else
	uint uInstances = 1849;
#endif
	//make the enemies
	for (int i = -10; i < 10; i += 2) {
		for (int j = 0; j < 10; j += 1) {
			//uIndex++;
			m_pEntityMngr->AddEntity("Minecraft\\Cube.obj", "NA", true);
			vector3 v3Position = vector3(i, j * 2, 0);
			matrix4 m4Position = glm::translate(v3Position);
			m_pEntityMngr->SetModelMatrix(m4Position);
		}
	}

	//setup root
	octLevels = 1;
	root = new MyOctant(octLevels, 5);

	m_pEntityMngr->Update();
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the ArcBall active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();
	
	//Update Entity Manager
	m_pEntityMngr->Update();

	//update bullet positions
	for (int k = 0; k < bullets.size(); k++)
		bullets[k].Update();

	//make the floor
	matrix4 floorMat = IDENTITY_M4;
	floorMat = glm::translate(floorMat, vector3(0, -15, 0));
	floorMat = glm::scale(floorMat, vector3(1000, 1, 1000));
	m_pMeshMngr->AddCubeToRenderList(floorMat, C_WHITE);

	//Add objects to render list
	m_pEntityMngr->AddEntityToRenderList(-1, true);
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	//display octree
	if (octID == -1) {
		root->Display(C_YELLOW);
	}
	else {
		root->Display(octID, C_YELLOW);
	}
	
	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();
	
	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();
	
	//draw gui,
	DrawGUI();
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	SafeDelete(root);

	//release GUI
	ShutdownGUI();
}