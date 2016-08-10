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
	/* �����ڴ棬��ʼ����Ϣ���� */
	int init(unsigned int block_num, unsigned int block_size);
	unsigned int block_num() const {return block_num_;}
	unsigned int block_size() const  {return block_size_;}
	/* ��ȡδ��ռ�õĶ��и��� */
	unsigned int unused_block_num();
	/* ������β������Ϣ */
	bool insert_record(const char *pmsg, unsigned int msg_len);
	/* �Ӷ����ײ���ȡ��Ϣ */
	bool get_front_record(char *&pmsg, unsigned int &len);
	/* ǰ�ƶ��������� */
	void advance_ridx();
	/* ǰ��д�������� */
	void advance_widx();
private:
	/* ���п��� */
	unsigned int block_num_;
	/* ���С */
	unsigned int block_size_;
	/* ����β��д���� */
	unsigned int widx_;
	/* �����ײ������� */
	unsigned int ridx_;
	/* ��Ϣ�����ڴ��׵�ַ */
	char *pmsg_buf_;
};

#endif  //_SAUAPP_COMMON_MSGLIST_H
