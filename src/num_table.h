/*
* =========================================================
*
*       Filename:  num_table.h
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
#if !defined(_NUMTABLE_H)
#define _NUMTABLE_H
#include "num_index_table.h"
#include "list_table.h"


class NumTable {
public:
	NumTable();
	~NumTable();

	int init_num_table(unsigned int max_count, unsigned int max_size);

	int init_num_index(unsigned int index_count, 
		unsigned int max_num_length, 
		unsigned int index_table_count);

	void pindex_table(NumIndexTable *index, int index_table_id);

	LIST_BLOCK_HEAD* add_num(char* number, 
		unsigned int number_Len);

	int remove_num(char* number, unsigned int number_len);
	int remove_all_num(void);
	LIST_BLOCK_HEAD* find_num(char* number, unsigned int number_len);
	LIST_BLOCK_HEAD* find_num(char* number, unsigned int number_len, unsigned int mode);

	ListTable* list_table() {return &list_table_;}
	bool self_index() const {return self_index_;}
	int index_table_id() const {return index_table_id_;}
	NumIndexTable* pindex_table() {return pindex_table_;}

private:
	NumIndexTable *pindex_table_;
	int index_table_id_;
	ListTable list_table_;
	bool self_index_;
};

#endif  //_NUMTABLE_H
