/*
 * mtd - simple memory technology device manipulation tool
 *
 * Copyright (c) 2006, 2007 Thorsten Glaser <tg@freewrt.org>
 * Copyright (C) 2005 Waldemar Brodkorb <wbx@freewrt.org>,
 *	                  Felix Fietkau <nbd@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * The code is based on the linux-mtd examples.
 */


#include <limits.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/reboot.h>
#include <sys/syscall.h>
#include <string.h>
#include <linux/reboot.h>
#include <linux/types.h>
#include <trx.h>
#include <mtd/mtd-user.h>
#include "flash_layout/tc_partition.h"
#include "ecnt_sys.h"

#define ART_AREA_NUM (sizeof(size_art)/ sizeof(int))

#define ECNT_MTD_CMD 			"/sbin/ecnt_mtd %s %s %d %d %s"
#define BOOTFLAG_LEN 			1
#define BOB_OFFSET 				0x060000

#define RESERVEAREA_NAME "art"

#if 0
#if defined(TCSUPPORT_MT7570)
#define MTD_BOB_DEFAULT_FILE "/tmp/7570_bob.conf"
#define BOB_LEN 				160
#elif defined(TCSUPPORT_MT7572)
#define MTD_BOB_DEFAULT_FILE "/etc/lddla/en7572_bob.conf"
#define BOB_LEN 				0x201
#endif
#endif

int swap_bootflag(int offset)
{
	char temp[256] = {0};
	FILE *fp=NULL;
	char boot_flag = -1;

	snprintf(temp, sizeof(temp), ECNT_MTD_CMD, "readflash", BOOT_FLAG_FILE, BOOTFLAG_LEN, offset, RESERVEAREA_NAME);
	system(temp);

	fp = fopen(BOOT_FLAG_FILE, "r+");
	if (fp == NULL)
		return -1;
	
	if (1 != fread(&boot_flag, 1, 1, fp)) {
		fprintf(stderr, "read error!\n");
		fclose(fp);
		return -1;
	}

	if (boot_flag == '1')
		boot_flag = '0';
	else
		boot_flag = '1';
	
	fseek(fp, 0, SEEK_SET);
	if (1 != fwrite(&boot_flag, 1, 1, fp)) {
		fprintf(stderr, "write error!\n");
		fclose(fp);
		return -1;
	}
	fclose(fp);

	snprintf(temp, sizeof(temp), ECNT_MTD_CMD,  "writeflash", BOOT_FLAG_FILE, BOOTFLAG_LEN, offset, RESERVEAREA_NAME);
	system(temp);
	unlink(BOOT_FLAG_FILE);
	return 0;
}
void
usage(void)
{
	fprintf(stderr, "Usage: ecnt_sys  <command> <flag> [<size>] [<imagefile>] \n\n"
"ecnt_sys recognises these commands :\n"
"	set : write <imagefile> to art\n"
"	get : read <imagefile> from art\n"
"	swap:  bootflag\n"
"	del:  delete bob file\n"


"	flag for get/set:reserve, bob, wifi_24G, wifi_5G, wifi_6G, prolinecmd, bootflag\n"
"	imagefile/size for get/set\n"

"Example:  \n"
"	ecnt_sys swap bootflag\n"
"	ecnt_sys set/get/del bob [7570/7572/dualbob/dualbob_7572]\n"
"	ecnt_sys get/set wifi_5G 1024 /tmp/wifi_5g\n"
"	ecnt_sys set bootflag /tmp/boot\n\n");
	exit(1);
}

int
main_mtd(int argc, char **argv)
{
	int block_size = 0;
	int offset = 0;
	int i = 0;
	int num = 0;
	int len_bob_1g = 0, len_bob_10g = 0;
	int block_num[ART_AREA_NUM] ={0};
	int block_offset[ART_AREA_NUM] ={0};
	char temp[128] = {0};
	char cmd[16] = {0};
	char *path_1g = NULL;
	char *path_10g = NULL;
	int offset_1g = 0;
	int offset_10g = 0;

	enum {
		FLAG_RESERVE,
		FLAG_6G,
		FLAG_BOB,
		FLAG_24G,
		FLAG_5G,
		PROLINECMD,
		FLAG_BOOTFLAG,
		SWAP_BOOTFLAG
	} flag;

	/*5:set/get bob,2.4G/5G cali; 4:bootflag; 3: swap bootflag*/
	if (argc != 5 && argc != 4 && argc != 3) 
		usage();

	if (strcmp(argv[1], "set") == 0 ) 
		strncpy(cmd, "writeflash", sizeof(cmd));
	else if (strcmp(argv[1], "get") == 0)
		strncpy(cmd, "readflash", sizeof(cmd));
	else if (strcmp(argv[1], "del") == 0)
		strncpy(cmd, "del", sizeof(cmd));
	else if (strcmp(argv[1], "swap") == 0 && strcmp(argv[2], "bootflag") == 0) 
		flag = SWAP_BOOTFLAG;
	else 
		usage();
	
	if (strcmp(argv[2], "bob") == 0)  {
			flag = FLAG_BOB;
	}
	else if (argc == 5 || argc == 4) {/*set/get or bootflag*/
		if (strcmp(argv[2], "reserve") == 0 ) {
			flag = FLAG_RESERVE;
		}
		else if (strcmp(argv[2], "wifi_24G") == 0) {
			flag = FLAG_24G;
		}
		else if (strcmp(argv[2], "wifi_5G") == 0) {	
			flag = FLAG_5G;
		}
		else if (strcmp(argv[2], "prolinecmd") == 0)  {
			flag = PROLINECMD;
		}
		else if (strcmp(argv[2], "bootflag") == 0)  {
			flag = FLAG_BOOTFLAG;
		}
		else if (strcmp(argv[2], "wifi_6G") == 0)  {
			flag = FLAG_6G;
		}
		else {
			usage();
		}
	}

	block_size = RESERVEAREA_ERASE_SIZE;
	/*get offset*/
	for (i = 0; i < (ART_AREA_NUM - 1); i++){
		if(size_art[i]%block_size > 0) 
			block_num[i] = size_art[i]/block_size + 1;
		else
			block_num[i] = size_art[i]/block_size;
	}
	
	block_offset[ART_AREA_NUM-1]=(TCSUPPORT_RESERVEAREA_BLOCK-1)*block_size;
	
	for (i = ART_AREA_NUM-2; i >= 0; i--) { 			
		block_offset[i] = block_offset[i+1] - block_num[i]*block_size;			
	}
	/*get offset end*/

	switch (flag) {
		case FLAG_RESERVE:
		case FLAG_24G:
		case FLAG_5G:
		case FLAG_6G:
		case PROLINECMD:
			/*/sbin/ecnt_sys get/set prolinecmd <size> path */
			snprintf(temp, sizeof(temp), ECNT_MTD_CMD, cmd, argv[4], atoi(argv[3]), block_offset[flag], RESERVEAREA_NAME);
			system(temp);
			break;
		case FLAG_BOB:
			if (argv[3] != NULL && 0 == strncmp(argv[3], bob_cmd[0], strlen(bob_cmd[0]))) { /*dualbob*/
				if(0 == strncmp(argv[3], bob_cmd[1], strlen(bob_cmd[1]))) { /*dualbob_7572*/
					/*bobcmd-->dualbob_7572: two bob in /tmp/*/
					len_bob_1g = len_bob_10g = BOB_7572_LEN;
					path_1g = bob_path[2];
					path_10g = bob_path[3];
				}
				else{
					/*bobcmd-->dualbob, 1g bob /tmp/7570_bob.conf, 10g bob /etc/lddla/en7572_bob.conf*/
					len_bob_1g = BOB_7570_LEN;
					len_bob_10g = BOB_7572_LEN;
					path_1g = bob_path[0];
					path_10g = bob_path[1];
				}
			}
			else if (argv[3] != NULL && 0 == strncmp(argv[3], bob_cmd[2], strlen(bob_cmd[2]))){ 
				/*bobcmd-->7570:/tmp/7570_bob.conf, use default flash offset*/
				path_10g = bob_path[0];					
				len_bob_10g = BOB_7570_LEN;
			}
			else if (argv[3] != NULL && 0 == strncmp(argv[3], bob_cmd[4], strlen(bob_cmd[4]))){ 
				/*bob_cmd-->8901:/etc/lddla/bob.conf */
				path_10g = bob_path[1];					
				len_bob_10g = BOB_8901_LEN;
			}
			else { 
				/*bob_cmd-->7572 or noset  /etc/lddla/bob.conf */
				path_10g = bob_path[1];					
				len_bob_10g = BOB_7572_LEN;
			}

			if (0 == strncmp(cmd, "del", sizeof(cmd))) {
				unlink(path_1g);
				unlink(path_10g);
			}
			else {
				if (path_1g != NULL) {
					snprintf(temp, sizeof(temp), ECNT_MTD_CMD, cmd, path_1g, len_bob_1g, block_offset[flag], RESERVEAREA_NAME);
					system(temp);
					chmod(path_1g, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
				}
				if (path_10g != NULL) {
					snprintf(temp, sizeof(temp), ECNT_MTD_CMD, cmd, path_10g, len_bob_10g, block_offset[flag] + block_size/2, RESERVEAREA_NAME);
			system(temp);
					chmod(path_10g, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
				}
			}
			break;
		case FLAG_BOOTFLAG:
			/*bootflag: the last block*/
			offset = (TCSUPPORT_RESERVEAREA_BLOCK-1)*block_size;
			
			/* ecnt_sys set bootflag /tmp/boot*/
			snprintf(temp, sizeof(temp), ECNT_MTD_CMD, cmd, argv[3], BOOTFLAG_LEN, offset, RESERVEAREA_NAME);
			system(temp);
			break;
		case SWAP_BOOTFLAG:
			offset = (TCSUPPORT_RESERVEAREA_BLOCK-1)*block_size;
			swap_bootflag(offset);
			break;
		default:
			fprintf(stderr, "unknown flag type!\n");

	}
	return 0;
}
