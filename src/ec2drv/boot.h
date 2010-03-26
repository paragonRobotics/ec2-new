/**	boot.h
	Provides routines for using the bootloader in the EC2 / EC3
	(C) Ricky White 2007
	Liscence GPL V2

	These functions can only be used just after we connect to the EC2, 
	is before the go or the first cmd 0x06 has been executed.
*/
#ifndef BOOT_H
#define BOOT_H
#include <stdint.h>
#include "ec2drv.h"

uint8_t boot_run_app( EC2DRV *obj );
uint8_t boot_get_version( EC2DRV *obj );
void boot_erase_flash_page( EC2DRV *obj );
void boot_select_flash_page( EC2DRV *obj, uint8_t page_num );
BOOL boot_write_flash_page( EC2DRV *obj, uint8_t *buf, BOOL do_xor );
uint8_t boot_read_byte( EC2DRV *obj, uint16_t addr );
uint16_t boot_calc_page_cksum( EC2DRV *obj );
uint16_t boot_local_calc_page_cksum(uint8_t *data);

#endif	// BOOT_H
