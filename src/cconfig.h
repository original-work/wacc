/*
* =========================================================
*
*       Filename:  cconfig.h
*
*    Description:  配置管理，基础类
*
*        Version:  1.0
*        Created:  2013.6.19
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
#if !defined(_SAUAPP_COMMON_CCONFIG_H)
#define _SAUAPP_COMMON_CCONFIG_H
#include <string>


class CConfig {
public:
	CConfig();
	virtual ~CConfig();
	virtual int  init(const char* cfg) = 0;
	std::string cfg_file() const {return cfg_file_;}
	void cfg_file(const char* cfg) {cfg_file_=cfg;}
private:
	std::string cfg_file_;
};

#endif  //_SAUAPP_COMMON_CCONFIG_H
