/*
* =========================================================
*
*       Filename:  signal_collection_server.h
*
*    Description:  建立Socket服务监听
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


#if !defined(_SAUAPP_COMMON_COLLECTIONSERVER_H)
#define _SAUAPP_COMMON_COLLECTIONSERVER_H
#include "tcp_server.h"
#include "base_collection_handler.h"


template < typename T>
class CollectionServer : public TcpServer {
public:
	CollectionServer();
	CollectionServer(unsigned short port);
	~CollectionServer();
	/* 处理客户端连接 */
	BaseCollectionHandler* deal_accept();
};

#include "collection_server.hpp"

#endif  //_SAUAPP_COMMON_COLLECTIONSERVER_H
