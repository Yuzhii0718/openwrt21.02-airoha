#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/init.h>
#include <linux/timer.h>
#include <linux/delay.h>
#include <linux/proc_fs.h>
#include <asm/io.h>
#include <asm/tc3162/tc3162.h>
#include <asm/tc3162/ledcetrl.h>
#include <linux/mtd/rt_flash.h>
#include <ecnt_hook/ecnt_hook.h>
#include <ecnt_hook/ecnt_hook_spi_nand.h>
#include "spi/spi_nand_flash.h"
#if defined(TCSUPPORT_CPU_EN7581)
#include "../../../../modules/private/auto_bench/7581/autobench.h"
#elif defined(TCSUPPORT_CPU_EN7523)
#include "../../../../modules/private/auto_bench/7523/autobench.h"
#elif defined(TCSUPPORT_CPU_EN7528)
#include "../../../../modules/private/auto_bench/7528/autobench.h"
#elif defined(TCSUPPORT_CPU_EN7580)
#include "../../../../modules/private/auto_bench/7580/autobench.h"
#else
#include "../../../../modules/private/auto_bench/751627/autobench.h"
#endif

#define SCRIPT_CMD "/usr/script/autobench_flash.sh"
#define ERASE_CHECK_FILE "/tmp/flash_erase_check"
#define WRITE_CHECK_FILE "/tmp/flash_write_check"
#define WRITE_FILE "/userfs/bin/mtd"
#define ENV_PARAM1 "HOME=/"
#define ENV_PARAM2 "TERM=vt100"
#define ENV_PARAM3 "PATH=/sbin:/usr/sbin:/bin:/usr/bin:/userfs/bin"
#define DBG_EN 0
#define dbg_printf(fmt, val...) {if(DBG_EN){printk("[DBG(%d) %s:]",__LINE__,__func__);printk(fmt, ##val);printk("\n");}}
#define TEST_SIZE 2048

u8 buf[TEST_SIZE], buf2[TEST_SIZE];

void (*flash_callusermodehelper)(int*cmdSeq, int wait);
EXPORT_SYMBOL(flash_callusermodehelper);

#if defined(TCSUPPORT_PARALLEL_NAND) && !defined(TCSUPPORT_CPU_EN7581)
extern void parallel_nand_initial_hw(void);
extern void parallel_nand_chip_select(u8 chip);
extern SPI_NAND_FLASH_RTN_T parallel_nand_protocol_reset (void);
extern SPI_NAND_FLASH_RTN_T parallel_nand_protocol_read_id(struct SPI_NAND_FLASH_INFO_T *ptr_rtn_flash_id);
extern unsigned long spinand_lock(void);
extern void spinand_unlock(unsigned long spinand_spinlock_flags);
int parallel_nand_test(void)
{
	int								rtn_status = -1;
	u8								chip = 0, chip_num = 0;
	u32								reg1 = 0, reg2= 0;
	struct SPI_NAND_FLASH_INFO_T	flashdev_info[2];
	unsigned long					spinand_spinlock_flags;

	memset(flashdev_info, 0, (sizeof(struct SPI_NAND_FLASH_INFO_T) * 2));

	spinand_spinlock_flags = spinand_lock();
	reg1 = VPint(0xBFA20224);
	reg2 = VPint(0xBFA1155C);
	VPint(0xBFA1155C) = 0x2;
	VPint(0xBFA20224) = 0x200;
	parallel_nand_initial_hw();

	for (chip = 0; chip < 2; chip++)
	{
		parallel_nand_chip_select(chip);
		parallel_nand_protocol_reset();
		parallel_nand_protocol_read_id(&flashdev_info[chip]);
		if ((flashdev_info[chip].mfr_id == _SPI_NAND_MANUFACTURER_ID_WINBOND) &&
			(flashdev_info[chip].dev_id == 0xDC) &&
			(flashdev_info[chip].ext_id == 0x549590))
		{
			chip_num++;
		}
	}

	if ((chip_num == 2))
	{
		rtn_status = 0;
	}
	VPint(0xBFA20224) = reg1;
	VPint(0xBFA1155C) = reg2;
	spinand_unlock(spinand_spinlock_flags);

	return rtn_status;
}
#endif

int flash_test(void){
	int i;
	struct file *file = NULL;
	mm_segment_t old_fs;

	
	int cmdSeq[]={FLASH_TEST,CMD_NULL};
	if(flash_callusermodehelper != NULL) {
		flash_callusermodehelper(cmdSeq, UMH_WAIT_PROC);
	}

	old_fs = get_fs();
	set_fs(KERNEL_DS);

	/* Erase check */
	file = filp_open(ERASE_CHECK_FILE, O_RDONLY, 0);
	if(IS_ERR(file)) {
		printk("\r\nwhen open file %s error!",ERASE_CHECK_FILE);
		return -1;
	} else {
		memset(buf, 0x00, TEST_SIZE);
		ecnt_kernel_fs_read(file, buf, TEST_SIZE, &file->f_pos);
		filp_close(file,NULL);
	}
	for(i = 0; i < TEST_SIZE; i++) { /* Erase check fail */
		if(buf[i] != 0xFF) {
			return -1;
		}
	}

	/* Write check */
	file = filp_open(WRITE_FILE, O_RDONLY, 0);
	if(IS_ERR(file)) {
		printk("\r\nwhen open file %s error!",WRITE_FILE);
		return -1;
	} else {
		memset(buf, 0x00, TEST_SIZE);
		ecnt_kernel_fs_read(file, buf, TEST_SIZE, &file->f_pos);
		filp_close(file,NULL);
	}
	file = filp_open(WRITE_CHECK_FILE, O_RDONLY, 0);
	if(IS_ERR(file)) {
		printk("\r\nwhen open file %s error!",WRITE_CHECK_FILE);
		return -1;
	} else {
		memset(buf2, 0x00, TEST_SIZE);
		ecnt_kernel_fs_read(file, buf2, TEST_SIZE, &file->f_pos);
		filp_close(file,NULL);
	}
	if(memcmp(buf, buf2, TEST_SIZE) != 0) { /* Write check fail */
		return -1;
	}
	
	set_fs(old_fs);
	return 0;
}

spi_nand_op_t ecnt_spi_nand_operation[] =
{
	/* autobench */
	flash_test,
#if defined(TCSUPPORT_PARALLEL_NAND) && !defined(TCSUPPORT_CPU_EN7581)
	parallel_nand_test,
#endif
};

ecnt_ret_val ecnt_spi_nand_hook(struct ecnt_data *in_data)
{
	struct ECNT_SPI_NAND_DATA *spi_nand_data = (struct ECNT_SPI_NAND_DATA *)in_data ;
	
	if(spi_nand_data->function_id >= SPI_NAND_FUNCTION_MAX_NUM) {
		printk("spi_nand_data->function_id is %d, exceed max number: %d", spi_nand_data->function_id, SPI_NAND_FUNCTION_MAX_NUM);
		return ECNT_HOOK_ERROR;
	}

	spi_nand_data->retValue = ecnt_spi_nand_operation[spi_nand_data->function_id](spi_nand_data) ;
	
	return ECNT_CONTINUE;
}

