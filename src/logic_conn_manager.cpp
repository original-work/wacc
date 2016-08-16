/*
 * =====================================================================================
 *
 *       Filename:  logic_conn_manager.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015/3/13 12:57:29
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
#include "logic_conn_manager.h"
#include "usracc_config.h"
#include <string.h>


/*
 *--------------------------------------------------------------------------------------
 *       Class:  LogicConnManager
 *      Method:  LogicConnManager
 * Description:  constructor
 *--------------------------------------------------------------------------------------
 */
LogicConnManager::LogicConnManager()
{
	logic_conns_ = NULL;
}  /* -----  end of method LogicConnManager::LogicConnManager(constructor)  ----- */

LogicConnManager::~LogicConnManager()
{
	if (logic_conns_ != NULL)
	{
		delete[] logic_conns_;
		logic_conns_ = NULL;
	}
}

int LogicConnManager::init()
{
	conns_count_ = UsrAccConfig::instance().serv_logic_server_list().size();
	logic_conns_ = new LogicConnInfo[conns_count_];
	memset(logic_conns_, 0, sizeof(LogicConnInfo)*conns_count_);
	return 0;
}		/* -----  end of method LogicConnManager::init  ----- */

int LogicConnManager::update_logic_conninfo(int logic_modid, TcpClient *client)
{
	logic_conns_[logic_modid].client = client;
	logic_conns_[logic_modid].used = 1;
	logic_conns_[logic_modid].reconnect_cnt += 1;

	return 0;
}		/* -----  end of method LogicConnManager::update_logic_conninfo  ----- */

int LogicConnManager::remove_logic_conninfo(int logic_modid)
{
	logic_conns_[logic_modid].client = NULL;
	logic_conns_[logic_modid].used = 0;
	return 0;
}		/* -----  end of method LogicConnManager::remove_logic_conninfo  ----- */

LogicConnInfo* LogicConnManager::get_conn_info(int logic_modid)
{
	if (logic_modid >= conns_count_)
	{
		return NULL;
	}
	else
	{
		return &(logic_conns_[logic_modid]);
	}
}		/* -----  end of method LogicConnManager::get_conn_info  ----- */

