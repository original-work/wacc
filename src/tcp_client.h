/*
* =========================================================
*
*       Filename:  tcp_client.h
*
*    Description:  TCP/IP连接客户端
*
*        Version:  1.0
*        Created:  2013.6.14
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  Zhang Huanli (zhanghl@liangjiang.com)
*   Organization:  lj
*
* =========================================================
* ==================== CHANGE REPORT HISTORY ===================
*|VERSION|UPDATE DATE|UPDATED BY|DESCRIPTION OF CHANGE|*

* ==================== END OF CHANGE REPORT HISTORY ============
*/
#if !defined(_SAUAPP_COMMON_TCPCLIENT_H)
#define _SAUAPP_COMMON_TCPCLIENT_H
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>


class TcpClient {
public:
	TcpClient();
	~TcpClient();
	/* 初始化客户端连接的IP地址，端口号 */
	 int init(const std::string &ip, const unsigned int &port);
	 /* 连接到服务器 */
	 int connect_to_server();
	 /* 与服务器断开连接 */
	 int disconnect_to_server();
	 virtual int handle_recv();
	 virtual int handle_send();
	 void ip(const std::string &ip_addr) {ip_=ip_addr;}
	 void port(const unsigned short &port) {port_=port;}
	 int send_data(const char *buf, const unsigned int &length);
	 int recv_data(char *buf, const unsigned int length);
	 std::string ip() const {return ip_;}
	 unsigned int port() const {return port_;}
	 int sockfd() const {return sockfd_;}
	 bool connected() const {return connected_;}
private:
	/* IP地址 */
	std::string ip_;
	/* 端口号 */
	unsigned short port_;
	/* 连接描述符 */
	int sockfd_;
	struct sockaddr_in sock_addr_;
	bool connected_;
};

#endif  //_SAUAPP_COMMON_TCPCLIENT_H
