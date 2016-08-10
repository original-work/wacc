/*
* =========================================================
*
*       Filename:  common_db.h
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
#if !defined(_COMMONDB_H)
#define _COMMONDB_H


#pragma pack(1)

typedef struct ListBlockHead {
	unsigned char used;
	struct ListBlockHead *pnext;
} LIST_BLOCK_HEAD;

typedef struct {
	unsigned int block_count;
	unsigned int block_len;
	unsigned int used_count;
	LIST_BLOCK_HEAD *phead;
	LIST_BLOCK_HEAD *ptail;
	unsigned char *block_buf;
} LIST_HEAD;

#define NUM_MAX_COUNT	10
typedef struct NumIndexBlock{
	LIST_BLOCK_HEAD block_head;
	struct NumIndexBlock *num_index[NUM_MAX_COUNT];
	unsigned char end_node[NUM_MAX_COUNT];	//0表示末尾节点，1表示非末尾节点
} NUM_INDEX_BLOCK;

typedef struct {
	unsigned char used;
	unsigned int min_num_len;
	unsigned int max_num_len;
	NUM_INDEX_BLOCK *num_sec_list;
} NUM_SEC_LIST;

typedef struct {
	NUM_SEC_LIST *num_table_list;
	unsigned int index_table_count;
} NUM_INDEX_TABLE_HEAD;

#pragma pack()

//#define NUM_END_MASK	0X80000000
//#define NUM_INDEX_MASK	0X7FFFFFFF

#define SEARCH_MODE_MINLEN	0
#define SEARCH_MODE_MAXLEN	1
#define SEARCH_MODE_WHOLE	2

#endif  //_COMMONDB_H
