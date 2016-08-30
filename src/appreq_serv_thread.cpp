/*
 * =====================================================================================
 *
 *       Filename:  appreq_serv_thread.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2015/3/9 10:31:31
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
#include "appreq_serv_thread.h"
#include "usracc_config.h"
#include "common_logger.h"
#include "tid_generator.h"
#include <sys/time.h>
#include <time.h>
#include "tools.h"


/*
 *--------------------------------------------------------------------------------------
 *       Class:  AppReqServThread
 *      Method:  AppReqServThread
 * Description:  constructor
 *--------------------------------------------------------------------------------------
 */
AppReqServThread::AppReqServThread() : CMaxEvents(1000)
{
}  /* -----  end of method AppReqServThread::AppReqServThread(constructor)  ----- */

AppReqServThread::~AppReqServThread()
{
}  /* -----  end of method AppReqServThread::~AppReqServThread()  ----- */

int AppReqServThread::open(void *args)
{

	collection_server_.open();

	if (start() == -1)
	{
		return -1;
	}

	return 0;
}		/* -----  end of method AppReqServThread::open  ----- */

int AppReqServThread::svc()
{
	efd_ = epoll_create(CMaxEvents);
	if (efd_ == -1)
	{
		return -1;
	}

	event_.data.fd = collection_server_.sockfd();
	event_.events = EPOLLIN;

	int res = epoll_ctl(efd_, EPOLL_CTL_ADD, collection_server_.sockfd(), &event_);
	if (res == -1)
	{
		return -1;
	}

	events_ = (struct epoll_event*)calloc(CMaxEvents, sizeof(struct epoll_event));

	for (;;) {
		if(test_cancel_thread()==1) {
			close_all_connection();
			free(events_);
			close(efd_);
			all_stopped_ = true;
			break;
		}

		if (loop_process()<0) {
			continue;
		}
	}

	return 0;
}		/* -----  end of method AppReqServThread::svc  ----- */


int AppReqServThread::init(InfoMemMgr *info_mem, MsgList* app_queue, MsgList* logic_queue, MsgList* recurrent_regnot_queue, map<unsigned int, char*>* add_user_req, MySQLConnWrapper* db)
{
	/* 获取服务端监听端口*/
	collection_server_.port(UsrAccConfig::instance().app_serv_port());
	events_ = NULL;
	all_stopped_ = false;

	info_mgr_ = info_mem;
	app_req_queue_ = app_queue;
	logic_resp_queue_ = logic_queue;
	recurrent_regnot_queue_ = recurrent_regnot_queue;
	add_user_req_ = add_user_req;
	db_=db;
	current_connection_count_ = 0;
	connection_count_limits_ = UsrAccConfig::instance().app_max_connection_count();
	TidGenerator::instance().init();
	
	return 0;
}		/* -----  end of method AppReqServThread::init  ----- */


int AppReqServThread::stop()
{
	cancel_thread();

	while(!all_stopped_) {
		usleep(1);
	}

	return 0;
}		/* -----  end of method AppReqServThread::stop  ----- */


int AppReqServThread::my_check_epoll()
{
	int nfds = epoll_wait(efd_, events_, CMaxEvents, 0);

	return nfds;
}		/* -----  end of method AppReqServThread::my_check_epoll  ----- */

int AppReqServThread::loop_process()
{
	//CommonLogger::instance().log_info("Enter into AppReqServThread loop_process");
	deal_logic_resp_queue();
	/* 监听连接*/
	int n = my_check_epoll();

	struct timeval start, end;

	gettimeofday(&start, NULL);

	for (int i=0; i < n; ++i)
	{
		/*检查是否为listen监听FD，是则执行accept建立连接，并生成连接FD */
		if (events_[i].data.fd == collection_server_.sockfd())
		{
			// deal new connection
			CommonLogger::instance().log_info("AppReqServThread: Recv new connection from APP");
			CommonLogger::instance().log_info("AppReqServThread: sockfd:%d",collection_server_.sockfd());
			if (deal_new_connection()<0)
		       {
				return -1;
			}
		}
		/* 不是listen监听FD，此连接已经建立，可以接收消息并处理 */
		else if (events_[i].events & EPOLLIN)
		{
			// handle request
			CommonLogger::instance().log_info("AppReqServThread: Recv request msg from App, fd:%d",events_[i].data.fd );
			mihao_fd_= events_[i].data.fd;
			handle_request(events_[i].data.fd);
		}
	}

	gettimeofday(&end, NULL);
	if (n > 0)
	{
		unsigned int cost = (end.tv_sec - start.tv_sec)*1000000 + (end.tv_usec - start.tv_usec);
		CommonLogger::instance().log_info("AppReqServThread: client num %u, connections %u, cost %u, cost per %f", n, client_list_.size(), cost, ((float)cost)/n);
	}
        else
        {
          usleep(5);
        }

	return 0;
}		/* -----  end of method AppReqServThread::loop_process  ----- */


int AppReqServThread::handle_request(int fd)
{
	BaseCollectionHandler *handler = collection_handler(fd);
	if (handler != NULL)
	{
		if (handler->handle_recv() < 0)
		{
			close_connection(fd);
		}
	}
	return 0;
}		/* -----  end of method AppReqServThread::handle_request  ----- */


int AppReqServThread::deal_new_connection()
{
	if (current_connection_count_ >= connection_count_limits_ && connection_count_limits_ > 0) {

		CommonLogger::instance().log_error("connection limited, cur %u, max %u",
				current_connection_count_,
				connection_count_limits_);
		return 0;
	}

	BaseCollectionHandler* handler = collection_server_.deal_accept();
	if (handler!=NULL) {
		do {
			event_.data.fd = handler->sockfd();
			event_.events = EPOLLIN;
			CommonLogger::instance().log_info("deal_new_connection: new sockfd:%d",handler->sockfd());
			int res = epoll_ctl(efd_, EPOLL_CTL_ADD, handler->sockfd(), &event_);
			if (res == -1)
			{
				close(handler->sockfd());
				delete handler;
				return -1;
			}

			map<int,BaseCollectionHandler*>::iterator it = client_list_.find(handler->sockfd());
			if (it != client_list_.end())
			{
				close(it->first);
				delete it->second;
				client_list_.erase(handler->sockfd());
			}

			client_list_.insert(map<int,BaseCollectionHandler*>::value_type(handler->sockfd(), handler));

			if (connection_count_limits_>0) {
				++current_connection_count_;
			}

			AppReqHandler *h = (AppReqHandler*)handler;
			h->info_mgr(info_mgr_);
			h->app_req_queue(app_req_queue_);
			h->client_list(&client_list_);
			h->db(db_);
			handler = collection_server_.deal_accept();
		} while (handler != NULL);

		return 0;
	}

	return -1;
}		/* -----  end of method AppReqServThread::deal_new_connection  ----- */

int AppReqServThread::deal_logic_resp_queue()
{
	char *pmsg = NULL;
	unsigned int len;
	char send_buf[600]= {0};
	unsigned int sendlen;
	//memset(send_buf,0,sizeof(send_buf));
	//CommonLogger::instance().log_info("Enter into AppReqServThread 	deal_logic_resp_queue process");
	if (logic_resp_queue_->get_front_record(pmsg, len))
	{
		RespMsg *resp = (RespMsg*)pmsg;
		MTMsg *mt = (MTMsg*)resp->msg;
		ActiveUser *user = NULL;
		NIF_MSG_UNIT2 *unit = NULL;
		AckMsg *ack = NULL;
		MTMsg *data = NULL;
		string msisdn;

		//CommonLogger::instance().log_debug("deal_logic_resp_queue### %u", resp->msg_type);
		switch (resp->msg_type)
		{
			case 4:	// MT
			{
				memset(send_buf,0,sizeof(send_buf));
                         	CommonLogger::instance().log_info("deal_logic_resp_queue: Deal MT msg, record len=%d",len);
				CommonLogger::instance().log_info("deal_logic_resp_queue: cd %s, tid %d", mt->cd,mt->tid);
				memset(bcd_buf_,0,sizeof(bcd_buf_));
				StrToBCD(mt->cd, bcd_buf_, sizeof(bcd_buf_));
				user = (ActiveUser*)info_mgr_->active_usr_table_.find_num((char*)bcd_buf_, strlen(mt->cd));
				if (user != NULL)
				{
					CommonLogger::instance().log_info("deal_logic_resp_queue: sms len=%d",mt->content_len);
					unit = (NIF_MSG_UNIT2*)send_buf;
					unit->head = htonl(0x1a2b3c4d);
					unit->invoke = htonl(SMS_PUSH);
					unit->dialog = htonl(BEGIN);
					unit->length = htonl(sizeof(MTMsg)-sizeof(mt->sms_content)+mt->content_len);
					unit->seq=10086;
					data = (MTMsg*)(send_buf + sizeof(NIF_MSG_UNIT2) - sizeof(unsigned char*));
					data->seq = mt->seq;
					data->tid = htonl(mt->tid);
					memcpy(data->cd, mt->cd, strlen(mt->cd));
					memcpy(data->cg, mt->cg, strlen(mt->cg));
					data->sms_code=mt->sms_code;
					data->content_len=htonl(mt->content_len);
					memcpy(data->sms_content, mt->sms_content, mt->content_len);
					if (strcmp(user->msisdn,mt->cd))
					{
						CommonLogger::instance().log_error("deal_logic_resp_queue: Check App number fail, App=%s",user->msisdn);
					}
					sendlen = send_data(user->fd, send_buf, sizeof(NIF_MSG_UNIT2)-sizeof(unsigned char*)+sizeof(MTMsg)-sizeof(mt->sms_content)+mt->content_len);
					if (sendlen != sizeof(NIF_MSG_UNIT2)-sizeof(unsigned char*)+sizeof(MTMsg)-sizeof(mt->sms_content)+mt->content_len)
					{
						CommonLogger::instance().log_info("deal_logic_resp_queue: send MT msg to %s 	FAIL!!! send length:%d", user->msisdn, sendlen);
					}
                                   CommonLogger::instance().log_info("deal_logic_resp_queue: send MT msg to %s, Socket:%d, len:%d, sms_code is %u, seq is %u, tid is %u, cd is %s, cg is %s",
                                	user->msisdn,user->fd,mt->content_len,data->sms_code,data->seq,data->tid,data->cd,data->cg);
					tools::print_hex((unsigned char*)mt->sms_content,mt->content_len);

					/*   向操作表中插入操作记录*/
					db_->prepare("INSERT INTO op_record(create_time, mdn, imsi, esn, opt_code, opt_result, seq) VALUES (?, ?, ?, ?, ?, ?, ?)");
					string now=tools::currentDateTime();
					db_->setString(1,now);
					db_->setString(2,user->msisdn);
					db_->setString(3,user->imsi);
					db_->setString(4,user->esn);
					db_->setString(5,"MT");
					db_->setInt(6,ack->result);
					db_->setInt(7,10086);
					db_->executeUpdate();
				}
				else
					CommonLogger::instance().log_info("deal_logic_resp_queue: Call find_num fail, user maybe not exist");
				break;
			}
			case 6: //NOTIFY_ACTIVE ACK
			{
				memset(send_buf,0,sizeof(send_buf));
				ack = (AckMsg*)resp->msg;
				CommonLogger::instance().log_info("deal_logic_resp_queue: NOTIFY_ACTIVE ACK");
				unit = (NIF_MSG_UNIT2*)send_buf;
				unit->head = htonl(0x1a2b3c4d);
				unit->dialog = htonl(END);
				unit->invoke = htonl(ack->msg_type);

				sendlen = send_data(mihao_fd_, send_buf, sizeof(NIF_MSG_UNIT2)-sizeof(unsigned char*)+sizeof(unsigned int));
				if (sendlen != sizeof(NIF_MSG_UNIT2)-sizeof(unsigned char*)+sizeof(unsigned int))
				{
					CommonLogger::instance().log_error("deal_logic_resp_queue: send NOTIFY_ACTIVE msg to %s FAIL!!!", mt->cd);
				}
				CommonLogger::instance().log_info("deal_logic_resp_queue: send NOTIFY_ACTIVE msg to APP");
				break;
			}
			case 1: // ACTIVATE ACK
			{
				memset(send_buf,0,sizeof(send_buf));
				ack = (AckMsg*)resp->msg;
				CommonLogger::instance().log_info("deal_logic_resp_queue: Deal ACTIVATE ACK msg, tid=%u", ack->tid);
				CommonLogger::instance().log_info("[%s %d] deal_logic_resp_queue: msg_type 0x%08x", __FILE__,__LINE__,ack->msg_type);

				map<unsigned int, char*>::iterator  iter = add_user_req_->find(ack->tid);
				if(iter != add_user_req_->end())
				{
					ActivateMsg* regnot = (ActivateMsg*)iter->second;	
					CommonLogger::instance().log_info("[%s %d] deal_logic_resp_queue: ACTIVATE ACK tid %u found mdn %s seq %u",__FILE__,__LINE__,ack->tid,regnot->msisdn,regnot->seq);
					memset(bcd_buf_,0,sizeof(bcd_buf_));
					StrToBCD(regnot->msisdn, bcd_buf_, sizeof(bcd_buf_));
					user = (ActiveUser*)info_mgr_->active_usr_table_.find_num((char*)bcd_buf_, strlen(regnot->msisdn));
					if (user != NULL)
					{
						unit = (NIF_MSG_UNIT2*)send_buf;
						unit->head = htonl(0x1a2b3c4d);
						unit->dialog = htonl(END);
						unit->invoke = htonl(ack->msg_type);
						unit->seq = htonl(regnot->seq);
						
						if(regnot->recurrent_regnot_flag){/*如果是周期性位置更新*/
							CommonLogger::instance().log_info("[%s %d] deal_logic_resp_queue: deal_recurrent_regnot_ack",__FILE__,__LINE__);
							deal_recurrent_regnot_ack(ack, regnot, user);//added by wangxx 20160818
						}
						else{/*只是普通的开户*/
							unit->length = htonl(sizeof(unsigned int));
							*((unsigned int*)(send_buf + sizeof(NIF_MSG_UNIT2) - sizeof(unsigned char*))) = htonl(ack->result);
							sendlen = send_data(user->fd, send_buf, sizeof(NIF_MSG_UNIT2)-sizeof(unsigned char*)+sizeof(unsigned int));
							if (sendlen != sizeof(NIF_MSG_UNIT2)-sizeof(unsigned char*)+sizeof(unsigned int))
							{
								CommonLogger::instance().log_error("deal_logic_resp_queue: send ACTIVATE ACK msg to %s FAIL!!!", mt->cd);
							}
							CommonLogger::instance().log_info("deal_logic_resp_queue: send ACTIVATE ACK msg to APP");

							/*开户成功则插入mysql*/
							if(0==ack->result){
								/*  用户表*/
								db_->prepare("INSERT INTO active_user(create_time, mdn, imsi, esn, fd) VALUES (?, ?, ?, ?, ?)");
								string now=tools::currentDateTime();
								db_->setString(1,now);
								db_->setString(2,user->msisdn);
								db_->setString(3,user->imsi);
								db_->setString(4,user->esn);
								db_->setInt(5,user->fd);
								db_->executeUpdate();								
							}
							/*   向操作表中插入操作记录*/
							db_->prepare("INSERT INTO op_record(create_time, mdn, imsi, esn, opt_code, opt_result, seq) VALUES (?, ?, ?, ?, ?, ?, ?)");
							string now=tools::currentDateTime();
							db_->setString(1,now);
							db_->setString(2,user->msisdn);
							db_->setString(3,user->imsi);
							db_->setString(4,user->esn);
							db_->setString(5,"ADD");
							db_->setInt(6,ack->result);
							db_->setInt(7,regnot->seq);
							db_->executeUpdate();
						}

					}
					else{
						CommonLogger::instance().log_info("deal_logic_resp_queue: Call find_num fail %s", msisdn.c_str());
					}
				}
				else{
					CommonLogger::instance().log_info("deal_logic_resp_queue: tid %u not found",ack->tid);
				}
				info_mgr_->remove_tid_msisdn(ack->tid);
				break;
			}
			case 8:	// MO ACK
			{
				memset(send_buf,0,sizeof(send_buf));
				ack = (AckMsg*)resp->msg;
				msisdn = info_mgr_->find_msisdn_by_tid(ack->tid);
				CommonLogger::instance().log_info("deal_logic_resp_queue: Deal MO ACK msg, tid=%u", ack->tid);
				CommonLogger::instance().log_info("deal_logic_resp_queue: num %s, result %u", msisdn.c_str(), ack->result);
				CommonLogger::instance().log_info("[%s %d] deal_logic_resp_queue: msg_type 0x%08x", __FILE__,__LINE__,ack->msg_type);
				if (msisdn != "")
				{				
					memset(bcd_buf_,0,sizeof(bcd_buf_));
					StrToBCD(msisdn.c_str(), bcd_buf_, sizeof(bcd_buf_));
					user = (ActiveUser*)info_mgr_->active_usr_table_.find_num((char*)bcd_buf_, msisdn.length());
					if (user != NULL)
					{
						unit = (NIF_MSG_UNIT2*)send_buf;
						unit->head = htonl(0x1a2b3c4d);
						unit->dialog = htonl(END);
						unit->invoke = htonl(ack->msg_type);
						unit->length = htonl(sizeof(unsigned int));
						unit->seq = htonl(info_mgr_->find_seq_by_tid(ack->tid));
						CommonLogger::instance().log_info("deal_logic_resp_queue: MO ACK seq %u", unit->seq);
						*((unsigned int*)(send_buf + sizeof(NIF_MSG_UNIT2) - sizeof(unsigned char*))) = htonl(ack->result);
						sendlen = send_data(user->fd, send_buf, sizeof(NIF_MSG_UNIT2)-sizeof(unsigned char*)+sizeof(unsigned int));
						if (sendlen != sizeof(NIF_MSG_UNIT2)-sizeof(unsigned char*)+sizeof(unsigned int))
						{
							CommonLogger::instance().log_error("deal_logic_resp_queue: send ACK msg to %s FAIL!!!", mt->cd);
						}else{
							CommonLogger::instance().log_info("deal_logic_resp_queue: send ACK msg to APP");
						}
					}
					else{
						CommonLogger::instance().log_info("deal_logic_resp_queue: Call find_num fail");
					}

					/*   向操作表中插入操作记录*/
					db_->prepare("INSERT INTO op_record(create_time, mdn, imsi, esn, opt_code, opt_result, seq) VALUES (?, ?, ?, ?, ?, ?, ?)");
					string now=tools::currentDateTime();
					db_->setString(1,now);
					db_->setString(2,ack->cd);
					db_->setString(3,"");
					db_->setString(4,"");
					db_->setString(5,"MO");
					db_->setInt(6,ack->result);
					db_->setInt(7,unit->seq);
					db_->executeUpdate();
				}
				else{
					CommonLogger::instance().log_info("deal_logic_resp_queue: tid %u not found",ack->tid);
				}
				info_mgr_->remove_tid_msisdn(ack->tid);
				info_mgr_->remove_tid_seq(ack->tid);
				break;
			}
			default:
				break;
		}

		CommonLogger::instance().log_error("\r\n\r\n\r\n");
		logic_resp_queue_->advance_ridx();
	}
	else
	{
		usleep(1);
	}
	return 0;
}		/* -----  end of method AppReqServThread::deal_logic_resp_queue  ----- */

int AppReqServThread::send_data(int fd, char *buf, unsigned int send_size)
{
	unsigned int need_send_length = send_size;
	int sended_length=0;
	while (need_send_length>0) {
		int ret = send(fd,buf+sended_length,need_send_length,0);
		CommonLogger::instance().log_info("AppReqServThread: send_data send %u Bytes", ret);
		if (ret<0) {
            if(errno == EINTR){
            } else if(errno == EAGAIN) {
                break;
            } else {
                return -1;
            }
        }else if(ret == 0){
            return -1;
        }

        sended_length+=ret;
        need_send_length-=ret;
	}
	return sended_length;
}		/* -----  end of method AppReqServThread::send_data  ----- */

BaseCollectionHandler* AppReqServThread::collection_handler(int fd)
{
	map<int,BaseCollectionHandler*>::iterator iter = client_list_.find(fd);
	if (iter!=client_list_.end()) {
		return iter->second;
	}

	return NULL;
}		/* -----  end of method AppReqServThread::collection_handler  ----- */


int AppReqServThread::close_connection(int fd)
{
	BaseCollectionHandler *handler = collection_handler(fd);
	if (handler != NULL)
	{
		epoll_ctl(efd_, EPOLL_CTL_DEL, fd, NULL);

		close(fd);
		delete handler;
		client_list_.erase(fd);

		if (connection_count_limits_>0) {
			--current_connection_count_;
		}

	}
	return 0;
}		/* -----  end of method AppReqServThread::close_connection  ----- */

int AppReqServThread::close_all_connection() {
	std::map<int,BaseCollectionHandler*>::iterator iter = client_list_.begin();
	while (iter!=client_list_.end()) {
		epoll_ctl(efd_, EPOLL_CTL_DEL, iter->first, NULL);

		close(iter->first);
		delete iter->second;
		client_list_.erase(iter++);

		if (connection_count_limits_>0) {
			--current_connection_count_;
		}
	}
	return 0;
}

bool AppReqServThread::StrToBCD(const char *Src,unsigned char *Des,int iDesLen)
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

int AppReqServThread::deal_recurrent_regnot_ack(AckMsg* ack, ActivateMsg* recurrent_regnot , ActiveUser* user)
{
	if(0==ack->result){/*位置登记成功*/
		CommonLogger::instance().log_info("[%s %d] deal_recurrent_regnot: SERVLOGIC_ACTIVATE_REQ return ok.  tid %u ",__FILE__,__LINE__,ack->tid);
		add_user_req_->erase(ack->tid);
	}
	else{
		if(recurrent_regnot->do_locreq_flag){/*位置登记失败的时候要不要发送LOCREQ，此处是要*/
			
			ReqMsg red_msg;
			red_msg.msg_type = 1;
			ActivateMsg *record = (ActivateMsg*)red_msg.msg;
			record->tid = TidGenerator::instance().generator_tid();
			record->mod_id = UsrAccConfig::instance().module_id();
			memcpy(record->msisdn, recurrent_regnot->msisdn, sizeof(record->msisdn));
			record->actived=1;
			record->user_info = user;
			record->recurrent_regnot_flag=true;
			record->do_locreq_flag=false;
			CommonLogger::instance().log_debug("[%s %d] send LOCREQ, record  tid %u mod_id %u",__FILE__,__LINE__,record->tid, record->mod_id);

			app_req_queue_->insert_record((char*)&red_msg, sizeof(ReqMsg));
			app_req_queue_->advance_widx();

			add_user_req_->erase(ack->tid);
		}
		else{
			CommonLogger::instance().log_info("[%s %d] deal_recurrent_regnot: SERVLOGIC_ACTIVATE_REQ return fail.  not send LOCREQ, tid %u return ok",__FILE__,__LINE__,ack->tid);
			add_user_req_->erase(ack->tid);
		}
	}
	return 0;
}

