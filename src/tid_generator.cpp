/*
 * =====================================================================================
 *
 *       Filename:  tid_generator.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015/3/13 13:56:51
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zhanghl (zhl), zhanghl@liangjiang.com
 *   Organization:  lj
 *
 * =====================================================================================
 * ============================== CHANGE REPORT HISTORY ================================
 * |   VERSION   |   UPDATE DATE   |   UPDATED BY   |   DESCRIPTION OF CHANGE   |*
 
 * =========================== END OF CHANGE REPORT HISTORY ============================
 */
#include "tid_generator.h"
#include "usracc_config.h"


/*
 *--------------------------------------------------------------------------------------
 *       Class:  TidGenerator
 *      Method:  TidGenerator
 * Description:  constructor
 *--------------------------------------------------------------------------------------
 */
TidGenerator::TidGenerator()
{
}  /* -----  end of method TidGenerator::TidGenerator(constructor)  ----- */

TidGenerator::TidGenerator(const TidGenerator &gen)
{
}  /* -----  end of method TidGenerator::TidGenerator(constructor)  ----- */

#if 0
TidGenerator& TidGenerator::operator=(const TidGenerator &gen)
{
}		/* -----  end of method TidGenerator::operator=  ----- */
#endif//deleted by wangxx 20160804

TidGenerator::~TidGenerator()
{
}

TidGenerator& TidGenerator::instance()
{
    static TidGenerator _;
    return _;
}		/* -----  end of method TidGenerator::instance  ----- */

int TidGenerator::init()
{
	tidparam_list_ = UsrAccConfig::instance().tid_num_seg_list();
	max_seg_num_ = tidparam_list_.size();
	cur_seg_num_ = 0;
	if (cur_seg_num_ < max_seg_num_)
	{
		cur_tid_ = tidparam_list_[cur_seg_num_].min_tid - 1;
	}
	return 0;
}		/* -----  end of method TidGenerator::init  ----- */

unsigned int TidGenerator::generator_tid()
{
	if (cur_seg_num_ < max_seg_num_)
	{
		++cur_tid_;
		if (cur_tid_ < tidparam_list_[cur_seg_num_].min_tid ||
			   	cur_tid_ > tidparam_list_[cur_seg_num_].max_tid)
		{
			++cur_seg_num_;
			if (cur_seg_num_ >= max_seg_num_)
			{
				cur_seg_num_ = 0;
			}
			cur_tid_ = tidparam_list_[cur_seg_num_].min_tid;
		}
	}
	return cur_tid_;
}		/* -----  end of method TidGenerator::generator_tid  ----- */

