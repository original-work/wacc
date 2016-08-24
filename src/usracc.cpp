/*
 * =====================================================================================
 *
 *       Filename:  epoll_test.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015/3/10 11:08:21
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
#include <stdio.h>
#include "appreq_serv_thread.h"
#include "loop_stopper.h"
#include "usracc_config.h"
#include "sig_analysis_info_shm_manager.h"
#include "logic_req_serv_thread.h"
#include "info_mem_mgr.h"
#include "msg_list.h"
#include "tcp_client_mgr_thread.h"


int main(int argc, char *argv[])
{
	if (argc==2 && 
		(strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "-V") == 0)) {
		printf("Version : %s", "$Name:  $");
		return 0;
	} else if(argc != 3) {
		printf("[USAGE] usracc -f path/configure_file \n");
		return -1;
	}

	char cfg_file[PATH_MAX];
	memset(cfg_file, 0, PATH_MAX);
	if (strcmp(argv[1], "-f") == 0) {
		strcpy(cfg_file, argv[2]);
	}

	if (UsrAccConfig::instance().init(cfg_file) != 0) {
		return 0;
	}

	SigAnalysisInfoShmManager::instance().init();

	MsgList app_req_queue;
	app_req_queue.init(UsrAccConfig::instance().app_req_queue_block_num(), 
			UsrAccConfig::instance().app_req_queue_block_size());

	MsgList logic_resp_queue;
	logic_resp_queue.init(UsrAccConfig::instance().servlogic_req_queue_block_num(), 
			UsrAccConfig::instance().servlogic_req_queue_block_size());

	MsgList recurrent_regnot_queue;
	recurrent_regnot_queue.init(UsrAccConfig::instance().recurrent_regnot_queue_block_num(), 
			UsrAccConfig::instance().recurrent_regnot_queue_block_size());

	map<unsigned int, char*> map_add_user_req;


	InfoMemMgr info_mem_mgr;
	info_mem_mgr.init();


	MySQLConnWrapper db;
	db.init(UsrAccConfig::instance().mysql_url(), UsrAccConfig::instance().mysql_user(), UsrAccConfig::instance().mysql_password());
	db.connect();
	db.switchDb("mihao");

	LogicReqServThread logic_req_serv_thread;
	logic_req_serv_thread.init(&info_mem_mgr, &app_req_queue, &logic_resp_queue, &recurrent_regnot_queue, &map_add_user_req, &db);
	TcpClientMgrThread tcp_client_mgr_thread;
	tcp_client_mgr_thread.init(logic_req_serv_thread.client_list());
	tcp_client_mgr_thread.open();
	logic_req_serv_thread.open();

	AppReqServThread app_req_serv_thread;
	app_req_serv_thread.init(&info_mem_mgr, &app_req_queue, &logic_resp_queue, &recurrent_regnot_queue, &map_add_user_req, &db);
	app_req_serv_thread.open();

	

	// loop
	LoopStopper loop_stopper;
	while (!loop_stopper.stopped()) {
		sleep(1);
	}
	
	app_req_serv_thread.stop();
	tcp_client_mgr_thread.stop();
	logic_req_serv_thread.stop();

	return 0;
}

