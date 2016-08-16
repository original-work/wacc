/*
* =========================================================
*
*       Filename:  ctimer.cpp
*
*    Description:  
*
*        Version:  1.0
*        Created:  2016.8.16
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
#include "ctimer.h"
#include <time.h>
#include "common_logger.h"


CTimer::CTimer(void)
{
	this->started_ = false;
}

CTimer::~CTimer(void)
{
}

CTimer::CTimer(unsigned int interval)
{
	this->time_interval_ = interval;
	this->started_ = false;
}

int CTimer::begin_timer(unsigned int cur_time)
{
	if(time_interval_>0 && !this->started_)
	{
		if(!cur_time)
		{
			time(&this->last_time_);
		}
		else
		{
			this->last_time_ = cur_time;
		}

		this->last_time_ = (this->last_time_ - this->last_time_%this->time_interval_);
		this->started_ = true;
	}
	return 0;
}

int CTimer::expired(unsigned int cur_time)
{
	if(this->started_)
	{
		time_t now;
		if(!cur_time)
		{
			time(&now);
		}
		else
		{
			now = cur_time;
		}
		if((now > this->last_time_)&&
			((now-this->last_time_) > this->time_interval_))
		{
			this->started_ = false;
			return 1;
		}
	}
	else
	{
		CommonLogger::instance().log_error("timer is not started ...");
	}
	return 0;
}

int CTimer::reset(unsigned int cur_time)
{
	begin_timer(cur_time);
	return 0;
}

void CTimer::time_interval(unsigned int interval)
{
	this->time_interval_ = interval;
}

