/*
* =========================================================
*
*       Filename:  file_write.cpp
*
*    Description:  实现往文件写二进制码流的功能
*
*        Version:  1.0
*        Created:  2013.6.14
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
#include "file_write.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


FileWrite::FileWrite() {
	fp_ = -1;
}

FileWrite::~FileWrite() {

}

int FileWrite::open_file(const char* tfile) {
	close_file();

	file_ = tfile;

	fp_ = open(tfile, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
	if (fp_ != -1) {
		return 0;
	}
	return -1;
}

int FileWrite::close_file() {
	if (fp_ != -1) {
		close(fp_);
	}
	return 0;
}

unsigned int FileWrite::write_file(const char *buf, const unsigned int length) {	
	unsigned int need_written_size = length;
	unsigned int written_size = 0;
	int ret;
	while (need_written_size>0) {
		lseek(fp_, 0, SEEK_END);
		if ((ret = write(fp_, buf+written_size, need_written_size)) > 0) {
			need_written_size -= ret;
			written_size += ret;
		} else if (ret==0) {
			break;
		} else {
			return -1;
		}
	}
	//fsync(fp_);
	return written_size;
}

unsigned int FileWrite::get_file_length() {
	int cur_file_pos ,file_length;
	cur_file_pos=lseek(fp_, 0, SEEK_CUR);
	file_length = lseek(fp_, 0, SEEK_END);
	lseek(fp_, cur_file_pos, SEEK_SET);
	return file_length;
}

unsigned int FileWrite::write_file(const char *buf,
								const unsigned int length,
								const unsigned int offset) {
	unsigned int need_written_size = length;
	unsigned int written_size = 0;
	int ret;
	while (need_written_size>0) {
		//printf("###%u\n", offset);
		lseek(fp_, offset+written_size, SEEK_SET);
		if ((ret = write(fp_, buf+written_size, need_written_size)) > 0) {
			need_written_size -= ret;
			written_size += ret;
		} else if (ret==0) {
			break;
		} else {
			return -1;
		}
	}
	//fsync(fp_);
	return written_size;
}
