/*
* =========================================================
*
*       Filename:  base_collection_handler.h
*
*    Description:  SOCKET服务端，信息接收处理模块
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

#if !defined(_SAUAPP_COMMON_BASECOLLECTIONHANDLER_H)
#define _SAUAPP_COMMON_BASECOLLECTIONHANDLER_H
#include <unistd.h>
#include <errno.h>


class BaseCollectionHandler {
public:
	BaseCollectionHandler();
	virtual ~BaseCollectionHandler();
	/* 接收处理服务请求 */
	virtual int handle_recv() = 0;
	/* 信息处理模块 */
	virtual int process(char *pmsg) = 0;
	void sockfd(int new_sockfd) {sockfd_=new_sockfd;}
	int sockfd() const {return sockfd_;}
	void client_ip(unsigned int ip) {client_ip_=ip;}
	unsigned long client_ip() const {return client_ip_;}
	void client_port(unsigned short port) {client_port_=port;}
	unsigned short client_port() const {return client_port_;}
	int logout(const char *buf, size_t send_size); /* luchq add 2015-10-29 */
protected:
	int recvn(char *buf, size_t recv_size);
	int sendn(const char *buf, size_t send_size);
private:
	/* 客户端连接socket描述符 */
	int sockfd_;
	/* 客户端IP地址 */
	unsigned long client_ip_;
	/* 客户端端口 */
	unsigned short client_port_;
};

#endif  //_SAUAPP_COMMON_BASECOLLECTIONHANDLER_H
