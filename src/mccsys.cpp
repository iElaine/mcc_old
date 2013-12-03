#include "mcckeyboardmanager.h"
#include "mccresourcemanager.h"
#include <assert.h>
#include <iostream>
#include <errno.h>
#include <arpa/inet.h>
#include <map>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
using namespace std;
#define MAX_DEPTH 10
int local_matrix_id = 0;
int max_cam = 1000;
int min_cam = 0;
int max_mon = 10;
int min_mon = 0;

class User {
	public:
		User(int i, int mi, int exp = 20) : id(i), matrix_id(mi), expires(exp) {}
		User() : id(-1), matrix_id(-1) ,expires(0) {};
		const User &operator=(const User &user) {
			id = user.id;
			matrix_id = user.matrix_id;
			expires = 0;
			update();
			return *this;
		}
		bool operator==(User &u) { return ((id == u.id) && (matrix_id == u.matrix_id));}
		bool operator<=(User &u) { 
			if(id < u.id) 
				return true;
			else if(id == u.id && matrix_id <= u.matrix_id)
				return true;
			else 
				return false;
		}
		User &update()
		{
			struct timeval t;
			gettimeofday(&t, NULL);
			expires = t.tv_sec + 20;
			return *this;
		}
		int id;
		int matrix_id;
		int expires;
};

User nulluser;

struct Matrix {
	Matrix(int id, const char *ip, unsigned int port) {
		memset(&host, 0, sizeof(host));
		host.sin_family = AF_INET;
		host.sin_port = htons(port);
		inet_pton(AF_INET, ip, &host.sin_addr);
	}
	Matrix() {}
	struct sockaddr_in host;
};

struct ResRequest{
	ResRequest(int mid, int cid, int uid, int exp = 20) : cam_id(cid), to_matrix_id(mid), from_matrix_id(local_matrix_id), user_id(uid), expires(exp) {
		jump = 0;
	}
	ResRequest() {}
	int cam_id;
	int to_matrix_id;
	int from_matrix_id;
	int user_id;
	int expires;
	int route[MAX_DEPTH];
	int jump;
};

int initRequestRoute(ResRequest &req)
{
	return 0;
}

struct ResResponse {
	ResResponse(int code, char *p_msg) {
		status_code = code;
		strcpy(msg, p_msg);
	}
	ResResponse() {}
	int status_code;
	char msg[100];
	int route[MAX_DEPTH];
	int jump;
};

//typedef vector<User> Userlist;
typedef map<int, User > ResourceMap;
//ResourceMap cam;
//ResourceMap mon;
//ResourceMap trunk;
User cam[100];
User mon[100];
User trunk[4];

struct TrunkInfo {
	int id;
	User user;
	struct {
		int mid;
		int cid;
	} g_cid;
} trunkinfo[4];

void *matrix_address_book[100];

int init_matrix_addr_book()
{
	memset(matrix_address_book, 0 ,sizeof(matrix_address_book));
	matrix_address_book[0] = new Matrix(0,"192.168.229.140", 10000); 
	matrix_address_book[1] = new Matrix(1,"192.168.229.141", 10000); 
	return 0;
}

int init_local_resource()
{
	for(int i=0;i<16;i++)
	{
		cout<<"add local cammera, id = "<<i<<endl;
	}
	for(int i=0;i<10;i++)
	{
		cout<<"add local monitor, id = "<<i<<endl;
	}
	return 0;
}

bool isExpired(User &user)
{
	struct timeval t;
	gettimeofday(&t, NULL);
	return (t.tv_sec > user.expires);
}

User &getMinTrunkUser(int &index)
{
	User *p = trunk;
	User *min = trunk;
	index = 0;
	for(int i=0;i<4;i++,p++)
	{
		if(*p <= *min)
		{
			min = p;
			index = i;
		}
	}
	return *min;
}

int getTrunkID(User &user)
{
	for(int i=0;i<4;i++)
	{
		if(trunk[i] == user)
			return i;
	}
	return -1;
}

class AutoSock {
	public:
		AutoSock(int sock) {};
		virtual ~AutoSock() { 
			if(sock > 0)
				close(sock);
		}
	private:
		int sock;
};

class Communication : public ThreadIf{
	public:
		int Request(int matrix_id, int cam_id, int user_id)
		{
			Matrix *m;
			if((m = (Matrix *)matrix_address_book[matrix_id]) == NULL)
			{
				cout<<"error matrix "<<matrix_id<<" dose not exit!"<<endl;
				return -1;
			}
			int sock;
			AutoSock s(sock);
			cout<<"local user "<<user_id<<" request remote camera "<<cam_id<<" in matrix "<<matrix_id<<endl;
			if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
			{
				perror("failed to create socket");
				return -1;
			}
			if(connect(sock, (struct sockaddr *)&(m->host), sizeof(m->host)) < 0)
			{
				perror("failed to connect");
				return -1;
			}
			ResRequest req(matrix_id, cam_id, user_id);
			ResResponse resp;
			if(send(sock, &req, sizeof(req), 0)< 0)
			{
				perror("failed to send");
				return -1;
			}
			if(recv(sock, &resp, sizeof(resp), 0) < 0)
			{
				perror("failed to recv");
				return -1;
			}
			if(resp.status_code == 200)
				return 0;
			cout<<"request remote camera "<<cam_id<<" at matrix "<<matrix_id<<" failed, because:"<<resp.msg<<endl;
			return -2;
		}
	protected:
		int process() {
			if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
			{
				perror("failed to create socket");
				return 0;
			}
			memset(&serv_addr, 0, sizeof(serv_addr));
			serv_addr.sin_family = AF_INET;
			serv_addr.sin_port = htons(10000);
			serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
			if(bind(listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
			{
				perror("failed to bind");
				return 0;
			}
			if(listen(listenfd, 10) < 0)
			{
				perror("failed to listen");
				return 0;
			}
			while(1)
			{
				int connectfd;
				ResRequest req;
				bool trunk_ok = false;
				bool camera_ok = false;
				socklen_t len = sizeof(clnt_addr);
				if((connectfd = accept(listenfd, (struct sockaddr *)&clnt_addr, &len)) < 0)
				{
					perror("accept error");
					return 0;
				}
				/**
				 *
				 *
				 *
				 *
				 */
				int n = recv(connectfd, &req, sizeof(req), 0);
				sleep(1);
				cout<<"Communication:"<<"receive request: from: "<<req.from_matrix_id<<" to: "<<req.to_matrix_id<<" camera: "
					<<req.cam_id<<" user: "<<req.user_id<<endl;
				assert(n == sizeof(req));

				int trunk_id;
				User user(req.user_id, req.from_matrix_id);
				User min_trunk_user(getMinTrunkUser(trunk_id));

				// check if the min user of trunk is smaller than user
				if( min_trunk_user == nulluser || min_trunk_user <= user || isExpired(min_trunk_user)){
					if(min_trunk_user == nulluser)
						cout<<"trunk free"<<endl;
					else if(min_trunk_user <= user)
						cout<<"priority of trunk's min previous user is smaller"<<endl;
					else if(isExpired(min_trunk_user))
						cout<<"trunk's min previous user expires"<<endl;
					cout<<"trunk is availble"<<endl;
					trunk_ok = true;
				}
				else
				{
					// if no trunk available, there is no need to check camera , so response with 400 
					// close the connection and continue to receive the next request;
					ResResponse response(400, "No Trunk Available");
					cout<<"no trunk is not available"<<endl;
					send(connectfd, &response, sizeof(response), 0);
					close(connectfd);
					continue;
				}
				// if trunk is ok then check if camera is ok
				if(req.to_matrix_id == local_matrix_id)
				{
					// if box hold the camera 
					cout<<"request local camera"<<endl;
					if((cam[req.cam_id] == nulluser || isExpired(cam[req.cam_id]) || cam[req.cam_id] <= user)
							/*&& (min_trunk_user <= user)*/)
					{
						// print why camera is ok
						if(cam[req.cam_id] == nulluser)
							cout<<"camera free"<<endl;
						else if(cam[req.cam_id] <= user)
							cout<<"priority of camera's previous user is smaller"<<endl;
						else if(isExpired(cam[req.cam_id]))
							cout<<"camera's previous user expires"<<endl;
						// set camera_ok flag true and update user
						camera_ok = true;
						cam[req.cam_id] = user;
						cout<<"camera "<<req.cam_id<<" <-- User("<<user.id<<":"<<user.matrix_id<<")"<<endl;
					}
				}
				else
				{
					// iff box don't hold the camera request next jump
					cout<<"request remote camera"<<endl;
					camera_ok = (Request(req.to_matrix_id, req.cam_id, req.user_id) == 0);
				}
				// when trunk and camera is both ok update the user of minium trunk
				if(trunk_ok && camera_ok)
				{
//					min_trunk_user = user;
					trunk[trunk_id] = user;
					cout<<"trunk  "<<trunk_id<<" <-- User("<<user.id<<":"<<user.matrix_id<<")"<<endl;
					ResResponse response(200, "Ok");
					send(connectfd, &response, sizeof(response), 0);
				}
				else
				{
					ResResponse response(400, "Camera is Not Available!");
					cout<<"camera is not availble"<<endl;
					send(connectfd, &response, sizeof(response), 0);
				}
				close(connectfd);
			}
			close(listenfd);
			return 0;
		}
		struct sockaddr_in serv_addr;
		struct sockaddr_in clnt_addr;
		int listenfd;
};

int init_sys_var(int argc, char *argv[])
{
	if(argc == 1)
	{
		cout<<"Usage: mccsys mid"<<endl;
		return 0;
	}
	local_matrix_id = atoi(argv[1]);
	cout<<"---------set local id:"<<local_matrix_id<<endl;
	return 0;
}

int main(int argc, char *argv[])
{
	init_sys_var(argc, argv);
	init_local_resource();
	init_matrix_addr_book();
	MCCKeyboardManager manager;
	Communication comm;
	manager.Start();
	comm.Start();
	manager.LoadKeyboard("generic","/dev/ttyUSB0", "/dev/ttyUSB1", 9600, 8, 1, 'n', 0);
	cout<<"load keyboard done"<<endl;
	while(1)
	{
		int cid, mid, moid, uid;
		cout<<"matrix id:";
		cin>>mid;
		cout<<"camera id:";
		cin>>cid;
		cout<<"monitor id:";
		cin>>moid;
		cout<<"user id:";
		cin>>uid;
		User user(uid, mid);
		if(moid > max_mon || moid < min_mon )
		{
			cout<<"monitor dosen't exist!"<<endl;
			continue;
		}
		else if(mon[moid].id > uid && !isExpired(mon[moid]))
		{
			cout<<"monitor is busy"<<endl;
			continue;
		}
		else
			cout<<"monitor is avialble!"<<endl;
		if(mid == local_matrix_id && cam[cid] <= user){
			cam[cid] = user;
			cout<<"camera is aviable!"<<endl;
		}
		else if(mid != local_matrix_id && comm.Request(mid, cid, uid) == 0){
			cout<<"camera is aviable!"<<endl;
		}
		else{
			cout<<"camera is not aviable!"<<endl;
			continue;
		}
		mon[moid] = User(uid, local_matrix_id);
	};
	manager.UnloadKeyboard(0);
	comm.Stop();
	manager.Stop();
	return 0;
}
