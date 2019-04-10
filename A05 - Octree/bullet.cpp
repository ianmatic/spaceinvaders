#include "bullet.h"
using namespace Simplex;


bullet::bullet()
{
	EntityMngr = MyEntityManager::GetInstance();

	//make the bullet
	matrix4 bulletMat = IDENTITY_M4;
	bulletMat = glm::translate(bulletMat, vector3(0, 0, 0));
	bulletMat = glm::scale(bulletMat, vector3(10, 10, 10));
	EntityMngr->AddEntity("Minecraft\\Steve.obj");
	EntityMngr->SetModelMatrix(bulletMat);
	//Update Entity Manager
	EntityMngr->Update();

	//Add objects to render list
	EntityMngr->AddEntityToRenderList(-1, true);
}


bullet::~bullet()
{
}

void bullet::Update()
{
}
