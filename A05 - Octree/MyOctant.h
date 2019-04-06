#pragma once

//include simplex data
#include "MyEntityManager.h"

namespace Simplex {
	class MyOctant {

		//number of octants instantiated
		int octCount;
		int maxLevel;
		//how many entities this oct will idealy contain
		int idealEntityCount;

		int ID = 0;
		int level = 0;
		//either 0 or 8
		int numChildren = 0;

		float size = 0.0f;
		
		//singletons
		MeshManager* meshMngr = nullptr;
		MyEntityManager* entityMngr = nullptr;
		

		vector3 center = vector3(0.0f);
		vector3 min = vector3(0.0f);
		vector3 max = vector3(0.0f);

		MyOctant* parent = nullptr;
		MyOctant* children[8];

		//indices of the entities inside this oct
		std::vector<int> entityList;

		//root and its children
		MyOctant* root = nullptr;
		std::vector<MyOctant*> rootChildren;

	public:
		//constructor
		MyOctant(int maxLevel, int idealEntityCount);

		//secondary constructor for children
		MyOctant(vector3 center, float size);

		//BIG 3
		//copy constructor
		MyOctant(MyOctant const& other);
		//copy assignment operator
		MyOctant& operator=(MyOctant const& other);
		//destructor
		~MyOctant();

		//helper method for copy assignment operator
		void Swap(MyOctant& other);

		//Getters: size, center, min, max, OctCount, child
		float GetSize();
		vector3 GetCenter();
		vector3 GetMin();
		vector3 GetMax();
		int GetOctCount();
		MyOctant* GetChild(int childIndex);

		//display methods
		void Display(int index, vector3 color);
		void Display(vector3 color);
		void DisplayLeafs(vector3 color);

		//determines collision with entity in bounding object manager
		//index == index of entity in entityMngr
		bool IsColliding(int index);

		//makes 8 more children and hooks them up
		void Subdivide();

		//destroys any children (and their children recursively) of this oct 
		void RemoveBranches();

		//construct tree of int amount of levels, call on root only
		void ConstructTree(int maxLevel);

		//finds Entities within leafs and sets ID
		void AssignIDToEntity();


		//helper functions
		
		//determines if this oct has more than int entities in it
		bool ContainsMorethan(int entities);

		//determines if leaf
		//leaf has 0 children
		bool IsLeaf();

		//clear lists of all octs
		void ClearEntityList();

		//Updates octCount of all octants recursively
		void UpdateOctCount();

	private:
		//helper methods
		void Release();
		void Init();

		//fills rootChildren with octs
		void ConstructList();
	};

}
