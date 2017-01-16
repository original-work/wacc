/*
* =========================================================
*
*       Filename:  num_table.cpp
*
*    Description:  
*
*        Version:  1.0
*        Created:  2013.11.29
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
#include "num_table.h"
#include <stdio.h>
#include "common_logger.h"


NumTable::NumTable() {
	self_index_ = false;
	pindex_table_ = NULL;
}

NumTable::~NumTable() {
	if (pindex_table_ && self_index_) {
		delete pindex_table_;
	}
}

int NumTable::init_num_table(unsigned int max_count, unsigned int max_size) {
	list_table_.init_list_block(max_count, max_size);
	return 0;
}

int NumTable::init_num_index(unsigned int index_count, 
	unsigned int max_num_length, 
	unsigned int index_table_count) {

	if (pindex_table_ == NULL) {
		pindex_table_ = new NumIndexTable();
		self_index_ = true;
		pindex_table_->init_num_index_table(index_count, max_num_length, index_table_count);
		if ((index_table_id_ = pindex_table_->acquire_index_table(1, max_num_length)) != -1) {
			return index_table_id_;
		}
	}

	return -1;
}

void NumTable::pindex_table(NumIndexTable *index, int index_table_id) {
	if (self_index_ == true) {
		return;
	}

	index_table_id_ = index_table_id;
	pindex_table_ = index;
}

LIST_BLOCK_HEAD* NumTable::add_num(char* number, 
	unsigned int number_len) {

	//获取一个空的节点
	LIST_BLOCK_HEAD* pblock = list_table_.get_block();

	CommonLogger::instance().log_error("add_num: pblock is %x", pblock);
	CommonLogger::instance().log_error("add_num: index_table_id_ is %d", index_table_id_);

	if (pblock) {
		if (pindex_table_->add_number(index_table_id_, number, number_len, pblock) < 0) {
			list_table_.free_block((LIST_BLOCK_HEAD*)pblock);
			pblock=0;
		}
	}

	return pblock;
}

LIST_BLOCK_HEAD* NumTable::get_specific_num_table(unsigned int id) {
	return list_table_.get_specific_block(id);
}

unsigned int NumTable::get_used_num(){
	return list_table_.get_used_num();
}

int NumTable::remove_num(char* number, unsigned int number_len) {

	LIST_BLOCK_HEAD* pblock = NULL;
	//从索引表中删除
	if (pindex_table_) {
		pblock = (LIST_BLOCK_HEAD*)pindex_table_->remove_number(index_table_id_, number, number_len);
	}

	if (pblock) {
		//释放数据块
		list_table_.free_block(pblock);
		return 0;
	}

	return -1;
}

int NumTable::remove_all_num(void) {

	//释放索引表
	if (pindex_table_) {
		if (pindex_table_->empty_index_table(index_table_id_)<0) {
			return -1;
		}
	}

	//释放数据节点
	list_table_.free_all_block();

	return 0;
}

LIST_BLOCK_HEAD* NumTable::find_num(char* number, unsigned int number_len) {

	LIST_BLOCK_HEAD* pblock = (LIST_BLOCK_HEAD*)pindex_table_->find_number(index_table_id_, number, number_len);
	
	if (pblock == NULL) {
		return NULL;
	}

	return pblock;
}

LIST_BLOCK_HEAD* NumTable::find_num(char* number, unsigned int number_len, unsigned int mode) {

	LIST_BLOCK_HEAD* pblock = (LIST_BLOCK_HEAD*)pindex_table_->find_number_with_mode(index_table_id_, number, number_len, mode);

	if (pblock == NULL) {
		return NULL;
	}

	return pblock;
}

