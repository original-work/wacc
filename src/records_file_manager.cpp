/*
* =========================================================
*
*       Filename:  records_file_manager.cpp
*
*    Description:  
*
*        Version:  1.0
*        Created:  2013.6.24
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
#include "records_file_manager.h"
#include <time.h>
#include <stdio.h>

using namespace std;


RecordsFileManager::RecordsFileManager() {

}

RecordsFileManager::~RecordsFileManager() {

}

int RecordsFileManager::init(const string &file_path,
		const string &file_prefix,
		const unsigned int &periodic) {
	file_path_ = file_path;
	file_prefix_ = file_prefix;
	write_periodic_ = periodic;

	memset(cur_filename_, 0, PATH_MAX);
	memset(cur_tmp_filename_, 0, PATH_MAX);
	memset(new_filename_, 0, PATH_MAX);
	memset(new_tmp_filename_, 0, PATH_MAX);
	return 0;
}

int RecordsFileManager::check_file() {
	time_t now;
	time(&now);
	now = (now - now%write_periodic_);
	generate_records_filename(now);
	if (is_filename_changed()) {
		if (file_writer_.open_file(new_tmp_filename_) < 0) {
			return -1;
		}
	
		if ((strcmp(cur_filename_, "") != 0) && (strcmp(cur_tmp_filename_, "") != 0))
		{
			rename(cur_tmp_filename_, cur_filename_);
			unlink(cur_tmp_filename_);
		}
		
		refresh_filename();
		return 1;
	}
	return 0;
}

int RecordsFileManager::generate_records_filename(const time_t &t)
{
	struct tm st;
	localtime_r(&t,&st);

	sprintf(new_filename(), "%s%s%04d%02d%02d.%02d%02d", file_path().c_str(), 
			file_prefix().c_str(),st.tm_year+1900,st.tm_mon+1,st.tm_mday,st.tm_hour,
			st.tm_min);

	sprintf(new_tmp_filename(), "%s%s%04d%02d%02d.%02d%02d_tmp", file_path().c_str(), 
			file_prefix().c_str(),st.tm_year+1900,st.tm_mon+1,st.tm_mday,st.tm_hour,
			st.tm_min);

	return 0;
}

unsigned int RecordsFileManager::get_records_file_length() {
	return file_writer_.get_file_length();
}

int RecordsFileManager::write_records_into_file(const char *buf, const unsigned int &length) {
	if (file_writer_.write_file(buf, length)!=length) {
		return -1;
	}
	return 0;
}

int RecordsFileManager::write_records_into_file(const char *buf, const unsigned int &length, const unsigned int &offset) {
	/*unsigned int value=*(unsigned int*)buf;	
	unsigned int hh = ((unsigned char)buf[3]<<24)|((unsigned char)buf[2]<<16)|((unsigned char)buf[1]<<8)|(unsigned char)buf[0];
	printf("index buf=%u, %u, length =%u, offset=%u\n", value, hh, length, offset);*/
	//printf("offset = %u\n", offset);
	if (file_writer_.write_file(buf, length, offset)!=length) {
		return -1;
	}
	return 0;
}

int RecordsFileManager::append_record_into_file(const char *buf, const unsigned int &length)
{
	if (check_file() < 0)
	{
		return -1;
	}

	return write_records_into_file(buf, length);
}

void RecordsFileManager::refresh_filename() {
	strcpy(cur_filename_, new_filename_);
	strcpy(cur_tmp_filename_, new_tmp_filename_);
}

