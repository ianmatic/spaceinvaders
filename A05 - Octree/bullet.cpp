#include "bullet.h"
#include "AppClass.h"
using namespace Simplex;


bullet::bullet()
{
}

bullet::bullet(vector3 position, uint id)
{
	EntityMngr = MyEntityManager::GetInstance();

	bulletPos = position;

	//make the bullet
	matrix4 bulletMat = IDENTITY_M4;
	bulletMat = glm::translate(bulletMat, bulletPos);
	bulletMat = glm::scale(bulletMat, vector3(1, 1, 1));

	uniqueID = std::to_string(id);

	EntityMngr->AddEntity("Minecraft\\Cow.obj", uniqueID);
	EntityMngr->SetModelMatrix(bulletMat);
	//Update Entity Manager
	EntityMngr->Update();
}


bullet::~bullet()
{
}

void bullet::Update(matrix4 mat4)
{
	bulletPos += vector3(0, 0, -1);

	matrix4 bulletMat = IDENTITY_M4;
	bulletMat = glm::translate(bulletMat, bulletPos);
	EntityMngr->SetModelMatrix(bulletMat, uniqueID);
}
