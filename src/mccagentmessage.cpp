#include "mccagentmessage.h"
#include <iostream>
#include <sstream>
#include <string.h>
using namespace std;


int
MCCAgentRequest::Parse(const char *buf, size_t len)
{
	TiXmlDocument doc;
	doc.Parse(buf);
	TiXmlElement *root = doc.RootElement();
	if(root->Attribute("type", (int *)&mType) == NULL)
		return 1;
	if(mType != REQUEST)
		return 1;
	if(root->Attribute("method", (int *)&mMethod) == NULL)
		return 1;
	if(root->Attribute("seq", &mId) == NULL)
		return 1;
	if(root->Attribute("from", &mFrom) == NULL)
		return 1;
	if(root->Attribute("to", &mTo) == NULL)
		return 1;
	if(root->Attribute("initiator", &mInitiatorId) == NULL)
		return 1;
	TiXmlHandle msg(root);
	TiXmlElement *param;
	switch(mMethod)
	{
		case REGISTER:
			{
				int expire;
				if((param = msg.FirstChild("Expire").ToElement()) == NULL)
					return 1;
				if(param->Attribute("time", &expire) == NULL)
					return 1;
				mContent.reg.expire = expire;
				TiXmlElement *item= msg.FirstChild("ChildMatrix").FirstChild("Item").ToElement();
				int matrixid, i;
				for(i = 0;item;item = item->NextSiblingElement(), ++i)
				{
					item->Attribute("id", &matrixid);
					mContent.reg.children[i] = matrixid;
				}
				mContent.reg.children_num = i;
			}
			break;
		case SWITCH:
			{
				int localid;
				if((param = msg.FirstChild("User").ToElement()) == NULL)
					return 1;
				param->Attribute("id", &localid);
				mContent.swi.userid = localid;
				if((param = msg.FirstChild("Camera").ToElement()) == NULL)
					return 1;
				param->Attribute("id", &localid);
				mContent.swi.cameraid = localid;
			}
			break;
		case PTZ:
			{
				int localid;
				if((param = msg.FirstChild("User").ToElement()) == NULL)
					return 1;
				param->Attribute("id", &localid);
				mContent.ptz.userid = localid;
				if((param = msg.FirstChild("Camera").ToElement()) == NULL)
					return 1;
				param->Attribute("id", &localid);
				mContent.ptz.cameraid = localid;
				if((param = msg.FirstChild("Operation").ToElement()) == NULL)
					return 1;
				int directioncode, angle;
				param->Attribute("direction", &directioncode);
				param->Attribute("angle", &angle);
				mContent.ptz.direction = directioncode;
				mContent.ptz.angle = angle;
			}
			break;
		default:
			return 1;
	}
	return 0;
}

size_t
MCCAgentRequest::print(char *buf, size_t len) const
{
	ostringstream ostr;
	ostr<<"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n"
		<<"<XML_MESSAGE type=\""<<mType<<"\" method=\""<<mMethod<<"\" seq=\""<<mId<<"\" from=\""<<mFrom<<"\" to=\""<<mTo<<"\" initiator=\""<<mInitiatorId<<"\">\r\n";
	if(mMethod == REGISTER)
	{
		ostr<<"<Expire time=\""<<mContent.reg.expire<<"\"/>\r\n"
			<<"<ChildMatrix>\r\n";
		for(int i=0;i<mContent.reg.children_num;i++)
		{
			ostr<<"<Item id=\""<<mContent.reg.children[i]<<"\"/>\r\n";
		}
		ostr<<"</ChildMatrix>\r\n"
			<<"</XML_MESSAGE>\r\n";
		strcpy(buf, ostr.str().c_str());
		return strlen(buf);
	}
	else if(mMethod == SWITCH)
	{
		ostr<<"<User id=\""<<mContent.swi.userid<<"\"/>\r\n"
			<<"<Camera id=\""<<mContent.swi.cameraid<<"\"/>\r\n"
			<<"</XML_MESSAGE>\r\n";
		strcpy(buf, ostr.str().c_str());
		return strlen(buf);
	}
	else if(mMethod == PTZ)
	{
		ostr<<"<User id=\""<<mContent.ptz.userid<<"\"/>\r\n"
			<<"<Camera id=\""<<mContent.ptz.cameraid<<"\"/>\r\n"
			<<"<Operation direction=\""<<mContent.ptz.direction<<"\" angle=\""<<mContent.ptz.angle<<"\"/>\r\n"
			<<"</XML_MESSAGE>\r\n";
		strcpy(buf, ostr.str().c_str());
		return strlen(buf);
	}
	return 0;
}

int 
MCCAgentResponse::Parse(const char *buf, size_t len)
{
	TiXmlDocument doc;
	doc.Parse(buf);
	TiXmlElement *root = doc.RootElement();
	if(root->Attribute("type", (int *)&mType) == NULL)
		return 1;
	if(mType != RESPONSE)
		return 1;
	if(root->Attribute("method", (int *)&mMethod) == NULL)
		return 1;
	if(root->Attribute("seq", &mId) == NULL)
		return 1;
	if(root->Attribute("from", &mFrom) == NULL)
		return 1;
	if(root->Attribute("to", &mTo) == NULL)
		return 1;
	if(root->Attribute("initiator", &mInitiatorId) == NULL)
		return 1;
	TiXmlHandle msg(root);
	TiXmlElement *param;
	if((param = msg.FirstChild("Status").ToElement()) == NULL)
		return 1;
	if(param->Attribute("code", &mStatusCode) == NULL)
		return 1;
	const char *reason;
	if((reason = param->Attribute("reason", &mInitiatorId)) == NULL)
		return 1;
	mReason = reason;
	switch(mMethod)
	{
		case SWITCH:
			{
				if((param = msg.FirstChild("Connection").ToElement()) == NULL)
					return 1;
				int portid;
				param->Attribute("outport", &portid);
				mContent.swi.outport= portid;
			}
			break;
		default:
			return 1;
	}
	return 0;
}

size_t
MCCAgentResponse::print(char *buf, size_t len) const
{
	ostringstream ostr;
	ostr<<"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n"
		<<"<XML_MESSAGE type=\""<<mType<<"\" method=\""<<mMethod<<"\" seq=\""<<mId<<"\" from=\""<<mFrom<<"\" to=\""<<mTo<<"\" initiator=\""<<mInitiatorId<<"\">\r\n"
		<<"<Status code=\""<<mStatusCode<<"\" reason=\""<<mReason<<"\"/>\r\n";
	if(mMethod == SWITCH)
		ostr<<"<Connection outport=\""<<mContent.swi.outport<<"\"/>\r\n";
		ostr<<"</XML_MESSAGE>\r\n";
	strcpy(buf, ostr.str().c_str());
	return strlen(buf);
}

const std::ostream &operator<<(std::ostream &os, const MCCAgentRequest &request)
{
	char obuf[1024];
	request.print(obuf, 0);
	os<<obuf;
	return os;
}

const std::ostream &operator<<(std::ostream &os, const MCCAgentResponse &response)
{
	char obuf[1024];
	response.print(obuf, 0);
	os<<obuf;
	return os;
}

int 
MCCAgentMessageHelper::buildRegRequest(const int mid,
		time_t expire,
		MCCAgentRequest &request)
{
	request.mType = REQUEST;
	request.mInitiatorId = 0;
	request.mFrom = mid;
	request.mTo = 0;
	request.mMethod = REGISTER;
	request.mId = 0;
	request.mContent.reg.expire = expire;
	return 0;
}

int 
MCCAgentMessageHelper::buildSwitchRequest(const MCCGlobalID &uid, 
		const MCCGlobalID &cid, 
		MCCAgentRequest &request) 
{ 
	request.mType = REQUEST;
	request.mInitiatorId = uid.localId();
	request.mFrom = uid.matrixId();
	request.mTo = cid.matrixId();
	request.mMethod = SWITCH;
	struct timespec tv;
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tv);
	srand(tv.tv_nsec);
	request.mId = rand();
	request.mContent.swi.userid = uid.localId();
	request.mContent.swi.cameraid = cid.localId();
	request.mContent.swi.outport = 0;
	return 0;
}

int 
MCCAgentMessageHelper::buildPtzRequest(const MCCGlobalID &uid, 
		const MCCGlobalID &cid, 
		const int opcode, 
		const int angle, 
		MCCAgentRequest &request) 
{ 
	request.mType = REQUEST;
	request.mInitiatorId = uid.localId();
	request.mFrom = uid.matrixId();
	request.mTo = cid.matrixId();
	request.mMethod = PTZ;
	struct timespec tv;
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tv);
	srand(tv.tv_nsec);
	request.mId = rand();
	request.mContent.ptz.userid = uid.localId();
	request.mContent.ptz.cameraid = cid.localId();
	request.mContent.ptz.direction = opcode;
	request.mContent.ptz.angle = angle;
	return 0;
}

int 
MCCAgentMessageHelper::buildSwitchResponse(const MCCAgentRequest &request, 
		const int statuscode, 
		const std::string &reason,
		const int outport,
		MCCAgentResponse &response) 
{
//	MCCAgentResponse response;
	response.mType = RESPONSE;
	response.mInitiatorId = request.mInitiatorId;
	response.mFrom = request.mTo;
	response.mTo = request.mFrom;
	response.mMethod = request.mMethod;
	response.mId = request.mId;
	response.mStatusCode = statuscode;
	response.mReason = reason;
	response.mContent.swi.outport = outport;
	return 0;
}

int 
MCCAgentMessageHelper::buildPtzResponse(const MCCAgentRequest &request, 
		const int statuscode, 
		const std::string &reason,
		MCCAgentResponse &response) 
{
//	MCCAgentResponse response;
	response.mType = RESPONSE;
	response.mInitiatorId = request.mInitiatorId;
	response.mFrom = request.mTo;
	response.mTo = request.mFrom;
	response.mMethod = request.mMethod;
	response.mId = request.mId;
	response.mStatusCode = statuscode;
	response.mReason = reason;
	return 0;
}
