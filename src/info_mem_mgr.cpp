/*
* =========================================================
*
*       Filename:  info_mem_mgr.cpp
*
*    Description:  
*
*        Version:  1.0
*        Created:  2013.12.4
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  wangxx
*   Organization:  lj
*
* ==========================================================
* ================== CHANGE REPORT HISTORY ==================
*|VERSION|UPDATE DATE|UPDATED BY|DESCRIPTION OF CHANGE|*

* ================== END OF CHANGE REPORT HISTORY ===========
*/
#include "info_mem_mgr.h"
#include "common_logger.h"
#include "usracc_config.h"


InfoMemMgr::InfoMemMgr() {
}

InfoMemMgr::~InfoMemMgr() {
}

int InfoMemMgr::init() {

	active_usr_table_.init_num_table(UsrAccConfig::instance().usr_count(), sizeof(ActiveUser)+sizeof(int)*UsrAccConfig::instance().serv_logic_server_list().size());

	unsigned int index_count = UsrAccConfig::instance().usr_hash_count();
	index_.init_num_index_table(index_count, 28, 4);

	int index_id;
	if ((index_id = index_.acquire_index_table(1,28)) >= 0) {
		active_usr_table_.pindex_table(&index_, index_id);
	}

	logic_conns_mgr_.init();

	CommonLogger::instance().log_info("info mem initialized.");

	return 0;
}

//map<int, string> tid_msisdn_list_
int InfoMemMgr::add_tid_msisdn(int tid, string msisdn)
{
  map<int, string>::iterator itr = tid_msisdn_list_.find(tid);
  if (itr != tid_msisdn_list_.end())
  {
    tid_msisdn_list_.erase(itr);
  }
  tid_msisdn_list_.insert(pair<int, string>(tid, msisdn));
	return 0;
}		/* -----  end of method InfoMemMgr::add_tid_msisdn  ----- */

int InfoMemMgr::remove_tid_msisdn(int tid)
{
      tid_msisdn_list_.erase(tid);
  	return 0;
}		/* -----  end of method InfoMemMgr::remove_tid_msisdn  ----- */

string InfoMemMgr::find_msisdn_by_tid(int tid)
{
  map<int, string>::iterator itr = tid_msisdn_list_.find(tid);
  if (itr != tid_msisdn_list_.end())
  {
    return itr->second;
  }
	return "";
}		/* -----  end of method InfoMemMgr::find_msisdn_by_tid  ----- */




int InfoMemMgr::add_tid_seq(unsigned int tid, unsigned int seq)
{
	map<int, string>::iterator itr = tid_seq_list_.find(tid);
	if (itr != tid_seq_list_.end())
	{
		tid_seq_list_.erase(itr);
	}
	tid_seq_list_.insert(pair<unsigned int, unsigned int>(tid, seq));
	return 0;
}

int InfoMemMgr::remove_tid_seq(unsigned int tid)
{
	tid_seq_list_.erase(tid);
	return 0;
}

string InfoMemMgr::find_seq_by_tid(int tid)
{
	map<int, string>::iterator itr = tid_seq_list_.find(tid);
	if (itr != tid_seq_list_.end())
	{
		return itr->second;
	}
	return "";
}

