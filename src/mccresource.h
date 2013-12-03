/**
 * @file resource.h
 * @author TommyWang Email<xuchu1986@gmail.com>
 * @brief 
 */
#ifndef __MCCRESOURCE_H__
#define __MCCRESOURCE_H__
#include "mccglobalid.h"
#include "mccuser.h"
#include "mutex.h"
#include <vector>

enum eMCCResourceType {
	eMCCResourceType_None = 0,
	eMCCResourceType_Camera,
	eMCCResourceType_Monitor,
	eMCCResourceType_Ptz,
	eMCCResourceType_Trunk,
};

class MCCResource {
	public:
		/**
		 * @brief constructor
		 */
		MCCResource(eMCCResourceType type, int local_id);
		/**
		 * @brief destructor
		 */
		virtual ~MCCResource();
		int AddUser(const MCCUser &user);
		int RemoveUser(const MCCGlobalID &id);
		int RemoveUser(const int matrix_id, const int local_id);
		
		bool Unlock();
		const int GetId() const { return lid;};
		const int Id() const { return lid;}
		const eMCCResourceType Type() const { return type;}
		MCCUser &GetMaxUser();
		MCCUser &GetMinUser();
		void Dump();
	protected:
		int updateAndGetMaxUser(MCCUser &maxuser);
		void updateUserList();
		int lid;
		eMCCResourceType	 type;
		Mutex				*m_lock;
		std::vector<MCCUser> userlist;
		bool				 mLocked;
};
typedef unsigned long ResourceId;

class MCCResource {
	public:
		ResourceId id() { return mId;}
		void addUser(const MCCUser& user);
		void updateUser(const MCCUserId& id);
		void updateUser(const MCCUser& user);
		void removeUser(const MCCUserId& id);
		void removeUser(const MCCUser& user);
	protected:
		Mutex				mMutex;
		vector<MCCUser *>	mUsers;
		ResourceId			mId;
};
#endif /* __MCCRESOURCE_H__ */
