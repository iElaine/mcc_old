/**
 * @file resource.cpp
 * @author TommyWang Email<xuchu1986@gmail.com>
 * @brief 
 */
#include <cstdio>
#include <cassert>
#include "mccresource.h"
#include "autolock.h"

using namespace std;

/**
 * @brief constructor
 */
MCCResource::MCCResource(eMCCResourceType t, int local_id) :
	mLocked(false)
{
	type = t;
	lid = local_id;
	m_lock = new Mutex();
	assert(m_lock);
}

/**
 * @brief destructor
 */
MCCResource::~MCCResource()
{

}

int MCCResource::AddUser(const MCCUser &user)
{
	if(mLocked)
		return -2;
	AutoLock lock(m_lock);
	userlist.push_back(user);
	return 0;
}

int MCCResource::RemoveUser(const MCCGlobalID &id)
{
	if(mLocked)
		return -2;
	AutoLock lock(m_lock);
	vector<MCCUser>::iterator i;
	for(i= userlist.begin(); i != userlist.end(); i++)
	{
		if(i->Id() == id)
		{
			userlist.erase(i);
			return 0;
		}
	}
	return -1;
}

int MCCResource::RemoveUser(const int matrix_id, const int local_id)
{
	MCCGlobalID id(matrix_id, local_id);
	return RemoveUser(id);
}

MCCUser &MCCResource::GetMaxUser()
{
	AutoLock lock(m_lock);
	vector<MCCUser>::iterator max = userlist.begin();
	return (*max);
}

MCCUser &MCCResource::GetMinUser()
{	
	AutoLock lock(m_lock);
	vector<MCCUser>::iterator min = userlist.begin();
	return (*min);
}

void MCCResource::Dump()
{
	printf("Type     : %d\n", type);
	printf("Local ID : %d\n", lid);
	printf("Users    : %d\n", userlist.size());
}

int
MCCResource::updateAndGetMaxUser(MCCUser &maxuser)
{
	if(mLocked)
		return -2;
	vector<MCCUser>::iterator i, i_maxuser;
	AutoLock lock(m_lock);
	i_maxuser = userlist.begin();
	for(i = userlist.begin(); i != userlist.end(); i++)
	{
		if(i->isExpired())
			userlist.erase(i);
		else if(i->isGreatThan(*i_maxuser))
			i_maxuser = i;
	}
	maxuser = *i_maxuser;
	return 0;
}

void 
MCCResource::updateUserList()
{
}
bool 
MCCResource::Lock()
{
	if(mLocked)
		return false;
	mLocked = true;
	return true;
}

bool 
MCCResource::Unlock()
{
	if(mLocked)
		mLocked = false;
	return true;
}
