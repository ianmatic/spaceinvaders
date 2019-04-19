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
		for (int j = -10; j < 10; j += 2) {
			//uIndex++;
			m_pEntityMngr->AddEntity("Minecraft\\Cube.obj", std::to_string(enemyID) + "e", true);
			vector3 v3Position = vector3(i, j, 0);
			matrix4 m4Position = glm::translate(v3Position);
			m_pEntityMngr->SetModelMatrix(m4Position);

			enemyID++;
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
	{
		bullets[k].Update(m_pEntityMngr->GetModelMatrix(bullets[k].uniqueID));
	}
	//make the floor
	matrix4 floorMat = IDENTITY_M4;
	floorMat = glm::translate(floorMat, vector3(0, -15, 0));
	floorMat = glm::scale(floorMat, vector3(1000, 1, 1000));
	m_pMeshMngr->AddCubeToRenderList(floorMat, C_WHITE);

	int randNum = rand() % 10;

	timer -= 1;

	//move enemies
	for (int i = 0; i < m_pEntityMngr->GetEnemyCount(); i++) {

		//get enemy
		String enemyID = m_pEntityMngr->GetEnemies()[i]->GetUniqueID();
		MyRigidBody* enemyRB = m_pEntityMngr->GetRigidBody(enemyID);


		matrix4 enemyMat = m_pEntityMngr->GetModelMatrix(enemyID);

		//enemy has been hit
		if (MyEntity::GetEntity(enemyID)->GetHit()) {
			//send enemy flying backward and down
			enemyMat = glm::translate(enemyMat, vector3(0, -1, -.5));
		}
		else {
			//move enemy toward player
			enemyMat = glm::translate(enemyMat, vector3(0, 0, .1));

			//move enemy in x and y directions

			if (timer <= 0) {
				if (randNum < 2) {
					enemyMat = glm::translate(enemyMat, vector3(1, 0, 0));
				}
				else if (randNum >= 2 && randNum < 4) {
					enemyMat = glm::translate(enemyMat, vector3(-1, 0, 0));
				}
				else if (randNum >= 4 && randNum < 6) {
					enemyMat = glm::translate(enemyMat, vector3(0, 1, 0));
				}
				else if (randNum >= 6 && randNum < 8) {
					enemyMat = glm::translate(enemyMat, vector3(0, -1, 0));
				}
			}
		}
		
		m_pEntityMngr->SetModelMatrix(enemyMat, enemyID);
	}

	if (timer <= 0) {
		timer = 60;
	}

	bool resolved = false;

	//do collisions
	for (int i = 0; i < bullets.size(); i++) {
		for (int j = 0; j < m_pEntityMngr->GetEnemyCount(); j++) {

			//get bullet
			MyRigidBody* bulletRB = m_pEntityMngr->GetRigidBody(bullets[i].uniqueID);
			//MyEntity* bullet = m_pEntityMngr->GetEntity(bullets[i].uniqueID);

			//get enemy
			String enemyID = m_pEntityMngr->GetEnemies()[j]->GetUniqueID();
			MyRigidBody* enemyRB = m_pEntityMngr->GetRigidBody(enemyID);

			//hasn't been hit yet
			if (!MyEntity::GetEntity(enemyID)->GetHit()) {
				//collision detection between bullets and enemies
				if (bulletRB->IsColliding(enemyRB)) {
					MyEntity::GetEntity(enemyID)->SetHit(true);
				}
			}
			//has been hit
			else {
				//enemy hits floor, don't feel like doing actual collisions, maybe later
				if (MyEntity::GetEntity(enemyID)->GetRigidBody()->GetMinGlobal().y <= -15) {
					//collision resolution
					printf("colliding");

					m_pEntityMngr->RemoveEntity(enemyID);
					//m_pEntityMngr->RemoveEntity(bullets[i].uniqueID);

					//remove from bullets
					bullets.erase(bullets.begin() + i);

					resolved = true;
					break;
				}
			}
		}

		if (resolved) {
			break;
		}
	}

	//Add objects to render list

	//bullets
	for (int i = 0; i < bullets.size(); i++) {
		m_pEntityMngr->AddEntityToRenderList(bullets[i].uniqueID, true);
	}

	//enemies
	m_pEntityMngr->AddEntityToRenderList(-1, true);
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	//display octree
	//if (octID == -1) {
	//	root->Display(C_YELLOW);
	//}
	//else {
	//	root->Display(octID, C_YELLOW);
	//}
	
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