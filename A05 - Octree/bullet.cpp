#include "bullet.h"
#include "AppClass.h"
using namespace Simplex;


bullet::bullet(vector3* position)
{
	EntityMngr = MyEntityManager::GetInstance();

	bulletPos = position;

	//make the bullet
	matrix4 bulletMat = IDENTITY_M4;
	bulletMat = glm::translate(bulletMat, *bulletPos);
	bulletMat = glm::scale(bulletMat, vector3(1, 1, 1));
	EntityMngr->AddEntity("Minecraft\\Pig.obj");
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
