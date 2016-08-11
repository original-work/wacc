/*
 * =====================================================================================
 *
 *       Filename:  appreq_handler.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2015/3/9 13:40:25
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zhanghl (zhl), zhanghl@liangjiang.com
 *   Organization:  lj
 *
 * =====================================================================================
 * ============================== CHANGE REPORT HISTORY ================================
 * |   VERSION   |   UPDATE DATE   |   UPDATED BY   |   DESCRIPTION OF CHANGE   |*

 * =========================== END OF CHANGE REPORT HISTORY ============================
 */
#include "appreq_handler.h"
#include "usracc_common.h"
#include "usracc_config.h"
#include "tid_generator.h"
#include "common_logger.h"
#include "tools.h" /* luchq add 2015-06-16 */
#include <sys/types.h>
#include <stdio.h>
#include <time.h>
/*
 *--------------------------------------------------------------------------------------
 *       Class:  AppReqHandler
 *      Method:  AppReqHandler
 * Description:  constructor
 *--------------------------------------------------------------------------------------
 */


AppReqHandler::AppReqHandler()
{
	memset(msisdn_, 0, sizeof(msisdn_));
	memset(bcd_buf_, 0, sizeof(bcd_buf_));
	offset_ = 0;
	send_offset_ = 0;
}  /* -----  end of method AppReqHandler::AppReqHandler(constructor)  ----- */

AppReqHandler::~AppReqHandler()
{

}  /* -----  end of method AppReqHandler::~AppReqHandler()  ----- */

int AppReqHandler::handle_recv()
{
	memset(recv_buf_,0,sizeof(recv_buf_)); /* luchq add 2015-08-03 */
	CommonLogger::instance().log_info("AppReqHandler::handle_recv: offset_=%u", offset_);
	
    int ret = recvn(recv_buf_+offset_, kBufferSize/2);
    if (ret<0) {
		CommonLogger::instance().log_error("AppReqHandler::handle_recv: error ret %d, offset %d", ret, offset_);
        return -1;
    }

    //CommonLogger::instance().log_info("ret=%u", ret);

    unsigned int buf_length = offset_ + ret;

    if (buf_length<=(sizeof(NIF_MSG_UNIT2)-sizeof(unsigned char*))) {
        offset_ += ret;
		CommonLogger::instance().log_error("AppReqHandler::handle_recv: length error, length=%d", buf_length);
        return 0;
    }

    NIF_MSG_UNIT2 *header = (NIF_MSG_UNIT2*)recv_buf_;

   CommonLogger::instance().log_error("recv_buf_ = 0x%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x\n",
	(unsigned char)recv_buf_[0],(unsigned char)recv_buf_[1],
	(unsigned char)recv_buf_[2],(unsigned char)recv_buf_[3],
	(unsigned char)recv_buf_[4],(unsigned char)recv_buf_[5],
	(unsigned char)recv_buf_[6],(unsigned char)recv_buf_[7]);

    unsigned int msg_len = ntohl(header->length);
    CommonLogger::instance().log_info("AppReqHandler::handle_recv: msg_len=%u", msg_len);

    if (msg_len>buf_length - (sizeof(NIF_MSG_UNIT2)-sizeof(unsigned char*))) {
        offset_ += ret;
        return 0;
    }

    //CommonLogger::instance().log_info("2ret=%u", ret);

    offset_ = 0;
    while (offset_<buf_length) {
        //CommonLogger::instance().log_info("3ret=%u", ret);
        process(recv_buf_+offset_);
        offset_ += (sizeof(NIF_MSG_UNIT2) - sizeof(unsigned char*));
        offset_ += msg_len;

        if ((buf_length-offset_)<=(sizeof(NIF_MSG_UNIT2)-sizeof(unsigned char*))) {
            break;
        }

        header = (NIF_MSG_UNIT2*)(recv_buf_+offset_);
        msg_len = ntohl(header->length);

        if (msg_len>buf_length-offset_-(sizeof(NIF_MSG_UNIT2)-sizeof(unsigned char*))) {
            break;
        }
    }
    memcpy(recv_buf_, recv_buf_+offset_, buf_length-offset_);
    offset_ = buf_length-offset_;

    return 0;
}		/* -----  end of method AppReqHandler::handle_recv  ----- */

int AppReqHandler::process(char *pmsg)
{
    NIF_MSG_UNIT2 *header = (NIF_MSG_UNIT2*)pmsg;

    unsigned int msg_type = ntohl(header->invoke);
    //unsigned int msg_dialog = ntohl(header->dialog);
    unsigned int msg_length = ntohl(header->length);

	/* luchq add 2015-06-15 */
	CommonLogger::instance().log_info("head    =0x%08x", ntohl(header->head));
	CommonLogger::instance().log_info("dIpAdrs =0x%08x", ntohl(header->dIpAdrs));
	CommonLogger::instance().log_info("sIpAdrs =0x%08x", ntohl(header->sIpAdrs));
	CommonLogger::instance().log_info("version =0x%08x", ntohl(header->version));
	CommonLogger::instance().log_info("invoke  =0x%08x", ntohl(header->invoke));
	CommonLogger::instance().log_info("dialog  =0x%08x", ntohl(header->dialog));
	CommonLogger::instance().log_info("seq     =0x%08x", ntohl(header->seq));
	CommonLogger::instance().log_info("length  =0x%08x", ntohl(header->length));
	CommonLogger::instance().log_info("msg content:");
	tools::print_hex((unsigned char*)(pmsg+sizeof(NIF_MSG_UNIT2)-sizeof(unsigned char*)),msg_length);
	/* end of luchq add */

    switch (msg_type) {
    case SEND_SMS_REQ:
	CommonLogger::instance().log_info("AppReqHandler: Recv a MO msg");
        deal_MO(pmsg);
        break;
    case ACTIVE_REQ:
	CommonLogger::instance().log_info("AppReqHandler: Recv a ACTIVE msg");
        deal_user_active(pmsg);
        break;
    case DEACTIVE_REQ:
	CommonLogger::instance().log_info("AppReqHandler: Recv a DEACTIVE msg");
        deal_user_deactive(pmsg);
        break;
    case CALL_REPORT_REQ:
	CommonLogger::instance().log_info("AppReqHandler: Recv a CALL REPORT msg");
        deal_call_record(pmsg);
        break;
    case PUSH_SMS_REQ:
	CommonLogger::instance().log_info("AppReqHandler: Recv a MT-ACK msg");
        deal_mt_ack(pmsg);
        break;
    default:
        break;
    }
	CommonLogger::instance().log_error("\r\n\r\n\r\n");
    return 0;
}		/* -----  end of method AppReqHandler::process(char *pmsg)  ----- */

int AppReqHandler::deal_call_record(char *data)
{
	return 0;
}/* -----  end of method AppReqHandler::deal_call_record(char *data)  ----- */


int AppReqHandler::deal_user_active(char *data)
{
	ReqMsg red_msg;

	red_msg.msg_type = 1;

	ActivateMsg *record = (ActivateMsg*)red_msg.msg;

	NIF_MSG_UNIT2 *header = (NIF_MSG_UNIT2*)data;
	unsigned int msg_length = ntohl(header->length);

	if (msg_length % sizeof(AddUser) != 0)
	{
		CommonLogger::instance().log_error("deal_user_active: length error");
		return 0;
	}
	/* 获取用户激活信息re*/
	AddUser *re = (AddUser*)(data+sizeof(NIF_MSG_UNIT2)-sizeof(unsigned char*));

	memcpy(msisdn_, re->mdn, strlen(re->mdn));

	StrToBCD(re->mdn, bcd_buf_, sizeof(bcd_buf_));
	ActiveUser* user = (ActiveUser*)info_mgr_->active_usr_table_.find_num((char*)bcd_buf_, strlen(re->mdn));
	if (user != NULL)
	{
		/* 用户已经激活/注册*/
		CommonLogger::instance().log_debug("deal_user_active: user already exists %s", re->mdn);
		CommonLogger::instance().log_debug("deal_user_active: msisdn %s",user->msisdn);
		record->actived = 1;

		if (user->fd != sockfd())
		{
			/* 用户重新连接了，需要更新FD */
			map<int, BaseCollectionHandler*>::iterator iter = client_list_->find(user->fd);
			if (iter != client_list_->end())
			{
				/* 存在相同FD 的用户信息，如果用户号码相同，则删除该记录*/
				if (strcmp(msisdn_, ((AppReqHandler*)iter->second)->msisdn()) == 0)
				{
					CommonLogger::instance().log_debug("deal_user_active:	sockfd is different, but msisdn is same, so delete the record ");
					delete iter->second;
					close(iter->first);
					client_list_->erase(iter);
				}
			}
			CommonLogger::instance().log_debug("deal_user_active: Update sockfd ");
			user->fd = sockfd();
		}
	}
	else
	{
		CommonLogger::instance().log_debug("deal_user_active: New active user is %s ", re->mdn);

		user = (ActiveUser*)info_mgr_->active_usr_table_.add_num((char*)bcd_buf_, strlen(re->mdn));
		if (user == NULL)
		{
			CommonLogger::instance().log_error("deal_user_active: failed to add new active user %s", re->mdn);
			return -1;
		}

		record->actived = 0;

		user->fd = sockfd();
		user->conn_cnt = UsrAccConfig::instance().serv_logic_server_list().size();
		user->reconnect_cnt_list = (unsigned int*)(((char*)user)+sizeof(ActiveUser));
		memset(user->reconnect_cnt_list, 0, user->conn_cnt*sizeof(unsigned int));
		memcpy(user->msisdn, re->mdn, strlen(re->mdn));
	}
	user->operTime = time(NULL); /* luchq add 2016-02-01 */
	memcpy(record->msisdn, re->mdn, sizeof(record->msisdn));

	record->user_info = user;
	record->mod_id = UsrAccConfig::instance().module_id();
	record->tid = generate_tid();
	info_mgr_->add_tid_msisdn(record->tid, record->msisdn);
	//CommonLogger::instance().log_debug("record  msg %u", record->tid);

	app_req_queue_->insert_record((char*)&red_msg, sizeof(ReqMsg));
	app_req_queue_->advance_widx();

	return 0;
}		/* -----  end of method AppReqHandler::deal_user_active  ----- */

int AppReqHandler::deal_user_deactive(char *data)
{
	return 0;
}		/* -----  end of method AppReqHandler::deal_user_deactive  ----- */


int AppReqHandler::deal_MO(char *data)
{
	ReqMsg red_msg;

	memset((char*)&red_msg,0,sizeof(red_msg));
    red_msg.msg_type = 3;
    SMSData *record = (SMSData*)red_msg.msg;

	//CommonLogger::instance().log_info("AppReqHandler: deal MO Msg");

    MOData *re = (MOData*)(data+sizeof(NIF_MSG_UNIT2)-sizeof(unsigned char*));

	StrToBCD(re->cg, bcd_buf_, sizeof(bcd_buf_));
    ActiveUser* user = (ActiveUser*)info_mgr_->active_usr_table_.find_num((char*)bcd_buf_, strlen(re->cg));
	if (user != NULL)
	{

			/*memcpy(record->cg, re->cg, sizeof(record->cg));
			memcpy(record->cd, re->cd, sizeof(record->cd));
			memcpy(record->sms_center, re->smsc, sizeof(record->sms_center));
			memcpy(record->sms_content, re->content, sizeof(record->sms_content));*/
			memcpy(record->cg, re->cg, strlen(re->cg));
			/*memcpy(record->cd, re->cd, strlen(re->cd));*/
			/* luchq add 2015-12-11 */
			if (re->cd[0] == '+')
			{
				memcpy(record->cd, &re->cd[1], strlen(re->cd)-1);
			}
			else
			{
				memcpy(record->cd, re->cd, strlen(re->cd));
			}
			memcpy(record->sms_center, re->smsc, strlen(re->smsc));
			memcpy(record->sms_content, re->content, strlen(re->content));

			CommonLogger::instance().log_info("deal_MO: cd:%s",re->cd);
			CommonLogger::instance().log_info("deal_MO: re cg len:%d, num:%s",strlen(re->cg),re->cg);
			CommonLogger::instance().log_info("deal_MO: record cg len:%d, num:%s",strlen(record->cg),record->cg);
			CommonLogger::instance().log_info("sms len:%d,content:", strlen(re->content));
			tools::print_hex((unsigned char*)re->content,strlen(re->content));/* luchq add 2015-06-18 */

			record->sms_code = re->sms_code;
			record->tid = generate_tid();
			app_req_queue_->insert_record((char*)&red_msg, sizeof(ReqMsg));
			app_req_queue_->advance_widx();
			CommonLogger::instance().log_info("deal_MO: insert MO Msg into app_req_queue");

	}
	else
	{
		CommonLogger::instance().log_error("deal_MO: Call find_num fail");
	}

    return 0;
}		/* -----  end of method AppReqHandler::deal_MO  ----- */

int AppReqHandler::deal_mt_ack(char *data)
{
	ReqMsg red_msg;

	memset((char*)&red_msg,0,sizeof(red_msg));
    red_msg.msg_type = 9;
    MTAckMsg *record = (MTAckMsg*)red_msg.msg;

    NIF_MSG_UNIT2 *header = (NIF_MSG_UNIT2*)data;
    unsigned int msg_length = ntohl(header->length);

	if (msg_length % sizeof(UsrMTAckData) != 0)
	{
		return 0;
	}

    UsrMTAckData *re = (UsrMTAckData*)(data+sizeof(NIF_MSG_UNIT2)-sizeof(unsigned char*));

	/*memcpy(record->cd, msisdn_, sizeof(record->cd));*/
	memcpy(record->cd, msisdn_, strlen(msisdn_));
	record->result = ntohl(re->result);
	record->tid = ntohl(re->tid);
	record->seq = re->seq;
	CommonLogger::instance().log_info("deal_mt_ack: cd=%s",record->cd);
	CommonLogger::instance().log_info("deal_mt_ack: result=%u, tid=%u",record->result, record->tid);
	app_req_queue_->insert_record((char*)&red_msg, sizeof(ReqMsg));
	app_req_queue_->advance_widx();
	CommonLogger::instance().log_info("deal_mt_ack: insert MT-ack Msg into app_req_queue_");
    return 0;
}

unsigned int AppReqHandler::generate_tid()
{
    return TidGenerator::instance().generator_tid();
}		/* -----  end of method AppReqHandler::generate_tid  ----- */

#if 0
int AppReqHandler::logout_notification(int fd)
{
	char send_buf[256];
	NIF_MSG_UNIT2 *unit = (NIF_MSG_UNIT2*)send_buf;
	unit->invoke = htonl(OFFLINE_REQ);
	unit->length = 0;
	unit->dialog = htonl(BEGIN);

	/* luchq modify 2015-10-28 	放弃使用函数sendn()，改用下面的方法发送消息，目的是可以指定fd
	if (-1 == sendn(send_buf, sizeof(NIF_MSG_UNIT2)-sizeof(unsigned char*)))
	{
		CommonLogger::instance().log_info("logout_notification: call sendn fail!");
	}*/
	CommonLogger::instance().log_info("logout_notification: fd=%d",fd);
	map<int,BaseCollectionHandler*>::iterator iter = client_list_->find(fd);
	if (iter != client_list_->end())
	{
		BaseCollectionHandler *handler = iter->second;
		if (-1 == handler->logout(send_buf, sizeof(NIF_MSG_UNIT2)-sizeof(unsigned char*)))
		{
			CommonLogger::instance().log_info("logout_notification: call logout fail!");
		}
	}
	else
	{
		CommonLogger::instance().log_info("logout_notification: can't find fd!");
	}
    	return 0;
}		/* -----  end of method AppReqHandler::logout_notification  ----- */
#endif
int AppReqHandler::logout_notification(int fd)
{
	char send_buf[256];
	NIF_MSG_UNIT2 *unit = (NIF_MSG_UNIT2*)send_buf;
	unit->invoke = htonl(OFFLINE_REQ);
	unit->length = 0;
	unit->dialog = htonl(BEGIN);
	if (-1 == sendn(send_buf, sizeof(NIF_MSG_UNIT2)-sizeof(unsigned char*)))
	{
		CommonLogger::instance().log_info("logout_notification: call sendn fail!");
	}

    	return 0;
}		/* -----  end of method AppReqHandler::logout_notification  ----- */


void AppReqHandler::info_mgr(InfoMemMgr *p)
{
    info_mgr_ = p;
}

void AppReqHandler::app_req_queue(MsgList *p)
{
    app_req_queue_ = p;
}



void AppReqHandler::client_list(map<int, BaseCollectionHandler*> *list)
{
	client_list_ = list;
}

unsigned long long AppReqHandler::ntoh64(unsigned long long inval)
{
	unsigned long long outval = 0;
	for(int i=0; i <8; i++)
	{
		outval = (outval << 8) + ((inval >> (i*8)) & 255);
	}
	return	outval;
}

bool AppReqHandler::StrToBCD(const char *Src,unsigned char *Des,int iDesLen)
{
	if (NULL == Src)
	{
		return false;
	}
	if (NULL == Des)
	{
		return false;
	}
	if (0 == iDesLen)
	{
		return false;
	}
	int iSrcLen = strlen(Src);
	if(iSrcLen==0)
	{
		memset(Des,0xFF,8);
		return true;
	}
	if (iSrcLen > iDesLen * 2)
	{
		return false;
	}
	unsigned char chTemp = 0;
	int i;
	for (i = 0; i < iSrcLen; i++)
	{
		if (i % 2 == 0)
		{
			chTemp = (Src[i]-'0'); // << 4) & 0xF0;
		}
		else
		{
			chTemp = chTemp | (((Src[i]-'0')<<4) & 0xF0);
			Des[i / 2] = chTemp;
		}
	}
	if (i % 2 != 0)
	{
		Des[i / 2] = (chTemp|0xf0);
	}
	return true;
}

