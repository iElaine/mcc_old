#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include "../client.h"

int main()
{
	int sock;
	struct sockaddr_in server_addr;
	char ibuf[1024];
	char obuf[1024];
	int len;
	struct client_packet p;
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock < 0)
	{
		perror("socket");
		exit(0);
	}
	printf("socket ok >> ");
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(10000);
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	if(connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{
		perror("connect");
		exit(0);
	}
	printf("connect ok >> ");
	while(1)
	{
		int cid, mid;
		printf("input camera id:");
		scanf("%d", &cid);
		printf("input monitor id:");
		scanf("%d", &mid);
		p.type = 1;
		p.bind_kb = 1;
		p.cid = cid;
		p.mid = mid;
		len = send(sock, &p, sizeof(struct client_packet), 0);
		if(len < 0)
		{
			perror("send");
			exit(0);
		}
//		len = recv(sock, ibuf, 10, 0);
//		if(len < 0)
//		{
//			perror("recv");
//			exit(0);
//		}
	}
	close(sock);
	return 0;
}
