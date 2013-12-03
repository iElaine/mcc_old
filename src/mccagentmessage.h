#ifndef __MCCAGENTMESSAGE_H__
#define __MCCAGENTMESSAGE_H__

/**
 * Register Request
 * <?xml version="1.0" encoding="UTF-8"?>
 * <XML_MESSAGE type="request" method="Register" seq="" from="" to="" initiator="" expire="">
 *	<ChildMatrix>
 *		<Item id="mid"/>
 *		<Item id="mid"/>
 *		<Item id="mid"/>
 *	</ChildMatrix>
 * </XML_MESSAGE>
 */

/**
 * Switch
 * <?xml version="1.0" encoding="UTF-8"?>
 * <XML_MESSAGE type="request" method="Switch" seq="" from="" to="" initiator="">
 *	<User id=""/>
 *	<Camera id=""/>
 * </XML_MESSAGE>
 */

/**
 * ptz 
 * <?xml version="1.0" encoding="UTF-8"?>
 * <XML_MESSAGE type="request" method="Switch" seq="" from="" to="" initiator="">
 *	<User id="local"/>
 *	<Camera id=""/>
 *	<Operation direction="directioncode" angle="angle"/>
 * </XML_MESSAGE>
 */


/**
 * Switch response
 * <?xml version="1.0" encoding="UTF-8"?>
 * <XML_MESSAGE type="response" method="Switch" seq="" from="" to="" initiator="">
 *	<Status code="" reason=""/>
 *	<Connection outport="100"/>
 * </XML_MESSAGE>
 */

#include <vector>
#include <string>
#include <tinyxml.h>
#include <iostream>
#include "mccglobalid.h"
#define MAX_CHILDREN_NUM 50
#define MAX_LEVEL_NUM 4
enum MessageType {
	REQUEST,
	RESPONSE
};
struct MessageHeader {
	public:
		size_t	length;
		int		type;
};

enum MessageMethod {
	REGISTER,
	SWITCH,
	PTZ
};

union Content {
	struct {
		int expire;
		int children_num;
		int children[MAX_CHILDREN_NUM];
	} reg;
	struct {
		int userid;
		int cameraid;
		int outport;
	} swi;
	struct {
		int userid;
		int cameraid;
		int direction;
		int angle;
	} ptz;	
};

class MCCAgentMessageHelper;
typedef int MessageId;
class MCCAgentMessage {
	public:
		MessageId id() const { return mId;}
		MessageMethod method() const { return mMethod;}
		int initiator() const { return mInitiatorId;}
		int from() const { return mFrom;}
		int to() const { return mTo;}
		Content &content() { return mContent;}
		MessageType type() { return mType;}
		virtual size_t print(char *buf, size_t len) const = 0;
		bool isValid() const { return mbValid;}
		void getUserId(MCCGlobalID &uid) { uid = MCCGlobalID(mFrom, mInitiatorId);}
	protected:
		virtual int Parse(const char *,size_t) = 0;
		int mInitiatorId;
		int mFrom;
		int mTo;
		MessageType mType;
		MessageMethod mMethod;
		MessageId mId;
		Content mContent;
		bool mbValid;
};

class MCCAgentRequest : public MCCAgentMessage {
	public:
		MCCAgentRequest(const char *buf, size_t len) {
			try{
				if(Parse(buf, len) != 0)
					mbValid = false;
				else
					mbValid = true;
			}
			catch (std::string &e)
			{
				mbValid = false;
				std::cout<<e<<std::endl;
			}
		}
		MCCAgentRequest() {}
		virtual size_t print(char *buf, size_t len) const;
	protected:
		friend class MCCAgentServer;
		friend class MCCAgentClient;
		friend class MCCAgentMessageHelper;
		virtual int Parse(const char *buf, size_t len);
};

class MCCAgentResponse : public MCCAgentMessage {
	public:
		MCCAgentResponse(const char *buf, size_t len) {
			try{
				if(Parse(buf, len) != 0)
					mbValid = false;
				else
					mbValid = true;

			}
			catch (std::string &e)
			{
				mbValid = false;
				std::cout<<e<<std::endl;
			}
		}
		MCCAgentResponse() {}
		virtual size_t print(char *buf, size_t len) const;
		int statuscode() const { return mStatusCode;}
		std::string reason() const { return mReason;}
	protected:
		friend class MCCAgentServer;
		friend class MCCAgentClient;
		friend class MCCAgentMessageHelper;
		virtual int Parse(const char *buf, size_t len);
		int mStatusCode;
		std::string mReason;
};

class MCCAgentMessageHelper {
	public:
		static int buildSwitchRequest(const MCCGlobalID &uid, 
				const MCCGlobalID &cid, 
				MCCAgentRequest &request);
		static int buildPtzRequest(const MCCGlobalID &uid, 
				const MCCGlobalID &cid, 
				const int opcode, 
				const int angle, 
				MCCAgentRequest &request);
		static int buildSwitchResponse(const MCCAgentRequest &request, 
				const int statuscode, 
				const std::string &reason,
				const int outport,
				MCCAgentResponse &response);
		static int buildPtzResponse(const MCCAgentRequest &request, 
				const int statuscode, 
				const std::string &reason,
				MCCAgentResponse &response);
		static int buildRegRequest(const int mid,
				time_t expire,
				MCCAgentRequest &request);
};

const std::ostream &operator<<(std::ostream &os, const MCCAgentRequest &request);
const std::ostream &operator<<(std::ostream &os, const MCCAgentResponse &request);

#endif 
