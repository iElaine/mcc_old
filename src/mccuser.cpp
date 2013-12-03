/**
 * @file mccuser.cpp
 * @author TommyWang Email<xuchu1986@gmail.com>
 * @brief 
 */
#include "mccuser.h"
#include <cstdio>

const MCCUser MCCUser::min = MCCUser(MCCGlobalID(-1,-1), eMCCUserType_Client, 0);

/**
 * @brief constructor
 */
MCCUser::MCCUser(const MCCGlobalID &gmId, eMCCUserType t, time_t exp) : 
	mId(gmId), 
	mType(t),
	mExpire(exp)
{
}

MCCUser::MCCUser() 
{
	mType = eMCCUserType_Keyboard;
	mExpire = DEFAULT_EXPIRE;
}

/**
 * @brief destructor
 */
MCCUser::~MCCUser()
{

}

void 
MCCUser::updateExpire()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	mExpireTime = tv.tv_sec + mExpire;
}

bool 
MCCUser::isExpired() const
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec > mExpireTime;
}

bool 
MCCUser::isValid() const
{
	return true;
}
