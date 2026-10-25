/*
 * usxgmii_eyescan.c: Simple program to dump rx eye diagram from usxgmii.
 *
 *  Copyright (C) 2022, Bo-Cun Chen (bc-bocun.chen@mediatek.com)
 *
 *  Based on the devmem2.c code
 *  Copyright (C) 2000, Jan-Derk Bakker (J.D.Bakker@its.tudelft.nl)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <ctype.h>
#include <termios.h>
#include <time.h>
#include <sys/types.h>
#include <sys/mman.h>

#define PRINT_ERROR \
	do { \
		fprintf(stderr, "Error at line %d, file %s (%d) [%s]\n", \
		__LINE__, __FILE__, errno, strerror(errno)); exit(1); \
	} while(0)

#define MAP_SIZE 4096UL
#define MAP_MASK (MAP_SIZE - 1)

#define readb(x)	(*((volatile uint8_t *)(x)))
#define readw(x)	(*((volatile uint16_t *)(x)))
#define readl(x)	(*((volatile uint32_t *)(x)))
#define writeb(x, y)	(*((volatile uint8_t *)(x)) = (y))
#define writew(x, y)	(*((volatile uint16_t *)(x)) = (y))
#define writel(x, y)	(*((volatile uint32_t *)(x)) = (y))

uint32_t debug = 0;
uint32_t errcount_delay_ms = 1;
uint32_t att, vga, ctle;
uint32_t tp1, tp2, tp3, tp4, tp5;

void usage(void)
{
	fprintf(stderr, "\nUsage:\tusxgmii_eyescan [Address] [Errcount delay(ms)]\n"
		"Example:\n"
		"\tUSXGMII_0: usxgmii_eyescan 0x11F20000 1\n"
		"\tUSXGMII_1: usxgmii_eyescan 0x11F30000 1\n");
}

void usxgmii_xfi_read_aeq(FILE *result_fd, void *virt_addr)
{
	printf("%s start...", __func__);

	//[PTA_INTERFACE = CVD]
	//RADIX DECIMAL
	//[PTA_VAR lane 0x0]
	//[PTA_VAR lctx_cm1 0xff]
	//[PTA_VAR lctx_c0 0xff]
	//[PTA_VAR lctx_cp1 0xff]
	//[PTA_VAR rmtx_cm1 0xff]
	//[PTA_VAR rmtx_c0 0xff]
	//[PTA_VAR rmtx_cp1 0xff]
	//[PTA_VAR att 0xff]
	//[PTA_VAR vga 0xff]
	//[PTA_VAR ctle 0xff]
	//[PTA_VAR tp1 0xff]
	//[PTA_VAR tp2 0xff]
	//[PTA_VAR tp3 0xff]
	//[PTA_VAR tp4 0xff]
	//[PTA_VAR tp5 0xff]
	//[PTA_VAR tp6 0xff]
	//[PTA_VAR tp7 0xff]
	//[PTA_VAR tp0_dt 0xff]
	//[PTA_VAR aeq_done 0xff]

	//L0
	//I2C  0x20  0xFC[31:24] 0x00  RW
	writel(virt_addr + 0x0000, readl(virt_addr + 0x0000) & ~(0x0f << 0) | (0x04 << 0));		//rg_xtp_prb_sel_l	,
	writel(virt_addr + 0x0000, readl(virt_addr + 0x0000) & ~(0x0f << 8) | (0x04 << 8));		//rg_xtp_prb_sel_h	,

	writel(virt_addr + 0x0010, readl(virt_addr + 0x0010) & ~(0x3ff << 0) | (0x02 << 0));		//rg_xtp_ln0_prb_sel_l	,
	writel(virt_addr + 0x0010, readl(virt_addr + 0x0010) & ~(0x3ff << 16) | (0x03 << 16));		//rg_xtp_ln0_prb_sel_h	,
	if (debug) {
		fprintf(result_fd, "da_xtp_ln_tx_lctxcm1-> %x\n", readl(virt_addr + 0x00D0) & 0x3F);		//rgs_xtp_probe_out	,READ;da_xtp_ln_tx_lctxcm1
		fprintf(result_fd, "da_xtp_ln_tx_lctxc0-> %x\n", readb(virt_addr + 0x00D1) & 0x3F);		//rgs_xtp_probe_out	,READ;da_xtp_ln_tx_lctxc0
	}
	//[PTA_READ lctx_cm1 0x11F200d0[05:00]]
	//[PTA_READ lctx_c0 0x11F200d1[05:00]]

	writel(virt_addr + 0x0010, readl(virt_addr + 0x0010) & ~(0x3ff << 0) | (0x04 << 0));		//rg_xtp_ln0_prb_sel_l	,
	if (debug)
		fprintf(result_fd, "da_xtp_ln_tx_lctxcp1-> %x\n", readl(virt_addr + 0x00D0) & 0x3F);		//rgs_xtp_probe_out	,READ;da_xtp_ln_tx_lctxcp1
	//[PTA_READ lctx_cp1 0x11F200d0[05:00]]

	writel(virt_addr + 0x0010, readl(virt_addr + 0x0010) & ~(0x3ff << 0) | (0xCA << 0));		//rg_xtp_ln0_prb_sel_l	,
	writel(virt_addr + 0x0010, readl(virt_addr + 0x0010) & ~(0x3ff << 16) | (0xCB << 16));		//rg_xtp_ln0_prb_sel_h	,
	if (debug) {
		fprintf(result_fd, "da_xtp_ln_rx_aeq_rmtxcm1-> %x\n", readl(virt_addr + 0x00D0) & 0x3F);	//rgs_xtp_probe_out	,READ;da_xtp_ln_rx_aeq_rmtxcm1
		fprintf(result_fd, "da_xtp_ln_rx_aeq_rmtxc0-> %x\n", readb(virt_addr + 0x00D1) & 0x3F);		//rgs_xtp_probe_out	,READ;da_xtp_ln_rx_aeq_rmtxc0
	}
	//[PTA_READ rmtx_cm1 0x11F200d0[05:00]]
	//[PTA_READ rmtx_c0 0x11F200d1[05:00]]

	writel(virt_addr + 0x0010, readl(virt_addr + 0x0010) & ~(0x3ff << 0) | (0xCC << 0));		//rg_xtp_ln0_prb_sel_l	,
	writel(virt_addr + 0x0010, readl(virt_addr + 0x0010) & ~(0x3ff << 16) | (0xD4 << 16));		//rg_xtp_ln0_prb_sel_h	,
	if (debug) {
		fprintf(result_fd, "da_xtp_ln_rx_aeq_rmtxcp1-> %x\n", readl(virt_addr + 0x00D0) & 0x3F);	//rgs_xtp_probe_out	,READ;da_xtp_ln_rx_aeq_rmtxcp1
		fprintf(result_fd, "ad_xtp_ln_rx_aeq_att-> %x\n", readb(virt_addr + 0x00D1) & 0x7);		//rgs_xtp_probe_out	,READ;ad_xtp_ln_rx_aeq_att
	}
	//[PTA_READ rmtx_cp1 0x11F200d0[05:00]]
	//[PTA_READ att 0x11F200d1[02:00]]
	att = readb(virt_addr + 0x00D1) & 0x7;

	writel(virt_addr + 0x0010, readl(virt_addr + 0x0010) & ~(0x3ff << 0) | (0xD5 << 0));		//rg_xtp_ln0_prb_sel_l	,
	writel(virt_addr + 0x0010, readl(virt_addr + 0x0010) & ~(0x3ff << 16) | (0xD6 << 16));		//rg_xtp_ln0_prb_sel_h	,
	if (debug) {
		fprintf(result_fd, "ad_xtp_ln_rx_aeq_vga-> %x\n", readl(virt_addr + 0x00D0) & 0x1F);		//rgs_xtp_probe_out	,READ;ad_xtp_ln_rx_aeq_vga
		fprintf(result_fd, "ad_xtp_ln_rx_aeq_ctle-> %x\n", readb(virt_addr + 0x00D1) & 0x1F);		//rgs_xtp_probe_out	,READ;ad_xtp_ln_rx_aeq_ctle
	}
	//[PTA_READ vga 0x11F200d0[04:00]]
	vga = readb(virt_addr + 0x00D0) & 0x1F;
	//[PTA_READ ctle 0x11F200d1[04:00]]
	ctle = readb(virt_addr + 0x00D1) & 0x1F;
	if (debug)
		printf("ctle = %d\n", ctle);

	writel(virt_addr + 0x0010, readl(virt_addr + 0x0010) & ~(0x3ff << 0) | (0xD7 << 0));		//rg_xtp_ln0_prb_sel_l	,
	writel(virt_addr + 0x0010, readl(virt_addr + 0x0010) & ~(0x3ff << 16) | (0xD8 << 16));		//rg_xtp_ln0_prb_sel_h	,
	if (debug) {
		fprintf(result_fd, "ad_xtp_ln_rx_aeq_dfetp1-> %x\n", readl(virt_addr + 0x00D0) & 0x7F);		//rgs_xtp_probe_out	,READ;ad_xtp_ln_rx_aeq_dfetp1
		fprintf(result_fd, "ad_xtp_ln_rx_aeq_dfetp2-> %x\n", readb(virt_addr + 0x00D1) & 0x3F);		//rgs_xtp_probe_out	,READ;ad_xtp_ln_rx_aeq_dfetp2
	}
	//[PTA_READ tp1 0x11F200d0[06:00]]
	tp1 = readb(virt_addr + 0x00D0) & 0x7F;
	//[PTA_READ tp2 0x11F200d1[05:00]]
	tp2 = readb(virt_addr + 0x00D1) & 0x7F;

	writel(virt_addr + 0x0010, readl(virt_addr + 0x0010) & ~(0x3ff << 0) | (0xD9 << 0));		//rg_xtp_ln0_prb_sel_l	,
	writel(virt_addr + 0x0010, readl(virt_addr + 0x0010) & ~(0x3ff << 16) | (0xDA << 16));		//rg_xtp_ln0_prb_sel_h	,
	if (debug) {
		fprintf(result_fd, "ad_xtp_ln_rx_aeq_dfetp3-> %x\n", readl(virt_addr + 0x00D0) & 0x3F);		//rgs_xtp_probe_out	,READ;ad_xtp_ln_rx_aeq_dfetp3
		fprintf(result_fd, "ad_xtp_ln_rx_aeq_dfetp4-> %x\n", readb(virt_addr + 0x00D1) & 0x1F);		//rgs_xtp_probe_out	,READ;ad_xtp_ln_rx_aeq_dfetp4
	}
	//[PTA_READ tp3 0x11F200d0[05:00]]
	tp3 = readb(virt_addr + 0x00D0) & 0x3F;
	//[PTA_READ tp4 0x11F200d1[04:00]]
	tp4 = readb(virt_addr + 0x00D1) & 0x1F;

	//MWriteS32 axi:0x11F20010 (MREAD("S32", axi:0x11F20010)&~(0x3ff<<00)|0xDB<<00)			;rg_xtp_ln0_prb_sel_l	,
	writel(virt_addr + 0x0010, readl(virt_addr + 0x0010) & ~(0x3ff << 0) | (0xDB << 0));		//rg_xtp_ln0_prb_sel_l	,
	writel(virt_addr + 0x0010, readl(virt_addr + 0x0010) & ~(0x3ff << 16) | (0xDC << 16));		//rg_xtp_ln0_prb_sel_h	,
	if (debug) {
		fprintf(result_fd, "ad_xtp_ln_rx_aeq_dfetp5-> %x\n", readl(virt_addr + 0x00D0) & 0x1F);		//rgs_xtp_probe_out	,READ;ad_xtp_ln_rx_aeq_dfetp5
		fprintf(result_fd, "ad_xtp_ln_rx_aeq_dfetp6-> %x\n", readb(virt_addr + 0x00D1) & 0x1F);		//rgs_xtp_probe_out	,READ;ad_xtp_ln_rx_aeq_dfetp6
	}
	//[PTA_READ tp5 0x11F200d0[04:00]]
	tp5 = readb(virt_addr + 0x00D0) & 0x1F;
	//[PTA_READ tp6 0x11F200d1[04:00]]

	writel(virt_addr + 0x0010, readl(virt_addr + 0x0010) & ~(0x3ff << 0) | (0xDD << 0));		//rg_xtp_ln0_prb_sel_l	,
	if (debug)
		fprintf(result_fd, "ad_xtp_ln_rx_aeq_dfetp7-> %x\n", readl(virt_addr + 0x00D0) & 0x1F);		//rgs_xtp_probe_out	,READ;ad_xtp_ln_rx_aeq_dfetp7
	//[PTA_READ tp7 0x11F200d0[04:00]]

	writel(virt_addr + 0x0010, readl(virt_addr + 0x0010) & ~(0x3ff << 0) | (0x138 << 0));		//rg_xtp_ln0_prb_sel_l	,
	if (debug)
		fprintf(result_fd, "da_xtp_ln_rx_aeq_dfetp0_dt-> %x\n", readl(virt_addr + 0x00D0) & 0x3F);	//rgs_xtp_probe_out	,READ;da_xtp_ln_rx_aeq_dfetp0_dt
	//[PTA_READ tp0_dt 0x11F200d0[05:00]]

	writel(virt_addr + 0x0010, readl(virt_addr + 0x0010) & ~(0x3ff << 16) | (0x69 << 16));		//rg_xtp_ln0_prb_sel_h	,
	if (debug)
		fprintf(result_fd, "aeq_done-> %x\n", readb(virt_addr + 0x00D1) >> 2 & 0x1);			//rgs_xtp_probe_out	,READ;aeq_done
	//[PTA_READ aeq_done 0x11F200d1[02:02]]

	printf("done\n");
}

void usxgmii_frc_AEQ(FILE *result_fd, void *virt_addr)
{
	printf("%s start...", __func__);

	//[PTA_INTERFACE = CVD]
	//RADIX DECIMAL
	//[PTA_VAR lane 0x0]
	//[PTA_VAR att 0xff]
	//[PTA_VAR ctle 0xff]
	//[PTA_VAR vga_test 0xff]
	//[PTA_VAR tp1 0xff]
	//[PTA_VAR tp2 0xff]
	//[PTA_VAR tp3 0xff]
	//[PTA_VAR tp4 0xff]
	//[PTA_VAR tp5 0xff]

	//LANE0
	//****************************************************
	// force ATT/VGA/CTLE
	//****************************************************
	//I2C  0x20  0xFC[31:24] 0xA0  RW
	writel(virt_addr + 0xA060, readl(virt_addr + 0xA060) & ~(0x07 << 16) | (0x07 << 16));		//AD_XTP_LNn_RX_AEQ_ATT	,AEQ force output code enable. [10:8] AD_XTP_LNn_RX_AEQ_ATT/CTLE/VGA

	//I2C  0x20  0xFC[31:24] 0x30  RW
	//[PTA_WRITE att 0x11F2306C[10:08]]
	//[PTA_SKIP START]
	writel(virt_addr + 0x306C, readl(virt_addr + 0x306C) & ~(0x07 << 8) | (att << 8));		//rg_xtp_ln_rx_aeq_att	,
	//[PTA_SKIP END]
	writel(virt_addr + 0x306C, readl(virt_addr + 0x306C) & ~(0x01 << 11) | (0x01 << 11));		//rg_xtp_ln_frc_rx_aeq_att	,

	//[PTA_WRITE vga_test 0x11F2306C[28:24]]
	//[PTA_SKIP START]
	writel(virt_addr + 0x306C, readl(virt_addr + 0x306C) & ~(0x1f << 24) | (vga << 24));		//rg_xtp_ln_rx_aeq_vga	,
	//[PTA_SKIP END]
	writel(virt_addr + 0x306C, readl(virt_addr + 0x306C) & ~(0x01 << 29) | (0x01 << 29));		//rg_xtp_ln_frc_rx_aeq_vga	,

	//[PTA_WRITE ctle 0x11F2306C[20:16]]
	//[PTA_SKIP START]
	writel(virt_addr + 0x306C, readl(virt_addr + 0x306C) & ~(0x1f << 16) | (ctle << 16));		//rg_xtp_ln_rx_aeq_ctle	,
	//[PTA_SKIP END]
	writel(virt_addr + 0x306C, readl(virt_addr + 0x306C) & ~(0x01 << 21) | (0x01 << 21));		//rg_xtp_ln_frc_rx_aeq_ctle	,

	//****************************************************
	// force DFETP1-5
	//****************************************************
	//I2C  0x20  0xFC[31:24] 0xA0  RW
	writel(virt_addr + 0xA060, readl(virt_addr + 0xA060) & ~(0x7f << 9) | (0x1F << 9));		//AD_XTP_LNn_RX_AEQ_DFETP7	,AEQ force output code enable. [7:1] AD_XTP_LNn_RX_AEQ_DFETP7~1

	//I2C  0x20  0xFC[31:24] 0x30  RW
	//[PTA_WRITE tp1 0x11F23074[22:16]]
	//[PTA_SKIP START]
	writel(virt_addr + 0x3074, readl(virt_addr + 0x3074) & ~(0x7f << 16) | (tp1 << 16));		//rg_xtp_ln_rx_aeq_dfetp1	,Default:5'b00100.
	//[PTA_SKIP END]
	writel(virt_addr + 0x3074, readl(virt_addr + 0x3074) & ~(0x01 << 23) | (0x01 << 23));		//rg_xtp_ln_frc_rx_aeq_dfetp1	,Default:1'd0.

	//[PTA_WRITE tp2 0x11F23074[13:08]]
	//[PTA_SKIP START]
	writel(virt_addr + 0x3074, readl(virt_addr + 0x3074) & ~(0x3f << 8) | (tp2 << 8));		//rg_xtp_ln_rx_aeq_dfetp2	,Default:5'b00100.
	//[PTA_SKIP END]
	writel(virt_addr + 0x3074, readl(virt_addr + 0x3074) & ~(0x01 << 14) | (0x01 << 14));		//rg_xtp_ln_frc_rx_aeq_dfetp2	,Default:1'd0.

	//[PTA_WRITE tp3 0x11F23074[05:00]]
	//[PTA_SKIP START]
	writel(virt_addr + 0x3074, readl(virt_addr + 0x3074) & ~(0x3f << 0) | (tp3 << 0));		//rg_xtp_ln_rx_aeq_dfetp3	,Default:5'b00100.
	//[PTA_SKIP END]
	writel(virt_addr + 0x3074, readl(virt_addr + 0x3074) & ~(0x01 << 6) | (0x01 << 6));		//rg_xtp_ln_frc_rx_aeq_dfetp3	,Default:1'd0.

	//[PTA_WRITE tp4 0x11F23070[28:24]]
	//[PTA_SKIP START]
	writel(virt_addr + 0x3070, readl(virt_addr + 0x3070) & ~(0x1f << 24) | (tp4 << 24));		//rg_xtp_ln_rx_aeq_dfetp4	,Default:5'b00100.
	//[PTA_SKIP END]
	writel(virt_addr + 0x3070, readl(virt_addr + 0x3070) & ~(0x01 << 29) | (0x01 << 29));		//rg_xtp_ln_frc_rx_aeq_dfetp4	,Default:1'd0.

	//[PTA_WRITE tp5 0x11F23070[20:16]]
	//[PTA_SKIP START]
	writel(virt_addr + 0x3070, readl(virt_addr + 0x3070) & ~(0x1f << 16) | (tp5 << 16));		//rg_xtp_ln_rx_aeq_dfetp5	,Default:5'b00100.
	//[PTA_SKIP END]
	writel(virt_addr + 0x3070, readl(virt_addr + 0x3070) & ~(0x01 << 21) | (0x01 << 21));		//rg_xtp_ln_frc_rx_aeq_dfetp5	,Default:1'd0.

	printf("done\n");
}

void usxgmii_read_rx_offset(FILE *result_fd, void *virt_addr)
{
	printf("%s start...", __func__);

	//[PTA_INTERFACE = CVD]
	//RADIX DECIMAL
	//[PTA_VAR lane 0x0]
	//[PTA_VAR compos 0xff]
	//[PTA_VAR lvshos 0xff]
	//[PTA_VAR ctle1ios 0xff]
	//[PTA_VAR ctle1vos 0xff]
	//[PTA_VAR ctle2ios 0xff]
	//[PTA_VAR vga1ios 0xff]
	//[PTA_VAR vga1vos 0xff]
	//[PTA_VAR vga2ios 0xff]
	//[PTA_VAR vga2vos 0xff]

	//[PTA_VAR SAOS_HLLH 0x0]
	//[PTA_VAR SAOS_LLLH 0x0]
	//[PTA_VAR SAOS_LHHL 0x0]
	//[PTA_VAR SAOS_HHHL 0x0]
	//[PTA_VAR SAOS_E0 0x0]
	//[PTA_VAR SAOS_E1 0x0]
	//[PTA_VAR SAOS_D0L 0x0]
	//[PTA_VAR SAOS_D0H 0x0]
	//[PTA_VAR SAOS_D1L 0x0]
	//[PTA_VAR SAOS_D1H 0x0]
	//[PTA_VAR SAOS_E0L 0x0]
	//[PTA_VAR SAOS_E0H 0x0]
	//[PTA_VAR SAOS_E1L 0x0]
	//[PTA_VAR SAOS_E1H 0x0]

	//SAOS 2's complement
	//TP1~7 2's complement
	//COMPOS~VGA2VOS sign-magnitude

	//**********************************************
	// RX Offset Calibration
	//**********************************************
	//I2C  0x20  0xFC[31:24] 0xa0  RW
	writel(virt_addr + 0xA04C, readl(virt_addr + 0xA04C) & ~(0x01 << 6) | (0x01 << 6));		//value	,enable monitor ctle2ios value

	//I2C  0x20  0xFC[31:24] 0x00  RW
	writel(virt_addr + 0x0000, readl(virt_addr + 0x0000) & ~(0x0f << 0) | (0x04 << 0));		//rg_xtp_prb_sel_l	,
	writel(virt_addr + 0x0000, readl(virt_addr + 0x0000) & ~(0x0f << 8) | (0x04 << 8));		//rg_xtp_prb_sel_h	,

	writel(virt_addr + 0x0010, readl(virt_addr + 0x0010) & ~(0x3ff << 0) | (0xBA << 0));		//rg_xtp_ln0_prb_sel_l	,
	writel(virt_addr + 0x0010, readl(virt_addr + 0x0010) & ~(0x3ff << 16) | (0xBB << 16));		//rg_xtp_ln0_prb_sel_h	,
	if (debug) {
		fprintf(result_fd, "ad_xtp_ln_rx_cal_compos-> %x\n", readl(virt_addr + 0x00D0) & 0x3F);		//rgs_xtp_probe_out	,READ,ad_xtp_ln_rx_cal_compos
		fprintf(result_fd, "ad_xtp_ln_rx_cal_lvshos-> %x\n", readb(virt_addr + 0x00D1) & 0x3F);		//rgs_xtp_probe_out	,READ,ad_xtp_ln_rx_cal_lvshos
	}
	//[PTA_READ compos 0x11F200d0[05:00]]
	//[PTA_READ lvshos 0x11F200d1[05:00]]

	writel(virt_addr + 0x0010, readl(virt_addr + 0x0010) & ~(0x3ff << 0) | (0xBC << 0));		//rg_xtp_ln0_prb_sel_l	,
	writel(virt_addr + 0x0010, readl(virt_addr + 0x0010) & ~(0x3ff << 16) | (0xBD << 16));		//rg_xtp_ln0_prb_sel_h	,
	if (debug) {
		fprintf(result_fd, "ad_xtp_ln_rx_cal_ctle1ios-> %x\n", readl(virt_addr + 0x00D0) & 0x3F);	//rgs_xtp_probe_out	,READ,ad_xtp_ln_rx_cal_ctle1ios
		fprintf(result_fd, "ad_xtp_ln_rx_cal_ctle1vos-> %x\n", readb(virt_addr + 0x00D1) & 0x3F);	//rgs_xtp_probe_out	,READ,ad_xtp_ln_rx_cal_ctle1vos
	}
	//[PTA_READ ctle1ios 0x11F200d0[05:00]]
	//[PTA_READ ctle1vos 0x11F200d1[05:00]]

	writel(virt_addr + 0x0010, readl(virt_addr + 0x0010) & ~(0x3ff << 0) | (0xBE << 0));		//rg_xtp_ln0_prb_sel_l	,
	if (debug)
		fprintf(result_fd, "ad_xtp_ln_rx_cal_ctle2ios-> %x\n", readl(virt_addr + 0x00D0) & 0x3F);	//rgs_xtp_probe_out	,READ,ad_xtp_ln_rx_cal_ctle2ios
	//[PTA_READ ctle2ios 0x11F200d0[05:00]]

	writel(virt_addr + 0x0010, readl(virt_addr + 0x0010) & ~(0x3ff << 0) | (0xC0 << 0));		//rg_xtp_ln0_prb_sel_l	,
	writel(virt_addr + 0x0010, readl(virt_addr + 0x0010) & ~(0x3ff << 16) | (0xC1 << 16));		//rg_xtp_ln0_prb_sel_h	,
	if (debug) {
		fprintf(result_fd, "ad_xtp_ln_rx_cal_vga1ios-> %x\n", readl(virt_addr + 0x00D0) & 0x3F);	//rgs_xtp_probe_out	,READ,ad_xtp_ln_rx_cal_vga1ios
		fprintf(result_fd, "ad_xtp_ln_rx_cal_vga1vos-> %x\n", readb(virt_addr + 0x00D1) & 0x3F);	//rgs_xtp_probe_out	,READ,ad_xtp_ln_rx_cal_vga1vos
	}
	//[PTA_READ vga1ios 0x11F200d0[05:00]]
	//[PTA_READ vga1vos 0x11F200d1[05:00]]

	writel(virt_addr + 0x0010, readl(virt_addr + 0x0010) & ~(0x3ff << 0) | (0xC2 << 0));		//rg_xtp_ln0_prb_sel_l	,
	writel(virt_addr + 0x0010, readl(virt_addr + 0x0010) & ~(0x3ff << 16) | (0xC3 << 16));		//rg_xtp_ln0_prb_sel_h	,
	if (debug) {
		fprintf(result_fd, "ad_xtp_ln_rx_cal_vga2ios-> %x\n", readl(virt_addr + 0x00D0) & 0x3F);	//rgs_xtp_probe_out	,READ,ad_xtp_ln_rx_cal_vga2ios
		fprintf(result_fd, "ad_xtp_ln_rx_cal_vga2vos-> %x\n", readb(virt_addr + 0x00D1) & 0x3F);	//rgs_xtp_probe_out	,READ,ad_xtp_ln_rx_cal_vga2vos
	}
	//[PTA_READ vga2ios 0x11F200d0[05:00]]
	//[PTA_READ vga2vos 0x11F200d1[05:00]]

	//SAOSMUX
	writel(virt_addr + 0x0010, readl(virt_addr + 0x0010) & ~(0x3ff << 0) | (0xD3 << 0));		//rg_xtp_ln0_prb_sel_l	,READ,ad_xtp_ln_rx_aeq_saos

	//I2C  0x20  0xFC[31:24] 0xA0  RW
	writel(virt_addr + 0xA05C, readl(virt_addr + 0xA05C) & ~(0x0f << 28) | (0x00 << 28));		//RG_XTP_LN_RX_AEQ_SAOSMUX	,
	//I2C  0x20  0xFC[31:24] 0x00  RW
	if (debug)
		fprintf(result_fd, "L0_SAOS_HLLH-> %x\n", readl(virt_addr + 0x00D0) & 0x7F);			//rgs_xtp_probe_out	,READ,L0_SAOS_HLLH
	//[PTA_READ SAOS_HLLH 0x11F200d0[06:00]]

	//I2C  0x20  0xFC[31:24] 0xA0  RW
	writel(virt_addr + 0xA05C, readl(virt_addr + 0xA05C) & ~(0x0f << 28) | (0x01 << 28));		//RG_XTP_LN_RX_AEQ_SAOSMUX	,
	//I2C  0x20  0xFC[31:24] 0x00  RW
	if (debug)
		fprintf(result_fd, "L0_SAOS_LLLH-> %x\n", readl(virt_addr + 0x00D0) & 0x7F);			//rgs_xtp_probe_out	,READ,L0_SAOS_LLLH
	//[PTA_READ SAOS_LLLH 0x11F200d0[06:00]]

	//I2C  0x20  0xFC[31:24] 0xA0  RW
	writel(virt_addr + 0xA05C, readl(virt_addr + 0xA05C) & ~(0x0f << 28) | (0x02 << 28));		//RG_XTP_LN_RX_AEQ_SAOSMUX	,
	//I2C  0x20  0xFC[31:24] 0x00  RW
	if (debug)
		fprintf(result_fd, "L0_SAOS_LHHL-> %x\n", readl(virt_addr + 0x00D0) & 0x7F);			//rgs_xtp_probe_out	,READ,L0_SAOS_LHHL
	//[PTA_READ SAOS_LHHL 0x11F200d0[06:00]]

	//I2C  0x20  0xFC[31:24] 0xA0  RW
	writel(virt_addr + 0xA05C, readl(virt_addr + 0xA05C) & ~(0x0f << 28) | (0x03 << 28));		//RG_XTP_LN_RX_AEQ_SAOSMUX	,
	//I2C  0x20  0xFC[31:24] 0x00  RW
	if (debug)
		fprintf(result_fd, "L0_SAOS_LHHL-> %x\n", readl(virt_addr + 0x00D0) & 0x7F);			//rgs_xtp_probe_out	,READ,L0_SAOS_HHHL
	//[PTA_READ SAOS_HHHL 0x11F200d0[06:00]]

	//I2C  0x20  0xFC[31:24] 0xA0  RW
	writel(virt_addr + 0xA05C, readl(virt_addr + 0xA05C) & ~(0x0f << 28) | (0x04 << 28));		//RG_XTP_LN_RX_AEQ_SAOSMUX	,
	//I2C  0x20  0xFC[31:24] 0x00  RW
	//PRINT "0x11F200D0[06:00]-> " %HEX GetBit(MREAD("S32", asd:0x11F200D0),0x0,0x6)		;rgs_xtp_probe_out	,READ,L0_SAOS_E0
	if (debug)
		fprintf(result_fd, "L0_SAOS_E0-> %x\n", readl(virt_addr + 0x00D0) & 0x7F);			//rgs_xtp_probe_out	,READ,L0_SAOS_E0
	//[PTA_READ SAOS_E0 0x11F200d0[06:00]]

	//I2C  0x20  0xFC[31:24] 0xA0  RW
	writel(virt_addr + 0xA05C, readl(virt_addr + 0xA05C) & ~(0x0f << 28) | (0x05 << 28));		//RG_XTP_LN_RX_AEQ_SAOSMUX	,
	//I2C  0x20  0xFC[31:24] 0x00  RW
	//PRINT "0x11F200D0[06:00]-> " %HEX GetBit(MREAD("S32", asd:0x11F200D0),0x0,0x6)		;rgs_xtp_probe_out	,READ,L0_SAOS_E1
	if (debug)
		fprintf(result_fd, "L0_SAOS_E1-> %x\n", readl(virt_addr + 0x00D0) & 0x7F);			//rgs_xtp_probe_out	,READ,L0_SAOS_E1
	//[PTA_READ SAOS_E1 0x11F200d0[06:00]]

	//I2C  0x20  0xFC[31:24] 0xA0  RW
	writel(virt_addr + 0xA05C, readl(virt_addr + 0xA05C) & ~(0x0f << 28) | (0x06 << 28));		//RG_XTP_LN_RX_AEQ_SAOSMUX	,
	//I2C  0x20  0xFC[31:24] 0x00  RW
	if (debug)
		fprintf(result_fd, "L0_SAOS_D0L-> %x\n", readl(virt_addr + 0x00D0) & 0x7F);			//rgs_xtp_probe_out	,READ,L0_SAOS_D0L
	//[PTA_READ SAOS_D0L 0x11F200d0[06:00]]

	//I2C  0x20  0xFC[31:24] 0xA0  RW
	writel(virt_addr + 0xA05C, readl(virt_addr + 0xA05C) & ~(0x0f << 28) | (0x07 << 28));		//RG_XTP_LN_RX_AEQ_SAOSMUX	,
	//I2C  0x20  0xFC[31:24] 0x00  RW
	if (debug)
		fprintf(result_fd, "L0_SAOS_D0H-> %x\n", readl(virt_addr + 0x00D0) & 0x7F);			//rgs_xtp_probe_out	,READ,L0_SAOS_D0H
	//[PTA_READ SAOS_D0H 0x11F200d0[06:00]]

	//I2C  0x20  0xFC[31:24] 0xA0  RW
	writel(virt_addr + 0xA05C, readl(virt_addr + 0xA05C) & ~(0x0f << 28) | (0x08 << 28));		//RG_XTP_LN_RX_AEQ_SAOSMUX	,
	//I2C  0x20  0xFC[31:24] 0x00  RW
	if (debug)
		fprintf(result_fd, "L0_SAOS_D1L-> %x\n", readl(virt_addr + 0x00D0) & 0x7F);			//rgs_xtp_probe_out	,READ,L0_SAOS_D1L
	//[PTA_READ SAOS_D1L 0x11F200d0[06:00]]

	//I2C  0x20  0xFC[31:24] 0xA0  RW
	writel(virt_addr + 0xA05C, readl(virt_addr + 0xA05C) & ~(0x0f << 28) | (0x09 << 28));		//RG_XTP_LN_RX_AEQ_SAOSMUX	,
	//I2C  0x20  0xFC[31:24] 0x00  RW
	if (debug)
		fprintf(result_fd, "L0_SAOS_D1H-> %x\n", readl(virt_addr + 0x00D0) & 0x7F);			//rgs_xtp_probe_out	,READ,L0_SAOS_D1H
	//[PTA_READ SAOS_D1H 0x11F200d0[06:00]]

	//I2C  0x20  0xFC[31:24] 0xA0  RW
	writel(virt_addr + 0xA05C, readl(virt_addr + 0xA05C) & ~(0x0f << 28) | (0x0A << 28));		//RG_XTP_LN_RX_AEQ_SAOSMUX	,
	//I2C  0x20  0xFC[31:24] 0x00  RW
	if (debug)
		fprintf(result_fd, "L0_SAOS_E0L-> %x\n", readl(virt_addr + 0x00D0) & 0x7F);			//rgs_xtp_probe_out	,READ,L0_SAOS_E0L
	//[PTA_READ SAOS_E0L 0x11F200d0[06:00]]

	//I2C  0x20  0xFC[31:24] 0xA0  RW
	writel(virt_addr + 0xA05C, readl(virt_addr + 0xA05C) & ~(0x0f << 28) | (0x0B << 28));		//RG_XTP_LN_RX_AEQ_SAOSMUX	,
	//I2C  0x20  0xFC[31:24] 0x00  RW
	if (debug)
		fprintf(result_fd, "L0_SAOS_E0H-> %x\n", readl(virt_addr + 0x00D0) & 0x7F);			//rgs_xtp_probe_out	,READ,L0_SAOS_E0H
	//[PTA_READ SAOS_E0H 0x11F200d0[06:00]]

	//I2C  0x20  0xFC[31:24] 0xA0  RW
	writel(virt_addr + 0xA05C, readl(virt_addr + 0xA05C) & ~(0x0f << 28) | (0x0C << 28));		//RG_XTP_LN_RX_AEQ_SAOSMUX	,
	//I2C  0x20  0xFC[31:24] 0x00  RW
	if (debug)
		fprintf(result_fd, "L0_SAOS_E1L-> %x\n", readl(virt_addr + 0x00D0) & 0x7F);			//rgs_xtp_probe_out	,READ,L0_SAOS_E1L
	//[PTA_READ SAOS_E1L 0x11F200d0[06:00]]

	//I2C  0x20  0xFC[31:24] 0xA0  RW
	writel(virt_addr + 0xA05C, readl(virt_addr + 0xA05C) & ~(0x0f << 28) | (0x0D << 28));		//RG_XTP_LN_RX_AEQ_SAOSMUX	,
	//I2C  0x20  0xFC[31:24] 0x00  RW
	if (debug)
		fprintf(result_fd, "L0_SAOS_E1H-> %x\n", readl(virt_addr + 0x00D0) & 0x7F);			//rgs_xtp_probe_out	,READ,L0_SAOS_E1H
	//[PTA_READ SAOS_E1H 0x11F200d0[06:00]]

	//RG_XTP_LN_RX_AEQ_SAOSMUX[3:0]=4'b0000 => SAOS[6:0]=HLLH
	//RG_XTP_LN_RX_AEQ_SAOSMUX[3:0]=4'b0001 => SAOS[6:0]=LLLH
	//RG_XTP_LN_RX_AEQ_SAOSMUX[3:0]=4'b0010 => SAOS[6:0]=LHHL
	//RG_XTP_LN_RX_AEQ_SAOSMUX[3:0]=4'b0011 => SAOS[6:0]=HHHL
	//RG_XTP_LN_RX_AEQ_SAOSMUX[3:0]=4'b0100 => SAOS[6:0]=E0
	//RG_XTP_LN_RX_AEQ_SAOSMUX[3:0]=4'b0101 => SAOS[6:0]=E1
	//RG_XTP_LN_RX_AEQ_SAOSMUX[3:0]=4'b0110 => SAOS[6:0]=D0L
	//RG_XTP_LN_RX_AEQ_SAOSMUX[3:0]=4'b0111 => SAOS[6:0]=D0H
	//RG_XTP_LN_RX_AEQ_SAOSMUX[3:0]=4'b1000 => SAOS[6:0]=D1L
	//RG_XTP_LN_RX_AEQ_SAOSMUX[3:0]=4'b1001 => SAOS[6:0]=D1H
	//RG_XTP_LN_RX_AEQ_SAOSMUX[3:0]=4'b1010 => SAOS[6:0]=E0L
	//RG_XTP_LN_RX_AEQ_SAOSMUX[3:0]=4'b1011 => SAOS[6:0]=E0H
	//RG_XTP_LN_RX_AEQ_SAOSMUX[3:0]=4'b1100 => SAOS[6:0]=E1L
	//RG_XTP_LN_RX_AEQ_SAOSMUX[3:0]=4'b1101 => SAOS[6:0]=E1H

	printf("done\n");
}

void usxgmii_eyescan_en(FILE *result_fd, void *virt_addr)
{
	printf("%s start...", __func__);

	/***********************************************
	 *Eye scan init
	 ***********************************************/

	//L0
	//I2C  0x20  0xFC[31:24] 0x30  RW
	writel(virt_addr + 0x3080, readl(virt_addr + 0x3080) & ~(0x01 << 0) | (0x01 << 0));		//rg_xtp_ln_rx_eyes_en	,
	writel(virt_addr + 0x3080, readl(virt_addr + 0x3080) & ~(0x01 << 1) | (0x01 << 1));		//rg_xtp_ln_frc_rx_eyes_en	,
	//I2C  0x20  0xFC[31:24] 0x30  RW
	writel(virt_addr + 0x3080, readl(virt_addr + 0x3080) & ~(0x01 << 15) | (0x01 << 15));		//rg_xtp_ln_frc_rx_eyes_xoffset	,
	writel(virt_addr + 0x3080, readl(virt_addr + 0x3080) & ~(0x7f << 8) | (0x00 << 8));		//rg_xtp_ln_rx_eyes_xoffset	,
	writel(virt_addr + 0x3080, readl(virt_addr + 0x3080) & ~(0x01 << 30) | (0x01 << 30));		//rg_xtp_ln_frc_rx_eyes_yoffset	,
	writel(virt_addr + 0x3080, readl(virt_addr + 0x3080) & ~(0x1ff << 20) | (0x00 << 20));		//rg_xtp_ln_rx_eyes_yoffset	,

	printf("done\n");
}

void usxgmii_eyescan_cal(FILE *result_fd, void *virt_addr)
{
	printf("%s start...", __func__);

	/***********************************************
	 *Eye scan init
	 ***********************************************/
	//L0
	//I2C  0x20  0xFC[31:24] 0x30  RW
	writel(virt_addr + 0x3080, readl(virt_addr + 0x3080) & ~(0x01 << 3) | 0x01 << 03);		//rg_xtp_ln_frc_rx_eyes_cal_en	,
	writel(virt_addr + 0x3080, readl(virt_addr + 0x3080) & ~(0x01 << 2) | 0x01 << 2);		//rg_xtp_ln_rx_eyes_cal_en	,
	//wait 1ms
	usleep(1000);
	//I2C  0x20  0xFC[31:24] 0x30  RW
	//I2C  0x20  0xFC[31:24] 0x30  RW
	//I2C  0x20  0xFC[31:24] 0x30  RW
	//I2C  0x20  0xFC[31:24] 0x30  RW
	//I2C  0x20  0xFC[31:24] 0x30  RW
	//I2C  0x20  0xFC[31:24] 0x30  RW
	//I2C  0x20  0xFC[31:24] 0x30  RW
	//I2C  0x20  0xFC[31:24] 0x30  RW
	//I2C  0x20  0xFC[31:24] 0x30  RW
	//I2C  0x20  0xFC[31:24] 0x30  RW

	//I2C  0x20  0xFC[31:24] 0x30  RW

	//I2C  0x20  0xFC[31:24] 0x30  RW
	//I2C  0x20  0xFC[31:24] 0x30  RW
	writel(virt_addr + 0x3080, readl(virt_addr + 0x3080) & ~(0x01 << 2) | 0x00 << 2);		//rg_xtp_ln_rx_eyes_cal_en	,

	//I2C  0x20  0xFC[31:24] 0x00  RW
	writel(virt_addr + 0x0000, readl(virt_addr + 0x0000) & ~(0x0f << 0) | 0x04 << 0);		//rg_xtp_prb_sel_l	,
	writel(virt_addr + 0x0000, readl(virt_addr + 0x0000) & ~(0x0f << 8) | 0x04 << 8);		//rg_xtp_prb_sel_h	,
	writel(virt_addr + 0x0010, readl(virt_addr + 0x0010) & ~(0x3ff << 0) | 0xc4 << 0);		//rg_xtp_ln0_prb_sel_l	,
	if (debug)
		fprintf(result_fd, "ad_xtp_ln_rx_eyes_os-> %x\n", readl(virt_addr + 0x00D0) & 0x7F);		//rgs_xtp_probe_out	,READ,ad_xtp_ln_rx_eyes_os[06:00]
	//[PTA_READ l0_eyes_os 0x11F200d0[06:00]]

	printf("done\n");
}

void usxgmii_eyescan_sweep(FILE *result_fd, void *virt_addr)
{
	time_t timep;
	uint32_t PI_PR_boundary, PI_PL_boundary, PI_NR_boundary, PI_NL_boundary;
	uint32_t DAC_PH_boundary, DAC_PL_boundary, DAC_NH_boundary, DAC_NL_boundary;
	uint32_t errcount_even, errcount_even_7_0, errcount_even_15_8, errcount_even_19_16;
	uint32_t errcount_odd, errcount_odd_7_0, errcount_odd_15_8, errcount_odd_19_16;
	uint32_t dacrange;
	uint32_t x, y, z;
	uint32_t eye_even[256][64], eye_odd[256][64];
	uint32_t i, j;

	time (&timep);

	printf("%s start...", __func__);

	PI_PR_boundary = 31;			//PI right side max value
	PI_PL_boundary = 0;			//PI right side min value
	PI_NR_boundary = 63;			//PI left side max value(0x3F)
	PI_NL_boundary = 32;			//PI left side min value

	DAC_PH_boundary = 127;			//DAC up side max value
	DAC_PL_boundary = 0;			//DAC up side min value
	DAC_NH_boundary = 511;			//DAC down side max value
	DAC_NL_boundary = 384;			//DAC down side min value

	z = 0;
	//move to 64, 64

	//[PTA_INTERFACE = CVD]
	//[PTA_VAR PI_code 0x00]
	//[PTA_VAR DAC_code 0x00]
	//[PTA_VAR eyes_os 0xFF]
	//[PTA_VAR err_odd_15_0 0xAD]
	//[PTA_VAR err_odd_19_16 0xDE]
	//[PTA_VAR err_evn_15_0 0xAD]
	//[PTA_VAR err_evn_19_16 0xDE]

	// SPHY2 2T1R combo architecture
	// Do eye scan sweep
	// 20210617
	// Roy Wang

	// RG_XTP_LN0_RX_EYES_YOFFSET
	// eye scan DAC code
	// 9'hFF: 255
	// 7'h00: 0
	// 9'h100: -256

	// RG_XTP_LN0_RX_EYES_XOFFSET
	// eye scan PI code
	// 7'h3f: 63
	// 7'h00: 0
	// 7'h40: -64

	dacrange = 384;
	y = 511;

	//Step1 move to leftmost 32,lowmost 384
	fprintf(result_fd, "Start time: %s\n", asctime(gmtime(&timep)));
	fprintf(result_fd, "eye_1[ %d ; %d ]\n", PI_NR_boundary, DAC_NH_boundary);

	writel(virt_addr + 0x3080, readl(virt_addr + 0x3080) & ~(0x01 << 15) | 0x01 << 15);		//rg_xtp_ln_frc_rx_eyes_xoffset	,
	writel(virt_addr + 0x3080, readl(virt_addr + 0x3080) & ~(0x01 << 30) | 0x01 << 30);		//rg_xtp_ln_frc_rx_eyes_yoffset	,

	writel(virt_addr + 0x3080, readl(virt_addr + 0x3080) & ~(0x7f << 8) | PI_NR_boundary << 8);	//rg_xtp_ln_rx_eyes_xoffset	,
	writel(virt_addr + 0x3080, readl(virt_addr + 0x3080) & ~(0x1ff << 20) | DAC_NH_boundary << 20);	//rg_xtp_ln_rx_eyes_yoffset	,

	x = PI_NR_boundary;
	while (x >= PI_NL_boundary)
	{
		writel(virt_addr + 0x3080, readl(virt_addr + 0x3080) & ~(0x7f << 8) | x << 8);		//WRITE PI_code rg_xtp_ln_rx_eyes_xoffset	,
		x--;
	}
	y = DAC_NH_boundary;
	while (y >= DAC_NL_boundary)
	{
		writel(virt_addr + 0x3080, readl(virt_addr + 0x3080) & ~(0x1ff << 20) | y << 20);	//WRITE DAC_code rg_xtp_ln_rx_eyes_yoffset	,
		y--;
	}
	fprintf(result_fd, "eye_start[ %d ; %d ]\n", x + 1, y + 1);

	//Step2 start from PI_NL_boundary to PI_PR_boundary left to right
	x = PI_NL_boundary;
	i = 0;
	while (x < (PI_NL_boundary+64))	//;32~64
	{
		printf(".");

		j = 0;

		if (x > 63)
		{
			writel(virt_addr + 0x3080, readl(virt_addr + 0x3080) & ~(0x7f << 8) | (x - 64) << 8);	//WRITE PI_code rg_xtp_ln_rx_eyes_xoffset	,
		}
		else
		{
			writel(virt_addr + 0x3080, readl(virt_addr + 0x3080) & ~(0x7f << 8) | x << 8);		//WRITE PI_code rg_xtp_ln_rx_eyes_xoffset	,
		}

		//start from DAC_NL_boundary to DAC_PH_boundary down to up
		if (z == 0)
		{
			y = DAC_NL_boundary;
			while (y <= (DAC_PH_boundary + 512))	//384~511,(512+0-512)-(512+127-512)
			{
				if (y > 511)
				{
					writel(virt_addr + 0x3080, readl(virt_addr + 0x3080) & ~(0x1ff << 20) | (y - 512) << 20);	//WRITE DAC_code rg_xtp_ln_rx_eyes_yoffset	,
				}
				else
				{
					writel(virt_addr + 0x3080, readl(virt_addr + 0x3080) & ~(0x1ff << 20) | y << 20);	//WRITE DAC_code rg_xtp_ln_rx_eyes_yoffset	,
				}

				//toggle err_clr
				writel(virt_addr + 0x3080, readl(virt_addr + 0x3080) & ~(0x01 << 5) | 0x01 << 5);	//rg_xtp_ln_frc_rx_eyes_err_step	,
				writel(virt_addr + 0x3080, readl(virt_addr + 0x3080) & ~(0x01 << 4) | 0x00 << 4);	//rg_xtp_ln_rx_eyes_err_step	,
				//toggle err_clr
				writel(virt_addr + 0x3080, readl(virt_addr + 0x3080) & ~(0x01 << 7) | 0x01 << 7);	//rg_xtp_ln_frc_rx_eyes_err_clr	,
				writel(virt_addr + 0x3080, readl(virt_addr + 0x3080) & ~(0x01 << 6) | 0x00 << 6);	//rg_xtp_ln_rx_eyes_err_clr	,
				writel(virt_addr + 0x3080, readl(virt_addr + 0x3080) & ~(0x01 << 6) | 0x01 << 6);	//rg_xtp_ln_rx_eyes_err_clr	,
				writel(virt_addr + 0x3080, readl(virt_addr + 0x3080) & ~(0x01 << 6) | 0x00 << 6);	//rg_xtp_ln_rx_eyes_err_clr	,
				writel(virt_addr + 0x3080, readl(virt_addr + 0x3080) & ~(0x01 << 17) | 0x01 << 17);	//rg_xtp_ln_frc_rx_eyes_err_en	,
				writel(virt_addr + 0x3080, readl(virt_addr + 0x3080) & ~(0x01 << 16) | 0x01 << 16);	//error	,err_en=1,start to count error
				usleep(errcount_delay_ms * 1000);	//wait 0.1ms,deassert err_en
				writel(virt_addr + 0x3080, readl(virt_addr + 0x3080) & ~(0x01 << 16) | 0x00 << 16);	//error	,err_en=0,stop count error
				writel(virt_addr + 0x3080, readl(virt_addr + 0x3080) & ~(0x01 << 5) | 0x01 << 5);	//rg_xtp_ln_frc_rx_eyes_err_step	,
				writel(virt_addr + 0x3080, readl(virt_addr + 0x3080) & ~(0x01 << 4) | 0x01 << 4);	//rg_xtp_ln_rx_eyes_err_step	,
				writel(virt_addr + 0x3080, readl(virt_addr + 0x3080) & ~(0x01 << 17) | 0x01 << 17);	//rg_xtp_ln_frc_rx_eyes_err_en	,
				writel(virt_addr + 0x3080, readl(virt_addr + 0x3080) & ~(0x01 << 16) | 0x01 << 16);	//rg_xtp_ln_rx_eyes_err_en	,
				usleep(1000);	//wait 0.1ms,deassert err_en
				writel(virt_addr + 0x3080, readl(virt_addr + 0x3080) & ~(0x01 << 16) | 0x00 << 16);	//rg_xtp_ln_rx_eyes_err_en	,

				writel(virt_addr + 0x0010, readl(virt_addr + 0x0010) & ~(0x3ff << 0) | 0xf3 << 0);	//rg_xtp_ln0_prb_sel_l	,
				writel(virt_addr + 0x0010, readl(virt_addr + 0x0010) & ~(0x3ff << 16) | 0xf2 << 16);	//rg_xtp_ln0_prb_sel_h	,
				//;PRINT "0x11F200d0[07:00]-> " %HEX GetBit(MREAD("S32", axi:0x11F200D0),0x0,0x7)	;rgs_xtp_probe_out	,READ,ad_xtp_ln_rx_eyes_err_evn[07:00]
				//;PRINT "0x11F200d1[07:00]-> " %HEX GetBit(MREAD("S32", axi:0x11F200D1),0x0,0x7)	;rgs_xtp_probe_out	,READ,ad_xtp_ln_rx_eyes_err_evn[15:08]
				errcount_even_7_0 = (readl(virt_addr + 0x00D0) & 0xFF) << 0;
				errcount_even_15_8 = (readb(virt_addr + 0x00D1) & 0xFF) << 8;

				writel(virt_addr + 0x0010, readl(virt_addr + 0x0010) & ~(0x3ff << 0) | 0xf5 << 0);	//rg_xtp_ln0_prb_sel_l	,
				writel(virt_addr + 0x0010, readl(virt_addr + 0x0010) & ~(0x3ff << 16) | 0xf4 << 16);	//rg_xtp_ln0_prb_sel_h	,
				//;PRINT "0x11F200d0[07:00]-> " %HEX GetBit(MREAD("S32", axi:0x11F200D0),0x0,0x7)	;rgs_xtp_probe_out	,READ,ad_xtp_ln_rx_eyes_err_odd[07:00]
				//;PRINT "0x11F200d1[07:00]-> " %HEX GetBit(MREAD("S32", axi:0x11F200D1),0x0,0x7)	;rgs_xtp_probe_out	,READ,ad_xtp_ln_rx_eyes_err_odd[15:08]
				errcount_odd_7_0 = (readl(virt_addr + 0x00D0) & 0xFF) << 0;
				errcount_odd_15_8 = (readb(virt_addr + 0x00D1) & 0xFF) << 8;

				writel(virt_addr + 0x0010, readl(virt_addr + 0x0010) & ~(0x3ff << 0) | 0xf6 << 0);	//rg_xtp_ln0_prb_sel_l	,
				writel(virt_addr + 0x0010, readl(virt_addr + 0x0010) & ~(0x3ff << 16) | 0x100 << 16);	//rg_xtp_ln0_prb_sel_h	,
				//;PRINT "0x11F200d0[03:00]-> " %HEX GetBit(MREAD("S32", axi:0x11F200D0),0x0,0x3)	;rgs_xtp_probe_out	,READ,ad_xtp_ln_rx_eyes_err_evn[19:16]
				//;PRINT "0x11F200d1[03:00]-> " %HEX GetBit(MREAD("S32", axi:0x11F200D1),0x0,0x3)	;rgs_xtp_probe_out	,READ,ad_xtp_ln_rx_eyes_err_odd[19:16]
				errcount_even_19_16 = (readl(virt_addr + 0x00D0) & 0xF) << 16;
				errcount_odd_19_16 = (readb(virt_addr + 0x00D1) & 0xF) << 16;

				errcount_even = errcount_even_7_0 + errcount_even_15_8 + errcount_even_19_16;
				errcount_odd = errcount_odd_7_0 + errcount_odd_15_8 + errcount_odd_19_16;
				//;PRINT "even err_evn_19_16 0x11E4308C[11:08] -> " %d &errcount|&errcount1
				//printf("%d  , %d  , %d  , %d\n", x, y, errcount_even, errcount_odd);

				eye_even[255 - j][i] = errcount_even;
				eye_odd[255 - j][i] = errcount_odd;

				y++;
				j++;
			}
			z = 1;
		}
		else
		{
			y = DAC_PH_boundary + 512;
			while (y >= DAC_NL_boundary)	 //384~511,(512+0-512)-(512+127-512)
			{
				if (y > 511)
				{
					writel(virt_addr + 0x3080, readl(virt_addr + 0x3080) & ~(0x1ff << 20) | (y - 512) << 20);	//WRITE DAC_code rg_xtp_ln_rx_eyes_yoffset	,
				}
				else
				{
					writel(virt_addr + 0x3080, readl(virt_addr + 0x3080) & ~(0x1ff << 20) | (y) << 20);	//WRITE DAC_code rg_xtp_ln_rx_eyes_yoffset	,
				}

				//toggle err_clr
				writel(virt_addr + 0x3080, readl(virt_addr + 0x3080) & ~(0x01 << 5) | 0x01 << 5);	//rg_xtp_ln_frc_rx_eyes_err_step	,
				writel(virt_addr + 0x3080, readl(virt_addr + 0x3080) & ~(0x01 << 4) | 0x00 << 4);	//rg_xtp_ln_rx_eyes_err_step	,
				//toggle err_clr
				writel(virt_addr + 0x3080, readl(virt_addr + 0x3080) & ~(0x01 << 7) | 0x01 << 7);	//rg_xtp_ln_frc_rx_eyes_err_clr	,
				writel(virt_addr + 0x3080, readl(virt_addr + 0x3080) & ~(0x01 << 6) | 0x00 << 6);	//rg_xtp_ln_rx_eyes_err_clr	,
				writel(virt_addr + 0x3080, readl(virt_addr + 0x3080) & ~(0x01 << 6) | 0x01 << 6);	//rg_xtp_ln_rx_eyes_err_clr	,
				writel(virt_addr + 0x3080, readl(virt_addr + 0x3080) & ~(0x01 << 6) | 0x00 << 6);	//rg_xtp_ln_rx_eyes_err_clr	,
				writel(virt_addr + 0x3080, readl(virt_addr + 0x3080) & ~(0x01 << 17) | 0x1 << 17);	//rg_xtp_ln_frc_rx_eyes_err_en	,
				writel(virt_addr + 0x3080, readl(virt_addr + 0x3080) & ~(0x01 << 16) | 0x1 << 16);	//error	,err_en=1,start to count error
				usleep(errcount_delay_ms * 1000);	//wait 0.1ms,deassert err_en
				writel(virt_addr + 0x3080, readl(virt_addr + 0x3080) & ~(0x01 << 16) | 0x00 << 16);	//error	,err_en=0,stop count error
				writel(virt_addr + 0x3080, readl(virt_addr + 0x3080) & ~(0x01 << 5) | 0x01 << 5);	//rg_xtp_ln_frc_rx_eyes_err_step	,
				writel(virt_addr + 0x3080, readl(virt_addr + 0x3080) & ~(0x01 << 4) | 0x01 << 4);	//rg_xtp_ln_rx_eyes_err_step	,
				writel(virt_addr + 0x3080, readl(virt_addr + 0x3080) & ~(0x01 << 17) | 0x01 << 17);	//rg_xtp_ln_frc_rx_eyes_err_en	,
				writel(virt_addr + 0x3080, readl(virt_addr + 0x3080) & ~(0x01 << 16) | 0x01 << 16);	//rg_xtp_ln_rx_eyes_err_en	,
				usleep(1000);	//wait 0.1ms,deassert err_en
				writel(virt_addr + 0x3080, readl(virt_addr + 0x3080) & ~(0x01 << 16) | 0x00 << 16);	//rg_xtp_ln_rx_eyes_err_en	,

				writel(virt_addr + 0x0010, readl(virt_addr + 0x0010) & ~(0x3ff << 0) | 0xf3 << 0);	//rg_xtp_ln0_prb_sel_l	,
				writel(virt_addr + 0x0010, readl(virt_addr + 0x0010) & ~(0x3ff << 16) | 0xf2 << 16);	//rg_xtp_ln0_prb_sel_h	,
				//;PRINT "0x11F200d0[07:00]-> " %HEX GetBit(MREAD("S32", axi:0x11F200D0),0x0,0x7)	;rgs_xtp_probe_out	,READ,ad_xtp_ln_rx_eyes_err_evn[07:00]
				//;PRINT "0x11F200d1[07:00]-> " %HEX GetBit(MREAD("S32", axi:0x11F200D1),0x0,0x7)	;rgs_xtp_probe_out	,READ,ad_xtp_ln_rx_eyes_err_evn[15:08]
				errcount_even_7_0 = (readl(virt_addr + 0x00D0) & 0xFF) << 0;
				errcount_even_15_8 = (readb(virt_addr + 0x00D1) & 0xFF) << 8;

				writel(virt_addr + 0x0010, readl(virt_addr + 0x0010) & ~(0x3ff << 00) | 0xf5 << 00);	//rg_xtp_ln0_prb_sel_l	,
				writel(virt_addr + 0x0010, readl(virt_addr + 0x0010) & ~(0x3ff << 16) | 0xf4 << 16);	//rg_xtp_ln0_prb_sel_h	,
				//;PRINT "0x11F200d0[07:00]-> " %HEX GetBit(MREAD("S32", axi:0x11F200D0),0x0,0x7)	;rgs_xtp_probe_out	,READ,ad_xtp_ln_rx_eyes_err_odd[07:00]
				//;PRINT "0x11F200d1[07:00]-> " %HEX GetBit(MREAD("S32", axi:0x11F200D1),0x0,0x7)	;rgs_xtp_probe_out	,READ,ad_xtp_ln_rx_eyes_err_odd[15:08]
				errcount_odd_7_0 = (readl(virt_addr + 0x00D0) & 0xFF) << 0;
				errcount_odd_15_8 = (readb(virt_addr + 0x00D1) & 0xFF) << 8;

				writel(virt_addr + 0x0010, readl(virt_addr + 0x0010) & ~(0x3ff << 0) | 0xf6 << 0);	//rg_xtp_ln0_prb_sel_l	,
				writel(virt_addr + 0x0010, readl(virt_addr + 0x0010) & ~(0x3ff << 16) | 0x100 << 16);	//rg_xtp_ln0_prb_sel_h	,
				//;PRINT "0x11F200d0[03:00]-> " %HEX GetBit(MREAD("S32", axi:0x11F200D0),0x0,0x3)	;rgs_xtp_probe_out	,READ,ad_xtp_ln_rx_eyes_err_evn[19:16]
				//;PRINT "0x11F200d1[03:00]-> " %HEX GetBit(MREAD("S32", axi:0x11F200D1),0x0,0x3)	;rgs_xtp_probe_out	,READ,ad_xtp_ln_rx_eyes_err_odd[19:16]
				errcount_even_19_16 = (readl(virt_addr + 0x00D0) & 0xF) << 16;
				errcount_odd_19_16 = (readb(virt_addr + 0x00D1) & 0xF) << 16;

				errcount_even = errcount_even_7_0 + errcount_even_15_8 + errcount_even_19_16;
				errcount_odd = errcount_odd_7_0 + errcount_odd_15_8 + errcount_odd_19_16;
				//;PRINT "even err_evn_19_16 0x11E4308C[11:08] -> " %d &errcount|&errcount1
				//printf("%d  , %d  , %d  , %d\n", x, y, errcount_even, errcount_odd);

				eye_even[j][i] = errcount_even;
				eye_odd[j][i] = errcount_odd;

				y--;
				j++;
			}
			z = 0;
		}
		x++;
		i++;
	}
	printf("\n");

	fprintf(result_fd, "End time: %s\n", asctime(gmtime(&timep)));

	fprintf(result_fd, "eye_even\n");
	for (j = 0; j < 256; j++) {
		for (i = 0; i < 64; i++) {
			fprintf(result_fd, "%d  , ", eye_even[j][i]);
		}
		fprintf(result_fd, "\n");
	}

	fprintf(result_fd, "\n");

	if (debug) {
		fprintf(result_fd, "eye_odd\n");
		for (j = 0; j < 256; j++) {
			for (i = 0; i < 64; i++) {
				fprintf(result_fd, "%d  , ", eye_odd[j][i]);
			}
			fprintf(result_fd, "\n");
		}
	}

	printf("done\n");
}

int main(int argc, char **argv) {
	int memory_fd;
	FILE *result_fd;
	void *map_base = NULL;
	void *virt_addr = NULL;
	char memory_filename[] = "/dev/mem";
	char result_filename[] = "/usxgmii_eyescan_result.csv";
	off_t offset = 0;

	if (argc < 3) {
		usage();
		exit(1);
	}

	if((memory_fd = open(memory_filename, O_RDWR | O_SYNC)) == -1)
		PRINT_ERROR;

	if ((result_fd = fopen(result_filename, "wb")) == NULL)
		PRINT_ERROR;

	/* Map four pages */
	offset = strtoul(argv[1], NULL, 16);
	map_base = mmap(0, 16 * MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, memory_fd, offset & ~MAP_MASK);
	if (map_base == (void *) -1)
		PRINT_ERROR;

	/* Geth errcount delay */
	errcount_delay_ms = strtoul(argv[2], NULL, 10);

	if (argc == 4 && !strcmp(argv[3], "dbg"))
		debug = 1;

	/* Get virtual address */
	virt_addr = map_base + (offset & MAP_MASK);

	/* Read USXGMII AEQ */
	usxgmii_xfi_read_aeq(result_fd, virt_addr);

	/* Force USXGMII AEQ */
	usxgmii_frc_AEQ(result_fd, virt_addr);

	/* Read USXGMII AEQ */
	usxgmii_xfi_read_aeq(result_fd, virt_addr);

	/* Enable USXGMII EYESCAN */
	usxgmii_eyescan_en(result_fd, virt_addr);

	/* Cal USXGMII EYESCAN */
	usxgmii_eyescan_cal(result_fd, virt_addr);

	/* Read USXGMII Rx Offset */
	usxgmii_read_rx_offset(result_fd, virt_addr);

	printf("Please press any keys to start eyescan sweep...\n");
	getchar();

	/* SWEEP USXGMII EYESCAN */
	usxgmii_eyescan_sweep(result_fd, virt_addr);

	/* Unmap four pages */
	if (munmap(map_base, 16 * MAP_SIZE) == -1)
		PRINT_ERROR;

	fclose(result_fd);

	close(memory_fd);

	return 0;
}
