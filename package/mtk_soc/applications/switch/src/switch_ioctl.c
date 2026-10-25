/*
 * switch_ioctl.c: switch(ioctl) set API
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/if.h>

#include "switch_fun.h"
#include "switch_ioctl.h"

static int esw_fd;

int switch_ioctl_init(void)
{
	esw_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (esw_fd < 0) {
		perror("socket");
		return -EINVAL;
	}

	return 0;
}

void switch_ioctl_fini(void)
{
	close(esw_fd);
}

int reg_read_ioctl(unsigned int offset, unsigned int *value)
{
	struct ifreq ifr;
	struct esw_reg reg;

	strncpy(ifr.ifr_name, ETH_DEVNAME, 5);
	ifr.ifr_data = &reg;

	reg.off = offset + 0x1fb58000;

	if (-1 == ioctl(esw_fd, RAETH_MII_READ, &ifr)) {
		perror("ioctl");
		close(esw_fd);
		exit(0);
	}
	*value = reg.val;
	return 0;
}

int reg_read_tr(int offset, int *value)
{
	struct ifreq ifr;
	struct esw_reg reg;

	strncpy(ifr.ifr_name, ETH_DEVNAME, 5);
	ifr.ifr_data = &reg;

	reg.off = offset + 0x1fb58000;

	if (-1 == ioctl(esw_fd, RAETH_MII_READ, &ifr)) {
		perror("ioctl");
		close(esw_fd);
		exit(0);
	}
	*value = reg.val;
	return 0;
}

int reg_write_ioctl(unsigned int offset, unsigned int value)
{
	struct ifreq ifr;
	struct esw_reg reg;

	strncpy(ifr.ifr_name, ETH_DEVNAME, 5);
	ifr.ifr_data = &reg;

	reg.off = offset + 0x1fb58000;
	reg.val = value;

	if (-1 == ioctl(esw_fd, RAETH_MII_WRITE, &ifr)) {
		perror("ioctl");
		close(esw_fd);
		exit(0);
	}
	return 0;
}

int reg_write_tr(int offset, int value)
{
	struct ifreq ifr;
	struct esw_reg reg;

	strncpy(ifr.ifr_name, ETH_DEVNAME, 5);
	ifr.ifr_data = &reg;

	reg.off = offset + 0x1fb58000;
	reg.val = value;

	if (-1 == ioctl(esw_fd, RAETH_MII_WRITE, &ifr)) {
		perror("ioctl");
		close(esw_fd);
		exit(0);
	}
	return 0;
}

int phy_dump_ioctl(unsigned int phy_addr)
{
	struct ifreq ifr;
	struct esw_reg reg;

	reg.val = phy_addr;
	strncpy(ifr.ifr_name, ETH_DEVNAME, 5);
	ifr.ifr_data = &reg;
	if (-1 == ioctl(esw_fd, RAETH_ESW_PHY_DUMP, &ifr)) {
		perror("ioctl");
		close(esw_fd);
		exit(0);
	}
	return 0;
}

int mii_mgr_cl22_read_ioctl(unsigned int port_num, unsigned int reg, unsigned int *value)
{
	struct ifreq ifr;
	struct ra_mii_ioctl_data mii;

	strncpy(ifr.ifr_name, ETH_DEVNAME, 5);
	ifr.ifr_data = &mii;

	mii.phy_id = port_num + 8;
	mii.reg_num = reg;

	if (-1 == ioctl(esw_fd, RAETH_ESW_PHY_READ, &ifr)) {
		perror("ioctl");
		close(esw_fd);
		exit(0);
	}
	*value = mii.val_out;
	return 0;
}

int mii_mgr_cl22_write_ioctl(unsigned int port_num, unsigned int reg, unsigned int value)
{
	struct ifreq ifr;
	struct ra_mii_ioctl_data mii;

	strncpy(ifr.ifr_name, ETH_DEVNAME, 5);
	ifr.ifr_data = &mii;

	mii.phy_id = port_num + 8;
	mii.reg_num = reg;
	mii.val_in = value;

	if (-1 == ioctl(esw_fd, RAETH_ESW_PHY_WRITE, &ifr)) {
		perror("ioctl");
		close(esw_fd);
		exit(0);
	}

	return 0;
}

int mii_mgr_cl45_read_indirect(unsigned int port_num, unsigned int dev,
			       unsigned int reg, unsigned int *value)
{
	const unsigned int MMD_Control_register=0xD;  	// 0xd=13
    const unsigned int MMD_addr_data_register=0xE;	// 0xe=14	
  	const unsigned int page_reg=31;
	
	mii_mgr_cl22_write_ioctl(port_num, page_reg, 0x00); //switch to main page
	mii_mgr_cl22_write_ioctl(port_num, MMD_Control_register, (0<<14)+dev);
	mii_mgr_cl22_write_ioctl(port_num, MMD_addr_data_register, reg);
	mii_mgr_cl22_write_ioctl(port_num, MMD_Control_register, (1<<14)+dev);
	mii_mgr_cl22_read_ioctl(port_num, MMD_addr_data_register, value);
	//printf("* mii_mgr_cl45_read_indirect=>phyaddr=%d,  dev_addr=%d, data_addr=0x%04lX , value=0x%04lX\r\n", port_num, dev, reg, *value);
	
	return 0;
}

int mii_mgr_cl45_write_indirect(unsigned int port_num, unsigned int dev,
				unsigned int reg, unsigned int value)
{
	const unsigned int MMD_Control_register=0xD;
    const unsigned int MMD_addr_data_register=0xE;
  	const unsigned int page_reg=31;
	
	mii_mgr_cl22_write_ioctl(port_num, page_reg, 0x00); //switch to main page
	mii_mgr_cl22_write_ioctl(port_num, MMD_Control_register, (0<<14)+dev);
	mii_mgr_cl22_write_ioctl(port_num, MMD_addr_data_register, reg);
	mii_mgr_cl22_write_ioctl(port_num, MMD_Control_register, (1<<14)+dev);
	mii_mgr_cl22_write_ioctl(port_num, MMD_addr_data_register, value);
	//printf("* mii_mgr_cl45_write_indirect=> phyaddr=%d, dev_addr=%d, data_addr=0x%04lX , value=0x%04lX\r\n", port_num, dev, reg, value);
	
	return 0;
}

int mii_mgr_cl45_read(unsigned int port_num, unsigned int dev,
		      unsigned int reg, unsigned int *value)
{
	unsigned int reg_value;
	int loop_cnt;
	int op_busy;
	int ret = 0;

	loop_cnt = 0;

	reg_value = 0x80000000 | (port_num << 20) | (dev << 25) | reg;
	reg_write(0x701c, reg_value);
	while (1)
	{
		reg_read(0x701c, &reg_value);
		op_busy = reg_value & (1 << 31);
		if (!op_busy) {
			break;
		} else if (loop_cnt < 10) {
			loop_cnt++;
		} else {
			printf("MDIO cl45 set dev opeartion timeout\n");
			reg_value = 0;
			ret = -1; 
			goto out;
		}
	}

	reg_value = 0x800c0000 | (port_num << 20) | (dev << 25);
	reg_write(0x701c, reg_value);
	while (1)
	{
		reg_read(0x701c, &reg_value);
		op_busy = reg_value & (1 << 31);
		if (!op_busy) {
			reg_value = reg_value & 0xFFFF;
			break;
		} else if (loop_cnt < 10) {
			loop_cnt++;
		} else {
			printf("MDIO cl45 read reg opeartion timeout\n");
			reg_value = 0;
			ret = -1; 
			break;
		}
	}
out:
	//printf(" PHY Indirect Access Control(0x701c) register read value =0x%x  \n", reg_value);
	*value = reg_value;

	return ret;
}

int mii_mgr_cl45_write(unsigned int port_num, unsigned int dev,
		       unsigned int reg, unsigned int value)
{
	unsigned int reg_value;
	int loop_cnt;
	int op_busy;
	int ret = 0;

	loop_cnt = 0;

	reg_value = 0x80000000 | (port_num << 20) | (dev << 25) | reg;
	reg_write(0x701c, reg_value);
	while (1)
	{
		reg_read(0x701c, &reg_value);
		op_busy = reg_value & (1 << 31);
		if (!op_busy)
			break;
		else if (loop_cnt < 10)
			loop_cnt++;
		else {
			printf("MDIO cl45 set dev opeartion timeout\n");
			ret = -1; 
			goto out;
		}
	}

	reg_value = 0x80040000 | (port_num << 20) | (dev << 25) | value;
	reg_write(0x701c, reg_value);
	while (1)
	{
		reg_read(0x701c, &reg_value);
		op_busy = reg_value & (1 << 31);
		if (!op_busy)
			break;
		else if (loop_cnt < 10)
			loop_cnt++;
		else {
			printf("MDIO cl45 write reg opeartion timeout\n");
			ret = -1; 
			break;
		}
	}
out:
	//printf(" PHY Indirect Access Control(0x701c) register write value =0x%x  \n", reg_value);
	return ret;
}

int mii_mgr_cl45_read_ioctl(unsigned int port_num, unsigned int dev,
			    unsigned int reg, unsigned int *value)
{
	if (chip_name == 0x7531 || chip_name == 0x7988)
		return mii_mgr_cl45_read(port_num, dev, reg, value);
	else if (chip_name == 0x7530)
		return mii_mgr_cl45_read_indirect(port_num, dev, reg, value);
	else
		return -1;
}

int mii_mgr_cl45_write_ioctl(unsigned int port_num, unsigned int dev,
			     unsigned int reg, unsigned int value)
{
	if (chip_name == 0x7531 || chip_name == 0x7988)
		return mii_mgr_cl45_write(port_num, dev, reg, value);
	else if (chip_name == 0x7530)
		return mii_mgr_cl45_write_indirect(port_num, dev, reg, value);
	else
		return -1;
}

int dump_gphy(void)
{
	int cl22_reg[6] = {0x00, 0x01, 0x04, 0x05, 0x09, 0x0A};
	int cl45_start_reg = 0x9B;
	int cl45_end_reg = 0xA2;
	unsigned int value;
	int port_num = 5;
	int i, j, ret;

	int sk, method;
	struct ifreq ifr;
	struct ra_mii_ioctl_data mii;

	sk = socket(AF_INET, SOCK_DGRAM, 0);
	if (sk < 0) {
		printf("Open socket failed\n");
		return -1;
	}

	strncpy(ifr.ifr_name, ETH_DEVNAME, 5);
	ifr.ifr_data = &mii;
	/* dump CL45 reg first*/
	for (i = 0; i < port_num; i++) {
		printf("== Port %d ==\n", i);
		for (j = cl45_start_reg; j < (cl45_end_reg + 1); j++) {
			ret = mii_mgr_cl45_read_ioctl(i, 0x1E, j, &value);
			if (ret)
				continue;
			printf("dev1Eh_reg%xh = 0x%x\n", j, value);
		}
	}
	printf("== Global ==\n");
	for (i = 0; i < sizeof(cl22_reg) / sizeof(cl22_reg[0]); i++) {
		method = RAETH_MII_READ;
		mii.phy_id = 0;
		mii.reg_num = cl22_reg[i];
		ret = ioctl(sk, method, &ifr);
		printf("Reg%xh = 0x%x\n", cl22_reg[i], mii.val_out);
	}

	close(sk);

	return ret;
}
