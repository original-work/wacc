/*
 * =====================================================================================
 *
 *       Filename:  appreq_serv_thread.h
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
#if !defined(APPREQSERVTHREAD_H_)
#define APPREQSERVTHREAD_H_
#include "cthread.h"
#include "collection_server.h"
#include "appreq_handler.h"
#include <map>
#include <sys/epoll.h>
#include "info_mem_mgr.h"
#include "msg_list.h"


using namespace std;


/*
 * =====================================================================================
 *        Class:  AppReqServThread
 *  Description:  
 * =====================================================================================
 */
class AppReqServThread : public CThread
{
	public:
		AppReqServThread();                             /* constructor */
		~AppReqServThread();                            /* destructor       */
		int init(InfoMemMgr *info_mem, MsgList* app_queue, MsgList* logic_queue, MsgList* recurrent_regnot_queue, map<unsigned int, char*>* p_map_recurrent_regnot);
		int open(void *args = 0);
		int svc();
		int loop_process();
		int handle_request(int fd);
		int deal_new_connection();
		int deal_logic_resp_queue();
		int send_data(int fd, char *data, unsigned int len);
		int my_check_epoll();
		int stop();
		int close_connection(int fd);
		int close_all_connection();
	protected:
		BaseCollectionHandler* collection_handler(int fd);
		bool StrToBCD(const char *Src,unsigned char *Des,int iDesLen);
	private:
		CollectionServer<AppReqHandler> collection_server_;
		map<int, BaseCollectionHandler*> client_list_;
		map<unsigned int, char*>* add_user_req_;
		int efd_;
		int mihao_fd_;//added by wangxx 20160811 因为发给我的ping 消息里面不带mdn,所以只好这样使用
		struct epoll_event *events_;
		struct epoll_event event_;
		const unsigned int CMaxEvents;

		InfoMemMgr *info_mgr_;
		MsgList *app_req_queue_;
		MsgList *logic_resp_queue_;
		MsgList* recurrent_regnot_queue_;
		unsigned int current_connection_count_;
		unsigned int connection_count_limits_;
		unsigned char bcd_buf_[256];
		bool all_stopped_;
}; /* -----  end of class AppReqServThread  ----- */

#endif

