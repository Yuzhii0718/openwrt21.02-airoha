/***************************************************************
Copyright Statement:

This software/firmware and related documentation (EcoNet Software) 
are protected under relevant copyright laws. The information contained herein 
is confidential and proprietary to EcoNet (HK) Limited (EcoNet) and/or 
its licensors. Without the prior written permission of EcoNet and/or its licensors, 
any reproduction, modification, use or disclosure of EcoNet Software, and 
information contained herein, in whole or in part, shall be strictly prohibited.

EcoNet (HK) Limited  EcoNet. ALL RIGHTS RESERVED.

BY OPENING OR USING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY 
ACKNOWLEDGES AND AGREES THAT THE SOFTWARE/FIRMWARE AND ITS 
DOCUMENTATIONS (ECONET SOFTWARE) RECEIVED FROM ECONET 
AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON AN AS IS 
BASIS ONLY. ECONET EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, 
WHETHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, 
OR NON-INFRINGEMENT. NOR DOES ECONET PROVIDE ANY WARRANTY 
WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTIES WHICH 
MAY BE USED BY, INCORPORATED IN, OR SUPPLIED WITH THE ECONET SOFTWARE. 
RECEIVER AGREES TO LOOK ONLY TO SUCH THIRD PARTIES FOR ANY AND ALL 
WARRANTY CLAIMS RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES 
THAT IT IS RECEIVERS SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD 
PARTY ALL PROPER LICENSES CONTAINED IN ECONET SOFTWARE.

ECONET SHALL NOT BE RESPONSIBLE FOR ANY ECONET SOFTWARE RELEASES 
MADE TO RECEIVERS SPECIFICATION OR CONFORMING TO A PARTICULAR 
STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND 
ECONET'S ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE ECONET 
SOFTWARE RELEASED HEREUNDER SHALL BE, AT ECONET'S SOLE OPTION, TO 
REVISE OR REPLACE THE ECONET SOFTWARE AT ISSUE OR REFUND ANY SOFTWARE 
LICENSE FEES OR SERVICE CHARGES PAID BY RECEIVER TO ECONET FOR SUCH 
ECONET SOFTWARE.
***************************************************************/

/************************************************************************
*                  I N C L U D E S
************************************************************************/
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>

#include "libapi_lib_utility.h"
#include <fcntl.h>
#include <limits.h>
#include <sys/stat.h>

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
************************************************************************/

/************************************************************************
*                  M A C R O S
************************************************************************/

/************************************************************************
*                  D A T A	 T Y P E S
************************************************************************/

/************************************************************************
*                  E X T E R N A L	 D A T A   D E C L A R A T I O N S
************************************************************************/

/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
************************************************************************/

/************************************************************************
*                  P U B L I C   D A T A
************************************************************************/

/************************************************************************
*                  P R I V A T E   D A T A
************************************************************************/

/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
************************************************************************/

/******************************************************************************
 Descriptor:	It's used to show p(cmds_t table) command usage.
 Input Args:	argc:
                 argv:
                     g: cmds_t table
 Ret Value:	API_LIB_SUCCESS.
******************************************************************************/
int api_lib_help(int argc, char* argv[], void *p)
{
	const cmds_t *cmdp;
	
#if API_LIB_DBG
		printf("enter %s.\n", __func__);
#endif

	printf("Usage:\n");
	for(cmdp = (const cmds_t *)p; cmdp->name != NULL; cmdp++ ) {
		printf(" %s: %s\n", cmdp->name, cmdp->argc_errmsg);
	}

	return API_LIB_SUCCESS;
}

/* Call a subcommand based on the first token in an already-parsed line */
int subcmd	(const cmds_t tab[], int argc, char *argv[], void *p)
{
	register const cmds_t *cmdp;
	int found = 0;
	int i, len;

	/* Strip off first token and pass rest of line to subcommand */
	if (argc < 2) {
		if (argc < 1)
			printf("SUBCMD - Don't know what to do?\n");
		else
			goto print_out_cmds;
			//printf("\"%s\" - takes at least one argument\n",argv[0]);
		return -1;
	}
	argc--;
	argv++;
	for(cmdp = tab;cmdp->name != NULL;cmdp++){
		if(strcmp(argv[0],cmdp->name) == 0){
			found = 1;
			break;
		}
	}
	if(!found){
		char buf[102];

print_out_cmds:
		printf("valid subcommands:\n");
		memset(buf,' ',sizeof(buf));
		buf[100] = '\n';
		buf[101] = '\0';
		for(i=0,cmdp = tab;cmdp->name != NULL;cmdp++){
			len = strlen(cmdp->name);
			strncpy(&buf[i*25],cmdp->name,(len > 25) ? 25 : len);
			if(i == 3){
				printf("%s",buf);
				memset(buf,' ',sizeof(buf));
				buf[100] = '\n';
				buf[101] = '\0';
			}
			i = (i+1)%4;
		}
		if(i != 0)
			printf("%s",buf);
		return -1;
	}
	if(argc <= cmdp->argcmin){
		if(cmdp->argc_errmsg != NULL)
			printf("Usage: %s\n",cmdp->argc_errmsg);
		return -1;
	}
	if(cmdp->func == NULL) 
		return 0;
	return (*cmdp->func)(argc,argv,p);
}

int api_lib_find_pid_by_name( char *ProcName, int *foundpid, int pidNum, int *foundnum)
{
	DIR *dir = NULL;
	struct dirent *d = NULL;
	int pid = 0, i = 0;
	char *s = NULL;
	int pnlen = 0;
	char exe[PATH_MAX_DNSMASQ+1] = {0};
	char path[PATH_MAX_DNSMASQ+1] = {0};
	int len = 0;
	int namelen = 0;

	if ( NULL == ProcName
		|| NULL == foundpid
		|| NULL == foundnum )
		return -1;

	foundpid[0] = 0;
	pnlen = strlen(ProcName);
	*foundnum = 0;

	/* Open the /proc directory. */
	dir = opendir("/proc");
	if (!dir)
	{
        printf("cannot open /proc");
        return -1;
	}

	/* Walk through the directory. */
	while ((d = readdir(dir)) != NULL) 
	{
		if(i >= pidNum)
			break;
		
        /* See if this is a process */
        if ((pid = atoi(d->d_name)) == 0)
			continue;

        snprintf(exe, sizeof(exe), "/proc/%s/exe", d->d_name);
		
        if ((len = readlink(exe, path, PATH_MAX_DNSMASQ)) < 0)
                continue;
        path[len] = '\0';

        /* Find ProcName */
        s = strrchr(path, '/');
        if(s == NULL)
			continue;
        s++;

        /* we don't need small name len */
        namelen = strlen(s);
        if(namelen < pnlen) 
			continue;

        if(!strncmp(ProcName, s, pnlen)) 
		{
            /* to avoid subname like search proc tao but proc taolinke matched */
            if(s[pnlen] == ' ' || s[pnlen] == '\0') 
			{
                foundpid[i] = pid;
                i++;
            }
        }
	}

	*foundnum = i;
	foundpid[i] = 0;
	closedir(dir);
	return  0;
}

void fileRead(char *path, char *buf, int size){
	int  fd=0, ret=0;

	memset(buf,0, size);
	fd = open(path,O_RDONLY);
	if(fd == -1){
		return;
	}

	ret = read(fd, buf, size - 1);
	if(ret <= 0){
		close(fd);
		buf[0] = '\0';
		return;
	}
	buf[size - 1] = '\0';
	close(fd);
}

int mtd_open(const char *mtd, int flags)
{
	FILE *fp;
	char dev[PATH_MAX] = {0};
	int i = 0;

	if ((fp = fopen("/proc/mtd", "r"))) 
	{
		while (fgets(dev, sizeof(dev), fp)) 
		{
			if (sscanf(dev, "mtd%d:", &i) && strstr(dev, mtd)) 
			{
				snprintf(dev, sizeof(dev), "/dev/mtd%d", i);
				fclose(fp);
				return open(dev, flags);
			}
		}
		fclose(fp);
	}

	return open(mtd, flags);
}

/*
escape or filter specail characters for linux
-1: NOT process.
 1: allow keep self.
 0: must do escape.
*/
static char s_escape_code_white_list[] = 
{
	-1, /*	0	  NUL */
	-1, /*	1	  SOH */
	-1, /*	2	  STX */
	-1, /*	3	  ETX */
	-1, /*	4	  EOT */
	-1, /*	5	  ENQ */
	-1, /*	6	  ACK */
	-1, /*	7	  BEL */
	-1, /*	8	   BS */
	-1, /*	9	   HT */
	-1, /*  10	   LF */
	-1, /*  11	   VT */
	-1, /*  12	   FF */
	-1, /*  13	   CR */
	-1, /*  14	   SO */
	-1, /*  15	   SI */
	-1, /*  16	  DLE */
	-1, /*  17	  DC1 */
	-1, /*  18	  DC2 */
	-1, /*  19	  DC3 */
	-1, /*  20	  DC4 */
	-1, /*  21	  NAK */
	-1, /*  22	  SYN */
	-1, /*  23	  ETB */
	-1, /*  24	  CAN */
	-1, /*  25	   EM */
	-1, /*  26	  SUB */
	-1, /*  27	  ESC */
	-1, /*  28	   FS */
	-1, /*  29	   GS */
	-1, /*  30	   RS */
	-1, /*  31	   US */
	 0, /*  32	SPACE */
	 0, /*  33	!	  */
	 0, /*  34	"	  */
	 0, /*  35	#	  */
	 0, /*  36	$	  */
	 0, /*  37	%	  */
	 0, /*  38	&	  */
	 0, /*  39	'	  */
	 0, /*  40	( 	  */
	 0, /*  41	)	  */
	 0, /*  42	*	  */
	 0, /*  43	+	  */
	 0, /*  44	,	  */
	 0, /*  45	-	  */
	 0, /*  46	.	  */
	 0, /*  47	/	  */
	 1, /*  48	0	  */
	 1, /*  49	1	  */
	 1, /*  50	2	  */
	 1, /*  51	3	  */
	 1, /*  52	4	  */
	 1, /*  53	5	  */
	 1, /*  54	6	  */
	 1, /*  55	7	  */
	 1, /*  56	8	  */
	 1, /*  57	9	  */
	 0, /*  58	:	  */
	 0, /*  59	;	  */
	 0, /*  60	<	  */
	 0, /*  61	=	  */
	 0, /*  62	>	  */
	 0, /*  63	?	  */
	 0, /*  64	@	  */
	 1, /*  65	A	  */
	 1, /*  66	B	  */
	 1, /*  67	C	  */
	 1, /*  68	D	  */
	 1, /*  69	E	  */
	 1, /*  70	F	  */
	 1, /*  71	G	  */
	 1, /*  72	H	  */
	 1, /*  73	I	  */
	 1, /*  74	J	  */
	 1, /*  75	K	  */
	 1, /*  76	L	  */
	 1, /*  77	M	  */
	 1, /*  78	N	  */
	 1, /*  79	O	  */
	 1, /*  80	P	  */
	 1, /*  81	Q	  */
	 1, /*  82	R	  */
	 1, /*  83	S	  */
	 1, /*  84	T	  */
	 1, /*  85	U	  */
	 1, /*  86	V	  */
	 1, /*  87	W	  */
	 1, /*  88	X	  */
	 1, /*  89	Y	  */
	 1, /*  90	Z	  */
	 0, /*  91	[	  */
	 0, /*  92	\	  */
	 0, /*  93	]	  */
	 0, /*  94	^	  */
	 0, /*  95	_	  */
	 0, /*  96	`	  */
	 1, /*  97	a	  */
	 1, /*  98	b	  */
	 1, /*  99	c	  */
	 1, /* 100	d	  */
	 1, /* 101	e	  */
	 1, /* 102	f	  */
	 1, /* 103	g	  */
	 1, /* 104	h	  */
	 1, /* 105	i	  */
	 1, /* 106	j	  */
	 1, /* 107	k	  */
	 1, /* 108	l	  */
	 1, /* 109	m	  */
	 1, /* 110	n	  */
	 1, /* 111	o	  */
	 1, /* 112	p	  */
	 1, /* 113	q	  */
	 1, /* 114	r	  */
	 1, /* 115	s	  */
	 1, /* 116	t	  */
	 1, /* 117	u	  */
	 1, /* 118	v	  */
	 1, /* 119	w	  */
	 1, /* 120	x	  */
	 1, /* 121	y	  */
	 1, /* 122	z	  */
	 0, /* 123	{	  */
	 0, /* 124	|	  */
	 0, /* 125	}	  */
	 0, /* 126	~	  */
	-1, /* 127	  DEL */
};

static int is_injection_code(unsigned char ch_at)
{
	if ( ch_at >= sizeof(s_escape_code_white_list) )
		return 0;

	if ( 1 != s_escape_code_white_list[ch_at] )
	{
		return 1;
	}

	return 0;
}

void api_escape_parameter(char *parameter, char *escape_parameter, int escape_parameter_len)
{
	int idx = 0, new_buf_len = 0;
	unsigned char ch_at = 0;
	char *new_buf = NULL;

	if ( NULL == parameter || NULL == escape_parameter || escape_parameter_len <= 0 )
	{
		printf("[%s]%d: parameter err.\n", __FUNCTION__, __LINE__);
		return;
	}

	bzero(escape_parameter, escape_parameter_len);

	new_buf = escape_parameter;
	while ( 0 != parameter[idx] )
	{
		ch_at = (unsigned char)parameter[idx];

		/* ASCII CTRL */
		if ( (ch_at >= 0x01 && ch_at <= 0x1F) || 0x7F == ch_at )
		{
			if ( (new_buf_len + 1) >= escape_parameter_len )
			{
				printf("[%s]%d: escape buffer is not enough.\n", __FUNCTION__, __LINE__);
				return;
			}

			new_buf[new_buf_len ++] = 0x00;
			return ; /* break down when meet ctrl ascii. */
		}
		else if ( ch_at < 0x7F )
		{
			if ( 1 == is_injection_code(ch_at) )
			{
				if ( (new_buf_len + 2) >= escape_parameter_len )
				{
					printf("[%s]%d: escape buffer is not enough.\n", __FUNCTION__, __LINE__);
					return;
				}

				new_buf[new_buf_len ++] = '\\';
				new_buf[new_buf_len ++] = ch_at;
			}
			else
			{
				if ( (new_buf_len + 1) >= escape_parameter_len )
				{
					printf("[%s]%d: escape buffer is not enough.\n", __FUNCTION__, __LINE__);
					return;
				}

				new_buf[new_buf_len ++] = ch_at;
			}
		}
		else if ( ch_at > 0x7F )
		{
			if ( (new_buf_len + 1) >= escape_parameter_len )
			{
				printf("[%s]%d: escape buffer is not enough.\n", __FUNCTION__, __LINE__);
				return;
			}

			new_buf[new_buf_len ++] = ch_at;
		}

		idx ++;
	}

	if ( (new_buf_len + 1) >= escape_parameter_len )
	{
		printf("[%s]%d: escape buffer is not enough.\n", __FUNCTION__, __LINE__);
		return;
	}
	new_buf[new_buf_len ++] = 0x00;

}

int is_emmc_blk(void)
{
	struct stat st;
	char *path = "/sys/class/block/mmcblk0";
	
	if(!stat(path, &st) && S_ISDIR(st.st_mode))
		return 1;
	
	return 0;
}