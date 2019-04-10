#pragma once
#include "MyMeshManager.h"
#include "MyEntityManager.h"
using namespace Simplex;
class Bullet
{
	MyMeshManager* MeshMngr = nullptr;
	MyEntityManager* EntityMngr = nullptr;
public:
	Bullet();
	~Bullet();
	//transform position
	void Update();

	
};

