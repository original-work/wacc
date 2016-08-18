/*
* =========================================================
*
*       Filename:  list_table.h
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
* =========================================================
* ==================== CHANGE REPORT HISTORY ===================
*|VERSION|UPDATE DATE|UPDATED BY|DESCRIPTION OF CHANGE|*

* ==================== END OF CHANGE REPORT HISTORY ============
*/
#if !defined(_LISTTABLE_H)
#define _LISTTABLE_H
#include "common_db.h"


class ListTable {
public:
	ListTable();
	~ListTable();
	int init_list_block(unsigned int block_count, unsigned int block_len);
	LIST_BLOCK_HEAD* get_specific_block(unsigned int id);
	unsigned int get_used_num();
	LIST_BLOCK_HEAD* get_block();
	void free_block(LIST_BLOCK_HEAD* pblock);
	void free_all_block();
	LIST_HEAD* list_head() {return &list_head_;}
private:
	LIST_HEAD list_head_;
};

#endif  //_LISTTABLE_H
