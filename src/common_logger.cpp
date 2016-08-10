/*
* =========================================================
*
*       Filename:  common_logger.cpp
*
*    Description:  
*
*        Version:  1.0
*        Created:  2013.6.14
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
#include "common_logger.h"


CommonLogger& CommonLogger::instance() {
	static CommonLogger _;
	return _;
}

CommonLogger::CommonLogger(void) {
	
}

CommonLogger::~CommonLogger(void) {

}

void CommonLogger::init(const char* log_cfg, const char* logger_name) {
	logger_ = log4cplus::Logger::getInstance(logger_name);
	cfg_file_=log_cfg;
	log4cplus::PropertyConfigurator::doConfigure(cfg_file_);
	initialized_ = true;
}

void CommonLogger::log_trace(const char* fmt, ...) {
	char plog_buf[kMaxBufferSize];
	memset(plog_buf,0,kMaxBufferSize);

	va_list ap;
	va_start(ap,fmt);
	vsprintf(plog_buf, fmt,ap);
	va_end(ap);

	LOG4CPLUS_TRACE_METHOD(logger_, plog_buf);
}

void CommonLogger::log_debug(const char* fmt, ...) {
	char plog_buf[kMaxBufferSize];
	memset(plog_buf,0,kMaxBufferSize);
	
	va_list ap;
	va_start(ap, fmt);
	vsprintf(plog_buf, fmt,ap);
	va_end(ap);

	LOG4CPLUS_DEBUG(logger_, plog_buf);
}

void CommonLogger::log_info(const char* fmt, ...) {
	char plog_buf[kMaxBufferSize];
	memset(plog_buf, 0, kMaxBufferSize);
	
	va_list ap;
	va_start(ap, fmt);
	vsprintf(plog_buf, fmt,ap);
	va_end(ap);

	LOG4CPLUS_INFO(logger_, plog_buf);	
}

void CommonLogger::log_fatal(const char* fmt, ...) {
	char plog_buf[kMaxBufferSize];
	memset(plog_buf, 0, kMaxBufferSize);
	
	va_list ap;
	va_start(ap, fmt);
	vsprintf(plog_buf, fmt,ap);
	va_end(ap);

	LOG4CPLUS_FATAL(logger_, plog_buf);
}

void CommonLogger::log_warn(const char* fmt, ...) {
	char plog_buf[kMaxBufferSize];
	memset(plog_buf, 0, kMaxBufferSize);
	
	va_list ap;
	va_start(ap, fmt);
	vsprintf(plog_buf, fmt, ap);
	va_end(ap);

	LOG4CPLUS_WARN(logger_, plog_buf);
}

void CommonLogger::log_error(const char* fmt, ...) {
	char plog_buf[kMaxBufferSize];
	memset(plog_buf,0,kMaxBufferSize);
	
	va_list ap;
	va_start(ap,fmt);
	vsprintf(plog_buf,fmt,ap);
	va_end(ap);

	LOG4CPLUS_ERROR(logger_, plog_buf);
}
