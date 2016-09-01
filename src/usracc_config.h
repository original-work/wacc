/*
* =========================================================
*
*       Filename:  usracc_config.h
*
*    Description:  配置信息管理
*
*        Version:  1.0
*        Created:  2013.6.19
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
#if !defined(_USRACC_CONFIG_H)
#define _USRACC_CONFIG_H
#include <vector>
#include "cconfig.h"
#include "usracc_common.h"


class UsrAccConfig : public CConfig {
public:
	static UsrAccConfig& instance(void);
	~UsrAccConfig(void);
	/* 初始化CDR合成模块配置信息 */
	int  init(const char* cfg);

	unsigned int module_id() const {return module_id_;}
	std::string logger_config_file(void) const {return logger_config_file_;}
	unsigned short app_serv_port() const {return app_serv_port_;}
	unsigned int app_max_connection_count() const {return app_max_connection_count_;}
	unsigned int app_heartbeat_overtime() const {return app_overtime_;};

	std::vector<ServerInfo> serv_logic_server_list() const {return serv_logic_server_list_;}
	unsigned int heartbeat_timeinterval() const {return heartbeat_timeinterval_;}
	unsigned int recurrent_regnot_timeinterval() const {return recurrent_regnot_timeinterval_;}

	std::vector<TidParam> tid_num_seg_list() const {return tid_num_seg_list_;}
	
	unsigned int usr_count() const {return usr_count_;}
	unsigned int usr_hash_count() const {return usr_hash_count_;}
	unsigned int app_req_queue_block_num() const {return app_req_queue_block_num_;}
	unsigned int app_req_queue_block_size() const {return app_req_queue_block_size_;}
	unsigned int recurrent_regnot_queue_block_num() const {return recurrent_regnot_queue_block_num_;}
	unsigned int recurrent_regnot_queue_block_size() const {return recurrent_regnot_queue_block_size_;}
	unsigned int servlogic_req_queue_block_num() const {return servlogic_req_queue_block_num_;}
	unsigned int servlogic_req_queue_block_size() const {return servlogic_req_queue_block_size_;}
	unsigned int user_op_queue_block_num() const {return user_op_queue_block_num_;}
	unsigned int user_op_queue_block_size() const {return user_op_queue_block_size_;}

	unsigned int call_record_write_switch() const {return call_record_write_switch_;}
	unsigned int call_record_write_periodic() const {return call_record_write_periodic_;}
	std::string call_record_save_path() const {return call_record_save_path_;}
	std::string call_record_file_prefix() const {return call_record_file_prefix_;}
	unsigned int sms_record_write_switch() const {return sms_record_write_switch_;}
	unsigned int sms_record_write_periodic() const {return sms_record_write_periodic_;}
	std::string sms_record_save_path() const {return sms_record_save_path_;}
	std::string sms_record_file_prefix() const {return sms_record_file_prefix_;}
	unsigned int user_op_write_switch() const {return user_op_write_switch_;}
	unsigned int user_op_write_periodic() const {return user_op_write_periodic_;}
	std::string user_op_save_path() const {return user_op_save_path_;}
	std::string user_op_file_prefix() const {return user_op_file_prefix_;}
	std::string mysql_url() const {return url_;}
	std::string mysql_user() const {return user_;}
	std::string mysql_password() const {return password_;}
	std::string mysql_database() const {return database_;}
protected:
	int deal_system(const std::string &line, const int &pos);
	int deal_logger_configuration(const std::string &line, const int &pos);
	int deal_app_req_server(const std::string &line, const int &pos);
	int deal_serv_logic(const std::string &line, const int &pos);
	int deal_mem(const std::string &line, const int &pos);
	int deal_file_manage(const std::string &line, const int &pos);
	int deal_db(const std::string &line, const int &pos);
	int get_key_value(const std::string &line, const int &pos, std::string &key, std::string &value);
	int trim_str(std::string &str);
	std::string split_first(std::string &strin, const std::string &split_key);
private:
	UsrAccConfig(void);
	UsrAccConfig(const UsrAccConfig &cf);
	UsrAccConfig& operator=(const UsrAccConfig &cf);
private:
	unsigned int module_id_;
	std::string logger_config_file_;
	/* 服务端口 */
	unsigned short app_serv_port_;
	unsigned int app_max_connection_count_;
	unsigned int app_overtime_; //luchq add  2016-02-01，心跳超时时间，用于监测连接是否存在

	std::vector<ServerInfo> serv_logic_server_list_;
	unsigned int heartbeat_timeinterval_;
	unsigned int recurrent_regnot_timeinterval_;

	std::vector<TidParam> tid_num_seg_list_;	//tid，每个tid段采用分号分隔，用冒号表示tid一个区段
	
	unsigned int usr_count_;	// 激活用户数量
	unsigned int usr_hash_count_;	// 激活用户信息索引量
	unsigned int app_req_queue_block_num_;		//手机App请求消息队列数量
	unsigned int app_req_queue_block_size_;		//手机App请求消息队列块大小
	unsigned int servlogic_req_queue_block_num_;		//业务逻辑请求消息队列数量
	unsigned int servlogic_req_queue_block_size_;		//业务逻辑请求消息队列块大小
	unsigned int user_op_queue_block_num_;		//用户操作消息队列数量
	unsigned int user_op_queue_block_size_;		//用户操作消息队列块大小
	unsigned int recurrent_regnot_queue_block_num_;   //周期性位置登记消息队列数量
	unsigned int recurrent_regnot_queue_block_size_;   //周期性位置登记消息队列块大小

	unsigned int call_record_write_switch_;
	unsigned int call_record_write_periodic_;		// 语音话单写文件周期
	std::string call_record_save_path_;	// 语音话单写文件目录
	std::string call_record_file_prefix_;	// 语音话单写文件前缀
	unsigned int sms_record_write_switch_;
	unsigned int sms_record_write_periodic_;	// 短信话单写文件周期
	std::string sms_record_save_path_;	// 短信话单写文件目录
	std::string sms_record_file_prefix_;	// 短信话单写文件前缀
	unsigned int user_op_write_switch_;
	unsigned int user_op_write_periodic_;	// 用户操作写文件周期
	std::string user_op_save_path_;	// 用户操作写文件目录
	std::string user_op_file_prefix_;	// 用户操作写文件前缀
	std::string url_;
	std::string user_;
	std::string password_;
	std::string database_;

};

#endif //_USRACC_CONFIG_H

