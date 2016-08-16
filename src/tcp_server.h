/*
* =========================================================
*
*       Filename:  tcp_server.h
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
	/* 建立服务器监听 */
	int open();
	/* 关闭服务器 */
	int close_server();
	void port(const unsigned short &port) {port_=port;}
	unsigned short port() const {return port_;}
	int sockfd() const {return sockfd_;}
private:
	/* 端口号 */
	unsigned short port_;
	/* 监听描述符 */
	int sockfd_;
};

#endif  //_SAUAPP_COMMON_TCPSERVER_H
