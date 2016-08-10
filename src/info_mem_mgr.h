/*
* =========================================================
*
*       Filename:  info_mem_mgr.h
*
*    Description:  
*
*        Version:  1.0
*        Created:  2013.12.4
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  Zhang Huanli (zhanghl@liangjiang.com)
*   Organization:  lj
*
* =========================================================
* ==================== CHANGE REPORT HISTORY ===================
*|VERSION|UPDATE DATE|UPDATED BY|DESCRIPTION OF CHANGE|*

* ==================== END OF CHANGE REPORT HISTORY ============
*/
#if !defined(_INFOMEMMGR_H)
#define _INFOMEMMGR_H
#include <set>
#include <string>
#include <map>
#include "logic_conn_manager.h"
#include "num_table.h"

using namespace std;

class InfoMemMgr {
public:
	InfoMemMgr();
	~InfoMemMgr();

	int init();
	int add_tid_msisdn(int tid, string msisdn);
	int remove_tid_msisdn(int tid);
	string find_msisdn_by_tid(int tid);
public:
	NumTable active_usr_table_;
	NumIndexTable index_;
	LogicConnManager logic_conns_mgr_;
private:
	map<int, string> tid_msisdn_list_;
};

#endif  //_INFOMEMMGR_H
