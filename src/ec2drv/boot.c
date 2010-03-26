/**	boot.c
	Provides routines for using the bootloader in the EC2 / EC3
	(C) Ricky White 2007
	Liscence GPL V2
 */
#include <string.h>
#include <stdint.h>
#include "boot.h"

/** Run the main debugger application.
	\returns the version of the firmware currently loaded into the debugger.
 */
uint8_t boot_run_app( EC2DRV *obj )
{
	DUMP_FUNC();
	uint8_t fw_ver;
	write_port(obj,"\x06\x00\x00",3);
	fw_ver = read_port_ch(obj);
	DUMP_FUNC_END();
	return fw_ver;
}

/** Get the version of the bootloader
	known versions are 2 for ec3 and 3 for ec2.
 */
uint8_t boot_get_version( EC2DRV *obj )
{
	DUMP_FUNC();
	write_port(obj,"\x00\x00\x00",3);
	obj->boot_ver = read_port_ch(obj);
	DUMP_FUNC_END();
	return obj->boot_ver;
}


/** Erase the currently addressed sector in the debugger
 */
void boot_erase_flash_page( EC2DRV *obj )
{
	DUMP_FUNC();
	trx(obj,"\x02\x00\x00",3,"\x00",1);
	DUMP_FUNC_END();
}


/** Select the active flash page
 */
void boot_select_flash_page( EC2DRV *obj, uint8_t page_num )
{
	DUMP_FUNC();
	char buf[3];
	buf[0] = 1;			// set sector
	buf[1] = page_num;
	buf[2] = 0;
	trx(obj,buf,3,"\x00",1);
	DUMP_FUNC_END();
}

/** Write a page of data into the debugger (active page)
	The buffer must contain full page data

	\returns TRUE=success, FALSE = failure.
 */
BOOL boot_write_flash_page( EC2DRV *obj, uint8_t *buf, BOOL do_xor )
{
	DUMP_FUNC();
	const uint16_t page_size = 512;
	uint8_t out_buf[page_size];
	char tmp[2];
	uint16_t local_csum;
	int i;
	
	trx(obj,"\x03\x02\x00",3,"\x00",1);
	if(do_xor)
	{	
		for(i=0;i<page_size;i++)
			out_buf[i] = buf[i] ^ 0x55;
		local_csum = boot_local_calc_page_cksum(buf);
	}
	else
	{
		// undo the xor so we can calculate the cksum.
		for(i=0;i<page_size;i++)
			out_buf[i] = buf[i] ^ 0x55;
		local_csum = boot_local_calc_page_cksum(out_buf);
		memcpy(out_buf,buf,page_size);	// load the raw pre xored data back.
	}
	
	if(obj->dbg_adaptor==EC3)
	{
		// write the data block
		// 8 * 63 byte blocks
		// + 1 * 8 byte block
		int k;
		uint16_t offset=0;
		for(k=0; k<8; k++, offset+=63 )
			write_port( obj, (char*)out_buf+offset, 63 );
		// now the 8 left over bytes 
		write_port( obj, (char*)out_buf+offset, 8 );
	}
	else if(obj->dbg_adaptor==EC2)
	{
		write_port(obj,(char*)out_buf,512);
	}
	read_port_ch(obj);			// 0x00
//	printf("local_csum = 0x%04x, remote_csum=0x%04x\n",local_csum, boot_calc_page_cksum(obj));
	BOOL r = boot_calc_page_cksum(obj) == local_csum;
	DUMP_FUNC_END();
	return r;
}


/** read a byte from the debuggers code memory
	\param obj		object to act on
	\param addr		Address in code space of the EC2 / EC3 to read
	\returns		Byte at the specified address in the flash of the debugger.
 */
uint8_t boot_read_byte( EC2DRV *obj, uint16_t addr )
{
	DUMP_FUNC();
	char cmd[3];
	uint8_t c;
	cmd[0] = 0x05;				// read
	cmd[1] = (addr>>8) & 0xff;
	cmd[2] = addr & 0xff; 
	write_port(obj,cmd,3);
	while(!read_port(obj,(char*)&c,1))
		write_port(obj,cmd,3);
	DUMP_FUNC_END();
	return c;
}


/** Calculate the CRC for the active page and return it
	this value is calculated in the debugger and can be compared to a local copy
	to ensure the firmware was transfered correctly.
*/
uint16_t boot_calc_page_cksum( EC2DRV *obj )
{
	DUMP_FUNC();
	char buf[2];
	uint16_t cksum;
	write_port(obj,"\x04\x00\x00",3);
	read_port(obj,buf,2);
	cksum = (buf[0]&0xff)<<8 | (buf[1]&0xff);
//	printf("checksum = 0x%04x\n",cksum);
	DUMP_FUNC();
	return cksum;
}


/** Calculates the same checksum as boot_calc_page_cksum asks the EC2/3 to do 
	but does but on a local	block of data.

	There's a little difference in the SiLabs code compared to CCITT-CRC16
	(although the polynomial looks the same): 
 
	For real CRC, the MSB test (&0x8000) should occur _before_ shifting the
	value, not afterwards
	(see http://en.wikipedia.org/wiki/Cyclic_redundancy_check ).
	Or use C after shifting, instead of MSB before shifting. 
 
	And CCITT-CRC16 uses (IMHO) 0xFFFF for initialisation, not 0. But that's
	not important for the strength of the CRC. 
	-- Kolja

	\param data buffer contains 1 page worth of data to calculate a checksum
				for (512 bytes)
	\returns	Calculated checksum.
*/
uint16_t boot_local_calc_page_cksum(uint8_t *data)
{ 
	int i,j; 
	uint16_t cksum = 0;
 
	for(i = 0; i<512; i++) 
	{ 
		cksum ^= (uint16_t)data[i] << 8; 
 
		for(j = 0; j < 8; j++) 
		{ 
			cksum <<= 1; 
			if(cksum & 0x8000) cksum ^= 0x1021; 
		}; 
	}; 
 
	return cksum; 
};
