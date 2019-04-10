#pragma once
#include "MyEntityManager.h"
using namespace Simplex;
class bullet
{
	MyEntityManager* EntityMngr = nullptr;
public:
	bullet();
	~bullet();
	//transform position
	void Update();
};

