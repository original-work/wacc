/*
* =========================================================
*
*       Filename:  info_shm.cpp
*
*    Description:  
*
*        Version:  1.0
*        Created:  2013.6.26
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  Zhang Huanli (zhanghl@liangjiang.com)
*   Organization:  lj
*
* ==========================================================
* ================== CHANGE REPORT HISTORY ==================
*|VERSION|UPDATE DATE|UPDATED BY|DESCRIPTION OF CHANGE|*

* ================== END OF CHANGE REPORT HISTORY ===========
*/
#include "info_shm.h"
#include <strings.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>


InfoShm::InfoShm() {
	initialized_ = false;
	phead_ = NULL;
}

InfoShm::~InfoShm() {
	if (phead_ != NULL) {
		free();
	}
}

int InfoShm::acquire(unsigned int key, unsigned int shmsize, bool be_created) {
	key_ = key;
	
	shmmid_ = shmget(key_, 0, 0);
	if (shmmid_<0) {
		if (be_created) {
			shm_size_ = shmsize;
			shmmid_ = shmget(key_, shm_size_, IPC_CREAT|0666);
			if (shmmid_<0) {
				return -1;
			} else {
				phead_ = (char*)shmat(shmmid_, 0, 0);
				if (phead_==(char*)-1) {
					return -1;
				}
				bzero(phead_, shm_size_);
			}
		} else {
			return -1;
		}
	} else {
		phead_ = (char*)shmat(shmmid_, 0, 0);
		if (phead_ == (char*)-1) {
			return -1;
		}
	}

	initialized_ = true;
	return 0;
}

int InfoShm::free() {
	if(shmdt(phead_)==-1)
	{
		return -1;
	}
	if(shmctl(shmmid_, IPC_RMID, NULL)==-1)
	{
		return -1;
	}

	phead_ = NULL;
	initialized_ = false;
	return 0;
}

//char* InfoShm::attach_shm(unsigned int shmid) {
//	if (shmmid_<0) {
//		return NULL;
//	}
//
//	phead_ = (char*)shmat(shmmid_, 0, 0);
//	if (phead_==(char*)-1) {
//		return NULL;
//	}
//
//	return phead_;
//}
//
//int InfoShm::detach_shm(unsigned int shmid) {
//	if(shmdt(phead_)==-1)
//	{
//		return -1;
//	}
//	return 0;
//}

