
#include "tools.h"
#include <arpa/inet.h>
namespace tools{


void net2host(void *msg,int num)
{
	int i;
	int *p_int = (int*)msg;
	for(i=0;i<num;i++)
	{
		*p_int = ntohl(*p_int);
		p_int++;
	}
}
void net2hosts(void *msg,int num)
{
	int i;
	short *p_int = (short*)msg;
	for(i=0;i<num;i++)
	{
		*p_int = ntohs(*p_int);
		p_int++;
	}
}
void host2net(void *msg,int num)
{
	int i;
	int *p_int = (int*)msg;
	for(i=0;i<num;i++)
	{
		*p_int = htonl(*p_int);
		p_int++;
	}
}
void host2nets(void *msg,int num)
{
	int i;
	short *p_int = (short*)msg;
	for(i=0;i<num;i++)
	{
		*p_int = htons(*p_int);
		p_int++;
	}
}


unsigned int get_host()
{
	char ul[100];
	memset(ul,0,sizeof(ul));
	struct hostent *h;
	u_int addr;
	if(gethostname(ul,20) == 0)
    	{
       	 h = gethostbyname(ul);
       	 if(h == NULL)
        		return 0;
    		 else
    		 {
    		 	addr = *(u_int *)h->h_addr;
			 return addr;
    		 }
    	}
	else
		return 0;
/*
    printf("hostname:%s\n",h->h_name);
    printf("IPAddress:%s   %08x\n",inet_ntoa(*((struct in_addr*)h->h_addr)),*(struct in_addr*)h->h_addr);
*/
}


unsigned int get_time()
{
	struct timeval tv;

	gettimeofday(&tv,NULL);
	return tv.tv_sec;
}



void ascii2bcd(const char *in_buf,const int in_len,u_char *o_buf,u_char &o_len)
{
#ifdef S__DEBUG__
	assert(in_buf && o_buf);
#endif
	int i;
	const char *p_in = in_buf;
	u_char *p_out = o_buf;
	for(i=0;i<in_len;i++)
	{
		if(*p_in > '9' || *p_in < '0')
			return;

		if(i%2 == 1)
		{
			*p_out |= (*p_in - '0') <<4;
			p_out++;
		}
		else
		{
			*p_out = (*p_in - '0') & 0x0f;
		}
		p_in++;
	}
	o_len = in_len;
}

void bcd2ascii (const u_char *in_buf,const int in_len,char *o_buf,int &o_len)
{
#ifdef S__DEBUG__
	assert(in_buf && o_buf);
#endif
	int i;
	char *p_in = (char *)in_buf;
	char *p_out = o_buf;
	for(i=0;i<in_len;i++)
	{
		if(i%2 == 1)
		{
			*p_out = ((*p_in >>4)&0x0f) + '0';
			p_in++;
		}
		else
		{
			*p_out = ( *p_in & 0x0f)+'0';
		}
		p_out++;
	}
	o_len = in_len;
}

/*×ª»»³É  Êý×Ö*/
void ascii2char (const char *in_buf,const int in_len,char *o_buf,int &o_len)
{
#ifdef S__DEBUG__
	assert(in_buf && o_buf);
#endif
	int i;
	const char *p_in = in_buf;
	char *p_out = o_buf;
	for(i=0; i<in_len;i++)
	{
		*p_out ++ = *p_in - '0';
		p_out++;
		p_in++;
	}
	o_len = in_len;
}

void char2ascii(const char *in_buf,const int in_len,char *o_buf,int &o_len)
{
#ifdef S__DEBUG__
	assert(in_buf && o_buf);
#endif
	int i;
	const char *p_in = in_buf;
	char *p_out = o_buf;
	for(i=0; i<in_len;i++)
	{
		*p_out = *p_in + '0';
		p_out++;
		p_in++;
	}
	o_len = in_len;
}
void char2bcd (const char *in_buf,const int in_len,u_char *o_buf,u_char &o_len)
{
#ifdef S__DEBUG__
	assert(in_buf && o_buf);
#endif
	int i;
	const char *p_in = in_buf;
	u_char *p_out = (u_char *)o_buf;
	for(i=0;i<in_len;i++)
	{
		if(i%2 == 1)
		{
			*p_out |= (*p_in) <<4;
			p_out++;
		}
		else
		{
			*p_out = (*p_in) & 0x0f;
		}
		p_in++;
	}
	o_len = in_len;

}

void bcd2char(const char *in_buf,const u_char in_len,char *o_buf,u_char &o_len)
{
#ifdef S__DEBUG__
	assert(in_buf && o_buf);
#endif
	int i;
	const char *p_in = in_buf;
	char *p_out = o_buf;
	for(i=0;i<in_len;i++)
	{
		if(i%2 == 1)
		{
			*p_out = ((*p_in >>4)&0x0f);
			p_in++;
		}
		else
		{
			*p_out = ( *p_in & 0x0f);
		}
		p_out++;
	}
	o_len = in_len;

}

void char2int(const char *in_buf,const int in_len, u_int &o_int)
{
	if(in_buf == NULL)
		return;

	o_int = 0;
	for(int i = 0;i<in_len;i++)
		o_int = 10*o_int + in_buf[i] - '0';
}

void char2long(const char *in_buf,const int in_len, unsigned long long &o_int)
{
	if(in_buf == NULL)
		return;

	o_int = 0;
	for(int i = 0;i<in_len;i++)
		o_int = 10*o_int + in_buf[i] - '0';

}
void long2char(unsigned long long i_int,char *o_buf)
{
	if(o_buf == NULL)
		return;
	char *p_buf = o_buf;
	int i = 0;
	int num[32];
	while(i_int/10 !=0 )
	{
		num[i] = i_int % 10;
		i_int = i_int / 10;
		i++;
	}
	num[i] = i_int;
	for(;i >= 0;i--)
		*p_buf++ = num[i] + '0';
}

int utf8_to_ucs2(unsigned char dst[], size_t max, const unsigned char* src, int len)
{
    int i = 0;
    size_t j = 0;
    while (i < len)
    {
        unsigned short ch = 0;
        if ((src[i] & 0xF0) == 0xF0)
        {
            assert(0);
            return -1;
        }
        else if ((src[i] & 0xF0) == 0xE0)
        {
            if (i + 2 >= len)
            {
                return -1;
            }
            if ((src[i+1] & 0xC0) != 0x80)
            {
                return -1;
            }
            if ((src[i+2] & 0xC0) != 0x80)
            {
                return -1;
            }
            ch = ((src[i] & 0x0F) << 12) | ((src[i+1] & 0x3F) << 6) | ((src[i+2] & 0x3F));
            i += 3;
            //printf("ch=%#x\n", ch);
        }
        else if ((src[i] & 0xE0) == 0xC0)
        {
            if (i + 1 >= len)
            {
                return -1;
            }
            if ((src[i+1] & 0xC0) != 0x80)
            {
                return -1;
            }
            ch = ((src[i] & 0x1F) << 6) | ((src[i+1] & 0x3F));
            i += 2;
        }
        else if ((src[i] & 0x80) == 0x00)
        {
            ch = src[i];
            i += 1;
        }
        else
        {
            assert(0);
            return -1;
        }
        if (j + 2 > max)
        {
            return -1;
        }
        dst[j++] = ch >> 8;
        dst[j++] = ch & 0xFF;
    }
    if (i != len)
    {
        return -1;
    }
    return j;
}
/************************************************
huangqb@2015-1-4 add for ucs-2 transfer to utf-8

**************************************************/

// return -1 if failed
// return used size in @dst
int ucs2_to_utf8(unsigned char dst[], size_t max, const unsigned char* src, int len)
{
    int i = 0;
    size_t j = 0;

    if (len % 2 != 0)
    {
        return -1;
    }

    while (i < len)
    {
        unsigned short ch = (src[i] << 8) | (src[i+1]);
        if (ch <= 0x7F) // 7 bits
        {
            if (j + 1 > max)
            {
                return -1;
            }
            dst[j++] = (unsigned char) ch;
            //printf("ch=%#x %#x\n", ch, dst[j-1]);
        }
        else if (ch <= 0x7FF) // 8..11 bits
        {
            if (j + 2 > max)
            {
                return -1;
            }
            dst[j++] = 0xC0 | (ch >> 6);
            dst[j++] = 0x80 | (ch & 0x3F);
            //printf("ch=%#x %#x %#x\n", ch, dst[j-2], dst[j-1]);
        }
        else // if (ch <= 0xFFFF) // 12..16 bits
        {
            if (j + 3 > max)
            {
                return -1;
            }
            dst[j++] = 0xE0 | (ch >> 12);
            dst[j++] = 0x80 | ((ch >> 6) & 0x3F);
            dst[j++] = 0x80 | (ch & 0x3F);
            //printf("ch=%#x %#x %#x %#x\n", ch, dst[j-3], dst[j-2], dst[j-1]);
        }
        i += 2;
    }
    if (i != len)
    {
        return -1;
    }
    return j;
}

/************************************************
luchq add 2015-06-16 for print hex string
**************************************************/
void print_hex(unsigned char* p_msg, int length)
{
	short i, j, len;
	char buf[2048];
	j   = 0;
	len = 0;
	memset(buf,0,2048);
	for (i = 0; (i<length)&&(i<512); i++)
	{
		sprintf(&buf[len], "%.2X ", *(p_msg + i));
		len += 3;
		j++;
		if (j >= 16)
		{
			buf[len] = '\r';
			len++;
			buf[len]= '\n';
			len++;
			j = 0;
		}
	}

	buf[len] = '\r';
	buf[len + 1] = '\n';
	buf[len + 2] = '\r';
	buf[len + 3] = '\n';
	buf[len + 4] = 0;

	CommonLogger::instance().log_info("%s",buf);
}

const std::string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

    return buf;
}

}
