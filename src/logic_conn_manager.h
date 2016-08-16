/*
 * =====================================================================================
 *
 *       Filename:  logic_conn_manager.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015/3/13 12:58:38
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
#if !defined(LOGIC_CONN_MANAGER_H_)
#define LOGIC_CONN_MANAGER_H_
#include "tcp_client.h"
#include "usracc_common.h"


/*
 * =====================================================================================
 *        Class:  LogicConnManager
 *  Description:  
 * =====================================================================================
 */
class LogicConnManager
{
	public:
		LogicConnManager();                             /* constructor */
		~LogicConnManager();                            /* destructor       */
		int init();
		int update_logic_conninfo(int logic_modid, TcpClient *client);
		int remove_logic_conninfo(int logic_modid);
		int conns_counts() const {return conns_count_;}
		LogicConnInfo *logic_conns() const {return logic_conns_;}
		LogicConnInfo *get_conn_info(int logic_modid);
	private:
		LogicConnInfo *logic_conns_;
		int conns_count_;
}; /* -----  end of class LogicConnManager  ----- */

#endif	// LOGIC_CONN_MANAGER_H_

