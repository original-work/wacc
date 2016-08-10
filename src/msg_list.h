/*
* =========================================================
*
*       Filename:  msg_list.h
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
* =========================================================
* ==================== CHANGE REPORT HISTORY ===================
*|VERSION|UPDATE DATE|UPDATED BY|DESCRIPTION OF CHANGE|*

* ==================== END OF CHANGE REPORT HISTORY ============
*/
#if !defined(_SAUAPP_COMMON_MSGLIST_H)
#define _SAUAPP_COMMON_MSGLIST_H
#include <stdio.h>
#include <string.h>

class MsgList {
public:
	MsgList();
	~MsgList();
	/* 分配内存，初始化消息队列 */
	int init(unsigned int block_num, unsigned int block_size);
	unsigned int block_num() const {return block_num_;}
	unsigned int block_size() const  {return block_size_;}
	/* 获取未被占用的队列个数 */
	unsigned int unused_block_num();
	/* 往队列尾插入消息 */
	bool insert_record(const char *pmsg, unsigned int msg_len);
	/* 从队列首部获取消息 */
	bool get_front_record(char *&pmsg, unsigned int &len);
	/* 前移读队列索引 */
	void advance_ridx();
	/* 前移写队列索引 */
	void advance_widx();
private:
	/* 队列块数 */
	unsigned int block_num_;
	/* 块大小 */
	unsigned int block_size_;
	/* 队列尾部写索引 */
	unsigned int widx_;
	/* 队列首部读索引 */
	unsigned int ridx_;
	/* 消息队列内存首地址 */
	char *pmsg_buf_;
};

#endif  //_SAUAPP_COMMON_MSGLIST_H
