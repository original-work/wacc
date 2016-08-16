/*
* =========================================================
*
*       Filename:  loop_stopper.h
*
*    Description:  循环检测系统是否停止
*
*        Version:  1.0
*        Created:  2013.6.27
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  wangxx
*   Organization:  lj
*
* =========================================================
* ==================== CHANGE REPORT HISTORY ===================
*|VERSION|UPDATE DATE|UPDATED BY|DESCRIPTION OF CHANGE|*

* ==================== END OF CHANGE REPORT HISTORY ============
*/
#if !defined(_SAUAPP_COMMON_LOOP_STOPPER_H)
#define _SAUAPP_COMMON_LOOP_STOPPER_H


class LoopStopper
{
public:
	LoopStopper(void);
	~LoopStopper(void);
	bool stopped(void) const {return stopped_;}
	/* 处理信号 */
	static void handle_signal(int t);
	/* 注册信号量 */
	int register_signal(int sig);
private:
	static bool stopped_;
};

#endif //_SAUAPP_COMMON_LOOP_STOPPER_H
