/*
* =========================================================
*
*       Filename:  signal_collection_server.cpp
*
*    Description:  建立Socket服务监听
*
*        Version:  1.0
*        Created:  2013.6.14
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  Zhang Huanli (zhanghl@liangjiang.com)
*   Organization:  lj
*
* ==========================================================
* ================== CHANGE REPORT HISTORY ==================
*|VERSION|UPDATE DATE|UPDATED BY|DESCRIPTION OF CHANGE|*

* ================== END OF CHANGE REPORT HISTORY ===========
*/
#include <string.h>
#include <fcntl.h>
#include <stdio.h>


template <typename T>
CollectionServer<T>::CollectionServer() {
}

template <typename T>
CollectionServer<T>::CollectionServer(unsigned short port) : TcpServer(port) {

}

template <typename T>
CollectionServer<T>::~CollectionServer() {
}

template <typename T>
BaseCollectionHandler* CollectionServer<T>::deal_accept() {
	struct sockaddr_in client_addr;
	memset(&client_addr, 0, sizeof(client_addr));
	unsigned int len = sizeof(client_addr);
	int new_sockfd = accept(sockfd(), (sockaddr *)&client_addr, &len);
	if (new_sockfd < 0 ) {
		return NULL;
	}

	if (fcntl(new_sockfd, F_SETFL, O_NDELAY) == -1) {
		close(new_sockfd);
		return NULL;
	}

	BaseCollectionHandler* handler = new T();
	handler->client_ip(client_addr.sin_addr.s_addr);
	handler->client_port(client_addr.sin_port);
	handler->sockfd(new_sockfd);
	return handler;
}
