#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <resolv.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAXBUF 1024*4096


/*
	./Client 127.0.0.1 7838
*/
#pragma pack(1)
typedef struct
{
	unsigned int head;
	unsigned int dIpAdrs;
	unsigned int sIpAdrs;
	unsigned int version;
	unsigned int invoke;
	unsigned int dialog;
	unsigned int seq;
	unsigned int length;
	unsigned char* pData;
} NIF_MSG_UNIT2;

typedef struct {
    char mdn[32];
} AddUser;
#pragma pack()


int main(int argc, char **argv)
{
	int sockfd, len;
	struct sockaddr_in dest;
	char buffer[MAXBUF + 1];
	if (argc != 3) 
	{
		printf("args error！correct usage is：\n\t\t%s ip  port\n\tfor example:\t%s 127.0.0.1 80\nthis program\
			receive from ip port MAXBUF Bytes for most",
		argv[0], argv[0]);
		exit(0);
	}
	
	/* create a socket for tcp */
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{
		perror("Socket");
		exit(errno);
	}
	
	printf("socket created\n");
	/* initialize peer ip and port */
	bzero(&dest, sizeof(dest));
	dest.sin_family = AF_INET;
	dest.sin_port = htons(atoi(argv[2]));
	if (inet_aton(argv[1], (struct in_addr *) &dest.sin_addr.s_addr) == 0) 
	{
		perror(argv[1]);
		exit(errno);
	}
	
	printf("address created\n");
	/* 连接服务器 */
	if (connect(sockfd, (struct sockaddr *) &dest, sizeof(dest)) != 0)
	{
		perror("Connect ");
		exit(errno);
	}
	
	while(1){
		char buf[1000];
		NIF_MSG_UNIT2* testMsg=(NIF_MSG_UNIT2*)buf;
		AddUser user;
		char msisdn[]={0x18,0x01,0x93,0x98,0x63,0x90};
		memcpy(user.mdn, msisdn, strlen(msisdn));

		printf("sizeof(msg_body) is %u\n", sizeof(user));
		printf("sizeof(NIF_MSG_UNIT2) is %u\n", sizeof(NIF_MSG_UNIT2));
		printf("sizeof(unsigned char *) is %u\n", sizeof(unsigned char *));
		
		testMsg->head=htonl(0x1a2b3c4d);
		testMsg->dIpAdrs=htonl(0xdddddd);
		testMsg->sIpAdrs=htonl(0xaaaaaa);
		testMsg->version=htonl(0x1);
		testMsg->invoke=htonl(0XEEEEEE05);
		testMsg->dialog=htonl(0x3);
		testMsg->seq=htonl(0x123456);
		testMsg->length=htonl(sizeof(user));
		char* p_user =(char*)user;
		memcpy(buf+sizeof(NIF_MSG_UNIT2)-8, p_user, sizeof(user));
		
		
	
		printf("server connected\n");
		bzero(buffer, MAXBUF + 1);
	
		memcpy(buffer, buf, sizeof(buf));
		/* 发消息给服务器 */

		len = send(sockfd, buffer, sizeof(NIF_MSG_UNIT2)-8+sizeof(user), 0);
		if(len < 0) printf("msg'%s send fail！error code is %d，error info is '%s'\n", buffer,
		errno, strerror(errno));
		else printf("msg'%s send success，sent %d Bytes！\n", buffer, len);
			
		sleep(1);	
		
	}	
	/* 关闭连接 */
	close(sockfd);

	return 0;
}
