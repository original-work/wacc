/*
* =========================================================
*
*       Filename:  loop_stopper.h
*
*    Description:  ѭ�����ϵͳ�Ƿ�ֹͣ
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
	/* �����ź� */
	static void handle_signal(int t);
	/* ע���ź��� */
	int register_signal(int sig);
private:
	static bool stopped_;
};

#endif //_SAUAPP_COMMON_LOOP_STOPPER_H
