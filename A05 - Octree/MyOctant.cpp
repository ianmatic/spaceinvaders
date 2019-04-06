#include "MyOctant.h"

#include "MyEntityManager.h"

namespace Simplex {

	//constructor
	MyOctant::MyOctant(int maxLevel, int idealEntityCount)
	{
		//initialize
		Init();

		this->maxLevel = maxLevel;
		this->idealEntityCount = idealEntityCount;

		root = this;
		rootChildren.clear();

		//holds mins and maxes for collision detection
		std::vector<vector3> minsMaxes;

		//get all of the entity mins and maxes
		for (int i = 0; i < entityMngr->GetEntityCount(); i++) {

			MyEntity* entity = entityMngr->GetEntity(i);
			MyRigidBody* rigidBody = entity->GetRigidBody();

			minsMaxes.push_back(rigidBody->GetMinGlobal());
			minsMaxes.push_back(rigidBody->GetMaxGlobal());
		}

		MyRigidBody* rigidBody = new MyRigidBody(minsMaxes);

		vector3 halfWidth = rigidBody->GetHalfWidth();
		float localMax = halfWidth.x;

		//find max
		for (int i = 1; i < 3; i++) {
			if (localMax < halfWidth[i]) {
				localMax = halfWidth[i];
			}
		}

		//set members
		size = localMax * 2.0f;
		center = rigidBody->GetCenterLocal();

		vector3 localMaxVec = (vector3)(localMax);

		min = center - localMaxVec;
		max = center + localMaxVec;

		octCount++;

		//free memory
		minsMaxes.clear();
		SafeDelete(rigidBody);


		//construct the structure
		ConstructTree(maxLevel);
	}

	//secondary constructor for children
	MyOctant::MyOctant(vector3 center, float size)
	{
		//initialize
		Init();

		this->center = center;
		this->size = size;

		min = center - (vector3)(size / 2.0f);
		max = center + (vector3)(size / 2.0f);

		octCount++;
	}

	//copy constructor
	MyOctant::MyOctant(MyOctant const & other)
	{
		//set members
		ID = other.ID;
		level = other.level;
		numChildren = other.numChildren;

		size = other.size;

		center = other.center;
		min = other.min;
		max = other.max;

		parent = other.parent;
		for (int i = 0; i < 8; i++) {
			children[i] = other.children[i];
		}

		//entityList = other.entityList?

		root = other.root;
		rootChildren = other.rootChildren;

		//singletons
		meshMngr = MeshManager::GetInstance();
		entityMngr = MyEntityManager::GetInstance();
	}

	//copy assignment operator
	MyOctant & MyOctant::operator=(MyOctant const & other)
	{
		//only swap if different object
		if (this != &other) {

			//reset
			Release();
			Init();

			//swap data
			MyOctant temp(other);
			Swap(temp);
		}
		return *this;
	}

	//destructor
	MyOctant::~MyOctant()
	{
		Release();
	}

	void MyOctant::Swap(MyOctant & other)
	{
		//swap members
		std::swap(numChildren, other.numChildren);
		std::swap(size, other.size);

		std::swap(ID, other.ID);

		std::swap(root, other.root);
		std::swap(children, other.children);

		std::swap(center, other.center);
		std::swap(min, other.min);
		std::swap(max, other.max);

		std::swap(level, other.level);
		std::swap(parent, other.parent);

		for (int i = 0; i < 8; i++) {
			std::swap(children[i], other.children[i]);
		}

		//singletons, so don't swap
		meshMngr = MeshManager::GetInstance();
		entityMngr = MyEntityManager::GetInstance();


	}
	//Getters
	float MyOctant::GetSize()
	{
		return size;
	}

	vector3 MyOctant::GetCenter()
	{
		return center;
	}

	vector3 MyOctant::GetMin()
	{
		return min;
	}

	vector3 MyOctant::GetMax()
	{
		return max;
	}

	int MyOctant::GetOctCount()
	{
		return octCount;
	}

	MyOctant * MyOctant::GetChild(int childIndex)
	{
		//bad index
		if (childIndex > 7 || childIndex < 0) {
			return nullptr;
		}

		return children[childIndex];
	}

	//check collision with entities
	bool MyOctant::IsColliding(int index)
	{
		int objCount = entityMngr->GetEntityCount();

		//bad index
		if (index >= objCount) {
			return false;
		}

		MyEntity* entity = entityMngr->GetEntity(index);
		MyRigidBody* rigidBody = entity->GetRigidBody();

		//used for AABB
		vector3 rigidMin = rigidBody->GetMinGlobal();
		vector3 rigidMax = rigidBody->GetMaxGlobal();

		//AABB
		if ((min.x <= rigidMax.x && max.x >= rigidMin.x) &&
			(min.y <= rigidMax.y && max.y >= rigidMin.y) &&
			(min.z <= rigidMax.z && max.z >= rigidMin.z)) {
			return true;
		}
		else {
			return false;
		}
	}

	void MyOctant::Subdivide()
	{
		//too many levels or already has subdivided
		if (level >= maxLevel || numChildren == 8) {
			return;
		}

		numChildren = 8;

		float childSize = size / 4.0f;

		//+x == right, -x == left
		//+y == up,	   -y == down
		//+z == front, -z == back

		//make 8 children in appropriate positions

		//start at center
		vector3 childCenter = center;

		//move to front
		childCenter.z += childSize;

		//front top left
		childCenter.y += childSize;
		childCenter.x -= childSize;
		children[0] = new MyOctant(childCenter, childSize * 2);

		//front top right
		childCenter.x += childSize * 2;
		children[1] = new MyOctant(childCenter, childSize * 2);

		//front bottom right
		childCenter.y -= childSize * 2;
		children[2] = new MyOctant(childCenter, childSize * 2);

		//front bottom left
		childCenter.x -= childSize * 2;
		children[3] = new MyOctant(childCenter, childSize * 2);

		//reset to center
		childCenter = center;

		//move to back
		childCenter.z -= childSize;

		//back top left
		childCenter.y += childSize;
		childCenter.x -= childSize;
		children[4] = new MyOctant(childCenter, childSize * 2);

		//back top right
		childCenter.x += childSize * 2;
		children[5] = new MyOctant(childCenter, childSize * 2);

		//back bottom right
		childCenter.y -= childSize * 2;
		children[6] = new MyOctant(childCenter, childSize * 2);

		//back bottom left
		childCenter.x -= childSize * 2;
		children[7] = new MyOctant(childCenter, childSize * 2);

		//connect each child appropriately of this layer
		for (int i = 0; i < numChildren; i++) {

			children[i]->root = root;
			children[i]->parent = this;
			children[i]->level = level + 1;
			children[i]->maxLevel = maxLevel;
			children[i]->ID = root->octCount;
			children[i]->octCount = root->octCount;

			root->octCount++;
		}

		//go to the next layer
		for (int i = 0; i < numChildren; i++) {
			//has too many children, subdivide it
			if (children[i]->ContainsMorethan(idealEntityCount)) {
				children[i]->Subdivide();
			}
		}

		//update the oct count for all octants
		UpdateOctCount();
	}

	void MyOctant::UpdateOctCount() {

		//check all children
		for (int i = 0; i < numChildren; i++) {

			//check grandchildren etc. recursively
			children[i]->UpdateOctCount();
		}
		//done recursing
		octCount = root->octCount;
	}

	void MyOctant::RemoveBranches()
	{
		//destroy each child
		for (int i = 0; i < numChildren; i++) {
			
			//do the same for grandchildren etc. via recursion
			children[i]->RemoveBranches();

			//at this point, we've stopped recursing
			//destroy the child
			//corrupt them all (not really, just destroy them)
			delete children[i];
			children[i] = nullptr;
		}

		//no more children
		numChildren = 0;
	}

	void MyOctant::ConstructTree(int maxLevel)
	{
		//only meant to be called on root
		if (level != 0) {
			return;
		}

		//reset data
		octCount = 1;
		entityList.clear();
		RemoveBranches();
		rootChildren.clear();

		this->maxLevel = maxLevel;

		//subdivid if too many entities
		if (ContainsMorethan(idealEntityCount)) {
			Subdivide();
		}

		//call helper methods to setup IDs and rootChildren
		AssignIDToEntity();

		//setup rootChildren
		ConstructList();
	}

	void MyOctant::AssignIDToEntity()
	{
		//find leafs recursively
		for (int i = 0; i < numChildren; i++) {
			children[i]->AssignIDToEntity();
		}

		//done recursing

		//leaf found (lowest level)
		if (numChildren == 0) {
			//check all entities
			for (int i = 0; i < entityMngr->GetEntityCount(); i++) {
				//found entity within oct
				if (IsColliding(i)) {

					//assign ID
					entityList.push_back(i);
					entityMngr->AddDimension(i, ID);
				}
			}
		}

	}

	bool MyOctant::ContainsMorethan(int numEntities)
	{
		int count = 0;

		for (int i = 0; i < entityMngr->GetEntityCount(); i++) {

			//found entity
			if (IsColliding(i)) {
				count++;
			}
			//more than numEntities
			if (count > numEntities) {
				return true;
			}
		}
		//not more than numEntities
		return false;
	}

	//determines if leaf
	bool MyOctant::IsLeaf()
	{
		//leaf
		if (numChildren == 0) {
			return true;
		}
		//branch
		else {
			return false;
		}
	}

	void MyOctant::ClearEntityList()
	{
		//check all children
		for (int i = 0; i < numChildren; i++) {
			//check grandchildren, etc. via recursion
			children[i]->ClearEntityList();
		}

		//done recursing, clear the list
		entityList.clear();
	}

	//display octLeaf by index
	void MyOctant::Display(int index, vector3 color)
	{
		//correct object
		if (ID == index) {
			meshMngr->AddCubeToRenderList(
				glm::translate(IDENTITY_M4, center) *
				glm::scale(vector3(size)), color, RENDER_WIRE
			);

			return;
		}

		//children
		for (int i = 0; i < numChildren; i++) {
			children[i]->Display(index, C_YELLOW);
		}
	}

	//display octLeaf recursively
	void MyOctant::Display(vector3 color)
	{
		//children
		for (int i = 0; i < numChildren; i++) {
			children[i]->Display(color);
		}

		//no more children, so display the octLeafs

		//get position, use IDENTIY_M4 since centered about origin of program
		matrix4 mat = glm::translate(IDENTITY_M4, center) *
			glm::scale(vector3(size));

		//display
		meshMngr->AddWireCubeToRenderList(mat, color, RENDER_WIRE);
	}

	void MyOctant::DisplayLeafs(vector3 color)
	{
		//check all children
		for (int i = 0; i < rootChildren.size(); i++) {
			//branch out and check them ALL recursively
			rootChildren[i]->DisplayLeafs(color);
		}

		//at this point we're done recursing

		//get position, use IDENTIY_M4 since centered about origin of program
		matrix4 mat = glm::translate(IDENTITY_M4, center) *
			glm::scale(vector3(size));

		//display
		meshMngr->AddWireCubeToRenderList(mat, color, RENDER_WIRE);

	}

	void MyOctant::Release()
	{
		//root release
		if (level == 0) {
			RemoveBranches();
		}

		//reset data
		numChildren = 0;
		size = 0;
		entityList.clear();
		rootChildren.clear();
	}

	void MyOctant::Init()
	{
		//initialize variables
		numChildren = 0;
		size = 0.0f;

		ID = octCount;

		level = 0;

		center = vector3(0.0f);
		min = vector3(0.0f);
		max = vector3(0.0f);

		root = nullptr;
		parent = nullptr;

		for (int i = 0; i < 8; i++) {
			children[i] = nullptr;
		}


		//get singletons
		meshMngr = MeshManager::GetInstance();
		entityMngr = MyEntityManager::GetInstance();
	}

	void MyOctant::ConstructList()
	{
		//find all children recursively
		for (int i = 0; i < numChildren; i++) {
			children[i]->ConstructList();
		}

		//done recursing

		//fill rootChildren with useful octs
		if (entityList.size() > 0) {
			root->rootChildren.push_back(this);
		}
	}

}

