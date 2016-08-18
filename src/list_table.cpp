/*
* =========================================================
*
*       Filename:  list_table.cpp
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
#include "list_table.h"
#include <stdio.h>
#include <string.h>


ListTable::ListTable() {
	list_head_.block_buf = NULL;
}

ListTable::~ListTable() {
	if (list_head_.block_buf) {
		delete[] list_head_.block_buf;
	}
}

int ListTable::init_list_block(unsigned int block_count, unsigned int block_len) {
	list_head_.block_count = block_count;
	list_head_.block_len = block_len;

	list_head_.block_buf = new unsigned char[block_count*block_len];
	memset(list_head_.block_buf, 0, block_count*block_len);

	if (list_head_.block_buf) {
		list_head_.used_count = 0;

		list_head_.phead = (LIST_BLOCK_HEAD*)(list_head_.block_buf);
		list_head_.ptail = (LIST_BLOCK_HEAD*)(list_head_.block_buf + (block_count-1)*block_len);

		LIST_BLOCK_HEAD *pblock_head = NULL;
		for (unsigned int i=0; i<(block_count-1); ++i) {
			pblock_head = (LIST_BLOCK_HEAD*)(list_head_.block_buf+i*block_len);
			pblock_head->pnext = (LIST_BLOCK_HEAD*)(list_head_.block_buf+(i+1)*block_len);
		}

		list_head_.ptail->pnext = 0;
	}

	return 0;
}

LIST_BLOCK_HEAD* ListTable::get_specific_block(unsigned int id) {
	LIST_HEAD* lihead = (LIST_HEAD*)list_head_.block_buf;
	LIST_BLOCK_HEAD *pblock_head = lihead->phead;

	for(unsigned int i=0;i<id;i++) {
		pblock_head = pblock_head->pnext;
	}
	return pblock_head;
}

unsigned int ListTable::get_used_num(){
	return list_head_.used_count;
}

LIST_BLOCK_HEAD* ListTable::get_block() {

	LIST_BLOCK_HEAD *pblock_head = NULL;

	if (list_head_.phead && list_head_.block_buf) {
		pblock_head = list_head_.phead;
		list_head_.phead = list_head_.phead->pnext;

		if (list_head_.phead == 0) {
			list_head_.ptail = 0;
		}

		pblock_head->used = 1;
		++list_head_.used_count;
	}

	return pblock_head;
}

void ListTable::free_block(LIST_BLOCK_HEAD* pblock) {

	if (pblock && list_head_.block_buf) {
		list_head_.ptail->pnext = pblock;
		list_head_.ptail = pblock;
		list_head_.ptail->used = 0;
		list_head_.ptail->pnext = 0;

		if (list_head_.phead == 0) {
			list_head_.phead = list_head_.ptail;
		}

		--list_head_.used_count;
	}
}

void ListTable::free_all_block() {

	if (list_head_.block_buf) {
		list_head_.used_count = 0;
		memset(list_head_.block_buf, 0, list_head_.block_count*list_head_.block_len);

		list_head_.phead = (LIST_BLOCK_HEAD*)(list_head_.block_buf);
		list_head_.ptail = (LIST_BLOCK_HEAD*)(list_head_.block_buf + (list_head_.block_count-1)*list_head_.block_len);

		LIST_BLOCK_HEAD *pblock_head = NULL;
		for (unsigned int i=0; i<(list_head_.block_count-1); ++i) {
			pblock_head = (LIST_BLOCK_HEAD*)(list_head_.block_buf+i*list_head_.block_len);
			pblock_head->pnext = (LIST_BLOCK_HEAD*)(list_head_.block_buf+(i+1)*list_head_.block_len);
		}

		list_head_.ptail->pnext = 0;
	}
}

