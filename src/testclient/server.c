#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>

int main()
{
	int mListenFd = 0;
	struct sockaddr_in server_addr;
	int connfd;
	char ibuf[1024];
	char obuf[1024];
	printf("init network!\n");
	mListenFd  = socket(AF_INET, SOCK_STREAM, 0);
	if(mListenFd < 0)
	{
		perror("socket");
		exit(0);
	}
	printf("socket ok >> ");
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(10000);
	server_addr.sin_addr.s_addr = INADDR_ANY;
	if(bind(mListenFd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{
		perror("bind");
		exit(0);
	}
	printf("bind ok >> ");
	if(listen(mListenFd, 5) < 0)
	{
		perror("listen");
		exit(0);
	}
	printf("listen ok >> ");
	connfd = accept(mListenFd, NULL, NULL);
	if(connfd < 0)
	{
		perror("accept");
		exit(0);
	}
	printf("accept ok!\n");
	while(1)
	{
		recv(connfd, ibuf, 10, 0);
		ibuf[10] = 0;
		printf("%s\n",ibuf);
		send(connfd, obuf, 10, 0);
		if(ibuf[0] == 'q' &&
				ibuf[1] == 'i' &&
				ibuf[2] == 'u' &&
				ibuf[3] == 't') 
			break;
	}
	close(connfd);
	return 0;
};
