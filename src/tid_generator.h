/*
 * =====================================================================================
 *
 *       Filename:  tid_generator.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015/3/13 13:59:33
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
#if !defined(TIDGENERATOR_H_)
#define TIDGENERATOR_H_
#include <vector>
#include "usracc_common.h"


using namespace std;

/*
 * =====================================================================================
 *        Class:  TidGenerator
 *  Description:  
 * =====================================================================================
 */
class TidGenerator
{
	public:
		static TidGenerator& instance();
		~TidGenerator();                            /* destructor       */
		int init();
		unsigned int generator_tid();
	private:
		TidGenerator();                             /* constructor */
		TidGenerator(const TidGenerator &gen);
		TidGenerator& operator=(const TidGenerator &gen);
	private:
		vector<TidParam> tidparam_list_;
		unsigned int max_seg_num_;
		unsigned int cur_seg_num_;
		unsigned int cur_tid_;
}; /* -----  end of class TidGenerator  ----- */


#endif	// TIDGENERATOR_H_

