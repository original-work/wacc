/*
 * ============================================================
 *
 *       Filename:  num_double_index_table.h
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
#if !defined(_NUMDOUBLEINDEXTABLE_H)
#define _NUMDOUBLEINDEXTABLE_H
#include "num_index_table.h"
#include "list_table.h"


class NumDoubleIndexTable {
public:
	NumDoubleIndexTable();
	~NumDoubleIndexTable();

	int init_num_table(unsigned int max_count, unsigned int max_size);

	int init_num_index(unsigned int index_count, unsigned int max_num_length);

	void index_one_id(int index_id);
	void index_two_id(int index_id);
	void pindex_table(NumIndexTable *index);

	LIST_BLOCK_HEAD* add_num(char* imsi, 
			unsigned int imsi_len, 
			char* msisdn, 
			unsigned int msisdn_len);

	int add_imsi(char* imsi, 
			unsigned int imsi_len, 
			LIST_BLOCK_HEAD *addr);

	int add_msisdn(char* msisdn, 
			unsigned int msisdn_len, 
			LIST_BLOCK_HEAD *addr);

	int remove_num(char* imsi, 
			unsigned int imsi_len, 
			char* msisdn, 
			unsigned int msisdn_len);

	int remove_imsi(char* imsi, 
			unsigned int imsi_len);

	int remove_msisdn(char* msisdn, 
			unsigned int msisdn_len);

	int remove_all_num(void);

	LIST_BLOCK_HEAD* find_imsi(char* imsi, 
			unsigned int imsi_len);

	LIST_BLOCK_HEAD* find_imsi(char* imsi, 
			unsigned int imsi_len, 
			unsigned int mode);

	LIST_BLOCK_HEAD* find_msisdn(char* msisdn, 
			unsigned int msisdn_len);

	LIST_BLOCK_HEAD* find_msisdn(char* msisdn, 
			unsigned int msisdn_len, 
			unsigned int mode);

	ListTable* list_table() {return &list_table_;}
	bool self_index() const {return self_index_;}
	int index_one_id() const {return index_one_id_;}
	int index_two_id() const {return index_two_id_;}
	NumIndexTable* pindex_table() {return pindex_table_;}

private:
	NumIndexTable *pindex_table_;
	int index_one_id_;
	int index_two_id_;
	ListTable list_table_;
	bool self_index_;
};

#endif  //_NUMDOUBLEINDEXTABLE_H

