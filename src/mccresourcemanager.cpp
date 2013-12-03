/**
 * @file mccresourcemanager.cpp
 * @author TommyWang Email<xuchu1986@gmail.com>
 * @brief 
 */
#include <cstdio>
#include <assert.h>
#include "mcckeyboardmanager.h"
#include "mccresourcemanager.h"
#include "log.h"
#include "configparser.h"
using namespace std;

/**
 * @brief constructor
 */
MCCResourceManager::MCCResourceManager(ConfigParser& config) : 
	mConfig(config)
{
	misClientConnected = false;
	InfoLog("Create Resource Manager "<<this);
}

/**
 * @brief destructor
 */
MCCResourceManager::~MCCResourceManager()
{
	InfoLog("Destroyed Resource Manager "<<this);
}

MCCResourceManager::ProcessResult
MCCResourceManager::processSwitchMessage(MCCKeyboardMessage &msg, MCCKeyboard *keyboard) 
{
	int mId = msg.camera_id.matrixId();
//	int matrixId;
	Monitor *monitor = mMonitors[msg.monitor_id];
	InfoLog("Switch camera "<<msg.camera_id.GetLocalID()<<" to monitor "<<msg.monitor_id);
	// check local resource
	if(monitor == NULL)
	{
		InfoLog("Monitor Dosn't Exist!");
		return REJECT;
	}	
	if(!monitor->user().isExpired() && monitor->user().Id().GetLocalID() > msg.keyboard_id)
	{
		InfoLog("Monitor is in use");
		return REJECT;
	}
	if(mId == mMatrixId)
	{
		// 检查本地摄像头和监视器是否存在，检查监视器是否可以被该键盘使用
		if(isCameraExist(msg.camera_id.localId()))
		{
			monitor->replace(msg.keyboard_id);
			return ACCEPT;
		}
		else
		{
			InfoLog("Camera or monitor dosn't exist!");
			return REJECT;
		}
	}
	else 
	{
		// 检查远程矩阵是否是本地矩阵的下级矩阵
		MatrixInfo *matrix = matchMatrix(mId);
		if(matrix)
		{
			// 检查本地监视器是否可用
			if(monitor == NULL && monitor->user().Id().GetLocalID() <= msg.keyboard_id)
			{
				InfoLog("Monitor not avalible");
				return REJECT;
			}
			else
			{
				MCCAgentRequest request;
				MCCAgentMessageHelper::buildSwitchRequest(MCCGlobalID(mMatrixId, msg.keyboard_id), msg.camera_id, request);
				ProcessContext *context = new ProcessContext(request.id(), keyboard, msg);
				{
					AutoLock g(mProcessContextsMutex);
					mProcessContexts[request.id()] = context;
				}
				int fd = matrix->sock();
				mServer->Send(fd, request);
				return PENDING;
			}
		}
		else
		{
			InfoLog("Matrix "<<mId<<" not found!");
			return REJECT;
		}
	}
}
MCCResourceManager::ProcessResult
MCCResourceManager::processLocalPtz(Camera& cam, MCCGlobalID& userId, int optcode, int lockcode)
{
	// 检查云台锁定占用是否过期，过期后先解锁再按无占用处理
	if(cam.isPtzLocked() && cam.ptzUser().isExpired())
		cam.unlockPtz();
	// 如果锁定占用没有过期，按照锁定处理，此时只响应锁定用户权限以上的用户的
	// 操作，并且停止操作不会解除锁定占用，其他用户的操作都拒绝
	if(cam.isPtzLocked())
	{
		if(cam.ptzUser().Id() <= userId || cam.ptzUser().isExpired())
		{
			if(lockcode == 1)
				cam.lockPtz(userId);
			else if(lockcode == 2)
				cam.unlockPtz();
			return ACCEPT;
		}
	}
	else  // 如果没有没有锁定占用，或者锁定占用过期，按照无锁定占用处理
	{
		if(lockcode == 1)
		{
			if(cam.ptzUser().Id() <= userId)
			{
				cam.lockPtz(userId);
				return ACCEPT;
			}
		}
		else if(lockcode == 2)
		{
			//在无锁定状态下收到解锁，可能是重复发送，也可能是A先锁定后B抢占锁定B先解锁，此时
			//A解锁，或者程序错误的只发送了解锁，此时应该忽略该操作，不做任何处理
		}
		else
		{
			// 如果操作者的权限高于占用者则允许操作，并占用云台
			//if(cam.ptzUser().Id() <= userId)
			if(cam.ptzUser().Id() <= userId || cam.ptzUser().isExpired())
			{
				//			cam->ptzUser().Id() = MCCGlobalID(mMatrixId, msg.keyboard_id);
				//			cam->ptzUser().updateExpire();
				// 如果用户操作是停止操作，且用户是之前占用云台的用户则直接允许并解除占用
				//if(optcode != 0)
					cam.acquirePtz(userId);
				//else
				//	cam.releasePtz();
				return ACCEPT;
			}
		}
	}
	return REJECT;
}

MCCResourceManager::ProcessResult
MCCResourceManager::processPtzMessage(MCCKeyboardMessage &msg, MCCKeyboard *keyboard) 
{	
	int mId = msg.camera_id.matrixId();
	// check local resource
	MCCGlobalID operatorId = MCCGlobalID(mId, msg.keyboard_id);
	if(isLocal(mId))
	{
		Camera *cam = mCameras[msg.camera_id.GetLocalID()];
		// 检查相机是否存在
		if(!cam)
			return REJECT;
		return processLocalPtz(*cam, operatorId, msg.ptz_opt_type, msg.lock_resource);
	}
	// 检查远程矩阵是否是本地矩阵的下级矩阵
	else
	{	
		MatrixInfo *matrix = matchMatrix(msg.camera_id.GetMatrixID());
		if(matrix)
		{
			MCCAgentRequest request;
			MCCAgentMessageHelper::buildPtzRequest(MCCGlobalID(mMatrixId, msg.keyboard_id), msg.camera_id, msg.ptz_opt_type, 2, request);
			ProcessContext *context = new ProcessContext(request.id(), keyboard, msg);
			{
				AutoLock g(mProcessContextsMutex);
				if(mProcessContexts.count(request.id()) == 0)
					mProcessContexts.insert(make_pair(request.id(), context));
			}
			mServer->Send(matrix->sock(), request);
			return PENDING;
		}
	}
	return ACCEPT;
}

static MCCGlobalID __last_switch_cid(0,1);


void
MCCResourceManager::onSwitch(MCCKeyboard *kb, MCCKeyboardMessage &msg)
{
	ProcessResult rc;
	if((rc = processSwitchMessage(msg, kb)) == ACCEPT) 
	{
//		kb->SendSwitch(msg);
		mKeyboardManager->SendSwitch(0, msg);
//		kb->Notify(msg, 200, "Ok");
		mClientManager->Notify(msg);
		__last_switch_cid = msg.camera_id;
		InfoLog("onononnonnoo");
	}
	else if(rc == REJECT)
		kb->Notify(msg, 400, "Rejected");
}

void
MCCResourceManager::onPtz(MCCKeyboard *kb, MCCKeyboardMessage &msg)
{
	ProcessResult rc;
	msg.camera_id = __last_switch_cid;
	InfoLog("Last camera id :"<<__last_switch_cid.GetMatrixID()<<","<<__last_switch_cid.GetLocalID());
	if((rc = processPtzMessage(msg, kb)) == ACCEPT) 
	{
		//kb->SendPtz(msg);
		mKeyboardManager->SendPtz(0, msg);
//		kb->Notify(msg, 200, "Ok");
		mClientManager->Notify(msg);
	}
	else if(rc == REJECT)
		kb->Notify(msg, 400, "Rejected");
}

int MCCResourceManager::init(ConfigParser *config) {
	initResource(config);
	return initAgentMessenger();
}

int toInteger(const char *&buf)
{
	int n = 0;
	while(*buf >= '0' && *buf <= '9')
	{
		n *= 10;
		n += (*buf - '0');
		++buf;
	}
	return n;
}

const char *skipSpace(const char *buf)
{
	const char *p = buf;
	while(*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n')
		++p;
	return p;
}

const char *skipToNumber(const char *buf)
{
	const char *p = buf;
	while(p && *p != 0 && *p != '\n' && (*p > '9' || *p < '0'))
		p++;
	if(!p || *p == '\r')
		return NULL;
	return p;
}

void MCCResourceManager::initResource(ConfigParser *config) {
	const char *buf = config->getConfigString("MCC", "Cameras", "1");
	int cam_id,mon_id;
	InfoLog("cam config:"<<buf);
	while(buf!=NULL)
	{
		const char *p = buf;
		cam_id = 0;
		while(*p>= '0' && *p<='9')
		{
			cam_id *= 10;
			cam_id += (*p - '0');	
			++p;
		}
		mCameras[cam_id] = new Camera(cam_id);
		InfoLog("add Camera "<<cam_id);
		if(*p==':')
		{
			++p;
			buf = p;
		}
		else
			buf = NULL;
	}
	buf = config->getConfigString("MCC", "Monitors", "1");
	InfoLog("mon config:"<<buf);
	while(buf!=NULL)
	{
		const char *p = buf;
		mon_id = 0;
		while(*p>= '0' && *p<='9')
		{
			mon_id *= 10;
			mon_id += (*p - '0');	
			++p;
		}
		mMonitors[mon_id] = new Monitor(mon_id);
		InfoLog("add Monitor "<<mon_id);
		if(*p==':')
		{
			++p;
			buf = p;
		}
		else
			buf = NULL;
	}
	buf = config->getConfigString("MCC", "Trunks", "1-1");
	InfoLog("trunk config:"<<buf);
	while(buf!=NULL)
	{
		const char *p = buf;
		int outport = 0;
		int inport = 0;
		while(*p>= '0' && *p<='9')
		{
			outport *= 10;
			outport += (*p - '0');	
			++p;
		}
		assert(*p=='-');
		++p;
		while(*p>= '0' && *p<='9')
		{
			inport *= 10;
			inport += (*p - '0');	
			++p;
		}
		if(*p==':')
		{
			++p;
			buf = p;
		}
		else
			buf = NULL;
		InfoLog("add trunk "<<outport<<'-'<<inport);
		mTrunks.push_back(new Trunk(outport, inport));
	}

	// load trunk resources
//	mTrunks.push_back(new Trunk(2, 3));
//	InfoLog("add Trunk 2");
}

int MCCResourceManager::initAgentMessenger() {
	mLevel = mConfig.getConfigInt("MCC","Level",0); 
	mMatrixId = mConfig.getConfigInt("MCC", "MatrixID", 0);
	mServer = new MCCAgentServer;
	mServer->setEventHandler(this);
	if(mServer->Start(10001))
		return 1;
	InfoLog("Agent Server started!");
	mClient = new MCCAgentClient;
	mClient->setEventHandler(this);
	const char *parentip = mConfig.getConfigString("MCC","ParentNodeIP", "");
	if(parentip && mLevel)
	{
		if(mClient->Start(parentip, 10001))
			return 1;
	}
	return 0;
}

bool MCCResourceManager::isChild(int matrixId) 
{ 
	map<int, MatrixInfo*>::iterator i;
	for(i = mChildMatrixs.begin(); i!= mChildMatrixs.end();i++)
		if(i->second->hasChild(matrixId))
			return true;
	return false;
}

MCCResourceManager::MatrixInfo *
MCCResourceManager::matchMatrix(int matrixId)
{
	map<int, MatrixInfo*>::iterator i;
	for(i = mChildMatrixs.begin(); i!= mChildMatrixs.end();i++)
		if(i->second->id() == matrixId || i->second->hasChild(matrixId))
			return i->second;
	return NULL;
}

void MCCResourceManager::onNewConnection(int fd, MCCAgentServer& server)
{
}

void MCCResourceManager::onDisconnect(int fd, MCCAgentServer& server)
{
	int mid = 0;
	AutoLock g(mChildMatrixsMutex);
	map<int, MatrixInfo*>::iterator i;
	for(i=mChildMatrixs.begin();i!=mChildMatrixs.end();++i)
	{
		if(i->second->sock() == fd)
		{
			MatrixInfo *matrix = i->second;
			mid = matrix->id();
			mChildMatrixs.erase(i);
			delete matrix;
			InfoLog("Matrix "<<mid<<" unregistered!");
		}
	}
}

void MCCResourceManager::onRegister(int fd, MCCAgentRequest &reg, MCCAgentServer& server)
{
	// discard message if its method is not REGISTER
	if(reg.method() != REGISTER)
		return;
	// put MatrixInfo in registered matrix list
	int mid = reg.from();
	AutoLock g(mChildMatrixsMutex);
	map<int, MatrixInfo*>::iterator i = mChildMatrixs.find(mid);
	// if there is no MatrixInfo
	if(reg.content().reg.expire > 0)
	{
		if(i == mChildMatrixs.end())
		{
			MatrixInfo *matrix = new MatrixInfo(mid, fd);
			int n = reg.content().reg.children_num;
			for(int i=0;i<n;i++)
				matrix->addChild(reg.content().reg.children[i]);
			mChildMatrixs.insert(make_pair(mid, matrix));
			InfoLog("Matrix "<<mid<<" registered!");
		}
		else
		{
			MatrixInfo *matrix = i->second;
			matrix->clearChildren();
			int n = reg.content().reg.children_num;
			for(int i=0;i<n;i++)
				matrix->addChild(reg.content().reg.children[i]);
			InfoLog("Matrix "<<mid<<" update!");
		}
	}
	else
	{
		if(i!=mChildMatrixs.end())
		{
			MatrixInfo *matrix = i->second;
			mChildMatrixs.erase(i);
			delete matrix;
			InfoLog("Matrix "<<mid<<" unregistered!");
		}
	}
	if(mLevel != 0 && misClientConnected)
	{
		MCCAgentRequest reg;
		buildRegister(reg);
		mClient->Send(reg);
	}
}

void MCCResourceManager::onServerRxRequest(int fd, MCCAgentRequest &request, MCCAgentServer& server)
{
	WarnLog("Server receive a request and discard it:"<<endl<<request);
	// AgentServer should not receive request other than REGISTER
	// so discard all request it received and do nothing
#if 0
	if(request.to() == mMatrixId)
	{
		if(request.method() == SWITCH)
		{
			// check if there is a trunk can be reuse;

			// check if thera is a free trunk can be used

			// check if there is a trunk can be occupyed

			// refuse
		}
		else if(request.method() == PTZ)
		{
			// check if camera if in use

			// check if camera can be occupied
			// refuse
		}
	}
	else
	{
		MatrixInfo *matrix = matchMatrix(request.to());
		if(matrix)
			server.Send(matrix->sock(), request);
	}
#endif
}

bool MCCResourceManager::forwardRequest(MCCAgentRequest& request)
{
	return false;
}

void MCCResourceManager::forwardResponse(MCCAgentResponse& response)
{
	mClient->Send(response);
}

void MCCResourceManager::onServerRxResponse(int fd, MCCAgentResponse &response, MCCAgentServer& server)
{
	ProcessContext *pcontext;
	{
		AutoLock g(mProcessContextsMutex);
		map<int, ProcessContext*>::iterator i = mProcessContexts.find(response.id());
		if(i == mProcessContexts.end())
			return;
		pcontext = i->second;
		mProcessContexts.erase(i);
	}
	auto_ptr<ProcessContext> context(pcontext);
	assert(context.get());
	// checkout if this matrix is the end receiver of the response
	if(response.to() == mMatrixId)
	{
		if(response.method() == SWITCH)
		{
			if(response.statuscode()/100 != 2 )
			{
				if(context->keyboard())
					context->keyboard()->Notify(context->event(), response.statuscode(), response.reason().c_str());
			}
			else
			{
				// switch 'outport' to monitor
				if(context->keyboard())
				{
					int inport = response.content().swi.outport;
					MCCKeyboardMessage msg = context->event();
					__last_switch_cid = msg.camera_id;
					msg.type = eKBMsg_SWITCH;
					msg.camera_id = MCCGlobalID(mMatrixId, inport);
					InfoLog("Trunk: switch "<<inport<<" to "<<msg.monitor_id);
					mKeyboardManager->SendSwitch(0, msg);
//					context->keyboard()->SendSwitch(msg);
				}
			}
		}
		else if(response.method() == PTZ)
		{
			// Do nothing, just notify user the result of event if request is rejected, actual ptz action take in
			// the end receiver of the original request
			if(response.statuscode()/100 != 2)
			{
				if(context->keyboard())
					context->keyboard()->Notify(context->event(), response.statuscode(), response.reason().c_str());
			}
			else
				mClientManager->Notify(context->event());
		}
	}
	else
	{
		if(response.method() == SWITCH)
		{
			// switch source from 'outport' to trunk to upper matrix,
			int inport = response.content().swi.outport;
			MCCKeyboardMessage msg;
			msg.type = eKBMsg_SWITCH;
			msg.keyboard_id = 0;
			msg.camera_id = MCCGlobalID(0,inport);
			vector<Trunk*>::iterator i;
			for(i=mTrunks.begin();i!=mTrunks.end();++i)
			{
			}
			mClient->Send(response);
		}
		else if(response.method() == PTZ)
		{
			// just forward the response to upper matrix agent
			mClient->Send(response);
		}
	}
}

void 
MCCResourceManager::buildRegister(MCCAgentRequest &reg) 
{
	MCCAgentRequest request;
	MCCAgentMessageHelper::buildRegRequest(mMatrixId,
			60,
			request);
	map<int, MatrixInfo *>::iterator i;
	int j = 0;
	set<int>::iterator k;
	for(i=mChildMatrixs.begin(), j=0; i!=mChildMatrixs.end();++i)
	{
		request.content().reg.children[j] = i->second->id();
		++j;
		for(k = i->second->mChildset.begin() ; k != i->second->mChildset.end(); ++k, ++j)
			request.content().reg.children[j] = *k;
	}
	request.content().reg.children_num = j;
	reg = request;
}
void 
MCCResourceManager::onClientConnected(int fd)
{
	// send REGISTER on connected
	misClientConnected = true;
	MCCAgentRequest request;
	buildRegister(request);
	mClient->Send(request);
}

void 
MCCResourceManager::onClientDisconnected(int fd)
{
	// do nothing
	misClientConnected = false;
}


bool 
MCCResourceManager::checkTrunk(MCCUser& user, const MCCGlobalID& cameraid, Trunk* &trunk)
{
	AutoLock g(mTrunksMutex);
	vector<Trunk*>::iterator i;

	// 检查之前先更新用户，去除超时用户
	for(i=mTrunks.begin(); i!= mTrunks.end();++i) {
		(*i)->updateUsers();
	}
	// 检查是否有可以重用的干线
	for(i=mTrunks.begin(); i!= mTrunks.end();++i)
	{
		if((*i)->cid() == cameraid)
		{
			trunk = *i;
			return true;
		}
	}
	// 如果没有可以重用的干线，则检查有没有空闲的干线
	for(i=mTrunks.begin(); i!= mTrunks.end();i++)
	{
		if((*i)->empty())
		{
			trunk = *i;
			return true;
		}
	}
	// 如果没有可以重用的干线，也没有空闲干线则先找到用户优先级最小的
	// 用户，再检查该用户是否能抢占该干线
	vector<Trunk*>::iterator minTrunk = mTrunks.begin();
	for(i=mTrunks.begin(); i!= mTrunks.end();i++)
	{
		if((*i)->top() > (*minTrunk)->top())
			minTrunk = i;
	}
	if((*minTrunk)->top() > user)
	{
		trunk = *minTrunk;
		return true;
	}
	return false;
}

void MCCResourceManager::onClientRxRequest(int fd, MCCAgentRequest &request, MCCAgentClient& client)
{
	MCCUser remoteUser(MCCGlobalID(request.from(), request.initiator()),eMCCUserType_Keyboard);
	// 处理SWITCH请求
	MCCGlobalID cameraId = MCCGlobalID(request.to(), request.content().swi.cameraid);
	if(request.method() == SWITCH)
	{
		Trunk *trunk = NULL;
		// 不论是否请求本级的摄像头都要先检查是否有可用干线
		if(checkTrunk(remoteUser, cameraId, trunk))
		{
			assert(trunk);
			// 检查是否是请求本级的摄像头
			if(request.to() == mMatrixId)
			{
				MCCAgentResponse response;
				// 检查摄像头是否存在
				if(mCameras.count(request.content().swi.cameraid))
				{
					remoteUser.updateExpire();
					trunk->push(remoteUser);
					trunk->cid() = MCCGlobalID(mMatrixId, request.content().swi.cameraid);
					MCCAgentMessageHelper::buildSwitchResponse(request,
							200, "Ok",
							trunk->outport(),
							response);	   
					MCCKeyboardMessage msg;
					msg.type = eKBMsg_SWITCH;
					msg.keyboard_id = 0;
					msg.camera_id = cameraId;
					msg.monitor_id = trunk->outport();
					InfoLog("Trunk: switch "<<msg.camera_id.GetLocalID()<<" to "<<msg.monitor_id);
					mKeyboardManager->SendSwitch(0, msg);
					mClientManager->Notify(msg);
					// TODO : perform switch
				}
				else
				{
					MCCAgentMessageHelper::buildSwitchResponse(request,
							404, "Camera Not Found",
							trunk->outport(),
							response);	   
				}
				mClient->Send(response);
			}
			else
			{
				// 如果不是本级的摄像头，则执行转发流程
				MatrixInfo *matrix = matchMatrix(request.to());
				// 检查下级矩阵是否存在
				if(matrix == NULL)
				{
					MCCAgentResponse response;
					MCCAgentMessageHelper::buildSwitchResponse(request,
							404, "Matrix Not Found",
							trunk->outport(),
							response);	   
					mClient->Send(response);
				}
				else
				{
					// 下级矩阵存在，创建处理上下午，并转发请求
					MCCKeyboardMessage null;
					ProcessContext *context = new ProcessContext(request.id(), NULL, null);
					// 检查是否创建了请求处理的上下文
					if(mProcessContexts.count(request.id()))
					{
						InfoLog("duplicate message id found");
						assert(0);
					}
					else
					{
						mProcessContexts.insert(make_pair(request.id(), context));
						mServer->Send(matrix->sock(), request);
					}
				}
			}
		}
		else
		{
			// 没有可用的干线，返回失败
			MCCAgentResponse response;
			MCCAgentMessageHelper::buildSwitchResponse(request,
					404, "Trunk Busy", 0,
					response);	   
			mClient->Send(response);
			return;
		}
	}
	else if(request.method() == PTZ)
	{
		// 处理ptz请求
		if(request.to() == mMatrixId)
		{
			map<int, Camera*>::iterator i = mCameras.find(request.content().ptz.cameraid);
			if(i != mCameras.end())
			{
				//---------------------------------------
				if(ACCEPT == processLocalPtz(*(i->second), remoteUser.Id(), request.content().ptz.direction, 0))
				{
					MCCAgentResponse response;
					MCCAgentMessageHelper::buildPtzResponse(request,
							200, "Ok", 
							response);	   
					mClient->Send(response);
					MCCKeyboardMessage msg;
					msg.type = eKBMsg_PTZ;
					msg.keyboard_id = 0;
					msg.camera_id = cameraId;
					msg.ptz_opt_type = request.content().ptz.direction;
					msg.ptz_opt_vstep = 32;
					msg.ptz_opt_hstep = 32;
					mKeyboardManager->SendPtz(0, msg);
					mClientManager->Notify(msg);
				}
				else
				{
					MCCAgentResponse response;
					MCCAgentMessageHelper::buildPtzResponse(request,
							400, "Ptz Not Avaliable", 
							response);	   
					mClient->Send(response);
				}
			}
			else
			{
				MCCAgentResponse response;
				MCCAgentMessageHelper::buildPtzResponse(request,
						404, "Ptz Not Found", 
						response);	   
				mClient->Send(response);
			}
		}
		else
		{
			// 如果不是本级的摄像头，则执行转发流程
			MatrixInfo *matrix = matchMatrix(request.to());
			// 检查下级矩阵是否存在
			if(matrix == NULL)
			{
				MCCAgentResponse response;
				MCCAgentMessageHelper::buildPtzResponse(request,
						404, "Matrix Not Found",
						response);	   
				mClient->Send(response);
			}
			else
			{
				// 下级矩阵存在，创建处理上下午，并转发请求
				ProcessContext *context = new ProcessContext(request.id(), NULL, *(MCCKeyboardMessage*)NULL);
				// 检查是否创建了请求处理的上下文
				if(mProcessContexts.count(request.id()))
				{
					InfoLog("duplicate message id found");
					assert(0);
				}
				else
				{
					mProcessContexts.insert(make_pair(request.id(), context));
					mServer->Send(matrix->sock(), request);
				}
			}
		}
	}
} 

void MCCResourceManager::onClientRxResponse(int fd, MCCAgentResponse &response, MCCAgentClient& client) {
	// client should never receive response so do nothing
	WarnLog("Client receive a response & discard it:"<<endl<<response);
}


