#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/un.h>
#include <stdarg.h>
#include "ecnt_lib_printf.h"

#include <time.h>
#include <string.h>
#include "modules/ecnt_log/ecnt_log.h"

int	
ecnt_printf(int module_num, int level, const char *fmt,...){
	FILE *proc_file = NULL;	
	char msg[MSG_LEN] = {0};	
	char tmp[32] = {0};	
	char *module_name = NULL;	
	char *module_type = NULL;		
	time_t timer;
	struct tm *tblock = NULL;	
	va_list args;
	
	va_start(args, fmt);	
	vsnprintf(msg, MSG_LEN, fmt, args);
	va_end(args);
	
	module_name = ecnt_log_conf[module_num].module_name;	
	module_type = enum_to_string(module_num);
	
	timer = time(NULL);	
	tblock = localtime(&timer);
	
	memset(tmp, 0, sizeof(tmp));	
	snprintf(tmp, sizeof(tmp), PROC_PATH_F, module_name); 
	
	proc_file = fopen(tmp, "w");	
	if (!proc_file) {		
		printf("open %s fail\n", tmp);			
		return; 
	}	
	fprintf(proc_file, "[%s][%d][%s]:%s",asctime(tblock), level, module_type, msg);	
	fclose(proc_file);	
}

int	
ecnt_raw_printf(int module_num, int level, char *data, int len){
	FILE *proc_file = NULL;	
	char raw_data[RAW_DATA_LEN] = {0};	
	char tmp[32] = {0};	
	char *module_name = NULL;	
	char *module_type = NULL;		
	time_t timer;
	struct tm *tblock = NULL;	


	memset(raw_data, 0, sizeof(raw_data));
	memcpy(raw_data, data, len);
	
	module_name = ecnt_log_conf[module_num].module_name;	
	module_type = enum_to_string(module_num);	
	
	timer = time(NULL);	
	tblock = localtime(&timer);
	
	memset(tmp, 0, sizeof(tmp));	
	snprintf(tmp, sizeof(tmp), PROC_PATH_F, module_name); 
	
	proc_file = fopen(tmp, "w");	
	if (!proc_file) {		
		printf("open %s fail\n", tmp);			
		return; 
	}	
	fprintf(proc_file, "[%s][%d][%s]\n%s",asctime(tblock), level, module_type, raw_data);	
	fclose(proc_file);	
}

