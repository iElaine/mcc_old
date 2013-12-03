class Connection {
	public:
		Connection(struct sockaddr_in *clientaddr, int fd);
		{
		}
};
class ConnectionHandle {
	public:
		virtual void onNewConnection(int fd);
		virtual void onDisconnected(int fd);
		virtual void onTransportError(int fd);
};
class ConnectionManager : public ThreadIf {
	public:
		ConnectionManager() : mListenFd(-1) {}
		bool startListen(unsigned short port) {
			if(mListenFd < 0)
				mListenFd = socket(AF_INET, SOCK_STREAM, 0);
			if(mListenFd < 0)
			{
				return false;
			}
			struct sockaddr_in server_addr;
			server_addr.sin_family = AF_INET;
			server_addr.sin_port = htons(port);
			server_addr.sin_addr.s_addr = INADDR_ANY;
			if(bind(mListenFd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
				return false;
			if(listen(mListenFd, 5) < 0)
				return false;
			return Start();
		}
	protected:
		int mListenFd;
		virtual int process() {
		}

};
