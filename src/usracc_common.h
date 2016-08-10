/*
 * =====================================================================================
 *
 *       Filename:  usracc_common.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015/3/11 9:57:56
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
#if !defined(USRACC_COMMON_H_)
#define USRACC_COMMON_H_
#include <string>
#include <time.h>
#include "tcp_client.h"


typedef struct{
	unsigned int min_tid;
	unsigned int max_tid;
} TidParam;

typedef struct {
	std::string ip;
	unsigned short port;
} ServerInfo;


#pragma pack(1)

//======================================================
//与业务逻辑模块接口

#define SERVLOGIC_HEART_BEAT	0XFFEEFFEE
#define SERVLOGIC_CONNECTION_REQ	0XEEEEEE01
#define SERVLOGIC_ACTIVATE_REQ	0XEEEEEE02
#define SERVLOGIC_MO_REQ	0XEEEEEE03
#define SERVLOGIC_MT_REQ	0XEEEEEE04
#define SERVLOGIC_USER_SYNC_REQ	0XEEEEEE05
#define SERVLOGIC_DEACTIVATE_REQ	0XEEEEEE06
#define SERVLOGIC_LOCREQ_REQ    0XEEEEEE07
#define SERVLOGIC_MH_MO_REQ     0XEEEEEE08
#define SERVLOGIC_MH_MT_REQ     0XEEEEEE09

#define HEARTBEAT	0x0
#define BEGIN	0x2
#define END	0x5
#define ABORT	0x6
#define ERROR	0x7

#define	ERR_BUF_ENPTY	2001
#define	ERR_BUF_LEN	2002
#define	ERR_BUF_PARAMETER	2003
#define	ERR_NOT_SUPPORT_CMD	2007
#define	ERR_OPERATION_VALUE	2008

#define  PATH_MAX 150

#define NIF_MSG_HEAD	(0x1a2b3c4d)
typedef struct
{
	unsigned int head;
	unsigned int dIpAdrs;
	unsigned int sIpAdrs;
	unsigned int version;
	unsigned int invoke;
	unsigned int dialog;
	unsigned int length;
	unsigned char* pData;
} NIF_MSG_UNIT;


typedef struct
{
	unsigned int head;
	unsigned int dIpAdrs;
	unsigned int sIpAdrs;
	unsigned int version;
	unsigned int invoke;
	unsigned int dialog;
	unsigned int seq;
	unsigned int length;
	unsigned char* pData;
} NIF_MSG_UNIT2;


typedef struct {
	unsigned int min_tid;
	unsigned int max_tid;
	unsigned char mod_id;
} ConnData;

typedef struct {
	unsigned int result;
	unsigned char mod_id;
} ConnAck;

typedef struct {
	unsigned int tid;
	unsigned char mod_id;	//模块id,从0开始编号
	char cd[32];
	char cg[32];
	char sms_center[32];
	char sms_code;
	char sms_content[256];
} LogicMTData;

typedef struct {
	char msisdn[32];
} DeactivateData;


typedef struct {
	unsigned int tid;
	char cd[32];	//被叫号码
	char cg[32];	//主叫号码
	char sms_center[32];	//短信中心号码
	char sms_code;	//短信编码格式
	char sms_content[2048];	 //短信内容
} LogicMOData;

typedef struct {
	unsigned int result;
	char cd[32];
	unsigned int tid;
} MTAckData;


typedef struct {
	unsigned int tid;
	unsigned char mod_id;	//模块id,从0开始编号
	char imsi[16];	
	char msisdn[32];
	char esn[32];
} PeriodData;


// ===================================================

typedef struct {
    unsigned char msg_type;		// 消息类型，6：用户操作记录消息；7：短信操作记录消息；8：话单记录消息；
    unsigned char msg[256];    // 消息
} RecordMsg;

typedef struct {
    unsigned char msg_type;		// 消息类型，1：用户激活；2：用户去激活；3：MO短信；9：MT短信的ACK；
    unsigned char msg[3000];    // 消息
} ReqMsg;

typedef struct {
    unsigned char msg_type;		//1：用户激活；2：用户去激活； 4：MT短信；5：ACK消息；
    unsigned char msg[456];    // 消息
} RespMsg;


typedef struct {
	int fd;
	char imsi[16];
	char msisdn[32];
	char esn[32];
	unsigned int *reconnect_cnt_list;
	unsigned int conn_cnt;
	time_t operTime; /* luchq add 2016-02-01 */
} ActiveUser;

typedef struct {
	TcpClient *client;
	unsigned int reconnect_cnt;
	int used;
} LogicConnInfo;

//用户激活消息结构
typedef struct {
	unsigned int tid;
	unsigned char mod_id;	//用户接入模块ID号
	char imsi[16];
	char msisdn[32];
	char esn[32];
	unsigned char actived;	//0：表示新增激活用户 1：表示已存在激活用户
	ActiveUser *user_info;	
} ActivateMsg ;

//用户去激活消息结构
typedef struct {
	char msisdn[32];
} DeactivateMsg;

//MO短信消息结构
typedef struct {
	unsigned int tid;
	char cd[32];	//被叫号码
	char cg[32];	//主叫号码
	char sms_center[32];	//短信中心号码
	char sms_code;	//短信编码格式
	char sms_content[2048];	 //短信内容
} SMSData;

//MT短信消息结构
typedef struct {
	unsigned int tid;
	unsigned char seq;
	char cd[32];
	char cg[32];
	char sms_center[32];
	char sms_code;
	char sms_content[256];
} MTMsg;

//RespMsg的char msg[456] 消息结构
typedef struct {
	char mdn[32];
	unsigned int rsCode;
} RespContent;


//LocReqAck  消息结构
typedef struct {
	unsigned int nResult;
	unsigned int tid;
	char mdn[32];
	char imsi[16];
	char esn[32];
} LocReqAck;

typedef struct {
	unsigned int result;
	char cd[32];
	unsigned int tid;
	unsigned char seq;
} MTAckMsg;

//ACK消息结构
typedef struct {
	unsigned int tid;
	unsigned int msg_type;
	unsigned int result;
	char cd[32];
} AckMsg;

//用户操作记录消息结构
typedef struct {
	unsigned int create_time;
	/*char mdn[15];
	char imsi[15];
	char esn[8];*/
	/* luchq modify 2015-08-04 主要是扩大imsi和esn的长度，否则没有/0的位置*/
	char mdn[32];
	char imsi[16];
	char esn[16];
	unsigned short opt_code;
	unsigned int opt_result;
	unsigned long long appid;
	unsigned int ip;
	char mcc[3];
	char mnc[2];
	char termtype[50];
} UserOperMsg;

//短信操作记录消息结构
typedef struct {
	unsigned int start_time;
	char cg[25];
	char cd[32];
	unsigned short opt_code;
	unsigned int opt_result;
	unsigned long long appid;
	unsigned int ip;
	char mcc[3];
	char mnc[2];
} SmsOperMsg;

//话单记录消息结构
typedef struct {
	unsigned int start_time;
	char cg[25];
	char cd[32];
	unsigned short opt_code;
	unsigned int duration;
	unsigned long long appid;
	unsigned int ip;
	char mcc[3];
	char mnc[2];
} CallRecordMsg;

//======================================================
//与用户APP接口结构

#define SEND_SMS_REQ	0XEEEEEE01
#define PUSH_SMS_REQ	0XEEEEEE02
#define ACTIVE_REQ	0XEEEEEE03
#define DEACTIVE_REQ	0XEEEEEE04
#define CALL_REPORT_REQ	0XEEEEEE05
#define OFFLINE_REQ	0XEEEEEE06



typedef struct {
    unsigned long long appid;
    unsigned int start_time;
    unsigned char type;
    unsigned int duration;
    char cg[32];
    char cd[32];
    char mcc[3];
    char mnc[2];
} CallRecord;

typedef struct {
    unsigned long long appid;
    char mdn[32];
    char mcc[3];
    char mnc[2];
} Deactivation;

typedef struct {
    unsigned long long appid;
    char imsi[16];
    char mdn[32];
    char esn[32];
    char mcc[3];
    char mnc[2];
} Activation;


typedef struct {
    char mdn[32];
} AddUser;



typedef struct {
    unsigned long long appid;
    char cd[32];
    char cg[32];
    char smsc[32];
    char sms_code;
    char mcc[3];
    char mnc[2];
    char content[2048];
} MOData;

typedef struct {
	unsigned int result;
	unsigned char seq;
	unsigned int tid;
} UsrMTAckData;

typedef struct {
    char seq;
	unsigned int tid;
    char sender[32];
    char content[256];
} MTData;

//================================================

typedef struct {
	unsigned int active_user_count;
	unsigned int total_active_count;
	unsigned int app_req_queue_used;
	unsigned int app_req_queue_total;
	unsigned int logic_resp_queue_used;
	unsigned int logic_resp_queue_total;
} RunInfo;

#pragma pack()

#endif // USRACC_COMMON_H_

