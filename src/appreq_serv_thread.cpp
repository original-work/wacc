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
 *         Author:  zhanghl (zhl), zhanghl@liangjiang.com
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


int AppReqServThread::init(InfoMemMgr *info_mem, MsgList *app_queue, MsgList *logic_queue)
{
	/* 获取服务端监听端口*/
	collection_server_.port(UsrAccConfig::instance().app_serv_port());
	events_ = NULL;
	all_stopped_ = false;

	info_mgr_ = info_mem;
	app_req_queue_ = app_queue;
	logic_resp_queue_ = logic_queue;

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
	if (current_connection_count_ >= connection_count_limits_ &&
			connection_count_limits_ > 0) {

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

			//pair<map<int,BaseCollectionHandler*>::iterator,bool> ret;
			//ret = client_list_.insert(map<int,BaseCollectionHandler*>::value_type(handler->sockfd(), handler));
			client_list_.insert(map<int,BaseCollectionHandler*>::value_type(handler->sockfd(), handler));
			/*
			if (ret.second == false)
			{
				CommonLogger::instance().log_debug("clients fd %d, %d",handler->sockfd(), client_list_.size());
				return -1;
			}
			*/

			if (connection_count_limits_>0) {
				++current_connection_count_;
			}

			AppReqHandler *h = (AppReqHandler*)handler;
			h->info_mgr(info_mgr_);
			h->app_req_queue(app_req_queue_);
			h->client_list(&client_list_);

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
	int sendlen;
	//memset(send_buf,0,sizeof(send_buf));
	//CommonLogger::instance().log_info("Enter into AppReqServThread 	deal_logic_resp_queue process");
	if (logic_resp_queue_->get_front_record(pmsg, len))
	{
		RespMsg *resp = (RespMsg*)pmsg;
		MTMsg *mt = NULL;
		ActiveUser *user = NULL;
		NIF_MSG_UNIT2 *unit = NULL;
		MTData *data = NULL;
		AckMsg *ack = NULL;
		string msisdn;
		//CommonLogger::instance().log_debug("deal_logic_resp_queue### %u", resp->msg_type);
		switch (resp->msg_type)
		{
			case 4:	// MT
				mt = (MTMsg*)resp->msg;
                         	CommonLogger::instance().log_info("deal_logic_resp_queue: Deal MT mag, len=%d",len);
				CommonLogger::instance().log_info("deal_logic_resp_queue: cd %s, tid %d", mt->cd,mt->tid);
				StrToBCD(mt->cd, bcd_buf_, sizeof(bcd_buf_));
				user = (ActiveUser*)info_mgr_->active_usr_table_.find_num((char*)bcd_buf_, strlen(mt->cd));
				if (user != NULL)
				{
					/* luchq add 2016-02-01 */
					if ( (time(NULL)-user->operTime) > UsrAccConfig::instance().app_heartbeat_overtime())
					{
						CommonLogger::instance().log_info("deal_logic_resp_queue: over time, user maybe not active");
						break;
					}
					/* end of luchq add */
					CommonLogger::instance().log_info("deal_logic_resp_queue: sms len=%d",strlen(mt->sms_content));
					unit = (NIF_MSG_UNIT2*)send_buf;
					unit->dialog = htonl(BEGIN);
					unit->invoke = htonl(SMS_PUSH);
					unit->length = htonl(sizeof(MTData));
					data = (MTData*)(send_buf + sizeof(NIF_MSG_UNIT2) - sizeof(unsigned char*));
					memset(data->content,0,sizeof(data->content));
					CommonLogger::instance().log_info("deal_logic_resp_queue: sms len=%d",strlen(mt->sms_content));
					data->seq = mt->seq;
					data->tid = htonl(mt->tid);
					memcpy(data->sender, mt->cg, strlen(mt->cg));
					memcpy(data->content, mt->sms_content, strlen(mt->sms_content));
					//send_data(user->fd, send_buf, sizeof(NIF_MSG_UNIT2)-sizeof(unsigned char*)+sizeof(MTData));
					if (strcmp(user->msisdn,mt->cd))
					{
						CommonLogger::instance().log_error("deal_logic_resp_queue: Check App number fail, App=%s",user->msisdn);
					}
					sendlen = send_data(user->fd, send_buf, sizeof(NIF_MSG_UNIT2)-sizeof(unsigned char*)+sizeof(MTData));
					if (sendlen != sizeof(NIF_MSG_UNIT2)-sizeof(unsigned char*)+sizeof(MTData))
					{
						CommonLogger::instance().log_info("deal_logic_resp_queue: send MT msg to %s 	FAIL!!! send length:%d", user->msisdn,len);
					}
                                CommonLogger::instance().log_info("deal_logic_resp_queue: send MT msg to %s, Socket:%d, len:%d",
                                	user->msisdn,user->fd,strlen(data->content));
					tools::print_hex((unsigned char*)data->content,256);

				}
				else
					CommonLogger::instance().log_info("deal_logic_resp_queue: Call find_num fail, user maybe not exist");
				break;
			case 5: //PING ACK
				ack = (AckMsg*)resp->msg;
				CommonLogger::instance().log_info("deal_logic_resp_queue: Call find_num fail, user maybe not exist");
				unit = (NIF_MSG_UNIT2*)send_buf;
				unit->dialog = htonl(END);
				unit->invoke = htonl(ack->msg_type);

				sendlen = send_data(mihao_fd_, send_buf, sizeof(NIF_MSG_UNIT2)-sizeof(unsigned char*)+sizeof(unsigned int));
				if (sendlen != sizeof(NIF_MSG_UNIT2)-sizeof(unsigned char*)+sizeof(unsigned int))
				{
					CommonLogger::instance().log_error("deal_logic_resp_queue: send PING msg to %s FAIL!!!", mt->cd);
				}
				CommonLogger::instance().log_info("deal_logic_resp_queue: send PING msg to APP");
				break;
			case 8:	// ACK
				ack = (AckMsg*)resp->msg;
				msisdn = info_mgr_->find_msisdn_by_tid(ack->tid);
				CommonLogger::instance().log_info("deal_logic_resp_queue: Deal ACK msg, tid=%u", ack->tid);
				CommonLogger::instance().log_info("deal_logic_resp_queue: num %s, result %d", msisdn.c_str(), ack->result);
				if (msisdn != "")
				{
					StrToBCD(msisdn.c_str(), bcd_buf_, sizeof(bcd_buf_));
					user = (ActiveUser*)info_mgr_->active_usr_table_.find_num((char*)bcd_buf_, msisdn.length());
					if (user != NULL)
					{
						unit = (NIF_MSG_UNIT2*)send_buf;
						unit->dialog = htonl(END);
						unit->invoke = htonl(ack->msg_type);
						unit->length = htonl(sizeof(unsigned int));
						*((unsigned int*)(send_buf + sizeof(NIF_MSG_UNIT2) - sizeof(unsigned char*))) = htonl(ack->result);
						sendlen = send_data(user->fd, send_buf, sizeof(NIF_MSG_UNIT2)-sizeof(unsigned char*)+sizeof(unsigned int));
						if (sendlen != sizeof(NIF_MSG_UNIT2)-sizeof(unsigned char*)+sizeof(unsigned int))
						{
							CommonLogger::instance().log_error("deal_logic_resp_queue: send ACK msg to %s FAIL!!!", mt->cd);
						}
						CommonLogger::instance().log_info("deal_logic_resp_queue: send ACK msg to APP");
					}
					else
						CommonLogger::instance().log_info("deal_logic_resp_queue: Call find_num fail");
                              		info_mgr_->remove_tid_msisdn(ack->tid);
				}
				break;
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


