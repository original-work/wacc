
#ifndef __GET_HOST__
#define __GET_HOST__


#include<stdio.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netdb.h>
#include<netinet/in.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/time.h>
#include<assert.h>
#include <stdarg.h>
#include "common_logger.h"
#include "arpa/inet.h"
namespace tools{


/*�ֽ����ת��*/
void net2host(void *msg,int num=1);
void host2net(void *msg,int num=1);
void net2hosts(void *msg,int num=1);
void host2nets(void *msg,int num=1);

/*��ȡ����ip*/
unsigned int get_host();
unsigned int get_time();

/*ת���ĺ��붼��0-9֮��*/

void ascii2bcd (const char *in_buf,const int in_len,u_char *o_buf,u_char &o_len);
void bcd2ascii(const u_char *in_buf,const int in_len,char *o_buf,int &o_len);

void ascii2char (const char *in_buf,const int in_len,char *o_buf,int &o_len);
void char2ascii(const char *in_buf,const int in_len,char *o_buf,int &o_len);

void char2bcd (const char *in_buf,const int in_len,u_char *o_buf,u_char &o_len);
void bcd2char(const char *in_buf,const u_char in_len,char *o_buf,u_char &o_len);

/*���ַ���ת�ɶ��ڵ�int��*/
void char2int(const char *in_buf,const int in_len, u_int &o_int);
void char2long(const char *in_buf,const int in_len, unsigned long long &o_int);
void long2char(unsigned long long i_int,char *o_buf);



// ��UTF8����ĳ�����ʶ��
//
// ���������кܶ�ط����в���UTF8���룬
// ����ʵ��Unicode��ͬ�࣬�����ڱ��뷽ʽ�ϲ�ͬ��
// ����UTF8�����Ĵ�С�ǲ�һ��������Unicode�����Ĵ�С��һ���ģ�
//
// ����������Unicode�ı��룺һ��Ӣ����ĸ ��a�� �͡�һ������ ���á����������ռ�õĿռ��С��һ���ģ����������ֽڣ�
//
// ��UTF8���룺һ��Ӣ����ĸ��a�� �͡�һ������ ���á��������ռ�õĿռ��С�Ͳ����ˣ�ǰ����һ���ֽڣ������������ֽڣ�
//
// ���ھ�������������UTF8�����ԭ��ɣ�
// ������Ϊһ����ĸ����һЩ�����ϵķ��ż�����ֻ�ö�������λ�Ϳ��Ա�ʾ��������һ���ֽھ��ǰ�λ��
// ����UTF8����һ���ֽ�����ʽ��ĸ��һЩ�����ϵķ��š�Ȼ���������õ���������һ���ֽں���ô֪��������ɣ�
// ���п�����Ӣ����ĸ��һ���ֽڣ�Ҳ�п����Ǻ��ֵ������ֽ��е�һ���ֽڣ����ԣ�UTF8���б�־λ�ģ�
//
// ������Ҫ��ʾ�������ǡ�7λ����ʱ�����һ���ֽڣ�0*******  ����һ��0Ϊ��־λ��ʣ�µĿռ����ÿ��Ա�ʾASCII��0��127�������ݡ�
//
// ������Ҫ��ʾ�������ڡ�8������11��λ��ʱ����������ֽڣ�110***** 10******  ����һ���ֽڵ�110�͵ڶ����ֽڵ�10Ϊ��־λ��
//
// ������Ҫ��ʾ�������ڡ�12������16��λ��ʱ����������ֽڣ�1110***** 10****** 10******
// ������������һ������һ���ֽڵ�1110�͵ڶ��������ֽڵ�10���Ǳ�־λ��ʣ�µĿռ����ÿ��Ա�ʾ���֡�
//
// �����Դ����ƣ�
//         �ĸ��ֽڣ�11110**** 10****** 10****** 10******
//         ��������ֽڣ�111110*** 10****** 10****** 10****** 10******
//         ���������ֽڣ�1111110** 10****** 10****** 10****** 10****** 10******
//         ����.............................................
//                ..............................................
//
//                ������û�У�
//                ����ķ����Ǵӵ�λ����λ
//
//                ���ھ�������������ʵ���ɣ�
//
//                 ��ɫΪ��־λ
//                 ������ɫΪ����ʾ�䣬������λ��
//
//	Unicodeʮ������ 	Unicode������		UTF8������			UTF8ʮ������	UTF8�ֽ���
//     		B             	00001011        	00001010        		B 		1
//             9D 		00010011101 		11000010 10011101 		C2 9D 		2
//             A89E 		10101000 10011110 	11101010 10100010 10011110 	EA A2 9E 	3
//
//

// return -1 if failed
// return used size in @dst
int utf8_to_ucs2(unsigned char dst[], size_t max, const unsigned char* src, int len);
int ucs2_to_utf8(unsigned char dst[], size_t max, const unsigned char* src, int len);
//int	decode_forward(const MSU_PARM  s_message, char * o_buf, int & o_len,const u_char *i_buf,const u_short i_len,int flag);


/* ��ӡ�ַ��� */
void print_hex(unsigned char* p_msg, int length);
const std::string currentDateTime();

}

#endif

