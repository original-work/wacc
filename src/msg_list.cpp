/*
* =========================================================
*
*       Filename:  msg_list.cpp
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
#include "msg_list.h"

MsgList::MsgList() {
	pmsg_buf_ = NULL;
}

MsgList::~MsgList() {
	if (pmsg_buf_ != NULL) {
		delete[] pmsg_buf_;
		pmsg_buf_ = NULL;
	}
}

int MsgList::init(unsigned int block_num, unsigned int block_size) {
	block_num_ = block_num;
	block_size_ = block_size;
	widx_ = 0;
	ridx_ = 0;
	pmsg_buf_ = new char[block_num*block_size];
	return 0;
}

unsigned int MsgList::unused_block_num() {

	if (ridx_ == widx_) {
		return block_num_;
	}

	return (ridx_ - widx_ + block_num_)%block_num_;
}

bool MsgList::insert_record(const char *pmsg, unsigned int msg_len) {
	if (unused_block_num()>1||ridx_==widx_) {
		if (block_size_>=(msg_len+4)) {
			memcpy(pmsg_buf_+widx_*block_size_, &msg_len, sizeof(msg_len));
			memcpy(pmsg_buf_+widx_*block_size_+4, pmsg, msg_len);
			return true;
		}
	}
	return false;
}

bool MsgList::get_front_record(char *&pmsg, unsigned int &len) {
	if (0<unused_block_num() && unused_block_num()<block_num_) {
		unsigned int *tmp_len = (unsigned int *)(pmsg_buf_+ridx_*block_size_);
		len = *tmp_len;
		//printf("%u, %u, msg length=%u, %u, %u\n", ridx_, block_size_, *tmp_len, widx_, ridx_);
		pmsg = pmsg_buf_+ridx_*block_size_+4;
		return true;
	}
	return false;
}

void MsgList::advance_ridx() {
	ridx_++;
	if (ridx_ == block_num_) {
		ridx_ = 0;
	}
}

void MsgList::advance_widx() {
	widx_++;
	if (widx_ == block_num_) {
		widx_ = 0;
	}
}

