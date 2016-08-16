/*
* =========================================================
*
*       Filename:  ctimer.h
*
*    Description:  
*
*        Version:  1.0
*        Created:  2013.11.1
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
#if !defined(SNOOPY_CTIMER_H_)
#define SNOOPY_CTIMER_H_
#include <time.h>


class CTimer
{
public:
	CTimer(void);
	CTimer(unsigned int interval);
	~CTimer(void);
	int begin_timer(unsigned int cur_time=0);
	int expired(unsigned int cur_time=0);
	int reset(unsigned int cur_time=0);
	void time_interval(unsigned int interval);
	unsigned int time_interval(){return this->time_interval_;}
	time_t get_last_time() const {return this->last_time_;}
private:
	unsigned int time_interval_;
	time_t last_time_;
	bool started_;
};

#endif // SNOOPY_CTIMER_H_
