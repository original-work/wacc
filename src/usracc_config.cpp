/*
* =========================================================
*
*       Filename:  usracc_config.cpp
*
*    Description:  
*
*        Version:  1.0
*        Created:  2013.6.19
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  wangxx
*   Organization:  lj
*
* ==========================================================
* ================== CHANGE REPORT HISTORY ==================
*|VERSION|UPDATE DATE|UPDATED BY|DESCRIPTION OF CHANGE|*

* ================== END OF CHANGE REPORT HISTORY ===========
*/
#include "usracc_config.h"
#include <fstream>
#include "common_logger.h"


UsrAccConfig& UsrAccConfig::instance(void) {
	static UsrAccConfig _;
	return _;
}

UsrAccConfig::UsrAccConfig(void) {

}

UsrAccConfig::~UsrAccConfig(void) {

}

int  UsrAccConfig::init(const char* cfg) {
	cfg_file(cfg);

	std::ifstream infile;
	infile.open(cfg,std::ios::in);
	std::string read_line;
	std::string read_section;
	unsigned int find_pos;
	std::string read_value;
	std::string read_key;
	if (infile.is_open()) {
		while (!infile.eof()) {
			std::getline(infile,read_line);

			if (read_line.find("#") != std::string::npos) {
				read_line.erase(read_line.find("#"));
			}

			if (read_line.find("//") != std::string::npos) {
				read_line.erase(read_line.find("//"));
			}

			if (read_line.length()==0) {continue;}


			if (read_line.find("[") != std::string::npos && read_line.find("]")!=std::string::npos) {
				read_section = read_line.substr(read_line.find("[")+1,read_line.find("]")-1);
			} else {
				if ((find_pos=read_line.find("=")) != std::string::npos) {
					if (read_section == "LoggerConfiguration") {
						deal_logger_configuration(read_line,find_pos);

						// initializing logger ...
						if (logger_config_file_ != "") {
							CommonLogger::instance().init(logger_config_file_.c_str(), "acc");
						}

					} else if (read_section == "System") {
						if (deal_system(read_line,find_pos) == -1) {
							return -1;
						}
					} else if (read_section == "AppReqServer") {
						if (deal_app_req_server(read_line,find_pos) == -1) {
							return -1;
						}
					} else if (read_section == "MEM") {
						if (deal_mem(read_line,find_pos)) {
							return -1;
						}
					} else if (read_section == "ServLogic") {
						if (deal_serv_logic(read_line,find_pos) == -1) {
							return -1;
						}
					}  else if (read_section == "FileManage") {
						if (deal_file_manage(read_line,find_pos) == -1) {
							return -1;
						}
					} else if (read_section == "DB") {
						if (deal_db(read_line,find_pos) == -1) {
							return -1;
						}
					} else {
						printf("%s%s\n","unknown section name : ", read_section.c_str());
						return -1;
					}
				}
			}
		}

		if (logger_config_file_ == "") {
			return -1;
		}
	} else {
		printf("%s\n","failed to open the configuration file.");
		return -1;
	}
	return 0;
}

int UsrAccConfig::deal_logger_configuration(const std::string &line, const int &pos) {
	std::string key;
	std::string value;
	get_key_value(line, pos, key, value);
	if (key == "logger_config_file") {
		logger_config_file_ = value;
	} else {
		printf("unknown key \" %s \" in section \" LoggerConfiguration \".\n", key.c_str());
		return -1;
	}
	return 0;
}

int UsrAccConfig::deal_app_req_server(const std::string &line, const int &pos) {
	std::string key;
	std::string value;
	get_key_value(line, pos, key, value);

	if (key == "app_serv_port") {
		app_serv_port_ = atoi(value.c_str());
		CommonLogger::instance().log_info(" app_serv_port_ is %u", app_serv_port_);
	} else if (key == "app_max_connection_count") {
		app_max_connection_count_ = atoi(value.c_str());
	} else if (key == "app_overtime"){
		app_overtime_ = atoi(value.c_str());
	}
	else{
		printf("unknown key \" %s \" in section \" AppReqServer \".\n", key.c_str());
		return -1;
	}
	return 0;
}

int UsrAccConfig::deal_system(const std::string &line, const int &pos) {
	std::string key;
	std::string value;
	get_key_value(line, pos, key, value);
	if (key == "module_id") {
		module_id_ = atoi(value.c_str());
	} else {
		printf("unknown key \" %s \" in section \" System \".\n", key.c_str());
		return -1;
	}
	return 0;
}

int UsrAccConfig::deal_mem(const std::string &line, const int &pos) {
	std::string key;
	std::string value;
	get_key_value(line, pos, key, value);
	if (key == "tid_num_seg") {
		// to do ...
		std::string tmp = "";
		TidParam tid_param;
		while ((tmp = split_first(value, ";")) != "")
		{
			std::string tmp2 = split_first(tmp, ":");
			CommonLogger::instance().log_info(" tmp is %s", tmp.c_str());
			CommonLogger::instance().log_info(" tmp2 is %s", tmp2.c_str());
			if (tmp2 != "" && tmp != "")
			{
				tid_param.min_tid = atoi(tmp2.c_str());
				tid_param.max_tid = atoi(tmp.c_str());
				tid_num_seg_list_.push_back(tid_param);
			}
		}
		if (value != "")
		{
			std::string tmp = split_first(value, ":");
			if (tmp != "" && value != "")
			{
				tid_param.min_tid = atoi(tmp.c_str());
				tid_param.max_tid = atoi(value.c_str());
				tid_num_seg_list_.push_back(tid_param);
			}
		}
	} else if (key == "usr_count") {
		usr_count_ = atoi(value.c_str());
	} else if (key == "usr_hash_count") {
		usr_hash_count_ = atoi(value.c_str());
	} else if (key == "app_req_queue_block_num") {
		app_req_queue_block_num_ = atoi(value.c_str());
	} else if (key == "app_req_queue_block_size") {
		app_req_queue_block_size_ = atoi(value.c_str());
	} else if (key == "recurrent_regnot_queue_block_num") {
		recurrent_regnot_queue_block_num_ = atoi(value.c_str());
	} else if (key == "recurrent_regnot_queue_block_size") {
		recurrent_regnot_queue_block_size_ = atoi(value.c_str());
	} else if (key == "servlogic_req_queue_block_num") {
		servlogic_req_queue_block_num_ = atoi(value.c_str());
	} else if (key == "servlogic_req_queue_block_size") {
		servlogic_req_queue_block_size_ = atoi(value.c_str());
	} else if (key == "user_op_queue_block_num") {
		user_op_queue_block_num_ = atoi(value.c_str());
	} else if (key == "user_op_queue_block_size") {
		user_op_queue_block_size_ = atoi(value.c_str());
	} else {
		printf("unknown key \" %s \" in section \" MEM \".\n", key.c_str());
		return -1;
	}
	return 0;
}

int UsrAccConfig::deal_serv_logic(const std::string &line, const int &pos) {
	std::string key;
	std::string value;
	get_key_value(line,pos,key,value);
	if (key == "serv_logic_server") {
		// todo ...
		std::string tmp = "";
		ServerInfo server_info;
		while ((tmp = split_first(value, ";")) != "")
		{
			std::string tmp2 = split_first(tmp, ":");
			if (tmp2 != "" && tmp != "")
			{
				server_info.ip = tmp2;
				server_info.port = atoi(tmp.c_str());
				serv_logic_server_list_.push_back(server_info);
			}
		}
		if (value != "")
		{
			std::string tmp = split_first(value, ":");
			if (tmp != "" && value != "")
			{
				server_info.ip = tmp;
				server_info.port = atoi(value.c_str());
				serv_logic_server_list_.push_back(server_info);
			}
		}
	} 
	else if (key == "heartbeat_timeinterval") {
		heartbeat_timeinterval_ = atoi(value.c_str());
	}
	else if(key == "recurrent_regnot_timeinterval"){
		recurrent_regnot_timeinterval_=atoi(value.c_str());
	} else {
		printf("unknown key \" %s \" in section \" CecsInfo \".\n", key.c_str());
		return -1;
	}
	return 0;
}

int UsrAccConfig::deal_file_manage(const std::string &line, const int &pos) {
	std::string key;
	std::string value;
	get_key_value(line,pos,key,value);
	if (key == "Call_record_write_switch") {
		call_record_write_switch_ = atoi(value.c_str());
	} else if (key == "call_record_write_periodic") { 
		call_record_write_periodic_ = atoi(value.c_str()) * 60;
	} else if (key == "call_record_save_path") { 
		call_record_save_path_ = value.c_str();
	} else if (key == "call_record_file_prefix") { 
		call_record_file_prefix_ = value.c_str();
	} else if (key == "sms_record_write_switch") { 
		sms_record_write_switch_ = atoi(value.c_str());
	} else if (key == "sms_record_write_periodic") { 
		sms_record_write_periodic_ = atoi(value.c_str()) * 60;
	} else if (key == "sms_record_save_path") { 
		sms_record_save_path_ = value.c_str();
	} else if (key == "sms_record_file_prefix") { 
		sms_record_file_prefix_ = value.c_str();
	} else if (key == "user_op_write_switch") { 
		user_op_write_switch_ = atoi(value.c_str());
	} else if (key == "user_op_write_periodic") { 
		user_op_write_periodic_ = atoi(value.c_str()) * 60;
	} else if (key == "user_op_save_path") { 
		user_op_save_path_ = value.c_str();
	} else if (key == "user_op_file_prefix") { 
		user_op_file_prefix_ = value.c_str();
	}else {
		printf("unknown key \" %s \" in section \" BusinessInfo \".\n", key.c_str());
		return -1;
	}
	return 0;
}


int UsrAccConfig::deal_db(const std::string &line, const int &pos) {
	std::string key;
	std::string value;
	get_key_value(line,pos,key,value);
	if (key == "url") { 
		url_ = value.c_str();
	} else if (key == "user") { 
		user_ = value.c_str();
	} else if (key == "password") { 
		password_ = value.c_str();
	}else if (key == "database") { 
		database_ = value.c_str();
	}else {
		printf("unknown key \" %s \" in section \" BusinessInfo \".\n", key.c_str());
		return -1;
	}
	return 0;
}

int UsrAccConfig::get_key_value(const std::string &line, const int &pos, std::string &key, std::string &value) {
	key = line.substr(0,pos);
	value = line.substr(pos+1);

	trim_str(key);
	trim_str(value);
	return 0;
}

int UsrAccConfig::trim_str(std::string &str) {
	unsigned int pos;
	pos = str.find_first_not_of(" ");
	if (pos != std::string::npos) {
		str = str.substr(pos);
	}

	if ((pos = str.find_last_of("\r")) != std::string::npos) {
		str = str.substr(0, pos);
	}

	if ((pos = str.find_last_of("\n")) != std::string::npos) {
		str = str.substr(0, pos);
	}

	if ((pos = str.find_last_not_of(" ")) != std::string::npos) {
		str = str.substr(0,pos+1);
	}

	if ((pos = str.find_last_not_of("\t")) != std::string::npos) {
		str = str.substr(0,pos+1);
	}
	//printf("%s,%d\n",str.c_str(),str.length());
	return 0;
}


std::string UsrAccConfig::split_first(std::string &strin, const std::string &split_key)
{
	std::size_t pos;
	pos = strin.find_first_of(split_key);
	if (pos == std::string::npos)
	{
		return "";
	}

	std::string ob = strin.substr(0, pos);
	strin = strin.substr(pos+1);

	return ob;
}		/* -----  end of method UsrAccConfig::split_first  ----- */

