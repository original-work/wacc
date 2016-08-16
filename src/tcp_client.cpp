/*
* =========================================================
*
*       Filename:  tcp_client.cpp
*
*    Description:  TCP/IP连接客户端
*
*        Version:  1.0
*        Created:  2013.6.14
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
#include "tcp_client.h"
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include "common_logger.h"

using  std::string;

TcpClient::TcpClient() {
	sockfd_ = -1;
	connected_ = false;
}

TcpClient::~TcpClient() {

}

int TcpClient::init(const std::string &ip, const unsigned int &port) {
		ip_=ip;
		port_=port;
		memset(&sock_addr_,0,sizeof(sock_addr_));
		sock_addr_.sin_family = AF_INET;
		sock_addr_.sin_addr.s_addr = inet_addr(ip_.c_str());
		sock_addr_.sin_port = htons(port_);
		return 0;
}

int TcpClient::connect_to_server() {
	if (sockfd_>0) {
		disconnect_to_server();
	}

	int error, len;
	struct timeval tm;
	fd_set set;

	if ((sockfd_ = socket(AF_INET, SOCK_STREAM, 0))==-1) {
		CommonLogger::instance().log_error("socket failed error = %d , strerror = %s",errno,strerror(errno));
		return -1;
	}

	/*if(fcntl(sockfd_, F_SETFL, O_NDELAY) == -1) {
		return -1;
	}*/

	if (fcntl(sockfd_, F_SETFL, O_NONBLOCK)==-1) {
		CommonLogger::instance().log_error("fcntl set no block failed error = %d , strerror = %s",errno,strerror(errno));
		return -1;
	}

	//unsigned long ul = 1;
	//ioctl(sockfd_, FIONBIO, &ul);  //设置为非阻塞模式 
	
	error = -1;
	len = sizeof(error);
	CommonLogger::instance().log_info("connect server ip = %s , port = %u",inet_ntoa(sock_addr_.sin_addr), ntohs(sock_addr_.sin_port));
	if (connect(sockfd_, (struct sockaddr *)&sock_addr_, sizeof(sock_addr_))==-1) {
		if (errno == EINPROGRESS) {
			CommonLogger::instance().log_info("connect failed error = EINPROGRESS");
			tm.tv_sec  = 5;
			tm.tv_usec = 0;
			FD_ZERO(&set);
			FD_SET(sockfd_, &set);
			if (select(sockfd_+1, NULL, &set, NULL, &tm) > 0) {
				if (getsockopt(sockfd_, SOL_SOCKET, SO_ERROR, &error, (socklen_t *)&len) == -1) {
					CommonLogger::instance().log_error("getsockopt failed error = %d , strerror = %s",errno,strerror(errno));
					return -1;
				}
				if (error == 0) {
					int flags = fcntl(sockfd_, F_GETFL, 0);
					fcntl(sockfd_, F_SETFL, flags&(~O_NONBLOCK));
					connected_ = true;
					return 0;
				}
			}
		}
		CommonLogger::instance().log_error("connect failed error = %d , strerror = %s",errno,strerror(errno));
		return -1;
	}
	CommonLogger::instance().log_info("connect success, fd=%d",sockfd_);
	connected_ = true;
	return 0;
}

int TcpClient::disconnect_to_server() {
	if (sockfd_>0) {
		close(sockfd_);
	}
	connected_ = false;
	sockfd_ = -1;
	return 0;
}

int TcpClient::send_data(const char *buf, const unsigned int &length) {
	unsigned int need_send_length = length;
	int sended_length = 0;
	while (need_send_length > 0) {
		int len = write(sockfd_, buf+sended_length, need_send_length);
		int error = errno;
		if (len < 0 && error == EINTR) {
			len = 0;
		} else if (len < 0 && error == EAGAIN) {
			usleep(1000);
			len = 0;
		} else if (len < 0) {
			return -1;
		} else if (len == 0) {
			break;
		}
		
		sended_length+=len;
		need_send_length-=len;
	}
	return sended_length;
}

int TcpClient::recv_data(char *buf, const unsigned int length) {
	static int i=0;
	unsigned int need_recv_length = length;
	unsigned int recved_length = 0;
	while (need_recv_length > 0) {
		int len = read(sockfd_, buf+recved_length, need_recv_length);
		int error = errno;
		if (len < 0 && error == EINTR) {
			len = 0;
			++i;
			if (i == 3000)
			{
				i = 0;
				break;
			}
		} else if (len < 0 && error == EAGAIN) {
			usleep(1000);
			len = 0;
			++i;
			if (i == 3000)
			{
				i = 0;
				break;
			}
		} else if (len<0) {
			return -1;
		} if (len == 0) {
			break;
		}
		
		recved_length += len;
		need_recv_length -= len;
	}
	if (recved_length == length)
	{
		i = 0;
	}
	return recved_length;
}

int TcpClient::handle_recv() {
	return 0;
}

int TcpClient::handle_send() {
	return 0;
}
