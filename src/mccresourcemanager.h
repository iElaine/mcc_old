/**
 * @file mccresourcemanager.h
 * @author TommyWang Email<xuchu1986@gmail.com>
 * @brief 
 */
#ifndef __MCCRESOURCEMANAGER_H__
#define __MCCRESOURCEMANAGER_H__
#include <vector>
#include <map>
#include <string>
#include <queue>
#include "mcckeyboardmessage.h"
#include "mcckeyboardmessagehandler.h"
#include "mcckeyboard.h"
#include "configparser.h"
#include "autolock.h"
#include "mccuser.h"
#include "mccglobalid.h"
#include "lockable.h"
#include "mccagenteventhandler.h"
#include "mccagentserver.h"
#include "mccagentclient.h"
#include "mccclientmanager.h"
class MCCKeyboard;
class ConfigParser;

class ProcessContext {
	public:
		ProcessContext(int id, MCCKeyboard *kb, MCCKeyboardMessage &msg) :
			mId(id),
			mKeyboard(kb),
			mKeyboardEvent(msg) {}
		MCCKeyboard *keyboard() { return mKeyboard;}
		MCCKeyboardMessage &event() { return mKeyboardEvent;}
		int id() { return mId;}
	protected:
		const int mId;
		MCCKeyboard *mKeyboard;
		MCCKeyboardMessage mKeyboardEvent;
};
/**
 * 资源管理类，继承了MCCKeyboardMessageHandler类和MCCAgentClientEventHandler类
 * 用来接收来自键盘和远程盒子的请求以及向上级注册，继承了MCCAgentServerEventHandler
 * 类用于接收来自下级的注册和向下级查询资源状态
 */
class MCCResourceManager : 
	public MCCKeyboardMessageHandler,
	public MCCAgentServerEventHandler,
	public MCCAgentClientEventHandler
{
	public:
		/**
		 * @brief constructor
		 */
		MCCResourceManager(ConfigParser &config);
		/**
		 * @brief destructor
		 */
		virtual ~MCCResourceManager();
		typedef std::priority_queue<MCCUser, std::vector<MCCUser>, std::greater<MCCUser> > UserListType;
		// 摄像机资源类
		class Camera : public Lockable {
			public:
				Camera(int id) : mId(id), mIsPtzLocked(false) {}
				int id() { return mId;}
				void setPtzUser(MCCUser& user) {
					mPtzUser = user;
					mPtzUser.updateExpire();
				}
				void addUser(const MCCUser &user) { 
					AutoLock g(mMutex);
					mUsers.push_back(user);
				}
				bool removeUser(const MCCGlobalID &userid) { 
					AutoLock g(mMutex);
					std::vector<MCCUser>::iterator i;
					for(i=mUsers.begin();i!=mUsers.end();i++)
					{
						if(i->Id() == userid)
						{
							mUsers.erase(i);
							return true;
						}
					}
					return false;
				}
				inline bool empty() {
					AutoLock g(mMutex);
					return empty();
				}
				inline size_t size() {
					AutoLock g(mMutex);
					return size();
				}
				MCCUser &ptzUser() { return mPtzUser;}
				inline void releasePtz() { mPtzUser == MCCUser::min;}
				inline void acquirePtz(MCCGlobalID userid) { mPtzUser.Id() = userid; mPtzUser.updateExpire();} 
				inline bool isPtzLocked() const { return mIsPtzLocked;}
				inline void lockPtz(MCCGlobalID userid) { acquirePtz(userid); mIsPtzLocked = true;}
				inline void unlockPtz() { releasePtz(); mIsPtzLocked = false;}
			protected:
				int					 mId;
				MCCUser				 mPtzUser;
				std::vector<MCCUser> mUsers;
				Mutex				 mMutex;
				bool				 mIsPtzLocked;
		};

		// 干线资源类
		class Trunk : public Lockable {
			public:
				Trunk(int id, int outport) : mId(id), mOutport(outport), mCameraId(MCCGlobalID::NullID) {}
				bool isIdle() { 
					AutoLock g(mMutex);
					return (mUsers.size() == 0); 
				}
				// 该干线接到本级矩阵的出口编号
				inline int id() { return mId;}
				// 本级矩阵接到上级矩阵的入口编号
				inline int outport() { return mOutport;}
				inline MCCGlobalID& cid() { return mCameraId;}
				void setCameraId(MCCGlobalID &cameraid) { mCameraId = cameraid;}
				void push(const MCCUser& user)
				{
					AutoLock g(mMutex);
					mUsers.push(user);
				}
				const MCCUser& top()
				{
					AutoLock g(mMutex);
					return mUsers.top();
				}
				void pop() 
				{
					AutoLock g(mMutex);
					mUsers.pop();
					if(mUsers.empty())
						mCameraId = MCCGlobalID::NullID;
				}
				bool empty() {
					AutoLock g(mMutex);
					return mUsers.empty();
				}
				void updateUsers() {
					AutoLock g(mMutex);
					while(!mUsers.empty() &&
							mUsers.top().isExpired())
						mUsers.pop();
					if(mUsers.empty())
						mCameraId = MCCGlobalID::NullID;
				}
			protected:
				int						mId;
				const int				mOutport;
				MCCGlobalID				mCameraId;
				UserListType			mUsers;
				Mutex					mMutex;
		};

		// 监视器资源类
		class Monitor : public Lockable {
			public:
				Monitor(const int id) : mId(id) {}
				MCCUser &user() { return mUser;}
				const MCCUser &user() const { return mUser;}
				void replace(int keyboardId) {
					mUser.Id() = MCCGlobalID(0,keyboardId);
					mUser.updateExpire();
				}
				void setUser(const MCCUser &user) {
					mUser = user;
					mUser.updateExpire();
				}
				int id() const { return mId;}
			protected:
				int mId;						// 监视器的编号，对应矩阵的出口编号
				MCCUser mUser;					// 大屏用户，因为是独占设备，因此只有一个用户
		};

		/**
		 * 已经注册到当前矩阵的下级矩阵，由运行时动态生成
		 */
		class MatrixInfo {
			public:
				MatrixInfo(int id, int fd) : mId(id), mFd(fd) {}
				int id() { return mId;}
				int sock() { return mFd;}
				void addChild(int matrixid) { 
					AutoLock g(mMutex);
					std::set<int>::iterator i = mChildset.find(matrixid);
					if(i == mChildset.end())
						mChildset.insert(matrixid);
				}
				bool hasChild(int matrixid) {
					AutoLock g(mMutex);
					std::set<int>::iterator i = mChildset.find(matrixid);
					if(i!=mChildset.end())
						return true;
					return false;
				}
				void clearChildren() {
					AutoLock g(mMutex);
					std::set<int>::iterator i = mChildset.begin();
					for(;i!=mChildset.end();i++)
						mChildset.erase(i);
				}
			protected:
				friend class MCCResourceManager;
				int					mId;			// 下级矩阵的id		
				int					mFd;			// 与下级矩阵之间建立的tcp连接对应的fd
				std::set<int>		mChildset;		// 下级矩阵所包含的所有子矩阵的id（包括子矩阵的子矩阵等）
													// 主要用于路由远程资源请求时选择下级矩阵
				Mutex				mMutex;			// 用于多线程时包含mChildset
				time_t				mExpires;
		};

		int init(ConfigParser *);
		void setClientManager(MCCClientManager *mgr) { mClientManager = mgr;}
		void setKeyboardManager(MCCKeyboardManager *mgr) {mKeyboardManager = mgr;}
		bool forwardRequest(MCCAgentRequest& request);
		void forwardResponse(MCCAgentResponse& response);
	protected:
		void initResource(ConfigParser*);
		int initAgentMessenger();
		bool checkTrunk(MCCUser& user, const MCCGlobalID& cid, Trunk* &);
		// 处理结果
		enum ProcessResult {
			ACCEPT,		// 消息通过
			REJECT,		// 消息拒绝
			PENDING		// 暂时不处理
		};
		virtual void onSwitch(MCCKeyboard *kb, MCCKeyboardMessage &msg);
		virtual void onPtz(MCCKeyboard *kb, MCCKeyboardMessage &msg);
		inline bool isCameraExist(int id) { return mCameras[id] != NULL;}
		inline bool isChild(int matrixId);
		MatrixInfo *matchMatrix(int matrixId);

		inline bool isLocal(int matrixId) { return matrixId == mMatrixId;}
		virtual ProcessResult processSwitchMessage(MCCKeyboardMessage &msg, MCCKeyboard *kb);
		virtual ProcessResult processPtzMessage(MCCKeyboardMessage &msg, MCCKeyboard *kb);
		virtual ProcessResult processLocalPtz(Camera& cam, MCCGlobalID& userId, int opcode, int lockcode);
		bool request(const MCCGlobalID &camera, const MCCGlobalID &user) { return true;}

		//communication interfaces

		virtual void onNewConnection(int fd, MCCAgentServer& server);
		virtual void onDisconnect(int fd, MCCAgentServer& server);
		virtual void onRegister(int fd, MCCAgentRequest &request, MCCAgentServer& server);
		virtual void onServerRxRequest(int fd, MCCAgentRequest &request, MCCAgentServer& server);
		virtual void onServerRxResponse(int fd, MCCAgentResponse &response, MCCAgentServer& server);
		virtual void onClientConnected(int fd);
		virtual void onClientDisconnected(int fd);
		virtual void onClientRxRequest(int fd, MCCAgentRequest &request, MCCAgentClient &client);
		virtual void onClientRxResponse(int fd, MCCAgentResponse &response, MCCAgentClient &client);

		void buildRegister(MCCAgentRequest &reg);

		int							mMatrixId;
		std::map<int, Camera*>		mCameras;
		Mutex						mCamerasMutex;
		std::vector<Trunk*>			mTrunks;
		Mutex						mTrunksMutex;
		std::map<int, Monitor*>		mMonitors;
		Mutex						mMonitorsMutex;
		std::map<int, MatrixInfo*>  mChildMatrixs;
		Mutex						mChildMatrixsMutex;
		std::map<int, ProcessContext*>
									mProcessContexts;
		Mutex						mProcessContextsMutex;
		MCCAgentServer			   *mServer;
		MCCAgentClient			   *mClient;
		bool						misClientConnected;
		ConfigParser&				mConfig;
		int							mLevel;
		MCCClientManager		   *mClientManager;
		MCCKeyboardManager		   *mKeyboardManager;
};
#endif /* __MCCRESOURCEMANAGER_H__ */
