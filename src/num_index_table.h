/*
* =========================================================
*
*       Filename:  num_index_table.h
*
*    Description:  
*
*        Version:  1.0
*        Created:  2013.11.29
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
#if !defined(_NUMINDEXTABLE_H)
#define _NUMINDEXTABLE_H
#include "list_table.h"
#include "common_db.h"


class NumIndexTable {
public:
	NumIndexTable();
	~NumIndexTable();
	int init_num_index_table(unsigned int block_count, 
		unsigned int num_max_len=25, 
		unsigned int index_table_count=1);

	int acquire_index_table(unsigned int num_min_len, unsigned int num_max_len);

	int empty_index(NUM_INDEX_BLOCK* pindex);
	int empty_index_table(unsigned int table_index_id);

	int add_number(unsigned int table_index_id, 
		char* number, 
		unsigned int number_len, 
		LIST_BLOCK_HEAD *data_addr);

	LIST_BLOCK_HEAD* remove_number(unsigned int table_index_id, 
		char* number, 
		unsigned int number_len);

	LIST_BLOCK_HEAD* find_number_with_mode(unsigned int table_index_id, 
		char* number, 
		unsigned int number_len, 
		unsigned int mode);

	LIST_BLOCK_HEAD* find_number(unsigned int table_index_id, 
		char* number, 
		unsigned int number_len);

	ListTable* list_table() {return &list_table_;}
	NUM_INDEX_TABLE_HEAD* index_table_head() {return &index_table_head_;}

private:
	NUM_INDEX_TABLE_HEAD index_table_head_;
	ListTable list_table_;
};

#endif  //_NUMINDEXTABLE_H
