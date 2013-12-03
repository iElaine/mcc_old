/**
 * @file mccuser.h
 * @author TommyWang Email<xuchu1986@gmail.com>
 * @brief 
 */
#ifndef __MCCUSER_H__
#define __MCCUSER_H__
#include "mccglobalid.h"
#include <sys/time.h>
#define DEFAULT_EXPIRE 20
enum eMCCUserType {
	eMCCUserType_Keyboard,
	eMCCUserType_Client
};

class MCCUser {
	public:
		/**
		 * @brief constructor
		 */
		MCCUser(const MCCGlobalID &gid, eMCCUserType t, time_t exp = DEFAULT_EXPIRE);
		MCCUser();
		/**
		 * @brief destructor
		 */
		virtual ~MCCUser();
		MCCGlobalID &Id() { return mId;}
		eMCCUserType Type() { return mType;}
		void updateExpire();
		bool isExpired() const;
		bool isValid() const;
		inline bool operator==(const MCCUser &user) const { return mId == user.mId;}
		inline bool operator!=(const MCCUser &user) const { return mId != user.mId;}
		inline bool operator<(const MCCUser &user) const { return mId < user.mId;}
		inline bool operator>(const MCCUser &user) const { return mId > user.mId;}
		inline bool operator<=(const MCCUser &user) const { return mId <= user.mId;}
		inline bool operator>=(const MCCUser &user) const { return mId >= user.mId;}
		static const MCCUser min;
	private:
		MCCGlobalID mId;
		eMCCUserType mType;
		time_t mExpire;
		time_t mExpireTime;
};
#endif /* __MCCUSER_H__ */
