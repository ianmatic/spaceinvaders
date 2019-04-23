#include "MyEntityManager.h"
using namespace Simplex;
//  MyEntityManager
Simplex::MyEntityManager* Simplex::MyEntityManager::m_pInstance = nullptr;
void Simplex::MyEntityManager::Init(void)
{
	m_sEntityCount = 0;
	enemyCount = 0;
	m_mEntityArray = nullptr;
}
void Simplex::MyEntityManager::Release(void)
{
	for (uint uEntity = 0; uEntity < m_sEntityCount; ++uEntity)
	{
		MyEntity* pEntity = m_mEntityArray[uEntity];
		SafeDelete(pEntity);
	}
	m_sEntityCount = 0;
	m_mEntityArray = nullptr;
	enemyArray = nullptr;
}
Simplex::MyEntityManager* Simplex::MyEntityManager::GetInstance()
{
	if(m_pInstance == nullptr)
	{
		m_pInstance = new MyEntityManager();
	}
	return m_pInstance;
}
void Simplex::MyEntityManager::ReleaseInstance()
{
	if(m_pInstance != nullptr)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}
int Simplex::MyEntityManager::GetEntityIndex(String a_sUniqueID)
{
	//look one by one for the specified unique id
	for (uint uIndex = 0; uIndex < m_sEntityCount; ++uIndex)
	{
		if (a_sUniqueID == m_mEntityArray[uIndex]->GetUniqueID())
			return uIndex;
	}
	//if not found return -1
	return -1;
}
//Accessors
Simplex::uint Simplex::MyEntityManager::GetEntityCount(void) {	return m_sEntityCount; }
MyEntity ** Simplex::MyEntityManager::GetEnemies()
{
	return enemyArray;
}
uint Simplex::MyEntityManager::GetEnemyCount()
{
	return enemyCount;
}
Simplex::Model* Simplex::MyEntityManager::GetModel(uint a_uIndex)
{
	//if the list is empty return
	if (m_sEntityCount == 0)
		return nullptr;

	// if out of bounds
	if (a_uIndex >= m_sEntityCount)
		a_uIndex = m_sEntityCount - 1;

	return m_mEntityArray[a_uIndex]->GetModel();
}
Simplex::Model* Simplex::MyEntityManager::GetModel(String a_sUniqueID)
{
	//Get the entity
	MyEntity* pTemp = MyEntity::GetEntity(a_sUniqueID);
	//if the entity exists
	if (pTemp)
	{
		return pTemp->GetModel();
	}
	return nullptr;
}
Simplex::MyRigidBody* Simplex::MyEntityManager::GetRigidBody(uint a_uIndex)
{
	//if the list is empty return
	if (m_sEntityCount == 0)
		return nullptr;

	// if out of bounds
	if (a_uIndex >= m_sEntityCount)
		a_uIndex = m_sEntityCount - 1;

	return m_mEntityArray[a_uIndex]->GetRigidBody();
}
Simplex::MyRigidBody* Simplex::MyEntityManager::GetRigidBody(String a_sUniqueID)
{
	//Get the entity
	MyEntity* pTemp = MyEntity::GetEntity(a_sUniqueID);
	//if the entity exists
	if (pTemp)
	{
		return pTemp->GetRigidBody();
	}
	return nullptr;
}
Simplex::matrix4 Simplex::MyEntityManager::GetModelMatrix(uint a_uIndex)
{
	//if the list is empty return
	if (m_sEntityCount == 0)
		return matrix4();

	// if out of bounds
	if (a_uIndex >= m_sEntityCount)
		a_uIndex = m_sEntityCount - 1;

	return m_mEntityArray[a_uIndex]->GetModelMatrix();
}
Simplex::matrix4 Simplex::MyEntityManager::GetModelMatrix(String a_sUniqueID)
{
	//Get the entity
	MyEntity* pTemp = MyEntity::GetEntity(a_sUniqueID);
	//if the entity exists
	if (pTemp)
	{
		return pTemp->GetModelMatrix();
	}
	return IDENTITY_M4;
}
void Simplex::MyEntityManager::SetModelMatrix(matrix4 a_m4ToWorld, String a_sUniqueID)
{
	//Get the entity
	MyEntity* pTemp = MyEntity::GetEntity(a_sUniqueID);
	//if the entity exists
	if (pTemp)
	{
		pTemp->SetModelMatrix(a_m4ToWorld);
	}
}
void Simplex::MyEntityManager::SetAxisVisibility(bool a_bVisibility, uint a_uIndex)
{
	//if the list is empty return
	if (m_sEntityCount == 0)
		return;

	//if the index is larger than the number of entries we are asking for the last one
	if (a_uIndex >= m_sEntityCount)
		a_uIndex = m_sEntityCount - 1;

	return m_mEntityArray[a_uIndex]->SetAxisVisible(a_bVisibility);
}
void Simplex::MyEntityManager::SetAxisVisibility(bool a_bVisibility, String a_sUniqueID)
{
	//Get the entity
	MyEntity* pTemp = MyEntity::GetEntity(a_sUniqueID);
	//if the entity exists
	if (pTemp)
	{
		pTemp->SetAxisVisible(a_bVisibility);
	}
}
void Simplex::MyEntityManager::SetModelMatrix(matrix4 a_m4ToWorld, uint a_uIndex)
{
	//if the list is empty return
	if (m_sEntityCount == 0)
		return;

	//if the index is larger than the number of entries we are asking for the last one
	if (a_uIndex >= m_sEntityCount)
		a_uIndex = m_sEntityCount - 1;

	m_mEntityArray[a_uIndex]->SetModelMatrix(a_m4ToWorld);
}
//The big 3
Simplex::MyEntityManager::MyEntityManager(){Init();}
Simplex::MyEntityManager::MyEntityManager(MyEntityManager const& a_pOther){ }
Simplex::MyEntityManager& Simplex::MyEntityManager::operator=(MyEntityManager const& a_pOther) { return *this; }
Simplex::MyEntityManager::~MyEntityManager(){Release();};
// other methods
void Simplex::MyEntityManager::Update(void)
{
	//Clear all collisions
	for (int i = 0; i < m_sEntityCount; i++)
	{
		m_mEntityArray[i]->ClearCollisionList();
	}

	//check collisions
	for (int i = 0; i < m_sEntityCount - 1; i++)
	{
		for (int j = i + 1; j < m_sEntityCount; j++)
		{
			if (m_mEntityArray[i]->SharesDimension(m_mEntityArray[j])) {
				m_mEntityArray[i]->IsColliding(m_mEntityArray[j]);
			}
		}
	}
}
void Simplex::MyEntityManager::AddEntity(String a_sFileName, String a_sUniqueID, bool enemy)
{

	//Create a temporal entity to store the object
	MyEntity* pTemp = new MyEntity(a_sFileName, a_sUniqueID, enemy);
	//if I was able to generate it add it to the list
	if (pTemp->IsInitialized())
	{
		//create a new temp array with one extra entry
		PEntity* tempArray = new PEntity[m_sEntityCount + 1];
		//start from 0 to the current count
		uint uCount = 0;
		for (uint i = 0; i < m_sEntityCount; ++i)
		{
			tempArray[uCount] = m_mEntityArray[i];
			++uCount;
		}
		tempArray[uCount] = pTemp;
		//if there was an older array delete
		if (m_mEntityArray)
		{
			delete[] m_mEntityArray;
		}
		//make the member pointer the temp pointer
		m_mEntityArray = tempArray;
		//add one entity to the count
		++m_sEntityCount;

		//deal with enemies
		if (enemy) {
			//allocate new enemy array
			MyEntity** tempEnemyArray = new MyEntity*[enemyCount + 1];
			
			int tempCount = 0;

			//copy over old array
			for (int i = 0; i < enemyCount; i++) {
				tempEnemyArray[tempCount] = enemyArray[i];
				tempCount++;
			}

			//add new enemy
			tempEnemyArray[tempCount] = pTemp;

			if (enemyArray) {
				delete[] enemyArray;
			}

			//set array
			enemyArray = tempEnemyArray;

			enemyCount++;
		}
	}
}
void Simplex::MyEntityManager::RemoveEntity(uint a_uIndex)
{
	//std::cout << std::endl;
	//for (int i = 0; i < m_uEntityCount; i++) {
	//	std::cout << m_mEntityArray[i]->GetUniqueID() << std::endl;
	//}



	//if the list is empty return
	if (m_sEntityCount == 0)
		return;

	// if out of bounds choose the last one
	if (a_uIndex >= m_sEntityCount) {
		a_uIndex = m_sEntityCount - 1;
	}


	// if the entity is not the very last we swap it for the last one
	if (a_uIndex != m_sEntityCount - 1)
	{
		std::swap(m_mEntityArray[a_uIndex], m_mEntityArray[m_sEntityCount - 1]);
	}

	//take care of enemy
	if (m_mEntityArray[m_sEntityCount - 1]->GetEnemy()) {

		int enemyIndex = - 1;

		//find enemy index
		for (int i = 0; i < enemyCount; i++) {
			if (m_mEntityArray[m_sEntityCount - 1] == enemyArray[i]) {
				enemyIndex = i;
			}
		}

		// if the enemy is not the very last we swap it for the last one
		if (enemyIndex != enemyCount - 1 && enemyIndex >= 0) {
			std::swap(enemyArray[enemyIndex], m_mEntityArray[enemyCount - 1]);
		}


		PEntity* tempEnemyArray = new PEntity[enemyCount - 1];

		//fill new array
		for (int i = 0; i < enemyCount - 1; i++) {
			tempEnemyArray[i] = enemyArray[i];
		}

		//delete old array
		if (enemyArray) {
			delete[] enemyArray;
		}

		//assign new array
		enemyArray = tempEnemyArray;

		enemyCount--;
	}

	
	//and then pop the last one
	//create a new temp array with one less entry
	PEntity* tempArray = new PEntity[m_sEntityCount - 1];
	//start from 0 to the current count
	for (uint i = 0; i < m_sEntityCount - 1; ++i)
	{
		tempArray[i] = m_mEntityArray[i];
	}
	//if there was an older array delete
	if (m_mEntityArray)
	{
		delete[] m_mEntityArray;
	}
	//make the member pointer the temp pointer
	m_mEntityArray = tempArray;


	//REMOVE one entity FROM the count
	--m_sEntityCount;

	std::cout << std::endl;

	//for (int i = 0; i < m_uEntityCount; i++) {
	//	std::cout << m_mEntityArray[i]->GetUniqueID() << std::endl;
	//}
}
void Simplex::MyEntityManager::RemoveEntity(String a_sUniqueID)
{
	int nIndex = GetEntityIndex(a_sUniqueID);
	RemoveEntity((uint)nIndex);
}
Simplex::String Simplex::MyEntityManager::GetUniqueID(uint a_uIndex)
{
	//if the list is empty return
	if (m_sEntityCount == 0)
		return "";

	//if the index is larger than the number of entries we are asking for the last one
	if (a_uIndex >= m_sEntityCount)
		a_uIndex = m_sEntityCount - 1;

	return m_mEntityArray[a_uIndex]->GetUniqueID();
}
Simplex::MyEntity* Simplex::MyEntityManager::GetEntity(uint a_uIndex)
{
	//if the list is empty return
	if (m_sEntityCount == 0)
		return nullptr;

	//if the index is larger than the number of entries we are asking for the last one
	if (a_uIndex >= m_sEntityCount)
		a_uIndex = m_sEntityCount - 1;

	return m_mEntityArray[a_uIndex];
}
void Simplex::MyEntityManager::AddEntityToRenderList(uint a_uIndex, bool a_bRigidBody)
{
	//if out of bounds will do it for all
	if (a_uIndex >= m_sEntityCount)
	{
		//add for each one in the entity list
		for (int i = 0; i < enemyCount; i++) {
			enemyArray[i]->AddToRenderList(a_bRigidBody);
		}


		/*for (a_uIndex = 0; a_uIndex < m_uEntityCount; ++a_uIndex)
		{
			m_mEntityArray[a_uIndex]->AddToRenderList(a_bRigidBody);
		}*/
	}
	else //do it for the specified one
	{
		m_mEntityArray[a_uIndex]->AddToRenderList(a_bRigidBody);
	}
}
void Simplex::MyEntityManager::AddEntityToRenderList(String a_sUniqueID, bool a_bRigidBody)
{
	//Get the entity
	MyEntity* pTemp = MyEntity::GetEntity(a_sUniqueID);
	//if the entity exists
	if (pTemp)
	{
		pTemp->AddToRenderList(a_bRigidBody);
	}
}
void Simplex::MyEntityManager::AddDimension(uint a_uIndex, uint a_uDimension)
{
	//if the list is empty return
	if (m_sEntityCount == 0)
		return;

	//if the index is larger than the number of entries we are asking for the last one
	if (a_uIndex >= m_sEntityCount)
		a_uIndex = m_sEntityCount - 1;

	return m_mEntityArray[a_uIndex]->AddDimension(a_uDimension);
}
void Simplex::MyEntityManager::AddDimension(String a_sUniqueID, uint a_uDimension)
{
	//Get the entity
	MyEntity* pTemp = MyEntity::GetEntity(a_sUniqueID);
	//if the entity exists
	if (pTemp)
	{
		pTemp->AddDimension(a_uDimension);
	}
}
void Simplex::MyEntityManager::RemoveDimension(uint a_uIndex, uint a_uDimension)
{
	//if the list is empty return
	if (m_sEntityCount == 0)
		return;

	//if the index is larger than the number of entries we are asking for the last one
	if (a_uIndex >= m_sEntityCount)
		a_uIndex = m_sEntityCount - 1;

	return m_mEntityArray[a_uIndex]->RemoveDimension(a_uDimension);
}
void Simplex::MyEntityManager::RemoveDimension(String a_sUniqueID, uint a_uDimension)
{
	//Get the entity
	MyEntity* pTemp = MyEntity::GetEntity(a_sUniqueID);
	//if the entity exists
	if (pTemp)
	{
		pTemp->RemoveDimension(a_uDimension);
	}
}
void Simplex::MyEntityManager::ClearDimensionSetAll(void)
{
	for (uint i = 0; i < m_sEntityCount; ++i)
	{
		ClearDimensionSet(i);
	}
}
void Simplex::MyEntityManager::ClearDimensionSet(uint a_uIndex)
{
	//if the list is empty return
	if (m_sEntityCount == 0)
		return;

	//if the index is larger than the number of entries we are asking for the last one
	if (a_uIndex >= m_sEntityCount)
		a_uIndex = m_sEntityCount - 1;

	return m_mEntityArray[a_uIndex]->ClearDimensionSet();
}
void Simplex::MyEntityManager::ClearDimensionSet(String a_sUniqueID)
{
	//Get the entity
	MyEntity* pTemp = MyEntity::GetEntity(a_sUniqueID);
	//if the entity exists
	if (pTemp)
	{
		pTemp->ClearDimensionSet();
	}
}
bool Simplex::MyEntityManager::IsInDimension(uint a_uIndex, uint a_uDimension)
{
	//if the list is empty return
	if (m_sEntityCount == 0)
		return false;

	//if the index is larger than the number of entries we are asking for the last one
	if (a_uIndex >= m_sEntityCount)
		a_uIndex = m_sEntityCount - 1;

	return m_mEntityArray[a_uIndex]->IsInDimension(a_uDimension);
}
bool Simplex::MyEntityManager::IsInDimension(String a_sUniqueID, uint a_uDimension)
{
	//Get the entity
	MyEntity* pTemp = MyEntity::GetEntity(a_sUniqueID);
	//if the entity exists
	if (pTemp)
	{
		return pTemp->IsInDimension(a_uDimension);
	}
	return false;
}
bool Simplex::MyEntityManager::SharesDimension(uint a_uIndex, MyEntity* const a_pOther)
{
	//if the list is empty return
	if (m_sEntityCount == 0)
		return false;

	//if the index is larger than the number of entries we are asking for the last one
	if (a_uIndex >= m_sEntityCount)
		a_uIndex = m_sEntityCount - 1;

	return m_mEntityArray[a_uIndex]->SharesDimension(a_pOther);
}
bool Simplex::MyEntityManager::SharesDimension(String a_sUniqueID, MyEntity* const a_pOther)
{
	//Get the entity
	MyEntity* pTemp = MyEntity::GetEntity(a_sUniqueID);
	//if the entity exists
	if (pTemp)
	{
		return pTemp->SharesDimension(a_pOther);
	}
	return false;
}