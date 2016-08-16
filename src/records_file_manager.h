/*
* =========================================================
*
*       Filename:  records_file_manager.h
*
*    Description:  ¼ÇÂ¼´æ´¢×ª·¢
*
*        Version:  1.0
*        Created:  2013.6.24
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
#if !defined(_SAUAPP_SAU_RECORDSFILEMANAGER_H)
#define _SAUAPP_SAU_RECORDSFILEMANAGER_H
#include <string>
#include <string.h>
#include <limits.h>
#include "file_write.h"
#include <unistd.h>


class RecordsFileManager {
public:
	RecordsFileManager();
	virtual ~RecordsFileManager();
	std::string file_path() const {return file_path_;}
	std::string file_prefix() const {return file_prefix_;}
	virtual int init(const std::string &file_path,
		const std::string &file_prefix,
		const unsigned int &periodic);
	unsigned int get_records_file_length();
	virtual int write_records_into_file(const char *buf, const unsigned int &length);
	virtual int write_records_into_file(const char *buf, const unsigned int &length, const unsigned int &offset);
	int append_record_into_file(const char *buf, const unsigned int &length);
protected:
	virtual int generate_records_filename(const time_t &t);
	virtual int check_file();
	virtual bool is_filename_changed() {return (strcmp( cur_filename_, new_filename_) != 0);}
	char* cur_filename() {return cur_filename_;}
	char* new_filename() {return new_filename_;}
	char* cur_tmp_filename() {return cur_tmp_filename_;}
	char* new_tmp_filename() {return new_tmp_filename_;}
	virtual void refresh_filename();
	unsigned int write_periodic() const {return write_periodic_;}
public:
	FileWrite file_writer_;
private:
	std::string file_path_;
	std::string file_prefix_;
	char cur_filename_[PATH_MAX];
	char new_filename_[PATH_MAX];
	char cur_tmp_filename_[PATH_MAX];
	char new_tmp_filename_[PATH_MAX];
	//time_t last_time_;
	unsigned int write_periodic_;
};

#endif //_SAUAPP_SAU_RECORDSFILEMANAGER_H
