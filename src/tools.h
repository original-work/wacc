
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


/*字节序的转换*/
void net2host(void *msg,int num=1);
void host2net(void *msg,int num=1);
void net2hosts(void *msg,int num=1);
void host2nets(void *msg,int num=1);

/*获取本地ip*/
unsigned int get_host();
unsigned int get_time();

/*转换的号码都在0-9之间*/

void ascii2bcd (const char *in_buf,const int in_len,u_char *o_buf,u_char &o_len);
void bcd2ascii(const u_char *in_buf,const int in_len,char *o_buf,int &o_len);

void ascii2char (const char *in_buf,const int in_len,char *o_buf,int &o_len);
void char2ascii(const char *in_buf,const int in_len,char *o_buf,int &o_len);

void char2bcd (const char *in_buf,const int in_len,u_char *o_buf,u_char &o_len);
void bcd2char(const char *in_buf,const u_char in_len,char *o_buf,u_char &o_len);

/*把字符串转成对于得int型*/
void char2int(const char *in_buf,const int in_len, u_int &o_int);
void char2long(const char *in_buf,const int in_len, unsigned long long &o_int);
void long2char(unsigned long long i_int,char *o_buf);



// 对UTF8编码的初步认识！
//
// 在网络中有很多地方都有采用UTF8编码，
// 它其实和Unicode是同类，就是在编码方式上不同！
// 首先UTF8编码后的大小是不一定，不像Unicode编码后的大小是一样的！
//
// 我们先来看Unicode的编码：一个英文字母 “a” 和　一个汉字 “好”，编码后都是占用的空间大小是一样的，都是两个字节！
//
// 而UTF8编码：一个英文字母“a” 和　一个汉字 “好”，编码后占用的空间大小就不样了，前者是一个字节，后者是三个字节！
//
// 现在就让我们来看看UTF8编码的原理吧：
// 　　因为一个字母还有一些键盘上的符号加起来只用二进制七位就可以表示出来，而一个字节就是八位，
// 所以UTF8就用一个字节来表式字母和一些键盘上的符号。然而当我们拿到被编码后的一个字节后怎么知道它的组成？
// 它有可能是英文字母的一个字节，也有可能是汉字的三个字节中的一个字节！所以，UTF8是有标志位的！
//
// 　　当要表示的内容是　7位　的时候就用一个字节：0*******  　第一个0为标志位，剩下的空间正好可以表示ASCII　0－127　的内容。
//
// 　　当要表示的内容在　8　到　11　位的时候就用两个字节：110***** 10******  　第一个字节的110和第二个字节的10为标志位。
//
// 　　当要表示的内容在　12　到　16　位的时候就用三个字节：1110***** 10****** 10******
// 　　　和上面一样，第一个字节的1110和第二、三个字节的10都是标志位，剩下的空间正好可以表示汉字。
//
// 　　以此类推：
//         四个字节：11110**** 10****** 10****** 10******
//         　　五个字节：111110*** 10****** 10****** 10****** 10******
//         　　六个字节：1111110** 10****** 10****** 10****** 10****** 10******
//         　　.............................................
//                ..............................................
//
//                明白了没有？
//                编码的方法是从低位到高位
//
//                现在就让我们来看看实例吧！
//
//                 黄色为标志位
//                 其它着色为了显示其，编码后的位置
//
//	Unicode十六进制 	Unicode二进制		UTF8二进制			UTF8十六进制	UTF8字节数
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


/* 打印字符串 */
void print_hex(unsigned char* p_msg, int length);
const std::string currentDateTime();

}

#endif

