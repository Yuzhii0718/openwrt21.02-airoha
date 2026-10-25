/***************************************************************************************
 *      Copyright(c) 2014 ECONET Incorporation All rights reserved.
 *
 *      This is unpublished proprietary source code of ECONET Networks Incorporation
 *
 *      The copyright notice above does not evidence any actual or intended
 *      publication of such source code.
 ***************************************************************************************
 */

#include <asm/tc3162.h>
#include "spi/spi_nor_flash.h"
#include "spi/spi_controller.h"

#define _SPI_NOR_ENABLE_MANUAL_MODE				SPI_CONTROLLER_Enable_Manual_Mode
//#define _SPI_NOR_ENABLE_AUTO_MODE				SPI_CONTROLLER_Enable_Auto_Mode
#define _SPI_NOR_WRITE_ONE_BYTE					SPI_CONTROLLER_Write_One_Byte
#define _SPI_NOR_WRITE_NBYTE					SPI_CONTROLLER_Write_NByte
#define _SPI_NOR_READ_NBYTE						SPI_CONTROLLER_Read_NByte
#define _SPI_NOR_READ_CHIP_SELECT_HIGH			SPI_CONTROLLER_Chip_Select_High
#define _SPI_NOR_READ_CHIP_SELECT_LOW			SPI_CONTROLLER_Chip_Select_Low
#define _SPI_NOR_OP_READ_FROM_CACHE_SINGLE		(0x03)	/* Read data from cache of SPI NOR chip, single speed*/

void spi_nor_read(unsigned char *data, unsigned long addr, int len)
{
	if((addr >= 0xbc000000) && (addr <= 0xbfffffff)) {		/* Reserver address area for system */
		if( (addr & 0xbfc00000) == 0xbfc00000) {
			addr &= 0x003fffff;
		} else {
			addr &= 0x03ffffff;
		}
	}

	/* Switch to manual mode*/
	_SPI_NOR_ENABLE_MANUAL_MODE();
	
	/* 1. Chip Select low */
	_SPI_NOR_READ_CHIP_SELECT_LOW();
	
	/* 2. Send opcode */
	_SPI_NOR_WRITE_ONE_BYTE( _SPI_NOR_OP_READ_FROM_CACHE_SINGLE );

	/* 3. Send data_offset addr, 0: 3Byte, 1: 4Byte*/
	if(VPint(SPI_CONTROLLER_REGS_STRAP) & (0x1) ) {
		_SPI_NOR_WRITE_ONE_BYTE( ((addr >> 24 ) &(0xff)) );
	}
	
	_SPI_NOR_WRITE_ONE_BYTE( ((addr >> 16 ) &(0xff)) );
	_SPI_NOR_WRITE_ONE_BYTE( ((addr >> 8 ) &(0xff)) );
	_SPI_NOR_WRITE_ONE_BYTE( ((addr >> 0 ) &(0xff)) );		
		
	/* 4. Read n byte (len) data */
	_SPI_NOR_READ_NBYTE(data, len, SPI_CONTROLLER_SPEED_SINGLE);	
		
	/* 5. Chip Select High */
	_SPI_NOR_READ_CHIP_SELECT_HIGH();

	//_SPI_NOR_ENABLE_AUTO_MODE();
}

unsigned char spi_nor_read_byte(unsigned long addr)
{
	unsigned char data;

	spi_nor_read(&data, addr, 1);

	return data;
}

/* End of [spi_nor_flash.c] package */
