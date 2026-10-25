#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include "../pt/lib_pt.h"
#include <signal.h>

#define FE_SRAM_BASE 0x1fa30000
#define STATUS_REG	0x1fb00f08
#define MSG_CHANNEL_TX	0x1fb00f00
#define MSG_CHANNEL_RX	0x1fb00f04
#define CRC_DISABLE		0x1fb00f0c
#define IMAGE_LEN 0x1fb00f10


#define MSG_READY 0x57414445
#define MSG_READY_INIC 0x5244494E
#define MSG_IMAGE_DONE 0x444f4E45
#define MSG_IMAGE_DONE_INIC 0x494E444E
#define MSG_SGMII2RGMII_MODE 0x53475247
#define MSG_CRC_FAIL 0x4352464c
#define MSG_CRC_SUCCESS 0x46574F4B
#define MSG_NO_CRC	0x4E4F4352
#define MSG_FINISH 0x66697368

#define BP1_TIMEOUT 10

/* ROM STATUS */

#define UART_INIT_DONE (0x1<<0)
#define CRC_CHECK_SUCCESS (0x1 <<1)
#define CRC_CHECK_FAIL	(0x1<<2)
#define MSG_HANDLER_DONE (0x1 <<3)
#define CPU_STANDBY	(0x1<<4)
#define NO_CRC_CHECK_BOOT	(0x1<<5)

/* HSM timer */
extern int hsm_timeout;

void EN7517_pll_setting(unsigned int phy_addr)
{
	unsigned int data = 0;
	int fd = -1;
	if (mdio_msg_init(phy_addr)!=0)
	{
		SISM_DBG_MSG(LOG_ERR, "create socket fail!");
		return;
	}
	SISM_DBG_MSG(LOG_INFO, "config EN7517 pll setting");
	mdio_msg_rcv(phy_addr, 0x1fb0009c, &data);
	if ((data & 0x1) == 0)
	{
		return;
	}
	else
	{
		mdio_msg_rcv(phy_addr, 0x1fb0005c, &data);
		if (data == 0x1)
		{
			mdio_msg_rcv(phy_addr, 0x1fa20174, &data);
			if(data & (1 << 16)) {
				/* FE PLL */
				mdio_msg_send(phy_addr, 0x1fa20190, 0x12);
				mdio_msg_send(phy_addr, 0x1fa201f8, 0x05001f08);
				mdio_msg_send(phy_addr, 0x1fa201f8, 0x05001f0a);
				mdio_msg_send(phy_addr, 0x1fa201f8, 0x05001f0e);
				mdio_msg_send(phy_addr, 0x1fa20208, 0x03000000);
				mdio_msg_send(phy_addr, 0x1fa20204, 0x402);

				/* CPU PLL */
				//mdio_msg_send(phy_addr, 0x1fa201dc, 0x04101f0a);
				//mdio_msg_send(phy_addr, 0x1fa201dc, 0x04101f0e);
				//mdio_msg_send(phy_addr, 0x1fa201ec, 0x03000000);
				//mdio_msg_send(phy_addr, 0x1fa201e8, 0x402);
			} else {
				/* FE PLL */
				mdio_msg_send(phy_addr, 0x1fa20190, 0x12);
				mdio_msg_send(phy_addr, 0x1fa2020c, 0x04000000);
				mdio_msg_send(phy_addr, 0x1fa20204, 0x402);
			}
		}

	}
}

void client_initialization(int gpio)
{
	char cmdbuf[50];

	/*Disable*/
	/* only allow INIC packet to sent through ae wan interface*/
	/*system("echo 1 > /proc/tc3162/ae_wan_inic_status");*/

	SISM_DBG_MSG(LOG_INFO, "reset Client with gpio %d", gpio);

	snprintf(cmdbuf, sizeof(cmdbuf), "echo %d > /sys/class/gpio/export", gpio); 
	system(cmdbuf);
	snprintf(cmdbuf, sizeof(cmdbuf), "echo out > /sys/class/gpio/gpio%d/direction", gpio); 
	system(cmdbuf);

	/* pull high/low to the reset gpio*/
	snprintf(cmdbuf, sizeof(cmdbuf), "echo 0 > /sys/class/gpio/gpio%d/value", gpio); 
	system(cmdbuf);
	sleep(1);
	snprintf(cmdbuf, sizeof(cmdbuf), "echo 1 > /sys/class/gpio/gpio%d/value", gpio); 
	system(cmdbuf);

	snprintf(cmdbuf, sizeof(cmdbuf), "echo %d > /sys/class/gpio/unexport", gpio); 
	system(cmdbuf);
}

int mdio_msg_handler_bp1 (unsigned int phy_addr)
{
	int i = 0;
	int fd = -1;
	unsigned int fe_addr = FE_SRAM_BASE;
	unsigned int data = 0;
	unsigned int byte = 0;
	unsigned int file_len = 0;
	unsigned int rcv_result = 0;
	
	int ret = UNHANDLE_FAIL;

	/* init bp1 timer*/
	hsm_timeout = 0;
	alarm(0);
	alarm(BP1_TIMEOUT);
	
	fd = open("/etc/iNIC/ddr_cal.bin",O_RDONLY);

	if (fd <0 )
	{
		SISM_DBG_MSG(LOG_ERR, "open ddr_cal.bin failed !");
		ret = SOCKET_FAIL;
		goto exit_handle;
	}
	
	while(1)
	{
		if (mdio_msg_rcv(phy_addr, MSG_CHANNEL_TX, &rcv_result)!= 0)
		{
			ret = SOCKET_FAIL;
			goto exit_handle;
		}
		if (rcv_result == MSG_READY)
			break;
		else if (hsm_timeout)
		{
			SISM_DBG_MSG(LOG_INFO, "host bp1 timeout !");
			ret = RECEIVE_TIMEOUT;
			goto exit_handle;
		}
		else
			continue;
	}
	SISM_DBG_MSG(LOG_INFO, "start writing ddr_cal.bin to client!");

	do
	{
		byte = read(fd, &data, 4);
		mdio_msg_send(phy_addr, fe_addr, data);
		fe_addr +=4;
		file_len+= byte;
	}while(byte == 4);

	mdio_msg_send(phy_addr,IMAGE_LEN,file_len);

	SISM_DBG_MSG(LOG_INFO, "write fw finished ! ");

	mdio_msg_send(phy_addr, MSG_CHANNEL_RX, MSG_IMAGE_DONE);
	while(1)
	{
		if (mdio_msg_rcv(phy_addr, MSG_CHANNEL_TX, &rcv_result)!= 0)
		{
			ret = SOCKET_FAIL;
			goto exit_handle;
		}
		if ((rcv_result == MSG_CRC_SUCCESS) ||(rcv_result == MSG_NO_CRC))
		{
			ret = PT_SUCCESS;
			break;
		}
		else if(rcv_result == MSG_CRC_FAIL)
		{
			ret = CRC_FAIL;
			SISM_DBG_MSG(LOG_ERR,"CRC Check failed !");
			break;
		}
		else if (hsm_timeout)
		{
			SISM_DBG_MSG(LOG_ERR,"host bp1 timeout !");
			ret = RECEIVE_TIMEOUT;
			goto exit_handle;
		}		
		else
			continue;
	}

	/* notify client that host has received result*/
	mdio_msg_send(phy_addr, MSG_CHANNEL_RX, MSG_FINISH);

	SISM_DBG_MSG(LOG_INFO, "client boot into ddr_cal!");

exit_handle:

	if (fd >= 0)
		close(fd);
	return ret;
}

int host_bp1(int gpio, unsigned int phy_addr)
{
	int ret = -1;
	client_initialization(gpio);
	EN7517_pll_setting(phy_addr);
	ret = mdio_msg_handler_bp1(phy_addr);
	return ret;
}

