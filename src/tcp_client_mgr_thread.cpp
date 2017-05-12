/*
 * =====================================================================================
 *
 *       Filename:  tcp_client_mgr_thread.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015/3/12 13:06:12
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  wangxx
 *   Organization:  lj
 *
 * =====================================================================================
 * ============================== CHANGE REPORT HISTORY ================================
 * |   VERSION   |   UPDATE DATE   |   UPDATED BY   |   DESCRIPTION OF CHANGE   |*
 
 * =========================== END OF CHANGE REPORT HISTORY ============================
 */
#include "tcp_client_mgr_thread.h"
#include <arpa/inet.h>
#include "usracc_common.h"
#include "usracc_config.h"
#include <string.h>
#include "common_logger.h"

/*
 *--------------------------------------------------------------------------------------
 *       Class:  TcpClientMgrThread
 *      Method:  TcpClientMgrThread
 * Description:  constructor
 *--------------------------------------------------------------------------------------
 */
TcpClientMgrThread::TcpClientMgrThread()
{
}  /* -----  end of method TcpClientMgrThread::TcpClientMgrThread(constructor)  ----- */

TcpClientMgrThread::~TcpClientMgrThread()
{
}

int TcpClientMgrThread::open(void *args)
{
	all_stopped_ = false;

	if (start() == -1)
	{
		return -1;
	}

	return 0;
}		/* -----  end of method TcpClientMgrThread::open  ----- */


int TcpClientMgrThread::svc()
{
	for(;;)
	{
		if (test_cancel_thread() == 1)
		{
			all_stopped_ = true;
			break;
		}
		else
		{
			loop_process();
			sleep(2);
		}
	}
	return 0;
}		/* -----  end of method TcpClientMgrThread::svc  ----- */


int TcpClientMgrThread::stop()
{
	cancel_thread();

	while (!all_stopped_)
	{
		usleep(1);
	}
	return 0;
}		/* -----  end of method TcpClientMgrThread::stop  ----- */


int TcpClientMgrThread::init(vector<TcpClient> *pclients)
{
	pclient_list_ = pclients;

	conn_data_len_ = 0;
	memset(conn_data_, 0, sizeof(conn_data_));

	conn_data_len_ = build_conndata((char*)conn_data_);

	return 0;
}		/* -----  end of method TcpClientMgrThread::init  ----- */


int TcpClientMgrThread::loop_process()
{
	int num = pclient_list_->size();
	for (int i = 0; i < num; ++i)
	{
		if ((*pclient_list_)[i].connected() == false)
		{
			if ((*pclient_list_)[i].connect_to_server() == 0)
			{
				/* wangxx add 2017-05-12 */
				NIF_MSG_UNIT* pointer=(NIF_MSG_UNIT*)conn_data_;
				CommonLogger::instance().log_info("TcpClientMgrThread::loop_process  head    =0x%08x", ntohl(pointer->head));
				CommonLogger::instance().log_info("TcpClientMgrThread::loop_process  invoke =0x%08x", ntohl(pointer->invoke));
				CommonLogger::instance().log_info("TcpClientMgrThread::loop_process  dialog  =0x%08x", ntohl(pointer->dialog));
				CommonLogger::instance().log_info("TcpClientMgrThread::loop_process  send to serviceLogic msg content:");
				tools::print_hex((unsigned char*)(conn_data_,sizeof(conn_data_));
				/* end of wangxx add */
				
				/*everytime acc module set up connecttion with serviceLogic module, acc send SERVLOGIC_CONNECTION_REQ msg to serviceLogic.
				if acc receive SERVLOGIC_CONNECTION_REQ ack from serviceLogic and result=0, acc begin to syndata with serviceLogic*/
				(*pclient_list_)[i].send_data((char*)conn_data_, conn_data_len_);
			}
		}
	}
	return 0;
}		/* -----  end of method TcpClientMgrThread::loop_process  ----- */


int TcpClientMgrThread::reconnect_to_server(TcpClient *client)
{
	client->disconnect_to_server();

	return client->connect_to_server();
}		/* -----  end of method TcpClientMgrThread::reconnect_to_server  ----- */


int TcpClientMgrThread::build_conndata(char *buf)
{
	NIF_MSG_UNIT *unit = (NIF_MSG_UNIT*)(buf);
	unit->head = htonl(NIF_MSG_HEAD);
	unit->invoke = htonl(SERVLOGIC_CONNECTION_REQ);
	unit->dialog = htonl(BEGIN);

	unsigned int len = 0;
	vector<TidParam> tid_list = UsrAccConfig::instance().tid_num_seg_list();
	unsigned int list_num = tid_list.size();
	CommonLogger::instance().log_info(" list_num is %u", list_num);
	for (unsigned int i = 0; i < list_num; ++i)
	{
		ConnData *conn = (ConnData*)(buf + sizeof(NIF_MSG_UNIT) - sizeof(unsigned char*) + i * sizeof(ConnData));
		//ConnData *conn = (ConnData *)unit->pData;
		
		conn->min_tid = htonl(tid_list[i].min_tid);
		conn->max_tid = htonl(tid_list[i].max_tid);
		conn->mod_id = UsrAccConfig::instance().module_id();
		
		CommonLogger::instance().log_info(" min_tid is %u", tid_list[i].min_tid);
		CommonLogger::instance().log_info(" max_tid is %u", tid_list[i].max_tid);
		CommonLogger::instance().log_info(" mod_id is %u", conn->mod_id);

		len += sizeof(ConnData);
		CommonLogger::instance().log_info(" len is %u", len);
	}

	unit->length = htonl(len);

	return (len + sizeof(NIF_MSG_UNIT) - sizeof(unsigned char*));
}		/* -----  end of method TcpClientMgrThread::build_conndata  ----- */

