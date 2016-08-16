/*
 * ============================================================
 *
 *       Filename:  num_double_index_table.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2014/2/26 10:37:37
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  wangxx
 *   Organization:  lj
 *
 * =============================================================
 * ==================== CHANGE REPORT HISTORY ===================
 *|VERSION|UPDATE DATE|UPDATED BY|DESCRIPTION OF CHANGE|*

 * ==================== END OF CHANGE REPORT HISTORY ============
 */
#include "num_double_index_table.h"
#include <stdio.h>
#include "common_logger.h"


NumDoubleIndexTable::NumDoubleIndexTable() {
	self_index_ = false;
	pindex_table_ = NULL;
}

NumDoubleIndexTable::~NumDoubleIndexTable() {
	if (self_index_) {
		if (pindex_table_ != NULL) {
			delete pindex_table_;
		}
	}
}

int NumDoubleIndexTable::init_num_table(unsigned int max_count, unsigned int max_size) {
	list_table_.init_list_block(max_count, max_size);
	return 0;
}

int NumDoubleIndexTable::init_num_index(unsigned int index_count, unsigned int max_num_length) {

	if (pindex_table_ == NULL) {
		pindex_table_ = new NumIndexTable();
		self_index_ = true;
		pindex_table_->init_num_index_table(index_count, max_num_length, 2);
		if ((index_one_id_ = pindex_table_->acquire_index_table(1, max_num_length)) == -1) {
			return -1;
		}

		if ((index_two_id_ = pindex_table_->acquire_index_table(1, max_num_length)) == -1) {
			return -1;
		}

		return 0;
	}

	return -1;
}

void NumDoubleIndexTable::pindex_table(NumIndexTable *index) {
	if (self_index_ == true) {
		return;
	}

	pindex_table_ = index;
}

void NumDoubleIndexTable::index_one_id(int index_id) {
	index_one_id_ = index_id;
}

void NumDoubleIndexTable::index_two_id(int index_id) {
	index_two_id_ = index_id;
}

LIST_BLOCK_HEAD* NumDoubleIndexTable::add_num(char* imsi, 
			unsigned int imsi_len, 
			char* msisdn, 
			unsigned int msisdn_len) {

	//获取一个空的节点
	LIST_BLOCK_HEAD* pblock = NULL;

	if (imsi_len >0 || msisdn_len > 0) {
		pblock = list_table_.get_block();
	}

	if (pblock) {
		if (imsi_len >0) {
			if (pindex_table_->add_number(index_one_id_, imsi, imsi_len, pblock) < 0) {
				list_table_.free_block((LIST_BLOCK_HEAD*)pblock);
				pblock=0;
				return pblock;
			}
		}

		if (msisdn_len >0) {
			if (pindex_table_->add_number(index_two_id_, msisdn, msisdn_len, pblock) < 0) {
				pindex_table_->remove_number(index_one_id_, imsi, imsi_len);
				list_table_.free_block((LIST_BLOCK_HEAD*)pblock);
				pblock=0;
				return pblock;
			}
		}
	}

	return pblock;
}

int NumDoubleIndexTable::add_imsi(char* imsi, 
			unsigned int imsi_len, 
			LIST_BLOCK_HEAD *addr) {

	if (imsi_len == 0 || addr == NULL) {
		return -1;
	}

	if (pindex_table_->add_number(index_one_id_, imsi, imsi_len, addr) < 0) {
		return -1;
	}

	return 0;
}

int NumDoubleIndexTable::add_msisdn(char* msisdn, 
		unsigned int msisdn_len, 
		LIST_BLOCK_HEAD *addr) {

	if (msisdn_len == 0 || addr == NULL) {
		return -1;
	}

	if (pindex_table_->add_number(index_two_id_, msisdn, msisdn_len, addr) < 0) {
		return -1;
	}

	return 0;
}

int NumDoubleIndexTable::remove_num(char* imsi, 
			unsigned int imsi_len, 
			char* msisdn, 
			unsigned int msisdn_len) {

	LIST_BLOCK_HEAD* pblock = NULL;
	//从索引表中删除
	if (pindex_table_) {
		if (imsi_len > 0) {
			pblock = (LIST_BLOCK_HEAD*)pindex_table_->remove_number(index_one_id_, imsi, imsi_len);
		}

		if (msisdn_len > 0) {
			pblock = (LIST_BLOCK_HEAD*)pindex_table_->remove_number(index_two_id_, msisdn, msisdn_len);
		}
	}

	if (pblock) {
		//释放数据块
		list_table_.free_block(pblock);
		return 0;
	}

	return -1;
}


int NumDoubleIndexTable::remove_imsi(char* imsi, 
		unsigned int imsi_len) {

	LIST_BLOCK_HEAD* pblock = NULL;

	if (pindex_table_) {
		if (imsi_len > 0) {
			pblock = (LIST_BLOCK_HEAD*)pindex_table_->remove_number(index_one_id_, imsi, imsi_len);
		}
	}

	if (pblock) {
		return 0;
	} else {
		return -1;
	}
}

int NumDoubleIndexTable::remove_msisdn(char* msisdn, 
		unsigned int msisdn_len) {

	LIST_BLOCK_HEAD* pblock = NULL;

	if (pindex_table_) {
		if (msisdn_len > 0) {
			pblock = (LIST_BLOCK_HEAD*)pindex_table_->remove_number(index_two_id_, msisdn, msisdn_len);
		}
	}

	if (pblock) {
		return 0;
	} else {
		return -1;
	}
}

int NumDoubleIndexTable::remove_all_num(void) {

	//释放索引表
	if (pindex_table_) {
		pindex_table_->empty_index_table(index_one_id_);

		pindex_table_->empty_index_table(index_two_id_);
	}

	//释放数据节点
	list_table_.free_all_block();

	return 0;
}

LIST_BLOCK_HEAD* NumDoubleIndexTable::find_imsi(char* imsi, 
			unsigned int imsi_len) {

	LIST_BLOCK_HEAD* pblock = (LIST_BLOCK_HEAD*)pindex_table_->find_number(index_one_id_, imsi, imsi_len);
	
	if (pblock == NULL) {
		return NULL;
	}

	return pblock;
}

LIST_BLOCK_HEAD* NumDoubleIndexTable::find_imsi(char* imsi, 
			unsigned int imsi_len, 
			unsigned int mode) {

	LIST_BLOCK_HEAD* pblock = (LIST_BLOCK_HEAD*)pindex_table_->find_number_with_mode(index_one_id_, imsi, imsi_len, mode);

	if (pblock == NULL) {
		return NULL;
	}

	return pblock;
}

LIST_BLOCK_HEAD* NumDoubleIndexTable::find_msisdn(char* msisdn, 
			unsigned int msisdn_len) {

	LIST_BLOCK_HEAD* pblock = (LIST_BLOCK_HEAD*)pindex_table_->find_number(index_two_id_, msisdn, msisdn_len);
	
	if (pblock == NULL) {
		return NULL;
	}

	return pblock;
}

LIST_BLOCK_HEAD* NumDoubleIndexTable::find_msisdn(char* msisdn, 
			unsigned int msisdn_len, 
			unsigned int mode) {

	LIST_BLOCK_HEAD* pblock = (LIST_BLOCK_HEAD*)pindex_table_->find_number_with_mode(index_two_id_, msisdn, msisdn_len, mode);

	if (pblock == NULL) {
		return NULL;
	}

	return pblock;
}

