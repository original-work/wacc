/*
* =========================================================
*
*       Filename:  file_write.h
*
*    Description:  ʵ�����ļ�д�����������Ĺ���
*
*        Version:  1.0
*        Created:  2013.6.14
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

#if !defined(_SAUAPP_COMMON_FILEWRITE_H)
#define _SAUAPP_COMMON_FILEWRITE_H
#include <stdio.h>
#include <string>


class FileWrite {
public:
	FileWrite();
	~FileWrite();
	/* ���ļ� */
	int open_file(const char* tfile);
	/* �ر��ļ� */
	int close_file();
	/* ��ȡ�ļ���������д�ļ���ϸ·�� */
	std::string file() const {return file_;}
	/* ��ȡ�ļ���С */
	unsigned int get_file_length();
	/* ����ƫ������д���� */
	unsigned int write_file(const char *buf, const unsigned int length, const unsigned int offset);
	/* ���ļ�βд���� */
	unsigned int write_file(const char *buf, const unsigned int length);
private:
	int fp_;
	std::string file_;
};

#endif  //_SAUAPP_COMMON_FILEWRITE_H
