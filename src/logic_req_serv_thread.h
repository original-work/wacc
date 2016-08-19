/*
 * =====================================================================================
 *
 *       Filename:  logic_req_serv_thread.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015/3/12 16:00:21
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
#if !defined(LOGICREQSERVTHREAD_H_)
#define	LOGICREQSERVTHREAD_H_
#include <vector>
#include "msg_list.h"
#include "tcp_client.h"
#include "ctimer.h"
#include "info_mem_mgr.h"
#include "cthread.h"

using namespace std;

/*
 * =====================================================================================
 *        Class:  LogicReqServThread
 *  Description:  
 * =====================================================================================
 */
class LogicReqServThread : public CThread
{
	public:
		LogicReqServThread();                             /* constructor */
		~LogicReqServThread();                            /* destructor       */
		int open(void *args = 0);
		int svc();
		int stop();
		int init(InfoMemMgr *info_mgr, MsgList* app_queue, MsgList* logic_queue, MsgList* recurrent_regnot_queue, map<unsigned int, char*>* p_map_recurrent_regnot);
		int select_check_fds();
		vector<TcpClient>* client_list() {return &client_list_;}
	protected:
		bool StrToBCD(const char *Src,unsigned char *Des,int iDesLen);
		int loop_process();
		int deal_app_req_queue();
		int send_msg(int fd, const char *buf, const unsigned int &len);
		int recv_msg(int fd, char *buf, const unsigned int &len);
		int deal_locreq_ack(unsigned char *data, unsigned int len);
		int deal_connack_msg(TcpClient *client, unsigned char *data);
		int deal_mt_req(unsigned char *data, unsigned int len);
		int deal_ack_req(unsigned int type, unsigned char *data, unsigned int len);
		int deal_heartbeat();
		int deal_recurrent_regnot();
	private:
		vector<TcpClient> client_list_;
		map<unsigned int, char*>* add_user_req_;
		unsigned int client_seq_;
		MsgList *app_req_queue_;
		MsgList *logic_resp_queue_;
		MsgList *recurrent_regnot_queue_;
		InfoMemMgr *info_mgr_;
		fd_set fdset_;
		unsigned char data_buf_[3000];
		CTimer timer_;
		bool all_stopped_;
		unsigned char bcd_buf_[256];
}; /* -----  end of class LogicReqServThread  ----- */


#endif // LOGICREQSERVTHREAD_H_ 

