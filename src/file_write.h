/*
* =========================================================
*
*       Filename:  file_write.h
*
*    Description:  实现往文件写二进制码流的功能
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
	/* 打开文件 */
	int open_file(const char* tfile);
	/* 关闭文件 */
	int close_file();
	/* 获取文件名，包括写文件详细路径 */
	std::string file() const {return file_;}
	/* 获取文件大小 */
	unsigned int get_file_length();
	/* 根据偏移量，写内容 */
	unsigned int write_file(const char *buf, const unsigned int length, const unsigned int offset);
	/* 往文件尾写内容 */
	unsigned int write_file(const char *buf, const unsigned int length);
private:
	int fp_;
	std::string file_;
};

#endif  //_SAUAPP_COMMON_FILEWRITE_H
