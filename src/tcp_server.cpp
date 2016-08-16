/*
* =========================================================
*
*       Filename:  tcp_server.cpp
*
*    Description:  TCP/IP连接服务器
*
*        Version:  1.0
*        Created:  2013.6.14
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  wangxx
*   Organization:  lj
*
* ==========================================================
* ================== CHANGE REPORT HISTORY ==================
*|VERSION|UPDATE DATE|UPDATED BY|DESCRIPTION OF CHANGE|*

* ================== END OF CHANGE REPORT HISTORY ===========
*/
#include "tcp_server.h"
#include <fcntl.h>
#include <string.h>


TcpServer::TcpServer() {
	port_ = 18000;
}

TcpServer::TcpServer(unsigned short port) {
	port_=port;
}

TcpServer::~TcpServer() {
}

int TcpServer::open() {

	close_server();  

	if ((sockfd_ = socket(AF_INET, SOCK_STREAM, 0)) < 0) {return -1;}

	int option = 1;
	if (setsockopt (sockfd_, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option) ) < 0) {return -1;}

	if (fcntl(sockfd_, F_SETFL, O_NDELAY) == -1) {return -1;}

	//linger lin = {1, 0};
	//if(setsockopt(sockfd_, SOL_SOCKET,SO_LINGER, (char *)&lin, sizeof(lin)) < 0 ) 
	//	return false;

	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(port_);

	if (bind(sockfd_, (sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) { return -1;}

	if (listen(sockfd_, SOMAXCONN)<0) {return -1;}

	return 0;
}

int TcpServer::close_server() {
	if (sockfd_>0) {
		close(sockfd_);
	}
	return 0;
}
