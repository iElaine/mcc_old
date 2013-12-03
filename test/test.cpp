#include <iostream>
#include <memory>
//#include <tinyxml/tinyxml.h>
#include <gtest/gtest.h>
#include "socketpair.h"
#include "serialport.h"
#include "mcckeyboard.h"
//#include "mcckeyboardmanager.h"
#include "mcckeyboardvirtual.h"
#include "serialportlistener.h"
#include "processfactory.h"
#include "threadif.h"
//#include "sipua.h"
#include "queue.h"
#include "mccglobalid.h"
#include "mccresourcemanager.h"
#include "mccsystem.h"
#include "log.h"
#include "configparser.h"
#include "semaphore.h"
#include "heap.h"
#include "threadpool.h"
#include "asynctcpserver.h"
#include "asynctcpclient.h"
#include "mccagentserver.h"
#include "mccagentclient.h"
#include "atomic.h"
#include "prioqueue.h"
#include "util.h"
#include "httpserver.h"
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/configurator.h>


#include <queue>
#include <stdexcept>
#include <tinyxml.h>
using namespace std;
using namespace log4cplus;

class MCCSystemEnvironment : public testing::Environment
{
	public:
		virtual void SetUp() {
			BasicConfigurator config;
			config.configure();
		}	
		virtual void TearDown() {
		}
};

int main(int argc, char *argv[])
{
	testing::AddGlobalTestEnvironment((testing::Environment*)new MCCSystemEnvironment);
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

class testthread : public ThreadIf {
	protected:
		virtual void thread() {
			while(!isStop())
				cout<<"hello world!"<<endl;
		}
};

class squeuetestthread : public ThreadIf {
	public:
		bool Start(Squeue<int> *queue) {
			mQueue = queue;
			return ThreadIf::Start() == 0;
		}
	protected:
		virtual void thread() {
			while(!isStop())
			{
				int a = 0;
				if(mQueue->get(a,100))
					cout<<this<<":"<<a<<endl;
			}
		}
		Squeue<int> *mQueue;
};
class testThread : public ThreadIf {
	protected:
		virtual void thread(){
			cout<<"thread start"<<endl;
			while(!isStop())
			{
				cout<<"thread running"<<endl;
				usleep(1000);
			}
		}
		virtual void atexit() {
			cout<<"exit"<<endl;
		}
};
TEST(TestThreadIf, test)
{
	testThread th;
	th.Start();
	usleep(1000);
	th.Stop();
	th.Start();
	usleep(10000);
	th.Shutdown();
}

class TestTask : public Task {
	virtual int operator()() {
		cout<<"task "<<this<<endl;
		return 0;
	}
};

TEST(SqueueTest, test)
{
	Squeue<int> sq;
	squeuetestthread threads[3];
	for(int i=0;i<3;i++)
		threads[i].Start(&sq);
	int a = 1;
	while(a)
	{
		cin>>a;
		cout<<a;
		sq.add(a);
	}
	for(int i=0;i<3;i++)
		threads[i].Stop();
}


TEST(ThreadPoolTest, test)
{
	ThreadPool pool(4);
	pool.Start();
	for(int i=0;i<5;i++)
	{
		pool.submitTask(new TestTask);
	}
	usleep(100000);
	pool.StopAll();
}

TEST(HeapTest, test)
{
	Heap<int> heap;
	for(int i=0;i<100;i++)
		heap.push(rand()/1000000);
	int last_e, e;
	heap.pop(last_e);
	for(int i=0;i<100;i++)
		if(heap.pop(e))
		{
			ASSERT_LE(e, last_e);
			last_e = e;
		}
}

class testthread1 : public ThreadIf {
	public:
		testthread1()
		{
			sem = new Semaphore;
			mutex = new Mutex;
		}
		~testthread1()
		{
			delete sem;
			delete mutex;
		}
		int Signal()
		{
			return sem->Signal();
		}
	protected:
		virtual void thread() {
			while(!isStop())
			{
				mutex->Lock();
				sem->Wait(*mutex);
				cout<<"receive a signal"<<endl;
				mutex->Unlock();
			}
		}
		Semaphore *sem;
		Mutex	  *mutex;
};

class TestHandler : public AsyncTCPServerHandler {

		virtual void onNewConnection(int fd)
		{
			cout<<"On new connection!"<<endl;
		}
		virtual void onDisconnect(int fd)
		{
			cout<<"On Disconnect!"<<endl;
		}
		virtual void onReadable(AsyncTCPServer &server, int fd)
		{
			char buf[1024];
			int len;
			len = server.Receive(fd, buf, sizeof(buf));
			len = server.Send(fd, buf, len);
			cout<<"On readable!"<<buf<<endl;
		}
		virtual void onWriteble(AsyncTCPServer &server, int fd)
		{
			cout<<"On writeble"<<endl;
		}
		virtual void onExcept(int fd)
		{
			cout<<"On except!"<<endl;
		}
};

class TestClientHandler : public AsyncTCPClientHandler {
	public:
		virtual void onConnected(AsyncTCPClient &client, int fd)
		{}
		virtual void onDisconnected(AsyncTCPClient &client, int fd)
		{}
		virtual void onReadable(AsyncTCPClient &client, int fd)
		{
			char buf[1024];
			int len;
			len = client.Receive(buf, sizeof(buf));
			buf[len] = 0;
			cout<<"On readable! "<<buf<<endl;
		}
};

//TEST(AsyncTCPTest, test)
//{
//	AsyncTCPServer server(new TestHandler);
//	server.Start(NULL, 10000);
//	AsyncTCPClient client(new TestClientHandler);
//	client.Start("127.0.0.1", 10000);
//	usleep(10000);
//	char msg[] = "This is a message from client";
//	client.Send(msg, sizeof(msg));
//	usleep(1000000);
//	client.Stop();
//	server.Stop();
//}


class TestAgentHandler : 
	public MCCAgentServerEventHandler,
	public MCCAgentClientEventHandler
{
	public:
		virtual void onNewConnection(int fd, MCCAgentServer& server) { cout<<"on connect"<<endl;}
		virtual void onDisconnect(int fd, MCCAgentServer& server) { cout<<"on disconnect"<<endl;}
		virtual void onRegister(int fd, MCCAgentRequest &request, MCCAgentServer& server) {}
		virtual void onServerRxRequest(int fd, MCCAgentRequest &request, MCCAgentServer& server){
			cout<<request;
			MCCAgentResponse response;
			MCCAgentMessageHelper::buildSwitchResponse(request, 200, "ok", 12, response);
			server.Send(fd, response);
		}
		virtual void onServerRxResponse(int fd, MCCAgentResponse &response, MCCAgentServer& server){}
		virtual void onClientConnected(int fd) {}
		virtual void onClientDisconnected(int fd) {}
		virtual void onClientRxRequest(int fd, MCCAgentRequest &request, MCCAgentClient& client){}
		virtual void onClientRxResponse(int fd, MCCAgentResponse &response, MCCAgentClient& client){
			cout<<response;
		}
};

TEST(MCCAgentTest, test)
{
	TestAgentHandler handler;
	MCCAgentServer agentserver;
	agentserver.setEventHandler(&handler);
	agentserver.Start(20000);
	MCCAgentClient agentclient;
	agentclient.setEventHandler(&handler);
	agentclient.Start("127.0.0.1", 20000);
	MCCAgentRequest request;
	MCCAgentMessageHelper::buildSwitchRequest(MCCGlobalID(1000,2), MCCGlobalID(2000,20), request);
	agentclient.Send(request);
	sleep(2);
	agentclient.Stop();
	agentserver.Stop();
}

TEST(GlobalIDTest, test)
{
	MCCGlobalID id1(1,10);
	MCCGlobalID id2(10,2);
	MCCGlobalID id3(1,2);
	ASSERT_TRUE(id1 != id2);
	ASSERT_TRUE(id1 == id1);
	ASSERT_TRUE(id1 <= id2);
	ASSERT_TRUE(id1 >= id3);
	ASSERT_EQ(id1.GetMatrixID(), 1);
}

TEST(MCCAgentMessageTest, RequestParseTest)
{
	char obuf[1024];
	// test SWITCH Request
	{
		char buf[] = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
			"<XML_MESSAGE type=\"0\" method=\"1\" seq=\"12345\" from=\"1000\" to=\"2000\" initiator=\"2\">"
			"<User id=\"2\"/>"
			"<Camera id=\"20\"/>"
			//				 "<Operation direction=\"1\" angle=\"20\"/>"
			"</XML_MESSAGE>";
		auto_ptr<MCCAgentRequest> request(new MCCAgentRequest(buf, sizeof(buf)));
		ASSERT_EQ(request->method(), 1);
		ASSERT_EQ(request->id(), 12345);
		ASSERT_EQ(request->from(), 1000);
		ASSERT_EQ(request->to(), 2000);
		ASSERT_EQ(request->initiator(), 2);
		ASSERT_EQ(request->content().swi.userid, 2);
		ASSERT_EQ(request->content().swi.cameraid, 20);
		request->print(obuf, 0);
		cout<<obuf<<endl;
	}
	// test REGISTER request
	{
		char buf[] = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
			"<XML_MESSAGE type=\"0\" method=\"0\" seq=\"12345\" from=\"1000\" to=\"2000\" initiator=\"2\">"
			"<Expire time=\"3600\"/>"
			"<ChildMatrix>"
			"<Item id=\"2001\"/>"
			"<Item id=\"2002\"/>"
			"<Item id=\"2003\"/>"
			"</ChildMatrix>"
			"</XML_MESSAGE>";
		auto_ptr<MCCAgentRequest> request(new MCCAgentRequest(buf,sizeof(buf)));
		ASSERT_EQ(request->method(), 0);
		ASSERT_EQ(request->id(), 12345);
		ASSERT_EQ(request->from(), 1000);
		ASSERT_EQ(request->to(), 2000);
		ASSERT_EQ(request->initiator(), 2);
		ASSERT_EQ(request->content().reg.expire, 3600);
		ASSERT_EQ(request->content().reg.children_num, 3);
		ASSERT_EQ(request->content().reg.children[0], 2001);
		ASSERT_EQ(request->content().reg.children[1], 2002);
		ASSERT_EQ(request->content().reg.children[2], 2003);
		request->print(obuf, 0);
		cout<<obuf<<endl;
	}
	{
		char buf[] = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
			"<XML_MESSAGE type=\"0\" method=\"2\" seq=\"12345\" from=\"1000\" to=\"2000\" initiator=\"2\">"
			"<User id=\"2\"/>"
			"<Camera id=\"20\"/>"
			"<Operation direction=\"1\" angle=\"20\"/>"
			"</XML_MESSAGE>";
		auto_ptr<MCCAgentRequest> request(new MCCAgentRequest(buf,sizeof(buf)));
		ASSERT_EQ(request->method(), 2);
		ASSERT_EQ(request->id(), 12345);
		ASSERT_EQ(request->from(), 1000);
		ASSERT_EQ(request->to(), 2000);
		ASSERT_EQ(request->initiator(), 2);
		ASSERT_EQ(request->content().ptz.userid, 2);
		ASSERT_EQ(request->content().ptz.cameraid, 20);
		ASSERT_EQ(request->content().ptz.direction, 1);
		ASSERT_EQ(request->content().ptz.angle, 20);
		request->print(obuf, 0);
		cout<<obuf<<endl;
	}
}

TEST(MCCAgentMessageTest, ResponseParseTest)
{
	char obuf[1024];
	char buf[] = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
		"<XML_MESSAGE type=\"1\" method=\"2\" seq=\"12345\" from=\"1000\" to=\"2000\" initiator=\"2\">"
		"<Status code=\"200\" reason=\"ok\"/>"
		"<Connection outport=\"10\"/>";
	auto_ptr<MCCAgentResponse> response(new MCCAgentResponse(buf,sizeof(buf)));
	ASSERT_EQ(response->method(), 2);
	ASSERT_EQ(response->id(), 12345);
	ASSERT_EQ(response->from(), 1000);
	ASSERT_EQ(response->to(), 2000);
	ASSERT_EQ(response->initiator(), 2);
	ASSERT_EQ(response->statuscode(), 200);
	ASSERT_EQ(response->reason(), "ok");
	response->print(obuf, 0);
	cout<<obuf<<endl;

}

TEST(MCCAgentMessageTest, RequestParseSpeedTest)
{	
	char swibuf[] = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
		"<XML_MESSAGE type=\"0\" method=\"1\" seq=\"12345\" from=\"1000\" to=\"2000\" initiator=\"2\">"
		"<User id=\"2\"/>"
		"<Camera id=\"20\"/>"
		"</XML_MESSAGE>";
	char regbuf[] = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
		"<XML_MESSAGE type=\"0\" method=\"0\" seq=\"12345\" from=\"1000\" to=\"2000\" initiator=\"2\">"
		"<Expire time=\"3600\">"
		"<ChildMatrix>"
		"<Item id=\"2001\"/>"
		"<Item id=\"2002\"/>"
		"<Item id=\"2003\"/>"
		"</ChildMatrix>"
		"</XML_MESSAGE>";

	for(int i=0;i<10000;i++)
	{
		auto_ptr<MCCAgentRequest> request1(new MCCAgentRequest(swibuf, sizeof(swibuf)));
		auto_ptr<MCCAgentRequest> request2(new MCCAgentRequest(regbuf, sizeof(regbuf)));
	}
}

TEST(MCCAgentMessageTest, ResponseParseSpeedTest)
{
	char buf[] = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
		"<XML_MESSAGE type=\"1\" method=\"2\" seq=\"12345\" from=\"1000\" to=\"2000\" initiator=\"2\">"
		"<Status code=\"200\" reason=\"ok\"/>"
		"<Connection outport=\"10\"/>";
	for(int i=0;i<10000;i++)
		auto_ptr<MCCAgentResponse> response(new MCCAgentResponse(buf,sizeof(buf)));

}

TEST(MCCAgentMessageTest, RequestPrintSpeedTest)
{
	char swibuf[] = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
		"<XML_MESSAGE type=\"0\" method=\"1\" seq=\"12345\" from=\"1000\" to=\"2000\" initiator=\"2\">"
		"<User id=\"2\"/>"
		"<Camera id=\"20\"/>"
		"</XML_MESSAGE>";
	char buf[1024];
	auto_ptr<MCCAgentRequest> request(new MCCAgentRequest(swibuf, sizeof(swibuf)));
	for(int i=0;i<10000;i++)
	{
		request->print(buf, 0);
	}
}
TEST(MCCAgentMessageTest, ResponsePrintSpeedTest)
{
	char obuf[1024];
	char buf[] = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
		"<XML_MESSAGE type=\"1\" method=\"2\" seq=\"12345\" from=\"1000\" to=\"2000\" initiator=\"2\">"
		"<Status code=\"200\" reason=\"ok\"/>"
		"<Connection outport=\"10\"/>";
	auto_ptr<MCCAgentResponse> response(new MCCAgentResponse(buf,sizeof(buf)));
	for(int i=0;i<10000;i++)
		response->print(obuf, 0);

}

TEST(MCCAgentServerTest, BuildTest)
{
	{
		auto_ptr<MCCAgentRequest> message(new MCCAgentRequest);
		MCCAgentMessageHelper::buildSwitchRequest(MCCGlobalID(1000,9), MCCGlobalID(2000,20), *message);
		ASSERT_EQ(message->from(), 1000);
		ASSERT_EQ(message->to(), 2000);
		ASSERT_EQ(message->initiator(), 9);
		ASSERT_EQ(message->method(), SWITCH);
		ASSERT_EQ(message->type(), REQUEST);
		ASSERT_EQ(message->content().swi.userid, 9);
		ASSERT_EQ(message->content().swi.cameraid, 20);
		cout<<*message;
	}
	{
		auto_ptr<MCCAgentRequest> message(new MCCAgentRequest);
		MCCAgentMessageHelper::buildPtzRequest(MCCGlobalID(1000,9), MCCGlobalID(2000,20), 1, 20, *message);
		ASSERT_EQ(message->from(), 1000);
		ASSERT_EQ(message->to(), 2000);
		ASSERT_EQ(message->initiator(), 9);
		ASSERT_EQ(message->method(), PTZ);
		ASSERT_EQ(message->type(), REQUEST);
		ASSERT_EQ(message->content().ptz.userid, 9);
		ASSERT_EQ(message->content().swi.cameraid, 20);
		ASSERT_EQ(message->content().ptz.direction , 1);
		ASSERT_EQ(message->content().ptz.angle, 20);
		cout<<*message;
	}
}



TEST(ConfigParserTest, test)
{
	ConfigParser config;
	config.Parse(0, NULL, "mcc.ini");
	cout<<config.getConfigString("default", "name", "mcckeyboard")<<endl;;
	cout<<config.getConfigInt("keyboard", "KeyboardNum", 10)<<endl;;
	cout<<config.getConfigInt("keyboard", "Keyboard2", 10)<<endl;;
}

TEST(TestSemaphore,test)
{
	testthread1 t;
	t.Start();
	usleep(100000);
	t.Signal();
	t.Stop();
}

#define TestLog(log_text) do { \
	log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("logger")); \
	LOG4CPLUS_WARN(logger, (log_text)); \
} while(0)

TEST(Log4cplusTest, test)
{
	Logger logger = Logger::getInstance(LOG4CPLUS_TEXT("logger"));
	LOG4CPLUS_WARN(logger, "hello world");
	TestLog("hahahhahaaha");
}

TEST(AtomicTest, test)
{
	Atomic<int> a(3);
	int b = ++a;
	cout<<b<<endl;
	b = a++;
	cout<<b<<endl;
}

TEST(PrioQueueTest, Test)
{
	PrioQueue<MCCGlobalID> queue;
	queue.add(MCCGlobalID(12, 9));
	queue.add(MCCGlobalID(11, 9));
	queue.add(MCCGlobalID(12, 8));
	queue.add(MCCGlobalID(2, 29));
	MCCGlobalID a;
	queue.get(a);
	cout<<a<<endl;
	queue.get(a);
	cout<<a<<endl;
	queue.get(a);
	cout<<a<<endl;
	queue.get(a);
	cout<<a<<endl;
}

TEST(PrioQueueTest, Test2)
{
	priority_queue<MCCGlobalID, vector<MCCGlobalID>, greater<MCCGlobalID> >queue;
	queue.push(MCCGlobalID(12, 9));
	queue.push(MCCGlobalID(11, 9));
	queue.push(MCCGlobalID(12, 8));
	queue.push(MCCGlobalID(2, 29));
	MCCGlobalID a;
	cout<<queue.top()<<endl;
	queue.pop();
	cout<<queue.top()<<endl;
	queue.pop();
	cout<<queue.top()<<endl;
	queue.pop();
	cout<<queue.top()<<endl;
	queue.pop();
}

TEST(StringUtilTest, Test)
{
	int a,b,c;
	char d;
	string src("9600,8,1,n");
	vector<string> res;
	StringUtil::split(src, string(","), res);
	vector<string>::iterator i;
	for(i=res.begin(); i!= res.end();++i)
	{
		cout<<*i<<endl;
	}
	a = StringUtil::atoi(res[0]);
	b = StringUtil::atoi(res[1]);
	c = StringUtil::atoi(res[2]);
	d = *(res[3].c_str());
	ASSERT_EQ(a, 9600);
	ASSERT_EQ(b, 8);
	ASSERT_EQ(c, 1);
	ASSERT_EQ(d, 'n');
}

TEST(HttpServerTest, Test)
{
	HttpServer server;
	server.Start(8080);
	while(1);
}
