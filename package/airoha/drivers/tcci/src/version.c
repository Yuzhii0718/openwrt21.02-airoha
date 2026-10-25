#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/version.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,4,0)
#include <linux/uaccess.h>
#endif

/* Version.C -- Version and Build Time */
#define MAJOR_VERSION  7
#define MINOR_VERSION  3
#define BUG_FIX        283
#define BUILD          1454

#define TCLINUX_STR "TCLinux"
#define FW_STR "Fw"

//#define RELEASE_PQA
#define RELEASE_VERIFICATION  //verification version

#ifdef RELEASE_VERIFICATION
#define VERIFICATION_VERSION		"v072"
#endif

#ifdef RELEASE_PQA
const char Prefix[]="<&^>*";    //formal release version
#else
#ifdef RELEASE_VERIFICATION
const char Prefix[]="<&@>$";    //verification version
#else
const char Prefix[]="<&@>#";    //test version
#endif                                            
#endif                                            

const char ModelDMTStr[]= "(F04.ZZ.D)3.3.2.5 ";

const char BuildTimeStr[]= "| 2024/09/30 ";

char VersionStr[64];
const char CustomerVerStr[]= "<#$>                                                                ";      
const char CustomerTimeStr[]= " ";
//const char VersionStr[] = "2.2.19(TE2.B2)3.0.11     ";

void InitVersionStr(void)
{
#ifdef RELEASE_VERIFICATION
	 /*sprintf( VersionStr, "%s %s %c%d.%d.%d.%d %s", TCLINUX_STR,FW_STR,Prefix[4],MAJOR_VERSION, MINOR_VERSION, BUG_FIX, BUILD, VERIFICATION_VERSION);*/
	 sprintf( VersionStr, "%s %s %d.%d.%d.%d %s", TCLINUX_STR,FW_STR,MAJOR_VERSION, MINOR_VERSION, BUG_FIX, BUILD, VERIFICATION_VERSION);
#else
	 /*sprintf( VersionStr, "%s %s %c%d.%d.%d.%d",TCLINUX_STR,FW_STR, Prefix[4],MAJOR_VERSION, MINOR_VERSION, BUG_FIX, BUILD);*/
	 sprintf( VersionStr, "%s %s %d.%d.%d.%d",TCLINUX_STR,FW_STR,MAJOR_VERSION, MINOR_VERSION, BUG_FIX, BUILD);
#endif
}

void printVersionStr(void)
{
	printk("%s\n",VersionStr);
}

#if !defined(TCSUPPORT_OPENWRT)
#define FWTCVER_CONF "/etc/fwTCver.conf"
void createFwConf(void)
{
	struct file* fd = NULL;
	mm_segment_t old_fs;
	char tmpVersionStr[80];

	memset(tmpVersionStr,0,80);
	sprintf(tmpVersionStr,"%s\n",VersionStr);

	fd = filp_open(FWTCVER_CONF,O_RDWR|O_CREAT,0644);

	if(IS_ERR(fd))
	{
		printk("ERROR!Create %s fail!\n",FWTCVER_CONF);
	}
	else
	{
		old_fs = get_fs();
		set_fs(KERNEL_DS);
		ecnt_kernel_fs_write(fd,tmpVersionStr,strlen(tmpVersionStr),&fd->f_pos);
		set_fs(old_fs);
		filp_close(fd,NULL);
		fd = NULL;
	}
}
#endif

