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
	SpawnEnemies(1);

	//setup root
	octLevels = 1;
	root = new MyOctant(octLevels, 5);

	m_pEntityMngr->Update();
	//seed rand
	srand(static_cast <unsigned> (time(0)));
}

void Simplex::Application::SpawnEnemies(int count)
{
	//make the enemies
	for (int i = 0; i < count; i++) {

		//id is num+e, like 0e
		m_pEntityMngr->AddEntity("Minecraft\\Cube.obj", std::to_string(enemyID) + "e", true);
		//get random x and y pos within a range
		float x = -10 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (10 - (-10))));
		float y = -10 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (10 - (-10))));
		vector3 v3Position = vector3(x, y, 0);
		matrix4 m4Position = glm::translate(v3Position);
		m_pEntityMngr->SetModelMatrix(m4Position);

		enemyID++;
	}


	//for (int z = 10; z > -10; z -= 10) {
	//	for (int x = -10; x < 10; x += 2) {
	//		for (int y = -10; y < 10; y += 2) {

	//			std::string tempID = std::to_string(enemyID) + "e";

	//			m_pEntityMngr->AddEntity("Minecraft\\Cube.obj", tempID, true);

	//			matrix4 m4Position = glm::translate(vector3(x, y, z));
	//			m_pEntityMngr->SetModelMatrix(m4Position);

	//			std::cout << tempID << std::endl;

	//			enemyID++;
	//		}
	//	}
	//}




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

	//make the floor
	matrix4 floorMat = IDENTITY_M4;
	floorMat = glm::translate(floorMat, vector3(0, -15, 0));
	floorMat = glm::scale(floorMat, vector3(1000, 1, 1000));
	m_pMeshMngr->AddCubeToRenderList(floorMat, C_WHITE);



	//update bullet positions
	for (int k = 0; k < bullets.size(); k++)
	{
		//3 seconds have elapsed, destroy bullet
		if (bullets[k].timer > 180) {

			//remove from gameplay
			m_pEntityMngr->RemoveEntity(bullets[k].uniqueID);

			//remove from bullets
			bullets.erase(bullets.begin() + k);

			k--;
			continue;
		}
		//move the bullet
		bullets[k].Update(m_pEntityMngr->GetModelMatrix(bullets[k].uniqueID));
	}

	//for random enemy movement
	int randNum = rand() % 10;

	//update timer
	timer -= 1;

	//check to see if all enemies are dead
	if (m_pEntityMngr->GetEnemyCount() <= 0) {

		//make greater volume of enemies
		level++;
		SpawnEnemies(level * 2);
	}


	//move enemies
	for (int i = 0; i < m_pEntityMngr->GetEnemyCount(); i++) {

		//get enemy from id
		String enemyID = m_pEntityMngr->GetEnemies()[i]->GetUniqueID();
		matrix4 enemyMat = m_pEntityMngr->GetModelMatrix(enemyID);

		//enemy has been hit by a bullet
		if (MyEntity::GetEntity(enemyID)->GetHit()) {
			//send enemy flying backward and down
			enemyMat = glm::translate(enemyMat, vector3(0, -1, -.5));
		}
		//not hit by a bullet yet
		else {
			//move enemy toward player
			enemyMat = glm::translate(enemyMat, vector3(0, 0, .05));

			//move enemy in x and y directions every second
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

		//update entity pos
		m_pEntityMngr->SetModelMatrix(enemyMat, enemyID);
	}

	//reset second long timer
	if (timer <= 0) {
		timer = 60;
	}


	//do collisions
	for (int i = 0; i < m_pEntityMngr->GetEnemyCount(); i++) {

		//get enemy
		String enemyID = m_pEntityMngr->GetEnemies()[i]->GetUniqueID();
		MyRigidBody* enemyRB = m_pEntityMngr->GetRigidBody(enemyID);

		//enemy in freefall
		if (MyEntity::GetEntity(enemyID)->GetHit()) {
			//enemy hits floor, don't feel like doing actual collisions, maybe later
			if (MyEntity::GetEntity(enemyID)->GetRigidBody()->GetMinGlobal().y <= -15) {
				//delete enemy
				std::cout << "deleting falling enemy id:" << enemyID << std::endl;
				m_pEntityMngr->RemoveEntity(enemyID);

				i--;
				continue;
			}
		}
		else {
			for (int j = 0; j < bullets.size(); j++) {

				//get bullet
				MyRigidBody* bulletRB = m_pEntityMngr->GetRigidBody(bullets[j].uniqueID);

				//collision detection between bullets and enemies
				if (bulletRB->IsColliding(enemyRB)) {

					//set in free fall
					MyEntity::GetEntity(enemyID)->SetHit(true);

					//delete bullet
					m_pEntityMngr->RemoveEntity(bullets[j].uniqueID);
					//remove from bullets
					bullets.erase(bullets.begin() + j);

					//no longer check this enemy against bul
					break;
				}
			}
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