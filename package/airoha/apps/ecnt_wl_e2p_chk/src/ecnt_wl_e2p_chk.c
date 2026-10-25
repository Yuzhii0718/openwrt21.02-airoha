/*****************************************************************************
 * Airoha (HK) Limited  Airoha. ALL RIGHTS RESERVED.
 * 
 * BY OPENING OR USING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY 
 * ACKNOWLEDGES AND AGREES THAT THE SOFTWARE/FIRMWARE AND ITS 
 * DOCUMENTATIONS (��Airoha SOFTWARE��) RECEIVED FROM Airoha 
 * AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON AN ��AS IS�� 
 * BASIS ONLY. Airoha EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, 
 * WHETHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, 
 * OR NON-INFRINGEMENT. NOR DOES Airoha PROVIDE ANY WARRANTY 
 * WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTIES WHICH 
 * MAY BE USED BY, INCORPORATED IN, OR SUPPLIED WITH THE Airoha SOFTWARE. 
 * RECEIVER AGREES TO LOOK ONLY TO SUCH THIRD PARTIES FOR ANY AND ALL 
 * WARRANTY CLAIMS RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES 
 * THAT IT IS RECEIVER��S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD 
 * PARTY ALL PROPER LICENSES CONTAINED IN Airoha SOFTWARE.
 * 
 * Airoha SHALL NOT BE RESPONSIBLE FOR ANY Airoha SOFTWARE RELEASES 
 * MADE TO RECEIVER��S SPECIFICATION OR CONFORMING TO A PARTICULAR 
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND 
 * Airoha'S ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE Airoha 
 * SOFTWARE RELEASED HEREUNDER SHALL BE, AT Airoha'S SOLE OPTION, TO 
 * REVISE OR REPLACE THE Airoha SOFTWARE AT ISSUE OR REFUND ANY SOFTWARE 
 * LICENSE FEES OR SERVICE CHARGES PAID BY RECEIVER TO Airoha FOR SUCH 
 * Airoha SOFTWARE.
 
 * $File:   ecnt_wl_e2p_chk.c
 * App for checking the wifi eeprom table is correct or not
 *
 * $Author: Geo Yang
 * $Date:   14 Apr, 2021
 *
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>    //to use bool
#include <string.h>
#include <errno.h>
#include <memory.h>
#include <getopt.h>
#include <unistd.h>     //to use read(), open(), close()
#include <sys/stat.h>
#include <fcntl.h>

//set the file path to be the same as Linux driver
//since we're not sure one day if mt76+mac80211.ko is porting to tclinux
#define BIN_FILE_PATH_5G        "/etc/Wireless/RT2860AP_AC/RT30xxEEPROM.bin"
#define BIN_FILE_PATH_2G        "/etc/RT30xxEEPROM.bin"
#define BIN_FILE_TMP_PATH       "/tmp/RT30xxEEPROM.bin"
#define BIN_FILE_TMP_2G         "/tmp/wifi_2g.bin"
#define BIN_FILE_TMP_5G         "/tmp/wifi_5g.bin"
#define BIN_FILE_TMP_6G         "/tmp/wifi_6g.bin"
#define CMD_ECNT_SYS_GET        "/sbin/ecnt_sys get"

#define BAND_2G 2
#define BAND_5G 5
#define BAND_6G 6

#define CHECK_EEPROM_OK       0
#define CHECK_EEPROM_NOK     -1
#define CHECK_2G_EEPROM_OK  0x1
#define CHECK_5G_EEPROM_OK  0x2
#define CHECK_6G_EEPROM_OK  0x3
#if defined(MT7915D) || defined(MT7916D)  
#if defined(MT7915D)
	char chip_name[32]="MT7915D";
#else
	char chip_name[32]="MT7916D";
#endif
#else
	char chip_name[32]={0};
#endif

static void usage(void)
{
    printf("ecnt_wl_e2p_chk: App for checking the wifi eeprom table is correct or not\n");
    printf("\nUsage:\n");
    printf("  ecnt_wl_e2p_chk -c <chip name> -b <band>\n");
    printf("\nExample:\n");
    printf("  ecnt_wl_e2p_chk -c mt7915a -b 5\n");
    printf("  ecnt_wl_e2p_chk -c mt7990 -b 6\n");
    printf("  Or you can issue \"ecnt_wl_e2p_chk -a\" will auto check by compile option\n");
    printf("\nArguments:\n");
    printf("  -a              auto check by compile option.\n");
    printf("                  2.4G, 5G or 6G eeprom will be generated at %s, %s or %s\n", BIN_FILE_TMP_2G, BIN_FILE_TMP_5G, BIN_FILE_TMP_6G);
    printf("                  return 1 for 2.4G ok, 2 for 5G ok, 3 for both, 0 for none\n");
    printf("  -c <chip name>  wifi chip name. ex:mt7990, mt7915a, mt7915d\n");
    printf("  -b <band>       band index. %d for 2.4G and %d for 5G\n", BAND_2G, BAND_5G);

    return;
}

struct chipset_tbl {
    const char *chip_name;
    unsigned int num_in_hex;
};

static struct chipset_tbl chipset_table_list[] = {
    {"mt7990", 0x7990},
	{"mt7992", 0x7992},
    {"mt7915a", 0x7915},
    {"mt7915d", 0x7915},
    {"mt7615n", 0x7615},
    {"mt7615d", 0x7615},
    {"mt7592", 0x7592},
    {"mt7613", 0x7663},
    {"mt7916d", 0x7916},
    {NULL, 0x0}
};


static void generate_e2p_file_at_tmp(int band, int size)
{
    char tmp[100] = {0};
    if (band == BAND_2G){
        snprintf(tmp, sizeof(tmp), "%s wifi_24G %d %s", CMD_ECNT_SYS_GET, size, BIN_FILE_TMP_2G);
    }
    else if(band == BAND_5G){
        snprintf(tmp, sizeof(tmp), "%s wifi_5G %d %s", CMD_ECNT_SYS_GET, size, BIN_FILE_TMP_5G);
    }
    else if(band == BAND_6G){
        snprintf(tmp, sizeof(tmp), "%s wifi_6G %d %s", CMD_ECNT_SYS_GET, size, BIN_FILE_TMP_6G);
    }
    else {
        printf("ecnt_wl_e2p_chk: wrong input!\n");
        return;
    }
    system(tmp);
    return;
}

static int check_e2p_file_at_tmp(const char *file_path, int chip_id)
{
    int fd = -1, len = 0;
    char tempbuffer[16] = {0};
    unsigned short id = 0;

    fd = open(file_path, O_RDONLY);
    if (fd < 0){
        printf("ecnt_wl_e2p_chk: open %s fail !\n", file_path);
        return CHECK_EEPROM_NOK;
    }

    len = read(fd, tempbuffer, sizeof(tempbuffer));
    close(fd);

    if((len != 0) && (len != -1)) {
        memmove(&id, tempbuffer, 2);
        //id = id & 0xFFFF;
        if (id == chip_id){
            printf("ecnt_wl_e2p_chk: this is %x chip table\n", chip_id);
            return CHECK_EEPROM_OK;
        }
    }

    return CHECK_EEPROM_NOK;
}

#if defined(MT7915N) && !defined(MT7915D) && !defined(MT7916D)  //first interface is MT7915A
static int gen_2g_e2p_w_compile_option()
{
    generate_e2p_file_at_tmp(BAND_2G, 131072);
    return 0x7915;
}
#endif //MT7915N

#if defined(MT7915E) && !defined(MT7915D) && !defined(MT7916D) //second interface is MT7915A
static int gen_5g_e2p_w_compile_option()
{
    generate_e2p_file_at_tmp(BAND_5G, 131072);
    return 0x7915;
}
#endif //MT7915E

#if defined(MT7915D) || defined(MT7916D)   //MT7915D case
static int gen_2g_e2p_w_compile_option()
{
    return 0;
}
static int gen_5g_e2p_w_compile_option()
{
	if(!strncasecmp(chip_name,"MT7915D",strlen("MT7915D")))
	{
	    generate_e2p_file_at_tmp(BAND_5G, 4096);
	    return 0x7915;
	}
	if(!strncasecmp(chip_name,"MT7916D",strlen("MT7916D")))
	{
		generate_e2p_file_at_tmp(BAND_5G, 204*1024);
		return 0x7916;
	}
	return 0;
}
#endif

static int auto_chk_e2p_w_option()
{
    int ret_2g = 0, ret_5g = 0;
    int _2g_chip_id = 0, _5g_chip_id = 0;

    _2g_chip_id = gen_2g_e2p_w_compile_option(); //func is defined by compile option
    _5g_chip_id = gen_5g_e2p_w_compile_option(); //func is defined by compile option

    // check 2g
    if (check_e2p_file_at_tmp(BIN_FILE_TMP_2G, _2g_chip_id) == CHECK_EEPROM_OK){
        ret_2g = CHECK_2G_EEPROM_OK;
    }
    else {
        printf("ecnt_wl_e2p_chk: %s doesn't matched profile\n", BIN_FILE_TMP_2G);
        unlink(BIN_FILE_TMP_2G);
    }

    // check 5g
    if (check_e2p_file_at_tmp(BIN_FILE_TMP_5G, _5g_chip_id) == CHECK_EEPROM_OK){
        ret_5g = CHECK_5G_EEPROM_OK;
    }
    else {
        printf("ecnt_wl_e2p_chk: %s doesn't matched profile\n", BIN_FILE_TMP_5G);
        unlink(BIN_FILE_TMP_5G);
    }

    return (ret_2g + ret_5g);
}


int main(int argc, char ** argv)
{
    int opt = 0, band = BAND_5G, fd = -1, len = 0, size = 0;
    unsigned short id = 0;
    char *chip = NULL;
    char tempbuffer[16] = {0}, tmp[100] = {0};
    struct chipset_tbl *table = chipset_table_list;
    bool check_ok = false;

    while ((opt = getopt (argc, argv, "ahc:b:s:")) != -1)
    {
        switch (opt)
        {
            case 'c':
                chip = optarg;
                printf("---- chip=\"%s\"\n", chip);
            	memcpy(chip_name,chip,sizeof(chip_name));
              	break;
            case 'b':
                band = atoi(optarg);
                printf("---- band=%dG\n", band);
                break;
            case 's':
                size = atoi(optarg);
                printf("---- size=%dG\n", size);
                break;
			case 'h':
                usage();
                return CHECK_EEPROM_OK;
            case 'a':
                printf("ecnt_wl_e2p_chk: run auto check with compile option chip_name=%s \n",chip_name);
                return auto_chk_e2p_w_option();
            default:
                usage();
                return CHECK_EEPROM_NOK;
        }
    }

    if(chip == NULL || (band != BAND_2G && band != BAND_5G && band != BAND_6G) || (size == 0)){
        printf("ecnt_wl_e2p_chk: the input parameter is wrong. Abort checking\n");
        return CHECK_EEPROM_NOK;
    }

	generate_e2p_file_at_tmp(band, size);

	switch (band)
	{
		case BAND_2G:
			strncpy(tmp, BIN_FILE_TMP_2G, strlen(BIN_FILE_TMP_2G));
			tmp[strlen(BIN_FILE_TMP_2G)] = '\0';
			break;
		case BAND_5G:
			strncpy(tmp, BIN_FILE_TMP_5G, strlen(BIN_FILE_TMP_5G));
			tmp[strlen(BIN_FILE_TMP_5G)] = '\0';
			break;
		case BAND_6G:
			strncpy(tmp, BIN_FILE_TMP_6G, strlen(BIN_FILE_TMP_6G));
			tmp[strlen(BIN_FILE_TMP_6G)] = '\0';
			break;
		default:
			printf("%s(): oops. wrong val \n", __FUNCTION__);
			return CHECK_EEPROM_NOK;
	}

    fd = open(tmp, O_RDONLY);
    if (fd < 0){
        printf("ecnt_wl_e2p_chk: open %s fail !\n", tmp);
        return CHECK_EEPROM_NOK;
    }

    len = read(fd, tempbuffer, sizeof(tempbuffer));
    close(fd);

    if((len != 0) && (len != -1)) {
        memmove(&id, tempbuffer, 2);

        id = id & 0xFFFF;
        while(table && table->chip_name){
            if (id == table->num_in_hex){
                printf("ecnt_wl_e2p_chk: this is %x chip table\n", table->num_in_hex);
                check_ok = true;
                break;
            }
            table++;
        }
    }

    if(check_ok == false){
        printf("ecnt_wl_e2p_chk: %s is not matched\n", tmp);
        unlink(tmp);
        return CHECK_EEPROM_NOK;
    }


    /* not work since /etc is read-only
    memset(tmp, 0, sizeof(tmp));
    if(band == BAND_2G){
        snprintf(tmp, sizeof(tmp), "cat %s > %s", BIN_FILE_TMP_PATH, BIN_FILE_PATH_2G);
    }
    else{
        snprintf(tmp, sizeof(tmp), "cat %s > %s", BIN_FILE_TMP_PATH, BIN_FILE_PATH_5G);
    }
    system(tmp);*/


    return CHECK_EEPROM_OK;
}
