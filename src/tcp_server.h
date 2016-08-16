/*
* =========================================================
*
*       Filename:  tcp_server.h
*
*    Description:  TCP/IP���ӷ�����
*
*        Version:  1.0
*        Created:  2013.6.14
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  wangxx
*   Organization:  lj
*
* =========================================================
* ==================== CHANGE REPORT HISTORY ===================
*|VERSION|UPDATE DATE|UPDATED BY|DESCRIPTION OF CHANGE|*

* ==================== END OF CHANGE REPORT HISTORY ============
*/

#if !defined(_SAUAPP_COMMON_TCPSERVER_H)
#define _SAUAPP_COMMON_TCPSERVER_H
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

class TcpServer {
public:
	TcpServer();
	TcpServer(unsigned short port);
	~TcpServer();
	/* �������������� */
	int open();
	/* �رշ����� */
	int close_server();
	void port(const unsigned short &port) {port_=port;}
	unsigned short port() const {return port_;}
	int sockfd() const {return sockfd_;}
private:
	/* �˿ں� */
	unsigned short port_;
	/* ���������� */
	int sockfd_;
};

#endif  //_SAUAPP_COMMON_TCPSERVER_H
