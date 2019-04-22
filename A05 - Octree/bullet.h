#pragma once
#include "MyEntityManager.h"
using namespace Simplex;
class bullet
{
	MyEntityManager* EntityMngr = nullptr;
public:
	bullet();
	bullet(vector3 positon,uint id);
	~bullet();
	//transform position
	void Update(matrix4 mat4);

	float timer;

	String uniqueID;

private:
	vector3 bulletPos;
};

