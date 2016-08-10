/*
* =========================================================
*
*       Filename:  sig_analysis_info_shm_manager.h
*
*    Description:  CDR合成模块CDR信息共享内存管理
*
*        Version:  1.0
*        Created:  2013.6.26
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
#if !defined(_SAUAPP_SAU_SIGANALYSISINFOSHMMANAGER_H)
#define _SAUAPP_SAU_SIGANALYSISINFOSHMMANAGER_H
#include "info_shm.h"


class SigAnalysisInfoShmManager {
public:
	static SigAnalysisInfoShmManager& instance(void);
	~SigAnalysisInfoShmManager(void);
	char* get_run_info(void);
	int init(void);
private:
	SigAnalysisInfoShmManager(void);
	SigAnalysisInfoShmManager(const SigAnalysisInfoShmManager &mgr);
	SigAnalysisInfoShmManager& operator=(const SigAnalysisInfoShmManager &mgr);
private:
	InfoShm shm_;
};

#endif //_SAUAPP_SAU_SIGANALYSISINFOSHMMANAGER_H
