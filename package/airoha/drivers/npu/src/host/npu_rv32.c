
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/module.h>
#include <linux/mm.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/timer.h>
#include <linux/proc_fs.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#ifndef TCSUPPORT_CPU_ARMV8
#include <asm/addrspace.h>
#endif
#include <asm/tc3162/tc3162.h>
#include <linux/dma-mapping.h>
#include <linux/random.h>

#include <modules/npu/npu_test_common.h>
#ifdef TCSUPPORT_NPU_V2
extern void host_set_npu_core_on_off(int core, int on);
extern void set_npu_sram_power_savs(unsigned int reg, unsigned int val);
extern void set_npu_clk_power_save(unsigned int reg, unsigned int bit, unsigned int val, char en);
#endif

#ifdef TCSUPPORT_NPU_WIFI_OFFLOAD
#include "modules/npu/wifi_mail.h"
#include "npu_rv32.h"
extern unsigned char isNpuBand0OnCpu;
extern char npu_wifi_offload_get_force_to_cpu_flag(void);
extern char npu_stat;
//unsigned int dbgCount;
//unsigned char band_num = 4;
unsigned long int counter_base[BAND_NUM]={0};
#endif
//#define CORE_BEATS_MONITOR_SUPPORT /* the same define needs to be truned on in NPU's software/main/Makefile */

#define NPU_BIN_FILE    "/userfs/npu_rv32.bin"
#define NPU_DATA_FILE   "/userfs/npu_data.bin"


#define MAX_NPU_BIN_SIZE   (0x200000) //2M


#define NPU_DATA_UNC_BASE   (0xbe900000) //NPU 64K SRAM
#define MAX_NPU_DATA_SIZE   (0x10000) //64K


#define REG_CORE0_BOOT_BASE (0xbec08020)
#define REG_CORE_BOOT_CONFIG    (0xbec08004)
#define REG_CORE_BOOT_TRIGGER   (0xbec08000)

#define CR_NPU_MIB0     (0xbec0c140)
#define CR_NPU_MIB10    (0xbec0c168)

#ifdef CORE_BEATS_MONITOR_SUPPORT
#define BEAT_POLL_TIME	msecs_to_jiffies(1000) //1 sec
#define CR_PLIC_IE_T0_S0_31 (0xbec09098)
#define MAX_BEATS_STOP_CNT  (20)
unsigned int prev_beats_cnt[MAX_CORE_NUM];
unsigned int beats_stop_cnt[MAX_CORE_NUM];
unsigned int init_core_num = 0;
#endif

#define S_64K     (0x10000)

#define NPU_BIN_DRAM_SIZE   (S_64K)
#define NPU_DATA_64K_SRAM_START  (0xbe900000)

unsigned int g_test_start_addr[] = {0, NPU_DATA_64K_SRAM_START, 0};
unsigned int g_test_size[] = {NPU_BIN_DRAM_SIZE, S_64K, 0};

unsigned int g_pattern[] = {0xa5a5a5a5, 0x5a5a5a5a, 0xffffffff, 0x00000000};

unsigned int npu_init_code_phy_addr, npu_init_code_unc_addr;

#ifdef NPU_TEST_CODE
void npu_test_proc_create(void);
void npu_test_proc_remove(void);
#ifdef HOST_ADAPTER_TEST_CODE
void hostadapter_regiter_interrupt(void);
void hostadapter_free_interrupt(void);
#endif
#endif

struct timer_list coreHeartBeatPollTimer;

#if defined(TCSUPPORT_CPU_ARMV8)
extern int test_ram_b4_npu_load(void);
extern void copy_file_to_ram(struct file *srcf, int bufSize, int fileType);
extern void host_dump_npu_csr(int core);
#endif

int fileCopyToSram(char *filename, unsigned int base_addr, unsigned int max_size, int fileType){
	struct file	*srcf;
	char *src;
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,29)
	int orgfsuid, orgfsgid;
#endif
	mm_segment_t orgfs;
	unsigned char *buffer;
	int bufSize;
	struct kstat stat;

    #ifndef TCSUPPORT_CPU_ARMV8
	buffer = (unsigned char *)base_addr;
    #endif

   	src = filename;
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,29)
	orgfsuid = current->fsuid;
	orgfsgid = current->fsgid;
	current->fsuid=current->fsgid = 0;
#endif
	orgfs = get_fs();
	set_fs(KERNEL_DS);

    vfs_stat(filename, &stat);
    bufSize = stat.size;
    if (bufSize>max_size) {
        printk("Error: NPU binary:%s(0x%x) is larger than 0x%x\n", filename, bufSize, max_size);
        #if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,29)
		current->fsuid = orgfsuid;
		current->fsgid = orgfsgid;
        #endif
		return 1;        
    }
    
    #ifdef TCSUPPORT_CPU_ARMV8
    printk("copy NPU binary:%s", filename);
    #else
    printk("copy NPU binary:%s(%d Bytes) to 0x%x\n", filename, bufSize, base_addr);
    #endif

	if (src && *src)
	{
		srcf = filp_open(src, O_RDONLY, 0);
		if (IS_ERR(srcf))
		{
			printk("--> Error opening!!\n");
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,29)
			current->fsuid = orgfsuid;
			current->fsgid = orgfsgid;
#endif
			return 1;
		}
		else
		{
        #ifdef TCSUPPORT_CPU_ARMV8
            copy_file_to_ram(srcf, bufSize, fileType);
            
        #else
			memset(buffer, 0x00, bufSize);
			ecnt_kernel_fs_read(srcf, buffer, bufSize, &srcf->f_pos);
        #endif
			filp_close(srcf,NULL);
		}
	}

	set_fs(orgfs);
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,29)
	current->fsuid = orgfsuid;
	current->fsgid = orgfsgid;
#endif
	return 0;
}

#ifdef CORE_BEATS_MONITOR_SUPPORT
static void coreHeartBeat_poll_func(unsigned long data)
{
    int c;
    unsigned int cur_beats, val;

    for (c=0; c<init_core_num; c++) {
        //printk("CR_MIB%d: 0x%x\n", c, regRead32(CR_NPU_MIB0+(c<<2)));
        cur_beats = regRead32(CR_NPU_MIB0+(c<<2));
        
        if (cur_beats == prev_beats_cnt[c])
            beats_stop_cnt[c]++;
        else
            beats_stop_cnt[c]=0;
        
        if (beats_stop_cnt[c]==MAX_BEATS_STOP_CNT) {
            
            printk("Core%d has stopped beating for 20 consecutive secs! Reboot it\n", c);
            
            val = regRead32(REG_CORE_BOOT_CONFIG)&0xff; /* keep old enabled bits */
            val |= (0x0100<<c); /* add reboot bit for CoreX */
            regWrite32(REG_CORE_BOOT_CONFIG, val);
            if (c==0) /* prevent Core0 from re-init bss, PLIC, timer, ... */
                regWrite32(CR_NPU_MIB0, ALL_FF); 
            regWrite32(REG_CORE_BOOT_TRIGGER, 0x2); /* trigger reboot */
            
            /* in case that there is garbage on the bus for CoreX, reset the CoreX again. */
            mdelay(100);
            printk("Reboot Core%d again\n", c);
            regWrite32(REG_CORE_BOOT_CONFIG, val);
            if (c==0) 
                regWrite32(CR_NPU_MIB0, ALL_FF);
            regWrite32(REG_CORE_BOOT_TRIGGER, 0x2);
            
            beats_stop_cnt[c]=0;
            mdelay(100);
        }
        
        prev_beats_cnt[c] = cur_beats;
    }

    mod_timer(&coreHeartBeatPollTimer, jiffies + BEAT_POLL_TIME);
    return;
}

static int npu_beats_cnts_read_proc(char *page, char **start, off_t off,
	int count, int *eof, void *data)
{
	int len, i;

	len = 0;

	for(i = 0; i < init_core_num; i++) {
		len += sprintf(page + len, "prev_beats_cnt[%d]:0x%x, beats_stop_cnt[%d]:0x%x\n", 
                                    i, prev_beats_cnt[i], i, beats_stop_cnt[i]);
	}

	len -= off;
	*start = page + off;

	if (len > count)
		len = count;
	else
		*eof = 1;

	if (len < 0)
		len = 0;

	return len;
}
#endif

void boot_all_npu_cores(void)
{
    int i;
    unsigned int val;

    #ifdef TCSUPPORT_NPU_V2
    boot_npu_all_cores();
    #else
    for (i=0; i<MAX_CORE_NUM; i++) {
        #ifdef TCSUPPORT_CPU_ARMV8
        boot_npu_core(i);

        #else
        
        regWrite32(REG_CORE0_BOOT_BASE+(i<<2), npu_init_code_phy_addr);
        mdelay(1);
        
        if (i==0) { /* power on NPU and boot Core0 */
            regWrite32(REG_CORE_BOOT_CONFIG, 0x1);
            regWrite32(REG_CORE_BOOT_TRIGGER, 0x1);
        }
        else { /* reboot Core1~7 */
            val = regRead32(REG_CORE_BOOT_CONFIG)&0xff; /* keep old enabled bits */
            val |= (0x1<<i); /* add new enable bit for CoreX */
            val |= (0x0100<<i); /* add reboot bit for CoreX */
            regWrite32(REG_CORE_BOOT_CONFIG, val);
            regWrite32(REG_CORE_BOOT_TRIGGER, 0x2);
        }
        mdelay(100);
        #endif
        
        #ifdef CORE_BEATS_MONITOR_SUPPORT
        init_core_num++;
        beats_stop_cnt[i]=0;
        /* enable NPU timer0 interrupt for Core_i */
        val = regRead32((CR_PLIC_IE_T0_S0_31+(i<<4)));
        val |= (1<<18);
        regWrite32((CR_PLIC_IE_T0_S0_31+(i<<4)), val);
        #endif
    }
    #endif
    return;
}

void AN7552_NPU_PS(void)
{
  #ifdef TCSUPPORT_NPU_V2
	//NPU
	/*
		sys modifybit 1ec06004 0 0
		sys memwl 1ec06000 0
		sys memwl 1ec06000 1
		sys modifybit 1ec06004 1 0
		sys memwl 1ec06000 0
		sys memwl 1ec06000 1
	*/
		host_set_npu_core_on_off(0, 0); // core0 off
		host_set_npu_core_on_off(1, 0); // core1 off
	//NPU SRAM
	/*
		sys modifybit 1ec06008 0 1
		sys modifybit 1ec06008 1 1
		sys modifybit 1ec06008 2 1
		sys modifybit 1ec06008 3 1
		sys modifybit 1ec06008 8 1
	*/
		set_npu_sram_power_save(0x8, 0x10f);
	/*
		sys modifybit 1ec06018 0 1
		sys modifybit 1ec06018 8 1
		sys modifybit 1ec06018 16 1
		sys modifybit 1ec06018 24 1
		sys modifybit 1ec06018 1 1
		sys modifybit 1ec06018 9 1
		sys modifybit 1ec06018 17 1
		sys modifybit 1ec06018 25 1
	*/
		set_npu_sram_power_save(0x18, 0x3030303);
	//NPU Clock
	/*
		sys modifybit 1fa201e4 4 0
		sys modifybit 1fa201ec 20 0
		sys modifybit 1fa20204 12 0
		sys modifybit 1fa20204 13 0
		sys modifybit 1fa20204 14 0
		sys modifybit 1fa20204 15 0
	*/
		set_npu_clk_power_save(0x1e4, 4, 1, 0);
		set_npu_clk_power_save(0x1ec, 20, 1, 0);
		set_npu_clk_power_save(0x204, 12, 15, 0);
  #endif
	return;
	
	
}

#ifndef TCSUPPORT_CPU_ARMV8
int npuLoadRamTest(void)
{
    unsigned int startAddr, testWord, pattern;
    int l, j, i;
    unsigned int *start_addr_p;
    unsigned int *test_size_p;
    unsigned int tmpAddr;


    g_test_start_addr[0] = npu_init_code_unc_addr;
    start_addr_p = g_test_start_addr;
    test_size_p = g_test_size;

    for(l=0; start_addr_p[l]!=0; l++) {
        
        startAddr = start_addr_p[l];
        testWord = (test_size_p[l]>>2);

	    printk("%s startAddr:0x%x, testSize:0x%x\n", __func__, startAddr, testWord<<2);

    	for(j=0; g_pattern[j]!=0; j++) {
    	
    	    pattern = g_pattern[j];

            /* write patterns */

            for(i=0, tmpAddr=startAddr; i<testWord; i++) {
                regWrite32(tmpAddr, pattern);
                tmpAddr += 4;
            }

            /* read and compare patterns */

            for(i=0, tmpAddr=startAddr; i<testWord; i++) {
                
                if (regRead32(tmpAddr) != pattern) {
                    printk("[H]ERROR: regRead32(0x%x):0x%x != 0x%x (at j:%d,i:%d)\n", tmpAddr, regRead32(tmpAddr), pattern, j, i);
                    return -1;
                }
                tmpAddr += 4;
            }
        }
    }

    return 0;
}
#endif

static int npu_dump_csr_write_proc(struct file *file, const char *buffer,
					  unsigned long count, void *data)
{
	char valString[5];
        unsigned int core_id=0;
	unsigned int i;
    
         
	if (count > sizeof(valString) - 1)
		return -EINVAL;

	if (copy_from_user(valString, buffer, count))
		return -EFAULT;

	valString[count] = '\0';

	/* test case is 10-digit based*/
	sscanf(valString, "%d", &core_id);

	if (core_id > MAX_CORE_NUM) {
		printk("Invalid core id\n");
		return count;
	} else if (core_id == MAX_CORE_NUM) {
		printk("Dump CSRs of All Cores\n");
		for (i = 0; i < MAX_CORE_NUM; i++)
			host_dump_npu_csr(i);
	} else {
		printk("Dump CSR of Core<%d>\n", core_id);
		host_dump_npu_csr(core_id);
	}

	return count;
}

static void npu_dump_csr_proc_create(void) 
{
	struct proc_dir_entry *npu_proc=NULL;

	npu_proc = create_proc_entry("npu_dump_core_csr", 0, NULL);
	npu_proc->write_proc = npu_dump_csr_write_proc;

	return;
}

static void npu_dump_csr_proc_remove(void) 
{
	remove_proc_entry("npu_dump_core_csr", NULL);
	return;
}

#ifdef TCSUPPORT_NPU_WIFI_OFFLOAD
void npu_wifi_offload_get_dbg_counter_address(void)
{
    int isMailBoxSuccess = 0, i=0;
    //unsigned long int counter_base[band_num]={0};
    dbg_counter_t get_CounterBase;
    for(i = 0; i< (BAND_NUM-1); i++)
    {
	if((isNpuBand0OnCpu) && (i == 0))
		continue;
        isMailBoxSuccess = WIFI_MAIL_API_GET_WAIT_DBG_COUNTER(i, &get_CounterBase);
    
        if(isMailBoxSuccess != 1) 
        {
            printk("Error: NPU_WIFI_OFFLOAD Get %s counter base fail\n", (i)?"5G":(i == 0)?"2.4G":"common");
            return;
        }
        else
        {
            if(i == 2)
            {
                
                counter_base[i] = ioremap((phys_addr_t)get_CounterBase.errCount, (UCOUNTER_BOTTOM * sizeof(unsigned int)));
            }
            else
            {
                counter_base[i] = ioremap((phys_addr_t)get_CounterBase.errCount, (COUNTER_BOTTOM * sizeof(unsigned int)));
            }
        }
        printk("counter_base[%d]=%lx(%x) \n", i,counter_base[i],get_CounterBase.errCount);
    }
    return;
}
void print_npu_wifi_offload_dbg_counter(unsigned short int band)
{
    if(counter_base[2] == 0)
    {
        npu_wifi_offload_get_dbg_counter_address();
    }
    if((isNpuBand0OnCpu) && (band == 0))
	printk("Band0 is on CPU, NPU does not support debug counter for Band0\n");
    else
        show_wifi_offload_counter(band);
        
        
    return;
}

#ifdef TCSUPPORT_NPU_WIFI_TX

void print_npu_wifi_offload_tx_dbg_counter(unsigned short int band)
{
    if(counter_base[2] == 0)
    {
        npu_wifi_offload_get_dbg_counter_address();
    }
    show_wifi_offload_tx_counter(band);
        
        
    return;
}
static int npu_wifi_offload_tx_dbg_counter_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
    char buf[16];
    unsigned long int len = count;
    unsigned short int n;

    //printk();
    if(len >= sizeof(buf))
    {
        len = sizeof(buf) - 1;
    }
    
    if(copy_from_user(buf, buffer, len))
    {
        return -EFAULT;
    }
    buf[len] = "\n";

    n = simple_strtol(buf, NULL, 10);
    if((n==0) || (n==1))
    {
        printk("[%s tx debug counter]\n",(n==0)?"2.4G/5G":"6G");
        print_npu_wifi_offload_tx_dbg_counter(n);
    }
    else
        printk("plz echo 0(2.4G) or 1(5G).\n");
    
    return len;
}
#endif
static int npu_wifi_offload_dbg_counter_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
    char buf[16];
    unsigned long int len = count;
    unsigned short int n;

    //printk();
    if(len >= sizeof(buf))
    {
        len = sizeof(buf) - 1;
    }
    
    if(copy_from_user(buf, buffer, len))
    {
        return -EFAULT;
    }
    buf[len] = "\n";

    n = simple_strtol(buf, NULL, 10);
    if((n==0) || (n==1))
    {
        printk("[%s debug counter]\n",(n==0)?"2.4G/5G":"6G");
        print_npu_wifi_offload_dbg_counter(n);
    }
    else
        printk("plz echo 0(2.4G) or 1(5G).\n");
    
    return len;
}

/* This proc is used to be a cmd to set/clear ICV_err table
 * Originally ADD_DEL_STA mail box is used on 7915/7916 for setting
 *      BA entry because BA reorder is handled in NPU.
 * For Kite/Eagle, this mail box is used to set/clear ICV_err table
 */
static int npu_wifi_offload_add_del_sta_proc(struct file *file,
    const char *buffer, unsigned long count, void *data)
{
    char buf[32] = {0};
    unsigned long int len = count;
    unsigned short int n;
    unsigned int _action, _tid, _wcid;
    unsigned int _msg_payload;

    //printk();
    if(len >= sizeof(buf))
    {
        len = sizeof(buf) - 1;
    }
    
    if(copy_from_user(buf, buffer, len))
    {
        return -EFAULT;
    }
    buf[len] = "\0";

    //n = simple_strtol(buf, NULL, 10);
    //printk("buf:%s \n", buf);

    if(sscanf(buf, "%d %d %d", &_action, &_wcid, &_tid) != 3){
        printk("usage: echo [action] [wcid] [tid] > /proc/tc3162/npu_wifioffload_add_del_sta\n");
        return count;
    }

    if(_tid > 8 || _wcid > 2047) {
        printk("invalid input. tid:%d wcid:%d\n", _tid, _wcid);
        return count;
    }

    _msg_payload = (_wcid & 0x7ff) | ((_tid & 0xf) << 11) ; //bit[10:0] is wcid and bit[14:11] is tid

    //printk("send add_del_sta act:%d wcid:%d tid:%d msg:%d len:%d count:%d\n", _action,
    //    _wcid, _tid, _msg_payload, len, count);
    WIFI_MAIL_API_SET_WAIT_ADD_DEL_STA(_action, _msg_payload);

    return len;
}

static void npu_wifi_offload_dbg_proc_create(void)
{
    struct proc_dir_entry *npu_proc=NULL, *npu_get_proc = NULL, *npu_tx_proc = NULL;
    struct proc_dir_entry *npu_add_del_sta_proc = NULL;

    npu_proc = create_proc_entry("tc3162/npu_wifioffload_dbg_counter", 0, NULL);
    npu_proc->write_proc = npu_wifi_offload_dbg_counter_proc;
#ifdef TCSUPPORT_NPU_WIFI_TX
    npu_tx_proc = create_proc_entry("tc3162/npu_wifioffload_tx_dbg_counter", 0, NULL);
    npu_tx_proc->write_proc = npu_wifi_offload_tx_dbg_counter_proc;
#endif

    npu_add_del_sta_proc = create_proc_entry("tc3162/npu_wifioffload_add_del_sta", 0, NULL);
    npu_add_del_sta_proc->write_proc = npu_wifi_offload_add_del_sta_proc;

    return;
}
static void npu_wifi_offload_dbg_proc_remove(void)
{
    remove_proc_entry("tc3162/npu_wifiOffload_dbg_counter", NULL);
#ifdef TCSUPPORT_NPU_WIFI_TX
    remove_proc_entry("tc3162/npu_wifiOffload_tx_dbg_counter", NULL);
#endif
    remove_proc_entry("tc3162/npu_wifioffload_add_del_sta", NULL);
    return;
}
#endif

static int npuInit(void)
{
    int i;
    
    #ifdef CORE_BEATS_MONITOR_SUPPORT
    struct proc_dir_entry *beats_proc=NULL;
    #endif

    //printk("%s\n", __func__);

#ifndef TCSUPPORT_CPU_ARMV8    
    npu_init_code_phy_addr = regRead32(CR_NPU_MIB10);
    #ifdef NPU_TEST_CODE
    npu_init_code_phy_addr+=(8<<20);
    #endif
    npu_init_code_unc_addr = (npu_init_code_phy_addr | 0xa0000000);
    
    /* "host_dram : ORIGIN" in NPU's link script (flash.ld) needs to be adjusted
     * if it's not expexted. */
    if(npu_init_code_phy_addr!=0x1b600000) {
        printk("ERROR: npu_init_code_phy_addr:0x%x is not expected\n\t" 
               "-->please adjust [host_dram : ORIGIN] in NPU's link script (flash.ld) !\n", npu_init_code_phy_addr);
        return 0;
    }
#endif /*TCSUPPORT_CPU_ARMV8*/

    /* for memory check before loading binary */
    #ifdef TCSUPPORT_CPU_ARMV8
    if (test_ram_b4_npu_load()==-1) {
    #else
    if (npuLoadRamTest()==-1) {
    #endif
        printk("npuLoadRamTest failed!  Just return!\n");
        return 0;
    }

    /* copy npu binary from filesystem to HOST DRAM */
    #ifdef NPU_CODE_IN_SRAM
    printk("Copy NPU Code to NPU 384K SRAM\n");
    #endif
    if (fileCopyToSram(NPU_BIN_FILE, npu_init_code_unc_addr, MAX_NPU_BIN_SIZE, 0)) {
        printk("fileCopyToSram failed for NPU_BIN_FILE!  Just return!\n");
        return 0;
    }
    /* copy npu data from filesystem to NPU 64K_SRAM */
    if (fileCopyToSram(NPU_DATA_FILE, NPU_DATA_UNC_BASE, MAX_NPU_DATA_SIZE, 1)) {
        printk("fileCopyToSram failed for NPU_DATA_FILE!  Just return!\n");
        return 0;
    }
    

    /* pass npu_test_area_base and host_l2c_sram_size to NPU */
    set_npu_needed_info();

#if 0 // test code for NPU using L2 cache
    printk("%s()NPU use L2 cache\n", __func__);
    regWrite32(0x1ec0f200, 0x1);
#endif

    mdelay(100);

#if defined(TCSUPPORT_NPU_WIFI_OFFLOAD) || defined(TCSUPPORT_NPU_TUNNEL_OFFLOAD) || defined(NPU_TEST_CODE) || defined(NPU_BMGR_TEST)
    /* boot Core0~7 */
    boot_all_npu_cores();
#else
    if(isAN7552)
    {
        AN7552_NPU_PS();
	goto theEnd;
    }
#endif

    npu_dump_csr_proc_create();
    #ifdef NPU_TEST_CODE
    npu_test_proc_create();
#ifdef HOST_ADAPTER_TEST_CODE
	hostadapter_regiter_interrupt();
#endif
    #endif

    #ifdef CORE_BEATS_MONITOR_SUPPORT
    beats_proc = create_proc_entry("npu_beats_cnts", 0, NULL);
    beats_proc->read_proc = npu_beats_cnts_read_proc;
    
	init_timer(&coreHeartBeatPollTimer);
	coreHeartBeatPollTimer.expires = jiffies + BEAT_POLL_TIME;
	coreHeartBeatPollTimer.function = coreHeartBeat_poll_func;
	coreHeartBeatPollTimer.data = (unsigned long) NULL;
	add_timer(&coreHeartBeatPollTimer);
    #endif
#if defined(TCSUPPORT_CPU_ARMV8) && !defined(NPU_TEST_CODE)   
#ifdef TCSUPPORT_NPU_WIFI_OFFLOAD
	// NPU wifi offload reserve 16M block to save packet when boot, and get the address in here (after npu bring up)
	npu_wifi_offload_dbg_proc_create();
	unsigned int isMailBoxSuccess = 0;
	isMailBoxSuccess = WIFI_MAIL_API_SET_WAIT_NPU_BAND0_ONCPU(1, isNpuBand0OnCpu);
	if(isMailBoxSuccess != 1) printk("Error: NPU_WIFI_OFFLOAD Get NPU Band0 On CPU flag Fail!!!\n");
	//printk("npu_wifi_offload_get_pkt_buf_addr=%x\n", npu_wifi_offload_get_pkt_buf_addr());
	isMailBoxSuccess = WIFI_MAIL_API_SET_WAIT_PKT_BUF_ADDR(0,npu_wifi_offload_get_pkt_buf_addr());
	if(isMailBoxSuccess != 1) printk("Error: NPU_WIFI_OFFLOAD Get packet buf address Fail!!!\n");
	npu_stat = 1;

	/* when TDMA tx is used with DRAM, need these func
#ifdef TCSUPPORT_NPU_WIFI_TDMA_OFFLOAD
	printk("npu_wifi_offload_get_tdma_tx_pkt_buf_addr=%x\n", npu_wifi_offload_get_tdma_tx_pkt_buf_addr());
	isMailBoxSuccess = WIFI_MAIL_API_SET_WAIT_TDMA_TX_PKT_BUF_ADDR(0, npu_wifi_offload_get_tdma_tx_pkt_buf_addr());
	if(isMailBoxSuccess != 1) printk("Error: NPU_WIFI_OFFLOAD Get packet buf address Fail!!!\n");
#endif
	*/
#ifdef TCSUPPORT_NPU_WIFI_TX
	//printk("npu_wifi_offload_get_tx_pkt_buf_addr=%x\n", npu_wifi_offload_get_tx_pkt_buf_addr());
	isMailBoxSuccess = WIFI_MAIL_API_SET_WAIT_TX_PKT_BUF_ADDR(0, npu_wifi_offload_get_tx_pkt_buf_addr());
	if(isMailBoxSuccess != 1) printk("Error: NPU_WIFI_OFFLOAD Get packet buf address Fail!!!\n");
#endif
	//printk("npu_wifi_offload_get_force_to_cpu_flag=%d\n", npu_wifi_offload_get_force_to_cpu_flag());
	isMailBoxSuccess = WIFI_MAIL_API_SET_WAIT_IS_FORCE_TO_CPU(0, npu_wifi_offload_get_force_to_cpu_flag());
	if(isMailBoxSuccess != 1) printk("Error: NPU_WIFI_OFFLOAD Get isForceToCpu flag Fail!!!\n");
#endif
#endif

theEnd:
    return 0;
}

static void __exit npuExit(void)
{
    printk("%s\n", __func__);
#if defined(TCSUPPORT_CPU_ARMV8) && !defined(NPU_TEST_CODE)
#ifdef TCSUPPORT_NPU_WIFI_OFFLOAD
    npu_stat = 0;
    npu_wifi_offload_dbg_proc_remove();
#endif
#endif

    npu_dump_csr_proc_remove();
    #ifdef NPU_TEST_CODE
    npu_test_proc_remove();
#ifdef HOST_ADAPTER_TEST_CODE
    hostadapter_free_interrupt();
#endif
    #endif
    #ifdef CORE_BEATS_MONITOR_SUPPORT
    remove_proc_entry("npu_beats_cnts", NULL);
    del_timer_sync(&coreHeartBeatPollTimer);
    #endif
}

module_init(npuInit);
module_exit(npuExit);

MODULE_DESCRIPTION("EcoNet NPU Host Driver");
MODULE_LICENSE("Proprietary");

