/*********************************************************************************
 * decleration and function prototype for Speed Test module
 *
 * Copyright (C) 2016 Econet Technologies, Corp.
 * All Rights Reserved.
 *
 *********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>

static FILE* s_file = NULL;
#define MAX_SOCKET_NUM	16
int speedtest_check_file(void)
{
	if (s_file != NULL)
	{
		return 1;
	}
	s_file = fopen("/proc/tc3162/speedtest_v2","r+");
	
	if (s_file == NULL)
	{
		fprintf(stderr,"open /proc/speedtest_v2 fail \n");
		return -1;
	}
	return 0;
}

int speedtest_lib_set_v2_mode(int en)
{
	int ret;
	
	if (speedtest_check_file() < 0)
		return -1;
	
	fprintf(s_file,"v2_enable %d \n",en);
	fflush(s_file);
	fseek(s_file,0,SEEK_SET);
	if(fscanf(s_file,"%d", &ret) < 0)
		return -1;
	
	return ret;		
}

int speedtest_lib_reset_global(void)
{
	int ret;
	
	if (speedtest_check_file() < 0)
		return -1;
	
	fprintf(s_file,"reset_global \n");
	fflush(s_file);
	fseek(s_file,0,SEEK_SET);
	if(fscanf(s_file,"%d", &ret) < 0)
		return -1;
	return ret;		
}

int speedtest_lib_get_buff_rate(int idx)
{
	int ret;
	
	if (speedtest_check_file() < 0)
		return -1;
	
	fprintf(s_file,"buff_rate %d \n",idx);
	fflush(s_file);
	fseek(s_file,0,SEEK_SET);
	if(fscanf(s_file,"%d", &ret) < 0)
		return -1;
	return ret;		
}

unsigned int speedtest_lib_get_peak_send(void)
{
	unsigned int ret;
	
	if (speedtest_check_file() < 0)
		return 0;
	
	fprintf(s_file,"peak_send \n");
	fflush(s_file);
	fseek(s_file,0,SEEK_SET);
	if(fscanf(s_file,"%u", &ret) < 0)
		return -1;
	
	return ret;		
}

unsigned int speedtest_lib_get_avg_rate(void)
{
	unsigned int ret;
	
	if (speedtest_check_file() < 0)
		return 0;
	
	fprintf(s_file,"avg_rate \n");
	fflush(s_file);
	fseek(s_file,0,SEEK_SET);
	if(fscanf(s_file,"%u", &ret) < 0)
		return -1;
	
	return ret;		
}

unsigned int speedtest_lib_get_send_total(void)
{
	unsigned long long ret;
	
	if (speedtest_check_file() < 0)
		return 0;
	
	fprintf(s_file,"send_acked \n");
	fflush(s_file);
	fseek(s_file,0,SEEK_SET);
	if(fscanf(s_file,"%llu", &ret) < 0)
		return -1;	
	
	return ret;		
}

int speedtest_lib_get_max_trans_rate(void)
{
	 int ret;
	
	if (speedtest_check_file() < 0)
		return -1;
	
	fprintf(s_file,"max_trans_rate \n");
	fflush(s_file);
	fseek(s_file,0,SEEK_SET);
	if(fscanf(s_file,"%u", &ret) < 0)
		return -1;

	return ret;		
}

int speedtest_lib_set_peak_period(int period)
{
	int ret;
	
	if (speedtest_check_file() < 0)
		return -1;
	
	fprintf(s_file,"peak_period %d \n",period);
	fflush(s_file);
	fseek(s_file,0,SEEK_SET);
	if(fscanf(s_file,"%d", &ret) < 0)
		return -1;
	
	return ret;		
}

int speedtest_lib_set_try(int flag)
{
	int ret;
	
	if (speedtest_check_file() < 0)
		return -1;
	
	fprintf(s_file,"try_rate %d \n",flag);
	fflush(s_file);
	fseek(s_file,0,SEEK_SET);
	if(fscanf(s_file,"%d", &ret) < 0)
		return -1;	
	return ret;		
}

int speedtest_lib_set_debug(int debug)
{
	int ret;
	
	if (speedtest_check_file() < 0)
		return -1;
	
	fprintf(s_file,"debug %d \n",debug);
	fflush(s_file);
	fseek(s_file,0,SEEK_SET);
	if(fscanf(s_file,"%d", &ret) < 0)
		return -1;	
	
	return ret;		
}

int speedtest_lib_set_step_len(int step)
{
	int ret;
	
	if (speedtest_check_file() < 0)
		return -1;
	
	fprintf(s_file,"step_len %d \n",step);
	fflush(s_file);
	fseek(s_file,0,SEEK_SET);
	if(fscanf(s_file,"%d", &ret) < 0)
		return -1;

	return ret;		
}

int speedtest_lib_set_fix_rate(int rate)
{
	int ret;
	
	if (speedtest_check_file() < 0)
		return -1;
	
	fprintf(s_file,"fix_rate %d \n",rate);
	fflush(s_file);
	fseek(s_file,0,SEEK_SET);
	if(fscanf(s_file,"%d", &ret) < 0)
		return -1;

	return ret;		
}

int speedtest_lib_set_start_rate(int rate)
{
	int ret;
	
	if (speedtest_check_file() < 0)
		return -1;
	
	fprintf(s_file,"start_rate %d \n",rate);
	fflush(s_file);
	fseek(s_file,0,SEEK_SET);
	if(fscanf(s_file,"%d", &ret) < 0)
		return -1;

	return ret;		
}

int speedtest_lib_set_max_rate(int rate)
{
	int ret;
	
	if (speedtest_check_file() < 0)
		return -1;
	
	fprintf(s_file,"max_rate %d \n",rate);
	fflush(s_file);
	fseek(s_file,0,SEEK_SET);
	if(fscanf(s_file,"%d", &ret) < 0)
		return -1;

	return ret;		
}

int speedtest_lib_auto_learn(char *ip, int port)
{
	int ret;
	
	if (speedtest_check_file() < 0)
		return -1;
	
	fprintf(s_file,"auto_learn %s %u \n",ip, port);
	fflush(s_file);
	fseek(s_file,0,SEEK_SET);
	if(fscanf(s_file,"%d", &ret) < 0)
		return -1;

	return ret;	
}

int speedtest_lib_has_learned(void)
{
	int ret;
	
	if (speedtest_check_file() < 0)
		return -1;
	
	fprintf(s_file,"has_learned \n");
	fflush(s_file);
	fseek(s_file,0,SEEK_SET);
	if(fscanf(s_file,"%d", &ret) < 0)
		return -1;

	return ret;		
}

int speedtest_lib_port_range(int port)
{
	int ret;
	
	if (speedtest_check_file() < 0)
		return -1;
	
	fprintf(s_file,"port_range %d \n",port);
	fflush(s_file);
	fseek(s_file,0,SEEK_SET);
	if(fscanf(s_file,"%d", &ret) < 0)
		return -1;
	
	return ret;		
}

int speedtest_lib_socket(void)
{
	int ret;
    if (speedtest_check_file() < 0)
		return -1;
	
	fprintf(s_file,"socket \n");
	fflush(s_file);
	fseek(s_file,0,SEEK_SET);
	if(fscanf(s_file,"%d", &ret) < 0)
		return -1;

	return ret;
}

int speedtest_lib_connect(int sock)
{
	int ret;
    if (speedtest_check_file() < 0)
		return -1;
	
	fprintf(s_file,"connect %d \n",sock);
	fflush(s_file);
	fseek(s_file,0,SEEK_SET);
	if(fscanf(s_file,"%d", &ret) < 0)
		return -1;

	return ret;
}

int speedtest_lib_write(int sock,int len)
{
	int ret;
    if (speedtest_check_file() < 0)
		return -1;
	
	fprintf(s_file,"write %d %d \n",sock,len);
	fflush(s_file);
	fseek(s_file,0,SEEK_SET);
	if(fscanf(s_file,"%d", &ret) < 0)
		return -1;

	return ret;
}

int speedtest_lib_read(int sock,int len)
{
	int ret;
    if (speedtest_check_file() < 0)
		return -1;
	
	fprintf(s_file,"read %d %d \n",sock,len);
	fflush(s_file);
	fseek(s_file,0,SEEK_SET);
	if(fscanf(s_file,"%d", &ret) < 0)
		return -1;

	return ret;
}

int speedtest_lib_listen(int sock)
{
	int ret;
    if (speedtest_check_file() < 0)
		return -1;
	
	fprintf(s_file,"listen %d \n",sock);
	fflush(s_file);
	fseek(s_file,0,SEEK_SET);
	if(fscanf(s_file,"%d", &ret) < 0)
		return -1;

	return ret;
}

int speedtest_lib_accept(int sock)
{
	int ret;
    if (speedtest_check_file() < 0)
		return -1;
	
	fprintf(s_file,"accept %d \n",sock);
	fflush(s_file);
	fseek(s_file,0,SEEK_SET);
	if(fscanf(s_file,"%d", &ret) < 0)
		return -1;

	return ret;
}

int speedtest_lib_close(int sock)
{
	int ret;
    if (speedtest_check_file() < 0)
		return -1;
	
	fprintf(s_file,"close %d \n",sock);
	fflush(s_file);
	fseek(s_file,0,SEEK_SET);
	if(fscanf(s_file,"%d", &ret) < 0)
		return -1;

	return ret;
}
int speedtest_lib_write_data(int sock,unsigned char* buff,int len)
{
	int ret,idx,off = 0;
	char* hex_buff = malloc(len * 2 + 1);

	if(hex_buff == NULL)
		return -1;
	
    if (speedtest_check_file() < 0){
		free(hex_buff);
		return -1;
    }
	
	for(idx = 0; idx < len; idx++)
		off += sprintf(hex_buff + off,"%02x",buff[idx]);
	hex_buff[off] = 0;
	fprintf(s_file,"write_data %d %d %s",sock,len * 2,hex_buff);
	fflush(s_file);
	fseek(s_file,0,SEEK_SET);
	if(fscanf(s_file,"%d", &ret) < 0)
		ret =  -1;	
	free(hex_buff);
	return ret;
}

int speedtest_lib_set_test_time(int test_time)
{
	int ret;
	
	if (speedtest_check_file() < 0)
		return -1;
	
	fprintf(s_file,"test_time %d \n",test_time);
	fflush(s_file);
	fseek(s_file,0,SEEK_SET);
	if(fscanf(s_file,"%d", &ret) < 0)
		return -1;

	return ret;
}

int speedtest_lib_set_step_mode(int step_mode)
{
	int ret;
	
	if (speedtest_check_file() < 0)
		return -1;
	
	fprintf(s_file,"step_mode %d \n",step_mode);
	fflush(s_file);
	fseek(s_file,0,SEEK_SET);
	if(fscanf(s_file,"%d", &ret) < 0)
		return -1;

	return ret;
}

////////////////////////////////////////////////////////////////////////////////
static int g_peak_period = 1000;
static void read_loop_n(int* sock,int n)
{
	int len = 0,msec_used = 0,idx;
	unsigned long long total = 0,overhead = 0,old = 0;
	struct timeval tm1,tm2;

	gettimeofday(&tm1,NULL);	
	
	do
	{
		old = total;
		sleep(1);
		for(idx = 0; idx < n;idx++)
		{	
			if (sock[idx] < 0)
				continue;
			len = speedtest_lib_read(sock[idx],2000000000);	
			if (len < 0)
			{
				sock[idx] = -1;
				continue;
			}
			total += len;
		}
	}while(old != total);
	
	for(idx = 0; idx < n; idx++)
		speedtest_lib_close(sock[idx]);
	
	gettimeofday(&tm2,NULL);
	overhead = (total / 1440) * 54;
	msec_used = (tm2.tv_sec - tm1.tv_sec) * 1000000 + tm2.tv_usec - tm1.tv_usec;
	msec_used /= 1000;
	if (msec_used == 0)
		msec_used = 1;
	printf("\nTotal read: %llu bytes in %d ms \nAverage Rate: %llu Mbps \n",total,msec_used,(total + overhead) / (125 * msec_used));		
}

static void write_loop_n(int* sock,int n,unsigned long long len,int test_time)
{
	unsigned int  msec_used,idx, last_time=0;
	unsigned long long  total_all = 0,len_all = 0,overhead = 0;
	unsigned long long  total[MAX_SOCKET_NUM];
	struct timeval tm1,tm2;
	int send = 0;

	len_all = n * len;
	memset(total,0,sizeof(total));
	gettimeofday(&tm1,NULL);

	while(total_all < len_all)
	{
		if(test_time && last_time++ > test_time)
			break;

		for(idx = 0; idx < n; idx++)		
		{
			if (sock[idx] < 0)
				continue;

			send = (len - total[idx]) > 2000000000 ? 2000000000 : (len - total[idx]);
			send = speedtest_lib_write(sock[idx],send);
			if (send < 0)
			{
				speedtest_lib_close(sock[idx]);
				sock[idx] = -1;
				continue;
			}
			total[idx] += send;
			total_all += send;
		}
		sleep(1);
	}

	for(idx = 0; idx < n; idx++)
	{	
		if (sock[idx] >= 0)
			speedtest_lib_close(sock[idx]);
	}	
	gettimeofday(&tm2,NULL);
	msec_used = (tm2.tv_sec - tm1.tv_sec) * 1000000 + tm2.tv_usec - tm1.tv_usec;
	msec_used /= 1000;
	if (msec_used == 0)
		msec_used = 1;
	total_all = speedtest_lib_get_send_total();
	overhead = (total_all / 1440) * 54;
	printf("\nTotal Send Successfully: %llu Bytes in %u ms \nAverage Rate: %llu Kbps\n",
	total_all,msec_used,
	((total_all + overhead)*8 / (msec_used)));
	printf("Peak Rate: %u Kbps in %d ms \nUp bandwidth %u Kbps\n", 
	speedtest_lib_get_peak_send(),
	g_peak_period,
	speedtest_lib_get_avg_rate());
}
#if 0
static void read_loop(int sock,int time)
{
	int len = 0,old = 0,msec_used = 0;
	unsigned long long total = 0,overhead;
	struct timeval tm1,tm2;

	gettimeofday(&tm1,NULL);	
	do
	{
		old = len;
		sleep(1);
	    len = speedtest_lib_read(sock,2000000000);	
		if (len < 0)
			break;
		total += len;
	}while(old != len);
	speedtest_lib_close(sock);
	gettimeofday(&tm2,NULL);
	overhead = (total / 1440) * 54;
	msec_used = (tm2.tv_sec - tm1.tv_sec) * 1000000 + tm2.tv_usec - tm1.tv_usec;
	msec_used /= 1000;
	if (msec_used == 0)
		msec_used = 1;
	printf("Total read: Time(%u ms)Len(%llu Bytes) speed(%llu Mbps) \n",msec_used,total,(total + overhead) / (125 * msec_used));	
}

static void write_loop(int sock, unsigned long long len,int time)
{
	unsigned int msec_used;
	unsigned long long  total = 0,overhead;
	struct timeval tm1,tm2;
	int send = 0;

	gettimeofday(&tm1,NULL);
	while(total < len || len == 0)
	{
		send = (len - total) > 2000000000 ? 2000000000 : (len - total);
		send = speedtest_lib_write(sock,send);
		if (send < 0)
			break;
		total += send;
		sleep(1);
	}
	speedtest_lib_close(sock);
	overhead = (total / 1440) * 54;
	gettimeofday(&tm2,NULL);
	msec_used = (tm2.tv_sec - tm1.tv_sec) * 1000000 + tm2.tv_usec - tm1.tv_usec;
	msec_used /= 1000;
	if (msec_used == 0)
		msec_used = 1;
	printf("Total Send: time(%d ms) len(%llu Bytes) speed(%llu Mbps)\n",msec_used,total,(total + overhead) / (125 * msec_used));
}
#endif
void dump_send_rate(char* fname)
{
	FILE* fp;
	int idx = 0,rate;
	
	if ((fp = fopen(fname,"w+")) == NULL)
	{
		printf("open file %s faile \n",fname);
		return;
	}
	while(1)
	{
		rate = speedtest_lib_get_buff_rate(idx);
		if (rate < 0)
			break;
		fprintf(fp,"%d \n",rate);
		idx++;
	}
	fclose(fp);
}


int main(int argc,char* argv[])
{
	char* action = NULL;
	char* url = NULL;
	unsigned int server,port;
	unsigned long long len = 1;
	int ch,off = 0,sock,socks[MAX_SOCKET_NUM],tries = 0;
	char buff[128];
	struct sockaddr_in serv_addr = {0};
	struct sockaddr_in6 serv_addr6 = {0};
	char str[INET6_ADDRSTRLEN];
	int fix_rate = 0,nsock = 1,max_rate = 0,step_len = 0,debug = 0,idx;
	int start_rate = 0, try_flag = 0, step_mode = 0, test_time = 0;
	char* fname = NULL;
	int ret = 0, ipv6 = 0;
	
	len <<= 30;
	while((ch = getopt(argc,argv,"a:u:l:f:D:n:p:m:s:b:td:T:M:")) != -1)
	{
		switch (ch)
		{
			case 'a':
			  action = optarg;
			  break;
			case 'u':
			  url = optarg;
			  break;
			case 'l':
			  sscanf(optarg,"%llu",&len);
			  break;
			case 'f':
			  fix_rate = atoi(optarg);
			  break;
			case 'b':
			  start_rate = atoi(optarg);
			  break;
			case 'm':
			  max_rate = atoi(optarg);
			  break;  
			case 'n':
			  nsock = atoi(optarg);
			  break;
			case 'D':
			  fname = optarg;
			  break;
			case 'p':
			  g_peak_period = atoi(optarg);
			  break;
			case 's':
			  step_len = atoi(optarg);
			  break;
			case 'd':
			  debug = atoi(optarg);
			  break;
			case 't':
			  try_flag = 1;
			  break; 
			case 'T':
			  test_time = atoi(optarg);
			  break; 
			case 'M':
			  step_mode = atoi(optarg);
			  break; 
			default:
			  break;		
		}
	}
	
	if (action == NULL || url == NULL || nsock > MAX_SOCKET_NUM)
	{
		printf("%s \n"
				"-a <get | put> HTTP Action \n"
				"-u <url>   HTTP URL \n"
				"[-l <len>]  HTTP Post Length,default 1GB \n"
				"[-b <start rate>] Start Rate,between 0(1Mbps) and 1900(10Gbps) \n"
		        "[-f <fix rate>]  Fix Rate,between 0(1Mbps) and 1900(10Gbps) \n"
				"[-m <max rate>] Max Rate used for rate adjust,between 100(101Mps) and 1900 (10Gbps)\n"
				"[-n <sock_num>] TCP streams number \n"
				"[-p <peak period>] Peak Rate Sample Time,between 10 and 1000 \n"
				"[-s <step length/rate>] Step length used for rate adjust,between 125(1Mbps) and 12500(100Mbps) \n"
				"[-t ] Try max transmit rate on the link\n"
				"[-d ] Debug output \n"
				"<server ip>  <server port> \n",argv[0]);
		goto finish;;
	}
	speedtest_lib_set_v2_mode(1);
	
	if (speedtest_lib_reset_global() < 0)
	{
		fprintf(stderr,"speed_test_reset_global fail \n");
		ret = -1;
		goto finish;	
	}

	if (debug && speedtest_lib_set_debug(debug) < 0)
	{
		fprintf(stderr,"speedtest_lib_set_debug fail \n");
		ret = -1;
		goto finish;			
	}
	
	if (try_flag && speedtest_lib_set_try(try_flag) < 0)
	{
		fprintf(stderr,"speedtest_lib_set_try fail \n");
		ret = -1;
		goto finish;			
	}
		
	if (fix_rate > 0 && speedtest_lib_set_fix_rate(fix_rate) < 0)
	{
		fprintf(stderr,"speedtest_lib_set_fix_rate fail \n");
		ret = -1;
		goto finish;	
	}
	
	if (start_rate > 0 && speedtest_lib_set_start_rate(start_rate) < 0)
	{
		fprintf(stderr,"speedtest_lib_set_start_rate fail \n");
		ret = -1;
		goto finish;	
	}
		
	if (max_rate > 0 && speedtest_lib_set_max_rate(max_rate) < 0)
	{
		fprintf(stderr,"speedtest_lib_set_max_rate fail \n");
		ret = -1;
		goto finish;	
	}
	
	if (g_peak_period > 0 && speedtest_lib_set_peak_period(g_peak_period) < 0)
	{
		fprintf(stderr,"speedtest_lib_set_peak_period fail \n");
		ret = -1;
		goto finish;		
	}
	if (step_len > 0 && speedtest_lib_set_step_len(step_len) < 0)
	{
		fprintf(stderr,"speedtest_lib_set_step_len fail \n");
		ret = -1;
		goto finish;			
	}

	if (test_time > 0 && speedtest_lib_set_test_time(test_time) < 0)
	{
		fprintf(stderr,"speedtest_lib_set_test_time fail \n");
		ret = -1;
		goto finish;			
	}

	if (step_mode > 0 && speedtest_lib_set_step_mode(step_mode) < 0)
	{
		fprintf(stderr,"speedtest_lib_set_step_mode fail \n");
		ret = -1;
		goto finish;			
	}

	port = atoi(argv[argc - 1]);

	if(1 == inet_pton(AF_INET6, argv[argc-2], &serv_addr6.sin6_addr))	
	{
		serv_addr6.sin6_family = AF_INET6;
		serv_addr6.sin6_port = htons(port);
		sock = socket(AF_INET6,SOCK_STREAM,0);
		ipv6 = 1;
	}
	else{
		server = inet_addr(argv[argc-2]);
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_port = htons(port);
		serv_addr.sin_addr.s_addr = server;
		sock = socket(AF_INET,SOCK_STREAM,0);		
	}

	if(sock < 0){
		ret = -1;
		goto finish;
	}
		
	if (speedtest_lib_auto_learn(argv[argc-2],port) < 0)
	{
		fprintf(stderr,"speedtest_lib_auto_learn fail \n");
		ret = -1;
		close(sock);
		goto finish;	
	}
	
	while(speedtest_lib_has_learned() == 0 && tries < 1)
	{
		if(ipv6){
			if(connect(sock,(struct sockaddr*)&serv_addr6,sizeof(serv_addr6))==-1){
				ret = -1;
				goto finish;
			}
		}		
		else{
			if(connect(sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr))==-1){
				ret = -1;
				goto finish;
			}
		}
		tries++;
		sleep(0);
	}
	close(sock);
	
	if (speedtest_lib_has_learned() == 0)
	{
		fprintf(stderr,"auto learn fail %s %s !\n",argv[argc-2], argv[argc-1]);
		goto finish;	;
	}
	
	for(idx = 0; idx < nsock; idx++)
	{
		if ((socks[idx] = speedtest_lib_socket()) < 0)
		{
			fprintf(stderr,"speedtest_socket %d fail \n",idx);
			ret = -1;
			goto finish;	
		}
	}
	for(idx = 0; idx < nsock; idx++)
	{	
		if (speedtest_lib_connect(socks[idx]) < 0)
		{
			fprintf(stderr,"speedtest_connect %d fail \n",idx);
			ret = -1;
			goto finish;	
		}
	}
	if (strcmp(action,"get") == 0){
		off = sprintf(buff,"GET %s HTTP/1.1\r\n",url);
		if(ipv6){
			if(inet_ntop(AF_INET6, &serv_addr6.sin6_addr, str, INET6_ADDRSTRLEN) == NULL){
				ret = -1;
				goto finish;
			}
			off += sprintf(buff + off ,"Host: [%s]\r\n",str);		
		}	
		else
			off += sprintf(buff + off ,"Host: %s\r\n",argv[argc-2]);			
		off += sprintf(buff + off,"Connection: close\r\n");
		off += sprintf(buff + off,"\r\n");	
		for(idx = 0; idx < nsock; idx++)
		{		
			if (speedtest_lib_write_data(socks[idx], (unsigned char*)buff,off) < 0)
			{
				fprintf(stderr,"speedtest_write_data %d fail \n",idx);
				ret = -1;
				goto finish;	
			}	
		}	
		read_loop_n(socks,nsock);
	}else if (strcmp(action,"put") == 0){
		off = sprintf(buff,"PUT %s HTTP/1.1\r\n",url);
		if(ipv6){
			if(inet_ntop(AF_INET6, &serv_addr6.sin6_addr, str, INET6_ADDRSTRLEN) == NULL){
				ret = -1;
				goto finish;
			}
			off += sprintf(buff + off ,"Host: [%s]\r\n",str);		
		}
		else
			off += sprintf(buff + off ,"Host: %s\r\n",argv[argc-2]);	
		off += sprintf(buff + off,"Accept:*/*\r\n");
		off += sprintf(buff + off,"Content-Type:text/plain\r\n");	 	
		//off += sprintf(buff + off,"Content-Type: multipart/form-data\r\n");
		off += sprintf(buff + off,"Content-Length:%llu\r\n",len);		
		off += sprintf(buff + off,"Connection:keep-alive\r\n");
		off += sprintf(buff + off,"\r\n");		
		for(idx = 0; idx < nsock; idx++)
		{	
			if (speedtest_lib_write_data(socks[idx], (unsigned char*)buff,off) < 0)
			{
				fprintf(stderr,"speedtest_write_data %d fail \n",idx);
				ret = -1;
				goto finish;	
			}
		}
		write_loop_n(socks,nsock,len,test_time);
		if (fname != NULL)
			dump_send_rate(fname);
	}
	if (try_flag)
		printf("max_trans_rate=%d\n",speedtest_lib_get_max_trans_rate());

finish:
	
	return ret;
}

