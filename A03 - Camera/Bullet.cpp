#include "Bullet.h"
using namespace Simplex;


Bullet::Bullet()
{
	//get refs to singletons
	MeshMngr = MyMeshManager::GetInstance();
	EntityMngr = MyEntityManager::GetInstance();
	
	//make the bullet
	matrix4 bulletMat = IDENTITY_M4;
	bulletMat = glm::translate(bulletMat, vector3(0, 0, 0));
	bulletMat = glm::scale(bulletMat, vector3(100, 100, 100));
	EntityMngr->AddEntity("Minecraft\\Cube.obj");
	EntityMngr->SetModelMatrix(bulletMat);
	//Update Entity Manager
	EntityMngr->Update();

	//Add objects to render list
	EntityMngr->AddEntityToRenderList(-1, true);
}


Bullet::~Bullet()
{
}

void Bullet::Update()
{
}
