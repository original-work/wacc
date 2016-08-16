/*
* =========================================================
*
*       Filename:  num_index_table.cpp
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
#include "num_index_table.h"
#include <string.h>
#include <stdio.h>
#include "common_logger.h"


NumIndexTable::NumIndexTable() {
	index_table_head_.num_table_list = NULL;
}

NumIndexTable::~NumIndexTable() {
	if (index_table_head_.num_table_list) {
		if (index_table_head_.index_table_count > 0) {
			for (unsigned int n=0; n<index_table_head_.index_table_count; ++n) {
				delete[] index_table_head_.num_table_list[n].num_sec_list;
			}
		}

		delete[] index_table_head_.num_table_list;
	}
}

int NumIndexTable::init_num_index_table(unsigned int block_count, 
	unsigned int num_max_len, 
	unsigned int index_table_count) {
	
	if (index_table_head_.num_table_list) {
		return -1;
	}

	index_table_head_.num_table_list = new NUM_SEC_LIST[index_table_count];
	index_table_head_.index_table_count = index_table_count;

	for (unsigned int n=0; n<index_table_count; ++n) {
		index_table_head_.num_table_list[n].num_sec_list = new NUM_INDEX_BLOCK[num_max_len];
		memset(index_table_head_.num_table_list[n].num_sec_list, 0, sizeof(NUM_INDEX_BLOCK)*num_max_len);
		index_table_head_.num_table_list[n].max_num_len = num_max_len;
		index_table_head_.num_table_list[n].min_num_len = 1;
		index_table_head_.num_table_list[n].used = 0;
	}

	list_table_.init_list_block(block_count, sizeof(NUM_INDEX_BLOCK));
	
	return 0;
}

int NumIndexTable::acquire_index_table(unsigned int num_min_len, unsigned int num_max_len) {
	if (index_table_head_.num_table_list && index_table_head_.index_table_count > 0) {

		for (unsigned int m=0; m<index_table_head_.index_table_count; ++m) {

			if (index_table_head_.num_table_list[m].used == 0) {

				if (index_table_head_.num_table_list[m].max_num_len >= num_max_len && 
					index_table_head_.num_table_list[m].min_num_len <= num_min_len && 
					num_max_len >= num_min_len) {

					index_table_head_.num_table_list[m].used = 1;
					return m;
				}

			}

		}

	}

	return -1;
}

int NumIndexTable::empty_index(NUM_INDEX_BLOCK* pindex) {
	NUM_INDEX_BLOCK *paddr;

	for (int j=0; j<NUM_MAX_COUNT; ++j) {
		paddr = pindex->num_index[j];
		if (paddr && pindex->end_node[j]) {
			empty_index(pindex->num_index[j]);
		}
	}

	list_table_.free_block((LIST_BLOCK_HEAD*)pindex);
	return 0;
}

int NumIndexTable::empty_index_table(unsigned int table_index_id) {

	if (table_index_id >= index_table_head_.index_table_count) {
		return -1;
	}

	if (index_table_head_.num_table_list[table_index_id].used == 0) {
		return -1;
	}

	NUM_INDEX_BLOCK *pindex_block = NULL;
	NUM_INDEX_BLOCK *addr_data;

	for (unsigned int j=index_table_head_.num_table_list[table_index_id].min_num_len-1; j<index_table_head_.num_table_list[table_index_id].max_num_len; ++j) {
		pindex_block = &(index_table_head_.num_table_list[table_index_id].num_sec_list[j]);
		for (int g=0; g<NUM_MAX_COUNT; ++g) {
			addr_data = pindex_block->num_index[g];
			if (addr_data && pindex_block->end_node[g]) {
				empty_index(pindex_block->num_index[g]);
				pindex_block->num_index[g] = 0;
				pindex_block->end_node[g] = 0;
			} else {
				if (!(pindex_block->end_node[g])) {
					pindex_block->num_index[g] = 0;
					pindex_block->end_node[g] = 0;
				}
			}
		}
	}

	return 0;
}

int NumIndexTable::add_number(unsigned int table_index_id, 
	char* number, 
	unsigned int number_len, 
	LIST_BLOCK_HEAD *data_addr) {

	if (table_index_id<0 || table_index_id>=index_table_head_.index_table_count) {
		return -1;
	}

	NUM_SEC_LIST *ptable = &(index_table_head_.num_table_list[table_index_id]);

	if (ptable->used == 0) {
		return -1;
	}

	if (number_len > ptable->max_num_len || number_len < ptable->min_num_len) {
		return -1;
	}

	NUM_INDEX_BLOCK *pindex = &(ptable->num_sec_list[number_len-1]);

	unsigned int num_value;

	//CommonLogger::instance().log_debug("number_len %u", number_len);
	for (unsigned int i=0; i<number_len; ++i) {
		if (i % 2) {
			num_value = ((number[i/2] >> 4) & 0x0F);
		} else {
			num_value = (number[i/2] & 0x0F);
		}

		//printf("%u\n", num_value);

		if (num_value >= NUM_MAX_COUNT) {
			return -1;
		}

		if (i == (number_len-1)) {
			if (pindex->num_index[num_value] == 0) {
				pindex->end_node[num_value] = 0;
				pindex->num_index[num_value] = (NUM_INDEX_BLOCK*)data_addr;
				return 0;
			} else {
				return -1;
			}
		}

		if (pindex->num_index[num_value] == 0) {
			LIST_BLOCK_HEAD *pblock = list_table_.get_block();
			if (!pblock) {
				return -1;
			}

			pindex->num_index[num_value] = (NUM_INDEX_BLOCK*)pblock;
			pindex->end_node[num_value] = 1;
		}

		if (!(pindex->end_node[num_value])) {
			return -1;
		}

		pindex = pindex->num_index[num_value];

	}

	return -1;
}

LIST_BLOCK_HEAD* NumIndexTable::remove_number(unsigned int table_index_id, 
	char* number, 
	unsigned int number_len) {

	if (table_index_id<0 || table_index_id>=index_table_head_.index_table_count) {
		return NULL;
	}

	NUM_SEC_LIST *ptable = &(index_table_head_.num_table_list[table_index_id]);

	if (ptable->used == 0) {
		return NULL;
	}

	if (number_len > ptable->max_num_len || number_len < ptable->min_num_len) {
		return NULL;
	}

	NUM_INDEX_BLOCK *pindex = &(ptable->num_sec_list[number_len-1]);

	unsigned int num_value;

	unsigned char num_value_array[51];
	NUM_INDEX_BLOCK* num_value_idx_array[51];

	for (unsigned int i=0; i<number_len; ++i) {
		if (i % 2) {
			num_value = ((number[i/2] >> 4) & 0x0F);
		} else {
			num_value = (number[i/2] & 0x0F);
		}

		if (num_value >= NUM_MAX_COUNT) {
			return NULL;
		}

		num_value_array[i] = num_value;

		if (pindex->num_index[num_value]>0) {
			num_value_idx_array[i+1] = pindex->num_index[num_value];
			pindex = pindex->num_index[num_value];
		} else {
			return NULL;
		}
	}

	NUM_INDEX_BLOCK *paddr = num_value_idx_array[number_len];

	int j;
	for (j=(number_len-1); j>0; --j) {
		num_value_idx_array[j]->num_index[num_value_array[j]] = 0;
		num_value_idx_array[j]->end_node[num_value_array[j]] = 0;

		int flag = 0;

		for (int i=0; i<NUM_MAX_COUNT; ++i) {
			if (num_value_idx_array[j]->num_index[i] != 0) {
				flag = 1;
				break;
			}
		}

		if (flag) {
			break;
		}

		list_table_.free_block((LIST_BLOCK_HEAD*)num_value_idx_array[j]);
	}

	if (j == 0) {
		ptable->num_sec_list[number_len-1].num_index[num_value_array[0]] = 0;
		ptable->num_sec_list[number_len-1].end_node[num_value_array[0]] = 0;
	}

	return (LIST_BLOCK_HEAD*)paddr;
}

LIST_BLOCK_HEAD* NumIndexTable::find_number_with_mode(unsigned int table_index_id, 
	char* number, 
	unsigned int number_len, 
	unsigned int mode) {

	if (table_index_id<0 || table_index_id>=index_table_head_.index_table_count) {
		return NULL;
	}

	NUM_SEC_LIST *ptable = &(index_table_head_.num_table_list[table_index_id]);

	if (ptable->used == 0) {
		return NULL;
	}

	if (number_len < ptable->min_num_len) {
		return NULL;
	}

	unsigned int max_len = ptable->max_num_len > number_len?number_len:ptable->max_num_len;

	LIST_BLOCK_HEAD *data = NULL;

	switch (mode) {
	case SEARCH_MODE_MINLEN:
		for (unsigned int i=ptable->min_num_len; i<=max_len; ++i) {
			data = find_number(table_index_id, number, i);
			if (data != NULL) {
				break;
			}
		}
		break;
	case SEARCH_MODE_MAXLEN:
		for (unsigned int i=max_len; i>=ptable->min_num_len; --i) {
			data = find_number(table_index_id, number, i);
			if (data != NULL) {
				break;
			}
		}
		break;
	case SEARCH_MODE_WHOLE:
		if ((number_len <= ptable->max_num_len)) {
			data = find_number(table_index_id, number, number_len);
		}
		break;
	}

	return data;
}

LIST_BLOCK_HEAD* NumIndexTable::find_number(unsigned int table_index_id, 
	char* number, 
	unsigned int number_len) {

	if (table_index_id<0 || table_index_id>=index_table_head_.index_table_count) {
		return NULL;
	}

	NUM_SEC_LIST *ptable = &(index_table_head_.num_table_list[table_index_id]);

	if (ptable->used == 0) {
		return NULL;
	}

	if (number_len > ptable->max_num_len || number_len < ptable->min_num_len) {
		return NULL;
	}

	NUM_INDEX_BLOCK *pindex = &(ptable->num_sec_list[number_len-1]);
	unsigned int num_value;

	for (unsigned int i=0; i<number_len; i++) {

		if (pindex == 0) {
			return NULL;
		}

		if (i % 2) {
			num_value = ((number[i/2] >> 4) & 0x0F);
		} else {
			num_value = (number[i/2] & 0x0F);
		}

		if (i == (number_len - 1)) {
			if (!(pindex->end_node[num_value])) {
				LIST_BLOCK_HEAD *data = (LIST_BLOCK_HEAD*)(pindex->num_index[num_value]);
				//CommonLogger::instance().log_debug("data");
				return data;
			} else {
				return NULL;
			}
		}

		if (!(pindex->end_node[num_value])) {
			return NULL;
		}

		pindex = pindex->num_index[num_value];

	}

	return NULL;
}

