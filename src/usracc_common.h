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
//��ҵ���߼�ģ��ӿ�

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
	unsigned char mod_id;	//ģ��id,��0��ʼ���
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
	char cd[32];	//���к���
	char cg[32];	//���к���
	char sms_center[32];	//�������ĺ���
	char sms_code;	//���ű����ʽ
	char sms_content[2048];	 //��������
} LogicMOData;

typedef struct {
	unsigned int result;
	char cd[32];
	unsigned int tid;
} MTAckData;


typedef struct {
	unsigned int tid;
	unsigned char mod_id;	//ģ��id,��0��ʼ���
	char imsi[16];	
	char msisdn[32];
	char esn[32];
} PeriodData;


// ===================================================

typedef struct {
    unsigned char msg_type;		// ��Ϣ���ͣ�6���û�������¼��Ϣ��7�����Ų�����¼��Ϣ��8��������¼��Ϣ��
    unsigned char msg[256];    // ��Ϣ
} RecordMsg;

typedef struct {
    unsigned char msg_type;		// ��Ϣ���ͣ�1���û����2���û�ȥ���3��MO���ţ�9��MT���ŵ�ACK��
    unsigned char msg[3000];    // ��Ϣ
} ReqMsg;

typedef struct {
    unsigned char msg_type;		//1���û����2���û�ȥ��� 4��MT���ţ�5��ACK��Ϣ��
    unsigned char msg[456];    // ��Ϣ
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

//�û�������Ϣ�ṹ
typedef struct {
	unsigned int tid;
	unsigned char mod_id;	//�û�����ģ��ID��
	char imsi[16];
	char msisdn[32];
	char esn[32];
	unsigned char actived;	//0����ʾ���������û� 1����ʾ�Ѵ��ڼ����û�
	ActiveUser *user_info;	
} ActivateMsg ;

//�û�ȥ������Ϣ�ṹ
typedef struct {
	char msisdn[32];
} DeactivateMsg;

//MO������Ϣ�ṹ
typedef struct {
	unsigned int tid;
	char cd[32];	//���к���
	char cg[32];	//���к���
	char sms_center[32];	//�������ĺ���
	char sms_code;	//���ű����ʽ
	char sms_content[2048];	 //��������
} SMSData;

//MT������Ϣ�ṹ
typedef struct {
	unsigned int tid;
	unsigned char seq;
	char cd[32];
	char cg[32];
	char sms_center[32];
	char sms_code;
	char sms_content[256];
} MTMsg;

//RespMsg��char msg[456] ��Ϣ�ṹ
typedef struct {
	char mdn[32];
	unsigned int rsCode;
} RespContent;


//LocReqAck  ��Ϣ�ṹ
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

//ACK��Ϣ�ṹ
typedef struct {
	unsigned int tid;
	unsigned int msg_type;
	unsigned int result;
	char cd[32];
} AckMsg;

//�û�������¼��Ϣ�ṹ
typedef struct {
	unsigned int create_time;
	/*char mdn[15];
	char imsi[15];
	char esn[8];*/
	/* luchq modify 2015-08-04 ��Ҫ������imsi��esn�ĳ��ȣ�����û��/0��λ��*/
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

//���Ų�����¼��Ϣ�ṹ
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

//������¼��Ϣ�ṹ
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
//���û�APP�ӿڽṹ

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

