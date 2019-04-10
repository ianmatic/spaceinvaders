#pragma once
#include "MyEntityManager.h"
using namespace Simplex;
class bullet
{
	MyEntityManager* EntityMngr = nullptr;
public:
	bullet();
	bullet(vector3* positon);
	~bullet();
	//transform position
	void Update();

private:
	vector3* bulletPos;
};

