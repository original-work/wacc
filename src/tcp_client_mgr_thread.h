/*
 * =====================================================================================
 *
 *       Filename:  tcp_client_mgr_thread.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015/3/12 13:07:11
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
#if !defined(TCPCLIENTMGRTHREAD_H_)
#define TCPCLIENTMGRTHREAD_H_
#include "cthread.h"
#include "tcp_client.h"
#include <vector>

using namespace std;

/*
 * =====================================================================================
 *        Class:  TcpClientMgrThread
 *  Description:  
 * =====================================================================================
 */
class TcpClientMgrThread : public CThread
{
	public:
		TcpClientMgrThread();                             /* constructor */
		~TcpClientMgrThread();                            /* destructor       */
		int open(void *args = 0);
		int svc();
		int init(vector<TcpClient> *pclients);
		int stop();
	protected:
		int loop_process();
		int reconnect_to_server(TcpClient *client);
		int build_conndata(char *buf);
	private:
		vector<TcpClient> *pclient_list_;
		unsigned char conn_data_[256];
		unsigned int conn_data_len_;
		bool all_stopped_;
}; /* -----  end of class TcpClientMgrThread  ----- */

#endif

