/*
* =========================================================
*
*       Filename:  info_shm.h
*
*    Description:  �����ڴ����
*
*        Version:  1.0
*        Created:  2013.6.26
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
#if !defined(_SAU_COMMON_INFOSHM_H)
#define _SAU_COMMON_INFOSHM_H


class InfoShm {
public:
	InfoShm();
	~InfoShm();
	/* ��ȡ�����ڴ� */
	int acquire(unsigned int key, unsigned int shmsize = 0, bool be_created = false);
	/* �ͷŹ����ڴ� */
	int free();
	char* head() {return phead_;}
	unsigned int shm_size() const {return shm_size_;}
	bool initialized() const {return initialized_;}
	unsigned int key() const {return key_;}
//protected:
//	char*  attach_shm(unsigned int shmid);
//	int detach_shm(unsigned int shmid);
private:
	unsigned int shm_size_;
	unsigned int key_;
	int shmmid_;
	/* �����ڴ��׵�ַ */
	char *phead_;
	bool initialized_;
};

#endif  //_SAU_COMMON_INFOSHM_H
