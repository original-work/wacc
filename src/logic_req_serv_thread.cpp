/*
 * =====================================================================================
 *
 *       Filename:  logic_req_serv_thread.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2015/3/12 15:58:07
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zhanghl (zhl), zhanghl@liangjiang.com
 *   Organization:  lj
 *
 * =====================================================================================
 * ============================== CHANGE REPORT HISTORY ================================
 * |   VERSION   |   UPDATE DATE   |   UPDATED BY   |   DESCRIPTION OF CHANGE   |*

 * =========================== END OF CHANGE REPORT HISTORY ============================
 */
#include "logic_req_serv_thread.h"
#include <sys/select.h>
#include "usracc_common.h"
#include "usracc_config.h"
#include "sig_analysis_info_shm_manager.h"
#include "errno.h"
#include "common_logger.h"


/*
 *--------------------------------------------------------------------------------------
 *       Class:  LogicReqServThread
 *      Method:  LogicReqServThread
 * Description:  constructor
 *--------------------------------------------------------------------------------------
 */
LogicReqServThread::LogicReqServThread()
{
}  /* -----  end of method LogicReqServThread::LogicReqServThread(constructor)  ----- */

LogicReqServThread::~LogicReqServThread()
{
}

int LogicReqServThread::open(void *args)
{
	all_stopped_ = false;

	if (start() == -1)
	{
		return -1;
	}
	return 0;
}		/* -----  end of method LogicReqServThread::open  ----- */

int LogicReqServThread::svc()
{
	timer_.begin_timer();

	for (;;)
	{
		if (test_cancel_thread() == 1)
		{
			int n = client_list_.size();
			for (int i=0; i<n; ++i)
			{
				if(client_list_[i].connected())
				{
					client_list_[i].disconnect_to_server();
				}
			}
			all_stopped_ = true;
			break;
		}
		else
		{
			loop_process();

			if (timer_.expired())
			{
				deal_heartbeat();
				timer_.reset();
			}

			RunInfo *info = (RunInfo*)SigAnalysisInfoShmManager::instance().get_run_info();
			info->app_req_queue_used = app_req_queue_->block_num() - app_req_queue_->unused_block_num();
			info->logic_resp_queue_used = logic_resp_queue_->block_num() - logic_resp_queue_->unused_block_num();
			info->active_user_count = info_mgr_->active_usr_table_.list_table()->list_head()->used_count;

		}
	}
	return 0;
}		/* -----  end of method LogicReqServThread::svc  ----- */


int LogicReqServThread::stop()
{
	cancel_thread();

	while(!all_stopped_)
	{
		usleep(1);
	}
	return 0;
}		/* -----  end of method LogicReqServThread::stop  ----- */

int LogicReqServThread::init(InfoMemMgr *info_mgr, MsgList *app_queue, MsgList *logic_queue)
{
	timer_.time_interval(UsrAccConfig::instance().heartbeat_timeinterval());

	TcpClient client;
	vector<ServerInfo> server_list = UsrAccConfig::instance().serv_logic_server_list();
	int n = server_list.size();
	for (int i = 0; i < n; ++i)
	{
		client.init(server_list[i].ip, server_list[i].port);
		client_list_.push_back(client);
	}

	info_mgr_ = info_mgr;
	app_req_queue_ = app_queue;
	logic_resp_queue_ = logic_queue;

	memset(data_buf_, 0, sizeof(data_buf_));

	client_seq_ = 0;

	return 0;
}		/* -----  end of method LogicReqServThread::init  ----- */

int LogicReqServThread::select_check_fds()
{
	FD_ZERO(&fdset_);

	int max_fd = 0;
	int n = client_list_.size();
	for (int i = 0; i < n; ++i)
	{
		if (client_list_[i].connected())
		{
			FD_SET(client_list_[i].sockfd(), &fdset_);
			if (client_list_[i].sockfd() > max_fd)
			{
				max_fd = client_list_[i].sockfd();
			}
		}
	}

	if (max_fd == 0)
	{
		return 0;
	}

	struct timeval tv = {1,0};
	int ret = select(max_fd+1, &fdset_, 0, 0, &tv);
	if (ret == 0 ) {
	} else if (ret < 0) {
		if (errno == EINTR || errno == EAGAIN) {
		} else {
			return -1;
		}
	} else {
		return  ret;
	}

	return 0;
}		/* -----  end of method LogicReqServThread::select_check_fds  ----- */

int LogicReqServThread::loop_process()
{
	//CommonLogger::instance().log_info("Enter into LogicReqServThread loop_process");
	deal_app_req_queue();

	//unsigned int da[500] = {1a,2b,3c,4d ,c0,a8,01,a5,c0,a8,01,1e,00,00,00,01,ee,ee,ee,02,00,00,00,02,00,00,00,0f, 00,00,00,81};
	//unsigned int da[500] = {0x4d,0x3c,0x2b,0x1a,0xc0,0xa8,0x01,0xa5,0xc0,0xa8,0x01,0x1e,0x00,0x00,0x00,0x01,0x02,0xee,0xee,0xee,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x0f};
	//LogicMTData dd;
	//strcpy(dd.cd,"18616278716");
	//strcpy(dd.sms_content, "1213232");
	//deal_mt_req((unsigned char*)&dd, sizeof(dd));
	//sleep(10);

	int ret;
	if ((ret = select_check_fds()) > 0)
	{
		int n = client_list_.size();
		for (int i = 0; i < n; ++i)
		{
			if (client_list_[i].connected())
			{
				if (FD_ISSET(client_list_[i].sockfd(), &fdset_))
				{
					int r = client_list_[i].recv_data((char*)data_buf_, sizeof(NIF_MSG_UNIT) - sizeof(unsigned char*));
					if (r  == (sizeof(NIF_MSG_UNIT) - sizeof(unsigned char*)))
					{
						NIF_MSG_UNIT *unit = (NIF_MSG_UNIT*)data_buf_;

						//CommonLogger::instance().log_debug("service i %d, n %d, %u, %u", i, n, sizeof(NIF_MSG_UNIT), ntohl(unit->length));
						r = client_list_[i].recv_data((char*)data_buf_ + sizeof(NIF_MSG_UNIT) - sizeof(unsigned char*), ntohl(unit->length));
						if (r == ntohl(unit->length))
						{
							switch (ntohl(unit->invoke))
							{
								case SERVLOGIC_CONNECTION_REQ:
									CommonLogger::instance().log_debug("LogicReqServThread: Recv a CONNECTION msg");
									deal_connack_msg(&client_list_[i], data_buf_ + sizeof(NIF_MSG_UNIT) - sizeof(unsigned char*));
									break;
								case SERVLOGIC_ACTIVATE_REQ:
								case SERVLOGIC_MO_REQ:
									CommonLogger::instance().log_info("LogicReqServThread: Recv a ACK  msg");
									deal_ack_req(ntohl(unit->invoke), (data_buf_ + sizeof(NIF_MSG_UNIT) - sizeof(unsigned char*)), ntohl(unit->length));
									break;
								case SERVLOGIC_USER_SYNC_REQ:
								case SERVLOGIC_DEACTIVATE_REQ:
									break;
								case SERVLOGIC_MT_REQ:
									CommonLogger::instance().log_info("LogicReqServThread: Recv a MT msg");
									deal_mt_req((data_buf_ + sizeof(NIF_MSG_UNIT) - sizeof(unsigned char*)), ntohl(unit->length));
									break;
								case SERVLOGIC_LOCREQ_REQ:
									CommonLogger::instance().log_info("LogicReqServThread: Recv a locreq ACK msg");
									deal_locreq_ack((data_buf_ + sizeof(NIF_MSG_UNIT) - sizeof(unsigned char*)), ntohl(unit->length));
									break;
								default:
									CommonLogger::instance().log_info("LogicReqServThread: unknown msg type from service logic mod");
									break;
							}

							continue;
						}
					}

					client_list_[i].disconnect_to_server();
				}
			}
			CommonLogger::instance().log_error("\r\n\r\n\r\n");
		}
	}
        else
        {
          usleep(5);
        }

	return ret;
}		/* -----  end of method LogicReqServThread::loop_process  ----- */

int LogicReqServThread::deal_app_req_queue()
{
	char *pmsg;
	unsigned int len;
	char send_buf[3000] = {0};
	CommonLogger::instance().log_info("Enter into LogicReqServThread deal_app_req_queue process");
	if (app_req_queue_->get_front_record(pmsg, len))
	{
		//CommonLogger::instance().log_debug("len %u", len);

		ReqMsg *req = (ReqMsg*)pmsg;
		/* �û�����*/
		if (req->msg_type == 1)	// active
		{
			CommonLogger::instance().log_debug("deal_app_req_queue: Deal ACTIVE MSG.");
			ActivateMsg *active = (ActivateMsg*)req->msg;

			NIF_MSG_UNIT *unit = (NIF_MSG_UNIT*)send_buf;
			unit->dialog = htonl(BEGIN);
			unit->invoke = htonl(SERVLOGIC_ACTIVATE_REQ);
			unit->length = htonl(sizeof(PeriodData));
			//*((unsigned int*)req->msg) = htonl(*((unsigned int*)req->msg));
			
			memcpy((send_buf + sizeof(NIF_MSG_UNIT) - sizeof(unsigned char*)), req->msg, sizeof(PeriodData));
			int send_len =  sizeof(NIF_MSG_UNIT) - sizeof(unsigned char*) + sizeof(PeriodData);

			int n = client_list_.size();
			int i = 0;
			/* ���û�*/
			if (active->actived == 0)
			{
				string mdn = active->msisdn;
				add_user_req_.insert(pair<string, char*>(mdn, (char*)active));

				for (; i < n; ++i)
				{
					if (client_list_[i].connected())
					{
						int r = client_list_[i].send_data(send_buf, send_len);
						if (r < send_len || r == -1)
						{
							client_list_[i].disconnect_to_server();
						}
						CommonLogger::instance().log_debug("deal_app_req_queue: Send Active Msg to first connected socket(servicelogic modle), index=%d",i);
						/* luchq add for test */
						CommonLogger::instance().log_debug("[%s %d] deal_app_req_queue: user msisdn 	%s",
							__FILE__,__LINE__,active->msisdn);
						++i;
						break;
					}
				}

				unit->invoke = htonl(SERVLOGIC_LOCREQ_REQ);
				CommonLogger::instance().log_debug("deal_app_req_queue:  Sync user info to other socket(servicelogic modle)");
				/* luchq add for test */
				CommonLogger::instance().log_debug("[%s %d] deal_app_req_queue: user esn:%s imsi:%s msisdn %s",
					__FILE__,__LINE__,active->esn,active->imsi,active->msisdn);
				for (; i < n; ++i)
				{
					if (client_list_[i].connected())
					{
						int r = client_list_[i].send_data(send_buf, send_len);
						if (r < send_len || r == -1)
						{
							client_list_[i].disconnect_to_server();
						}
					}
				}

				LogicConnInfo *info = info_mgr_->logic_conns_mgr_.logic_conns();
				int info_cnt = info_mgr_->logic_conns_mgr_.conns_counts();
				for (int n = 0; n < info_cnt; ++n)
				{
					active->user_info->reconnect_cnt_list[n] = info[n].reconnect_cnt;
				}
			}
			else
			{
				/* �Ѵ洢�����û����ظ��ɹ���Ӧ*/

				//todo ��logic_resp_queue_ ��Ϣ���в���ɹ���Ӧ

			}
		}
		else if (req->msg_type == 2)	//deactive
		{
			CommonLogger::instance().log_debug("deal_app_req_queue: Deal DEACTIVE MSG.");
			/* luchq add for test */
			//CommonLogger::instance().log_debug("deal_app_req_queue: deactive user msisdn %s",req->msg);

			NIF_MSG_UNIT *unit = (NIF_MSG_UNIT*)send_buf;
			unit->dialog = htonl(BEGIN);
			unit->invoke = htonl(SERVLOGIC_DEACTIVATE_REQ);
			unit->length = htonl(sizeof(DeactivateData));
			memcpy((send_buf + sizeof(NIF_MSG_UNIT) - sizeof(unsigned char*)), req->msg, sizeof(DeactivateData));
			int send_len =  sizeof(NIF_MSG_UNIT) - sizeof(unsigned char*) + sizeof(DeactivateData);

			int n = client_list_.size();
			for (int i = 0; i < n; ++i)
			{
				if (client_list_[i].connected())
				{
					int r = client_list_[i].send_data(send_buf, send_len);
					if (r < send_len || r == -1)
					{
						client_list_[i].disconnect_to_server();
					}
				}
			}
		}
		else if (req->msg_type == 3)	//MO
		{
			CommonLogger::instance().log_info("deal_app_req_queue: Deal MO MSG.");

			SMSData *mo = (SMSData*)req->msg;
			NIF_MSG_UNIT *unit = (NIF_MSG_UNIT*)send_buf;
			unit->dialog = htonl(BEGIN);
			unit->invoke = htonl(SERVLOGIC_MO_REQ);
			unit->length = htonl(sizeof(LogicMOData));

			LogicMOData *logic_mo = (LogicMOData*)(send_buf + sizeof(NIF_MSG_UNIT) - sizeof(unsigned char*));
			logic_mo->tid = htonl(mo->tid);

			memcpy(logic_mo->cd, mo->cd, strlen(mo->cd));
			memcpy(logic_mo->cg, mo->cg, strlen(mo->cg));
			memcpy(logic_mo->sms_center, mo->sms_center, strlen(mo->sms_center));
			memcpy(logic_mo->sms_content, mo->sms_content, strlen(mo->sms_content));
			logic_mo->sms_code = mo->sms_code;
			CommonLogger::instance().log_info("deal_app_req_queue: mo cg len:%d, num:%s",strlen(mo->cg),mo->cg);
			CommonLogger::instance().log_info("deal_app_req_queue: logic-mo cg len:%d, num:%s",strlen(logic_mo->cg),logic_mo->cg);
			int send_len =  sizeof(NIF_MSG_UNIT) - sizeof(unsigned char*) + sizeof(LogicMOData);

			unsigned int n = client_list_.size();
			unsigned int tmp_seq = client_seq_;
			while (!client_list_[client_seq_].connected())
			{
				++client_seq_;
				if (client_seq_ >= n)
				{
					client_seq_ = 0;
				}

				if (client_seq_ == tmp_seq)
				{
					app_req_queue_->advance_ridx();
					return -1;
				}
			}

			int r =client_list_[client_seq_].send_data(send_buf, send_len);
			if (r < send_len || r == -1)
			{
				client_list_[client_seq_].disconnect_to_server();
			}

			++client_seq_;
			if (client_seq_ >= n)
			{
				client_seq_ = 0;
			}
		}
		else if (req->msg_type == 9) //mtack
		{
			CommonLogger::instance().log_info("deal_app_req_queue: Deal MT-ACK MSG.");

			MTAckMsg *mtack = (MTAckMsg*)req->msg;
			NIF_MSG_UNIT *unit = (NIF_MSG_UNIT*)send_buf;
			unit->dialog = htonl(END);
			unit->invoke = htonl(SERVLOGIC_MT_REQ);
			unit->length = htonl(sizeof(MTAckData));

			CommonLogger::instance().log_info("deal_app_req_queue: result=%d, tid=%d",mtack->result,mtack->tid);
			MTAckData *logic_mtack = (MTAckData*)(send_buf + sizeof(NIF_MSG_UNIT) - sizeof(unsigned char*));
			logic_mtack->tid = htonl(mtack->tid);

			memcpy(logic_mtack->cd, mtack->cd, strlen(mtack->cd));
			logic_mtack->result = htonl(mtack->result);

			int send_len =  sizeof(NIF_MSG_UNIT) - sizeof(unsigned char*) + sizeof(MTAckData);

			LogicConnInfo *conninfo = info_mgr_->logic_conns_mgr_.get_conn_info(mtack->seq);
			if (conninfo != NULL && conninfo->used == 1 && conninfo->client->connected())
			{
				int r = conninfo->client->send_data(send_buf, send_len);
				if (r < send_len || r == -1)
				{
					conninfo->client->disconnect_to_server();
				}
			}
		}

		CommonLogger::instance().log_error("\r\n\r\n\r\n");

		app_req_queue_->advance_ridx();
	}
	else
	{
		usleep(1);
	}
	return 0;
}

int LogicReqServThread::send_msg(int fd, const char *buf, const unsigned int &length)
{
	unsigned int need_send_length = length;
	int sended_length = 0;
	while (need_send_length > 0) {
		int len = write(fd, buf+sended_length, need_send_length);
		int error = errno;
		if (len < 0 && error == EINTR) {
			len = 0;
		} else if (len < 0 && error == EAGAIN) {
			usleep(1000);
			len = 0;
		} else if (len < 0) {
			return -1;
		} else if (len == 0) {
			break;
		}

		sended_length+=len;
		need_send_length-=len;
	}
	return sended_length;
}		/* -----  end of method LogicReqServThread::send_msg  ----- */

int LogicReqServThread::recv_msg(int fd, char *buf, const unsigned int &length)
{
	unsigned int need_recv_length = length;
	int recved_length = 0;
	while (need_recv_length > 0) {
		int len = read(fd, buf+recved_length, need_recv_length);
		int error = errno;
		if (len < 0 && error == EINTR) {
			len = 0;
		} else if (len < 0 && error == EAGAIN) {
			usleep(1000);
			len = 0;
		} else if (len<0) {
			return -1;
		} if (len == 0) {
			break;
		}

		recved_length += len;
		need_recv_length -= len;
	}
	return recved_length;
}		/* -----  end of method LogicReqServThread::recv_msg  ----- */

int LogicReqServThread::deal_connack_msg(TcpClient *client, unsigned char *data)
{
	ConnAck *conn = (ConnAck *)data;

	CommonLogger::instance().log_debug("deal_connack_msg: service logic module id %d, result %d", conn->mod_id, ntohl(conn->result));

	info_mgr_->logic_conns_mgr_.update_logic_conninfo(conn->mod_id, client);

	return 0;
}		/* -----  end of method LogicReqServThread::deal_connack_msg  ----- */

int LogicReqServThread::deal_locreq_ack(unsigned char *data, unsigned int len)
{
	LocReqAck *ack = (LocReqAck*)data;
	int rsCode = 1;//0 �ɹ�1 ʧ��

	/* ���esn, imsi, mdn �������ֶ�Ϊ�գ��򷵻�ʧ��*/
	if(strlen(ack->esn)==0||strlen(ack->imsi)==0||strlen(ack->mdn)==0){
		
		info_mgr_->remove_tid_msisdn(ack->tid);
		return rsCode;
		//todo ��logic_resp_queue_ ��Ϣ���в���ʧ����Ӧ

	}
	else{
			char send_buf[3000] = {0};
			CommonLogger::instance().log_debug("deal_locreq_ack: Deal ACTIVE MSG.");
			map<string, char*>::iterator  iter = add_user_req_.find(ack->mdn);
			if(iter != add_user_req_.end())
			{
				ActivateMsg* msg = (ActivateMsg*)iter->second;
			       CommonLogger::instance().log_debug("deal_locreq_ack: Find mdn %s.", msg->msisdn);
				ActiveUser* user = (ActiveUser*)info_mgr_->active_usr_table_.find_num(msg->msisdn, strlen(msg->msisdn));
				memcpy(user->imsi, ack->imsi, strlen(ack->imsi));
				memcpy(user->esn, ack->esn, strlen(ack->esn));


				NIF_MSG_UNIT *unit = (NIF_MSG_UNIT*)send_buf;
				unit->dialog = htonl(BEGIN);
				unit->invoke = htonl(SERVLOGIC_ACTIVATE_REQ);
				unit->length = htonl(sizeof(PeriodData));
				memcpy((send_buf + sizeof(NIF_MSG_UNIT) - sizeof(unsigned char*)), msg, sizeof(PeriodData));
				int send_len =  sizeof(NIF_MSG_UNIT) - sizeof(unsigned char*) + sizeof(PeriodData);

				int n = client_list_.size();
				int i = 0;
				/* ���û�*/

				if (msg->actived == 0)
				{
					for (; i < n; ++i)
					{
						if (client_list_[i].connected())
						{
							int r = client_list_[i].send_data(send_buf, send_len);
							if (r < send_len || r == -1)
							{
								client_list_[i].disconnect_to_server();
							}
							CommonLogger::instance().log_debug("deal_app_req_queue: Send Active Msg to first connected socket(servicelogic modle), index=%d",i);
							/* luchq add for test */
							CommonLogger::instance().log_debug("[%s %d] deal_app_req_queue: user msisdn 	%s  esn  %s  imsi  %s ",
								__FILE__,__LINE__,msg->msisdn, msg->esn, msg->imsi);
							++i;
							break;
						}
					}

					unit->invoke = htonl(SERVLOGIC_USER_SYNC_REQ);
					CommonLogger::instance().log_debug("deal_app_req_queue:  Sync user info to other socket(servicelogic modle)");

					for (; i < n; ++i)
					{
						if (client_list_[i].connected())
						{
							int r = client_list_[i].send_data(send_buf, send_len);
							if (r < send_len || r == -1)
							{
								client_list_[i].disconnect_to_server();
							}
						}
					}

					LogicConnInfo *info = info_mgr_->logic_conns_mgr_.logic_conns();
					int info_cnt = info_mgr_->logic_conns_mgr_.conns_counts();
					for (int n = 0; n < info_cnt; ++n)
					{
						msg->user_info->reconnect_cnt_list[n] = info[n].reconnect_cnt;
					}

					//todo ��logic_resp_queue_ ��Ϣ���в���ɹ���Ӧ
					rsCode=0;
					return rsCode;
					
				}
				else
				{
					/* �Ѵ洢�����û����������ߵ�����*/
					CommonLogger::instance().log_debug("deal_locreq_ack: Error, in't possible to go here.");
					return rsCode;
				}
			}
			else
			{
			       CommonLogger::instance().log_debug("deal_locreq_ack: Not find mdn %s.", ack->mdn);
				 return rsCode;
			}
	}
}

int LogicReqServThread::deal_mt_req(unsigned char *data, unsigned int len)
{
	RespMsg resp;
	resp.msg_type = 4;
	MTMsg *mt = (MTMsg*)resp.msg;
	memset(mt,0,sizeof(MTMsg));
	LogicMTData *logic_mt = (LogicMTData*)data;
	mt->tid = ntohl(logic_mt->tid);
	mt->sms_code = logic_mt->sms_code;
	mt->seq = logic_mt->mod_id;

	memcpy(mt->cd, logic_mt->cd, strlen(logic_mt->cd));
	memcpy(mt->cg, logic_mt->cg, strlen(logic_mt->cg));
	memcpy(mt->sms_center, logic_mt->sms_center, strlen(logic_mt->sms_center));
	memcpy(mt->sms_content, logic_mt->sms_content, strlen(logic_mt->sms_content));

	CommonLogger::instance().log_info("deal_mt_req: MT %s, len %d, tid %u",
		logic_mt->cd, strlen(logic_mt->sms_content), mt->tid);

	logic_resp_queue_->insert_record((char*)&resp, sizeof(RespMsg));
	logic_resp_queue_->advance_widx();
	return 0;
}		/* -----  end of method LogicReqServThread::deal_mt_req  ----- */

int LogicReqServThread::deal_ack_req(unsigned int type, unsigned char *data, unsigned int len)
{
	RespMsg resp;
	resp.msg_type = 5;
	AckMsg *ack = (AckMsg*)resp.msg;
	ack->result = ntohl(*((unsigned int*)data));
	ack->tid = ntohl(*((unsigned int*)(data+sizeof(unsigned int))));
	if (type == SERVLOGIC_ACTIVATE_REQ)
	{
		ack->msg_type = ACTIVE_REQ;
	}
	else
	{
		ack->msg_type = SEND_SMS_REQ;
	}

	logic_resp_queue_->insert_record((char*)&resp, sizeof(RespMsg));
	logic_resp_queue_->advance_widx();
	return 0;
}		/* -----  end of method LogicReqServThread::deal_ack_req  ----- */

int LogicReqServThread::deal_heartbeat()
{
	char send_buf[256];
	NIF_MSG_UNIT *unit = (NIF_MSG_UNIT*)send_buf;
	unit->dialog = htonl(BEGIN);
	unit->invoke = htonl(SERVLOGIC_HEART_BEAT);
	unit->length = 0;
	int send_len =  sizeof(NIF_MSG_UNIT) - sizeof(unsigned char*);

	int n = client_list_.size();
	for (int i = 0; i < n; ++i)
	{
		if (client_list_[i].connected())
		{
			int r = client_list_[i].send_data(send_buf, send_len);
			if (r < send_len || r == -1)
			{
				client_list_[i].disconnect_to_server();
			}
		}
	}
	return 0;
}
