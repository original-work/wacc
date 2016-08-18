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
 *         Author:  wangxx
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


bool LogicReqServThread::StrToBCD(const char *Src,unsigned char *Des,int iDesLen)
{
	if (NULL == Src)
	{
		return false;
	}
	if (NULL == Des)
	{
		return false;
	}
	if (0 == iDesLen)
	{
		return false;
	}
	int iSrcLen = strlen(Src);
	if(iSrcLen==0)
	{
		memset(Des,0xFF,8);
		return true;
	}
	if (iSrcLen > iDesLen * 2)
	{
		return false;
	}
	unsigned char chTemp = 0;
	int i;
	for (i = 0; i < iSrcLen; i++)
	{
		if (i % 2 == 0)
		{
			chTemp = (Src[i]-'0'); // << 4) & 0xF0;
		}
		else
		{
			chTemp = chTemp | (((Src[i]-'0')<<4) & 0xF0);
			Des[i / 2] = chTemp;
		}
	}
	if (i % 2 != 0)
	{
		Des[i / 2] = (chTemp|0xf0);
	}
	return true;
}


int LogicReqServThread::loop_process()
{
	//CommonLogger::instance().log_info("Enter into LogicReqServThread loop_process");
	deal_app_req_queue();

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
									CommonLogger::instance().log_info("[%s %d] LogicReqServThread: Recv a ACK  msg 0x%08x", __FILE__,__LINE__,ntohl(unit->invoke));
									deal_ack_req(ntohl(unit->invoke), (data_buf_ + sizeof(NIF_MSG_UNIT) - sizeof(unsigned char*)), ntohl(unit->length));
									break;
								case SERVLOGIC_USER_SYNC_REQ:
								case SERVLOGIC_DEACTIVATE_REQ:
									CommonLogger::instance().log_info("[%s %d] LogicReqServThread: Recv a ACK  msg 0x%08x", __FILE__,__LINE__,ntohl(unit->invoke));
									break;
								case SERVLOGIC_MH_MT_REQ:
									CommonLogger::instance().log_info("LogicReqServThread: Recv a MT req msg");
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
		/* Add User*/
		if (req->msg_type == 1)	// add user
		{
			ActivateMsg *active = (ActivateMsg*)req->msg;

			NIF_MSG_UNIT *unit = (NIF_MSG_UNIT*)send_buf;
			unit->head = htonl(0x1a2b3c4d);
			unit->dialog = htonl(BEGIN);
			unit->invoke = htonl(SERVLOGIC_LOCREQ_REQ);
			unit->length = htonl(sizeof(LocreqData));
			LocreqData body;
			body.tid=htonl(active->tid);
			body.mod_id=active->mod_id;
			CommonLogger::instance().log_debug("deal_app_req_queue: Deal ACTIVE MSG. tid=%u mod_id=%u", active->tid,body.mod_id);
			memset(body.msisdn,0,sizeof(body.msisdn));
			memcpy(body.msisdn, active->msisdn, strlen(active->msisdn));
			
			memcpy((send_buf + sizeof(NIF_MSG_UNIT) - sizeof(unsigned char*)), (char*)&body, sizeof(LocreqData));
			int send_len =  sizeof(NIF_MSG_UNIT) - sizeof(unsigned char*) + sizeof(LocreqData);

			int n = client_list_.size();
			int i = 0;
			#if 0
			/* 新用户*/
			if (active->actived == 0)
			#endif
			{
				add_user_req_.insert(pair<unsigned int, char*>(active->tid, (char*)active));

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

				LogicConnInfo *info = info_mgr_->logic_conns_mgr_.logic_conns();
				int info_cnt = info_mgr_->logic_conns_mgr_.conns_counts();
				for (int n = 0; n < info_cnt; ++n)
				{
					active->user_info->reconnect_cnt_list[n] = info[n].reconnect_cnt;
				}
			}
			#if 0
			else
			{
				/* 已存储激活用户，回复成功响应*/

				//todo 向logic_resp_queue_ 消息队列插入成功响应
				RespMsg resp;
				resp.msg_type = 8;
				AckMsg *ack = (AckMsg*)resp.msg;
				ReqMsg* p = (ReqMsg*)pmsg;
				ActivateMsg* record = (ActivateMsg*)p->msg;
				
				ack->tid = record->tid;
				CommonLogger::instance().log_debug("deal_app_req_queue: Deal ACTIVE MSG, already activated, tid=%u.",ack->tid);
				ack->msg_type = ADD_USER;
				ack->result= 2;/*0,success   1,fail   2,already activated*/

				logic_resp_queue_->insert_record((char*)&resp, sizeof(RespMsg));
				logic_resp_queue_->advance_widx();
			}
			#endif
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
			SMSData *mo = (SMSData*)req->msg;
			NIF_MSG_UNIT *unit = (NIF_MSG_UNIT*)send_buf;
			unit->dialog = htonl(BEGIN);
			unit->invoke = htonl(SERVLOGIC_MH_MO_REQ);
			unit->length = htonl(sizeof(LogicMOData));

			CommonLogger::instance().log_info("deal_app_req_queue: Deal MO MSG. tid is %u", mo->tid);

			LogicMOData *logic_mo = (LogicMOData*)(send_buf + sizeof(NIF_MSG_UNIT) - sizeof(unsigned char*));
			logic_mo->tid = htonl(mo->tid);

			memcpy(logic_mo->cd, mo->cd, strlen(mo->cd));
			memcpy(logic_mo->cg, mo->cg, strlen(mo->cg));
			logic_mo->content_len = htonl(mo->content_len);
			CommonLogger::instance().log_debug("deal_app_req_queue: mo->content_len:%u",mo->content_len);
			memcpy(logic_mo->sms_content, mo->sms_content, mo->content_len);
			logic_mo->sms_code = mo->sms_code;
			CommonLogger::instance().log_debug("deal_app_req_queue: sms_code is %u",mo->sms_code);
			CommonLogger::instance().log_debug("deal_app_req_queue: mo cg len:%d, num:%s",strlen(mo->cg),mo->cg);
			CommonLogger::instance().log_debug("deal_app_req_queue: logic-mo cg len:%d, num:%s",strlen(logic_mo->cg),logic_mo->cg);
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
		else if (req->msg_type == 5) //PING
		{
			CommonLogger::instance().log_info("deal_app_req_queue: Deal PING MSG.");

			RespMsg resp;
			resp.msg_type = 5;
			AckMsg *ack = (AckMsg*)resp.msg;
			ack->msg_type = PING;
			
			logic_resp_queue_->insert_record((char*)&resp, sizeof(RespMsg));
			logic_resp_queue_->advance_widx();
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
	int rsCode = 1;//0 成功1 失败

	
	/* 如果esn, imsi 中任意字段为空，则返回失败*/
	if(strlen(ack->esn)==0||strlen(ack->imsi)==0){
		
		CommonLogger::instance().log_debug("[%s %d] deal_locreq_ack: esn or imsi in locreq ack is null .", __FILE__,__LINE__);

		/*虽然可能没有tid，还是假装删除下*/
		info_mgr_->remove_tid_msisdn(ack->tid);
		//todo 向logic_resp_queue_ 消息队列插入失败响应
		
		RespMsg resp;
		resp.msg_type = 8;
		AckMsg *body = (AckMsg*)resp.msg;
		body->tid=ack->tid;
		body->msg_type=ADD_USER;
		body->result=1;
		map<unsigned int, char*>::iterator  iter = add_user_req_.find(ack->tid);
		if(iter != add_user_req_.end())
		{
			ActivateMsg* ActReq = (ActivateMsg*)iter->second;
			memcpy(body->cd,ActReq->msisdn,strlen(ActReq->msisdn));
		}

		logic_resp_queue_->insert_record((char*)&resp, sizeof(RespMsg));
		logic_resp_queue_->advance_widx();
		return rsCode;

	}
	else{
			char send_buf[3000] = {0};
			CommonLogger::instance().log_debug("deal_locreq_ack: Deal ACTIVE MSG. tid=%u", ntohl(ack->tid));
			map<unsigned int, char*>::iterator  iter = add_user_req_.find(ntohl(ack->tid));
			if(iter != add_user_req_.end())
			{
				ActivateMsg* ActReq = (ActivateMsg*)iter->second;
			       CommonLogger::instance().log_debug("deal_locreq_ack: Find mdn %s.", ActReq->msisdn);

				if(1==ack->nResult){
/*当收到来自业务逻辑模块的周期性位置登记失败响应消息时，
重新要求业务逻辑模块发起LOCREQ请求*/	

					CommonLogger::instance().log_debug("deal_locreq_ack: locreq fail, ask ServiceLogic do LOCREQ again %s.", ActReq->msisdn);
					
					NIF_MSG_UNIT *unit = (NIF_MSG_UNIT*)send_buf;
					unit->head = htonl(0x1a2b3c4d);
					unit->dialog = htonl(BEGIN);
					unit->invoke = htonl(SERVLOGIC_LOCREQ_REQ);
					unit->length = htonl(sizeof(LocreqData));
					
					LocreqData body;
					body.tid=htonl(ack->tid);
					body.mod_id=ActReq->mod_id;
					CommonLogger::instance().log_debug("deal_locreq_ack: locreq fail, ask ServiceLogic do LOCREQ again mod_id %u.", ActReq->msisdn, body.mod_id);
					memcpy(body.msisdn, ActReq->msisdn, strlen(ActReq->msisdn));
					
					memcpy((send_buf + sizeof(NIF_MSG_UNIT) - sizeof(unsigned char*)), (char*)&body, sizeof(LocreqData));
					int send_len =  sizeof(NIF_MSG_UNIT) - sizeof(unsigned char*) + sizeof(LocreqData);
					
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
					
					return rsCode;
				}

				memset(bcd_buf_,0,sizeof(bcd_buf_));
				StrToBCD(ActReq->msisdn, bcd_buf_, sizeof(bcd_buf_));
				
				ActiveUser* user = (ActiveUser*)info_mgr_->active_usr_table_.find_num((char*)bcd_buf_, strlen(ActReq->msisdn));
				if (user != NULL)
				{
					CommonLogger::instance().log_debug("[%s %d] deal_locreq_ack: info_mgr_->active_usr_table_.find_num %s success.", __FILE__,__LINE__,ActReq->msisdn );
					memcpy(user->imsi, ack->imsi, strlen(ack->imsi));
					memcpy(user->esn, ack->esn, strlen(ack->esn));

					NIF_MSG_UNIT *unit = (NIF_MSG_UNIT*)send_buf;
					unit->dialog = htonl(BEGIN);
					unit->invoke = htonl(SERVLOGIC_ACTIVATE_REQ);
					unit->length = htonl(sizeof(PeriodData));
					PeriodData body;
					memset(body.imsi,0,sizeof(body.imsi));
					memset(body.msisdn,0,sizeof(body.msisdn));
					memset(body.esn,0,sizeof(body.esn));
					body.tid=htonl(ActReq->tid);
					body.mod_id=ActReq->mod_id;
					CommonLogger::instance().log_debug("[%s %d] deal_locreq_ack: mod_id %u.", __FILE__,__LINE__,body.mod_id);
					memcpy(body.imsi,ack->imsi, strlen(ack->imsi));
					memcpy(body.msisdn,ActReq->msisdn, strlen(ActReq->msisdn));
					memcpy(body.esn,ack->esn, strlen(ack->esn));
					memcpy((send_buf + sizeof(NIF_MSG_UNIT) - sizeof(unsigned char*)), (char*)&body, sizeof(PeriodData));
					int send_len =  sizeof(NIF_MSG_UNIT) - sizeof(unsigned char*) + sizeof(PeriodData);

					int n = client_list_.size();
					int i = 0;
					/* 新用户*/

					if (ActReq->actived == 0)
					{
						CommonLogger::instance().log_debug("deal_locreq_ack: New user.");
						for (; i < n; ++i)
						{
							if (client_list_[i].connected())
							{
								int r = client_list_[i].send_data(send_buf, send_len);
								if (r < send_len || r == -1)
								{
									client_list_[i].disconnect_to_server();
								}
								CommonLogger::instance().log_debug("deal_locreq_ack: Send Active Msg to first connected socket(servicelogic modle), index=%d",i);
								/* luchq add for test */
								CommonLogger::instance().log_debug("[%s %d] deal_locreq_ack: user msisdn 	%s  esn  %s  imsi  %s ",
									__FILE__,__LINE__,user->msisdn, user->esn, user->imsi);
								++i;
								break;
							}
						}

						unit->invoke = htonl(SERVLOGIC_USER_SYNC_REQ);
						CommonLogger::instance().log_debug("deal_locreq_ack:  Sync user info to other socket(servicelogic modle)");

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
							ActReq->user_info->reconnect_cnt_list[n] = info[n].reconnect_cnt;
						}
#if 0
						/*todo 向logic_resp_queue_ 消息队列插入成功响应*/
						
						RespMsg resp;
						resp.msg_type = 8;
						AckMsg *ack = (AckMsg*)resp.msg;

						ack->tid = ntohl(*((unsigned int*)(data+sizeof(unsigned int))));
						ack->msg_type = ADD_USER;
						ack->result= 0;

						logic_resp_queue_->insert_record((char*)&resp, sizeof(RespMsg));
						logic_resp_queue_->advance_widx();
#endif						
						
						rsCode=0;
						return rsCode;
						
					}
					else
					{
						/* 已存储激活用户，不可能走到这里*/
						CommonLogger::instance().log_debug("deal_locreq_ack: Error, in't possible to go here.");
						return rsCode;
					}
				}
				else{
					/*内存数据库中没有这个mdn，删除数据返回失败*/

					CommonLogger::instance().log_debug("[%s %d] deal_locreq_ack: no mdn %s in info_mgr_->active_usr_table_.", __FILE__,__LINE__, ActReq->msisdn);
					info_mgr_->remove_tid_msisdn(ack->tid);
					//todo 向logic_resp_queue_ 消息队列插入失败响应
					
					RespMsg resp;
					resp.msg_type = 8;
					AckMsg *body = (AckMsg*)resp.msg;
					body->tid=ack->tid;
					body->msg_type=ADD_USER;
					body->result=1;
					memcpy(body->cd,ack->mdn,strlen(ack->mdn));

					logic_resp_queue_->insert_record((char*)&resp, sizeof(RespMsg));
					logic_resp_queue_->advance_widx();
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
	mt->seq = logic_mt->mod_id;
	mt->tid = ntohl(logic_mt->tid);
	memcpy(mt->cd, logic_mt->cd, strlen(logic_mt->cd));
	memcpy(mt->cg, logic_mt->cg, strlen(logic_mt->cg));
	mt->sms_code = logic_mt->sms_code;
	mt->content_len = ntohl(logic_mt->content_len);
	memcpy(mt->sms_content, logic_mt->sms_content, mt->content_len);

	CommonLogger::instance().log_info("deal_mt_req: MT %s, len %d, tid %u, mod_id %u",
		logic_mt->cd, mt->content_len, mt->tid, logic_mt->mod_id);

	logic_resp_queue_->insert_record((char*)&resp, sizeof(RespMsg));
	logic_resp_queue_->advance_widx();
	return 0;
}		/* -----  end of method LogicReqServThread::deal_mt_req  ----- */

int LogicReqServThread::deal_ack_req(unsigned int type, unsigned char *data, unsigned int len)
{
	RespMsg resp;
	resp.msg_type = 8;
	AckMsg *ack = (AckMsg*)resp.msg;
	ack->result = ntohl(*((unsigned int*)data));
	ack->tid = ntohl(*((unsigned int*)(data+sizeof(unsigned int))));
	if (type == SERVLOGIC_ACTIVATE_REQ)
	{
		ack->msg_type = ADD_USER;
		CommonLogger::instance().log_debug("deal_ack_req: ADD_USER  result is %u tid is %u",ack->result,ack->tid);

	}
	else
	{
		CommonLogger::instance().log_debug("deal_ack_req: MO  result is %u tid is %u",ack->result,ack->tid);
		ack->msg_type = SMS_SEND;
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


int LogicReqServThread::deal_recurrent_regnot()
{
	char send_buf[600] = {0};
	unsigned int num=info_mgr_.active_usr_table_.get_used_num();
	for(unsigned int i=0; i<num; i++){

		 
	}
	return 0;
}


