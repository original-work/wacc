/*
* =========================================================
*
*       Filename:  sig_analysis_info_shm_manager.cpp
*
*    Description:  
*
*        Version:  1.0
*        Created:  2013.6.26
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  Zhang Huanli (zhanghl@liangjiang.com)
*   Organization:  lj
*
* ==========================================================
* ================== CHANGE REPORT HISTORY ==================
*|VERSION|UPDATE DATE|UPDATED BY|DESCRIPTION OF CHANGE|*

* ================== END OF CHANGE REPORT HISTORY ===========
*/
#include "sig_analysis_info_shm_manager.h"
#include <sys/types.h>
#include <unistd.h>
#include "common_logger.h"
#include "usracc_config.h"
#include "usracc_common.h"


SigAnalysisInfoShmManager& SigAnalysisInfoShmManager::instance(void) {
	static SigAnalysisInfoShmManager _;
	return _;
}

SigAnalysisInfoShmManager::SigAnalysisInfoShmManager(void) {
}

SigAnalysisInfoShmManager::~SigAnalysisInfoShmManager(void) {

}

char* SigAnalysisInfoShmManager::get_run_info(void) {
	return shm_.head() + sizeof(unsigned int);
}

int SigAnalysisInfoShmManager::init(void) {

	unsigned int key = getpid();

	unsigned int shm_size = sizeof(RunInfo);
	if (shm_.acquire(key, shm_size+sizeof(unsigned int), true) ==-1) {
		CommonLogger::instance().log_error("[%s:%d] failed to init shm", __FILE__, __LINE__);
		return -1;
	}

	char* pshm = shm_.head();
	memcpy(pshm, &shm_size, sizeof(shm_size));

	RunInfo *info = (RunInfo*)get_run_info();
	info->total_active_count = UsrAccConfig::instance().usr_count();
	info->app_req_queue_total = UsrAccConfig::instance().app_req_queue_block_num();
	info->logic_resp_queue_total = UsrAccConfig::instance().servlogic_req_queue_block_num();

	return 0;
}

