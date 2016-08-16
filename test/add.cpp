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

typedef struct {
	char cd[32];
	char cg[32];
	char smsCode;
	unsigned int content_len;
	unsigned char content[256];
} MO;
#pragma pack()


void print_hex(char* p_msg, int length)
{
	short i, j, len;
	char buf[2048];
	j   = 0;
	len = 0;
	memset(buf,0,2048);
	for (i = 0; (i<length)&&(i<512); i++)
	{
		sprintf(&buf[len], "%.2X ", *(p_msg + i));
		len += 3;
		j++;
		if (j >= 16)
		{
			buf[len] = '\r';
			len++;
			buf[len]= '\n';
			len++;
			j = 0;
		}
	}

	buf[len] = '\r';
	buf[len + 1] = '\n';
	buf[len + 2] = '\r';
	buf[len + 3] = '\n';
	buf[len + 4] = 0;

	printf("%s",buf);
}



int main(int argc, char **argv)
{
	int sockfd, len;
	struct sockaddr_in dest;
	char buffer[MAXBUF + 1];
	if (argc != 3) 
	{
		printf("args error! correct usage is：\n\t\t%s ip  port\n\tfor example:\t%s 127.0.0.1 80\nthis program\
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
	
	char buf[1000]={0};
	NIF_MSG_UNIT2* testMsg=(NIF_MSG_UNIT2*)buf;
	AddUser user;
	memset(user.mdn,0,sizeof(user.mdn));
	strcpy(user.mdn, "13301605333");

	printf("sizeof(msg_body) is %u\n", sizeof(user));
	printf("sizeof(NIF_MSG_UNIT2) is %u\n", sizeof(NIF_MSG_UNIT2));
	printf("sizeof(unsigned char *) is %u\n", sizeof(unsigned char *));
	
	testMsg->head=htonl(0x1a2b3c4d);
	testMsg->dIpAdrs=htonl(0xdddddd);
	testMsg->sIpAdrs=htonl(0xaaaaaa);
	testMsg->version=htonl(0x1);
	testMsg->invoke=htonl(0XEEEEEE01);
	testMsg->dialog=htonl(0x3);
	testMsg->seq=htonl(0x123456);
	testMsg->length=htonl(sizeof(user));
	char* p_user =(char*)&user;
	memcpy(buf+sizeof(NIF_MSG_UNIT2)-8, p_user, sizeof(AddUser));
	
	

	printf("server connected\n");
	bzero(buffer, MAXBUF + 1);

	memcpy(buffer, buf, sizeof(buf));
	/* 发消息给服务器 */

	len = send(sockfd, buffer, sizeof(NIF_MSG_UNIT2)-8+sizeof(user), 0);
	if(len < 0) {
		printf("send fail!  error code is %d,  error info is '%s'\n", errno, strerror(errno));
	}
	else{
		printf("send success,  sent %d Bytes  \n", len);
		print_hex(buffer, len);
	}
		
	sleep(6);	

	


	MO mo_msg;
	unsigned char data[]="this is a test mo msg!";
	strcpy(mo_msg.cd, "13816154202");
	strcpy(mo_msg.cg, "13301605333");
	mo_msg.smsCode=8;
	memcpy((void*)mo_msg.content,(void*)&data,strlen((char*)data));
	mo_msg.content_len=htonl(strlen((char*)data));

	printf("sizeof(msg_body) is %u\n", sizeof(mo_msg));
	printf("sizeof(NIF_MSG_UNIT2) is %u\n", sizeof(NIF_MSG_UNIT2));
	printf("sizeof(unsigned char *) is %u\n", sizeof(unsigned char *));
	printf("mo_msg.content_len is %u\n", strlen((char*)data));
	
	testMsg->head=htonl(0x1a2b3c4d);
	testMsg->dIpAdrs=htonl(0xdddddd);
	testMsg->sIpAdrs=htonl(0xaaaaaa);
	testMsg->version=htonl(0x1);
	testMsg->invoke=htonl(0XEEEEEE03);
	testMsg->dialog=htonl(0x3);
	testMsg->seq=htonl(0x123456);
	testMsg->length=htonl(sizeof(mo_msg));
	p_user =(char*)&mo_msg;
	memcpy(buf+sizeof(NIF_MSG_UNIT2)-8, p_user, sizeof(mo_msg));
	
	

	printf("server connected\n");
	bzero(buffer, MAXBUF + 1);

	memcpy(buffer, buf, sizeof(buf));
	/* 发消息给服务器 */

	len = send(sockfd, buffer, sizeof(NIF_MSG_UNIT2)-8+sizeof(mo_msg), 0);
	if(len < 0) {
		printf("send fail!  error code is %d,  error info is '%s'\n", errno, strerror(errno));
	}
	else{
		printf("send success,  sent %d Bytes  \n", len);
		print_hex(buffer, len);
	}
		
	sleep(20);	





	
		
	/* 关闭连接 */
	close(sockfd);

	return 0;
}
