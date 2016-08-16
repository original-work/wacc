/*
* =========================================================
*
*       Filename:  common_logger.h
*
*    Description:  日志管理
*
*        Version:  1.0
*        Created:  2013.6.14
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
#if !defined(_SAUAPP_COMMON_COMMONLOGGER_H)
#define _SAUAPP_COMMON_COMMONLOGGER_H
#include <string>
#include "log4cplus/logger.h"
#include "log4cplus/configurator.h"
#include "log4cplus/helpers/stringhelper.h"
#include "log4cplus/loggingmacros.h"
#include <string.h>
#include <stdio.h>


class CommonLogger {
public:
	static CommonLogger& instance();
	~CommonLogger(void);
	/* 初始化 */
	void init(const char* log_cfg, const char* logger_name);
	void log_trace(const char* fmt, ...);
	void log_debug(const char* fmt, ...);
	void log_info(const char* fmt, ...);
	void log_fatal(const char* fmt, ...);
	void log_warn(const char* fmt, ...);
	void log_error(const char* fmt, ...);
	bool initialized() const {return initialized_;}
private:
	CommonLogger(void);
	CommonLogger(const CommonLogger &logger);
	CommonLogger& operator=(const CommonLogger &logger);
private:
	bool initialized_;
	log4cplus::Logger logger_;
	std::string cfg_file_;
	const static unsigned int kMaxBufferSize=2048;
};

#endif  //_SAUAPP_COMMON_COMMONLOGGER_H
