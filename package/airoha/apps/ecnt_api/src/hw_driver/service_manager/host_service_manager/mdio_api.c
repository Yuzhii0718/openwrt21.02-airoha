/*
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 */


#include <linux/if_packet.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <time.h>
#include <linux/sockios.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <termios.h>

#include <stdint.h>
#include <sys/mman.h>
#include <termios.h>

#include <stdarg.h>


static int skfd = -1;
static struct ifreq ifr;
#define LAN_IF "eth0"

/* This data structure is used for all the MII ioctl's */
struct mii_data {
      unsigned short   phy_id;
      unsigned short  reg_num;
      unsigned short  val_in;
      unsigned short  val_out;
};

unsigned int client_mdio_addr = 0xffffffff;

int mdio_msg_send(unsigned int phy_addr, unsigned int addr, unsigned int data);

#define printf tcdbg_printf

void
tcdbg_printf(char *fmt,...){

	FILE *proc_file;
	char msg[256];
	va_list args;

	va_start(args, fmt);
	vsnprintf(msg, 256, fmt, args);	

    proc_file = fopen("/proc/tc3162/dbg_msg", "w");
	if (!proc_file) {
		printf("open /proc/tc3162/dbg_msg fail\n");
		va_end(args);
		return;
	}
	fprintf(proc_file, "%s", msg);
	fclose(proc_file);
	va_end(args);
}

int mdio_msg_init(unsigned int phy_addr)
{
	if (client_mdio_addr != phy_addr)
	{
		client_mdio_addr = phy_addr;
		if ((skfd = socket(AF_INET,SOCK_DGRAM,0))<0){
			perror("socket");
			return -1;
		}
		
		strncpy(ifr.ifr_name, LAN_IF, IFNAMSIZ);
		if (ioctl(skfd, SIOCGMIIPHY, &ifr) < 0) {
			if (errno != ENODEV)
				fprintf(stderr, "SIOCGMIIPHY on '%s' failed: %s\n",
				LAN_IF, strerror(errno));
			close(skfd);
			return -1;
		}
	}
	return 0;
}

int mdio_msg_rcv(unsigned int phy_addr, unsigned int addr, unsigned int* result)
{

	struct mii_data mii;

	/* enable MDIO output before read*/
	mdio_msg_send(phy_addr, 0x1fa20160, 0x0);

	ifr.ifr_data = (char *)(&mii);
	unsigned int r = 0;
	mii.phy_id = phy_addr;
	mii.val_in = (addr & 0xffff);
	mii.reg_num = 1;
	if (ioctl(skfd, SIOCSMIIREG, &ifr) < 0) {
		fprintf(stderr, "SIOCSMIIREG on %s failed: %s\n", ifr.ifr_name, strerror(errno));
		return (-1);
	}

	mii.val_in = ((addr >> 16)&0xffff);
	mii.reg_num = 2;
	if (ioctl(skfd, SIOCSMIIREG, &ifr) < 0) {
		fprintf(stderr, "SIOCSMIIREG on %s failed: %s\n", ifr.ifr_name, strerror(errno));
		return (-1);
	}
	
	mii.val_in = 0xf0;
	mii.reg_num = 0;
	if (ioctl(skfd, SIOCSMIIREG, &ifr) < 0) {
		fprintf(stderr, "SIOCSMIIREG on %s failed: %s\n", ifr.ifr_name, strerror(errno));
		return (-1);
	}


	mii.reg_num = 3;
	if (ioctl(skfd, SIOCGMIIREG, &ifr) < 0) {
	fprintf(stderr, "SIOCGMIIREG on %s failed: %s\n", ifr.ifr_name, strerror(errno));
	return (-1);
	}

	*result = mii.val_out;
	mii.reg_num = 4;
	if (ioctl(skfd, SIOCGMIIREG, &ifr) < 0) {
	fprintf(stderr, "SIOCGMIIREG on %s failed: %s\n", ifr.ifr_name, strerror(errno));
	return (-1);
	}
	r = mii.val_out;

	*result += (r<<16);

	/* disable MDIO output after read*/
	mdio_msg_send(phy_addr, 0x1fa20160, 0x800);

	return 0;

}

int mdio_msg_send(unsigned int phy_addr, unsigned int addr, unsigned int data)
{
	struct mii_data mii;

	ifr.ifr_data = (char *)(&mii);

	unsigned int r = 0;
	mii.phy_id = phy_addr;
	mii.val_in = (addr & 0xffff);
	mii.reg_num = 1;
	if (ioctl(skfd, SIOCSMIIREG, &ifr) < 0) {
		fprintf(stderr, "SIOCSMIIREG on %s failed: %s\n", ifr.ifr_name, strerror(errno));
		return (-1);
	}

	mii.val_in = ((addr >> 16)&0xffff);
	mii.reg_num = 2;
	if (ioctl(skfd, SIOCSMIIREG, &ifr) < 0) {
		fprintf(stderr, "SIOCSMIIREG on %s failed: %s\n", ifr.ifr_name, strerror(errno));
		return (-1);
	}
	
	mii.val_in = (data & 0xffff);
	mii.reg_num = 3;
	if (ioctl(skfd, SIOCSMIIREG, &ifr) < 0) {
		fprintf(stderr, "SIOCSMIIREG on %s failed: %s\n", ifr.ifr_name, strerror(errno));
		return (-1);
	}

	mii.val_in = ((data >> 16)&0xffff);
	mii.reg_num = 4;
	if (ioctl(skfd, SIOCSMIIREG, &ifr) < 0) {
	fprintf(stderr, "SIOCSMIIREG on %s failed: %s\n", ifr.ifr_name, strerror(errno));
	return (-1);
	}

	mii.val_in = 0x00ff;
	mii.reg_num = 0;
	if (ioctl(skfd, SIOCSMIIREG, &ifr) < 0) {
	fprintf(stderr, "SIOCSMIIREG on %s failed: %s\n", ifr.ifr_name, strerror(errno));
	return (-1);
	}
	return 0;
}

 


