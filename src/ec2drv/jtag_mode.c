#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "jtag_mode.h"

static BOOL jtag_write_xdata_page( EC2DRV *obj, char *buf, unsigned char page,
							unsigned char start, int len );
//static void ec2_read_xdata_page( EC2DRV *obj, char *buf, unsigned char page,
//						  unsigned char start, int len );
void jtag_read_xdata_page( EC2DRV *obj, char *buf, unsigned char page,
						   unsigned char start, int len );


/** Connect to a device using JTAG mode.
 */
void jtag_connect_target( EC2DRV *obj )
{
	trx(obj,"\x04",1,"\x0d",1);
}

uint16_t jtag_device_id( EC2DRV *obj )
{
	char buf[6];
//	trx( obj,"\x0A\x00",2,"\x21\x01\x03\x00\x00\x12",6);	
	write_port( obj, "\x0A\x00", 2 );
	read_port( obj, buf, 6 );
	return buf[2]<<8 | 0;	// no rev id known yet
}

uint16_t jtag_unique_device_id( EC2DRV *obj )
{
	char buf[4];
	//		trx(obj,"\x16\x01\xE0",3,"\x00",1);	// test
// why 15/10/06?		trx(obj,"\x0b\x02\x02\x00",4,"\x0D",1);	// sys reset
	trx(obj,"\x0b\x02\x02\x00",4,"\x0D",1);	// sys reset	Makes system halt when required.
	ec2_target_halt(obj);	// halt needed otherwise device may return garbage!
	trx(obj,"\x10\x00",2,"\x07\x0D",2);
	write_port(obj,"\x0C\x02\x80\x12",4);
	read_port(obj,buf,4);
//		print_buf( buf,4);
	return buf[2];
}

BOOL jtag_flashcon( EC2DRV *obj, uint8_t value)
{
	//T 0d 05 82 08 01 00 00	R 0d	code
	//			    81					scratchpad
	return jtag_write_IR( obj, 0x82, 8, (uint32_t)value );
}


BOOL jtag_halt( EC2DRV *obj )
{
	return trx( obj, "\x0B\x02\x01\x00", 4, "\x0d", 1 );
}


// These registers for the F120
extern SFRREG SFR_SFRPAGE;
extern SFRREG SFR_FLSCL;
extern SFRREG SFR_CCH0LC;
extern SFRREG SFR_OSCICN;
extern SFRREG SFR_CLKSEL;
extern SFRREG SFR_CCH0CN;

/** Write an entire flash sector.
	NOTE this function does not erase the sector first.
	\param obj	ec2drv object to act on
	\param buf	Buffer containing data to write, must contain a full sectors worth of data.
	\returns TRUE on success, FALSE on failure
*/
BOOL jtag_write_flash_sector( EC2DRV *obj, uint32_t sect_addr, uint8_t *buf,
							  BOOL scratchpad )
{
	DUMP_FUNC();
	uint8_t max_block_len = (obj->dbg_adaptor==EC2) ? 0x0c : 0x3F;
	BOOL result = TRUE;
//	printf("jtag_write_flash_sector(...)    addr = 0x%05x\n",sect_addr);
	jtag_flashcon( obj, 0x00 );
	trx( obj, "\x0b\x02\x01\x00",4,"\x0d",1);
	
	ec2_write_paged_sfr( obj, SFR_FLSCL, 0x80 );
	ec2_write_paged_sfr( obj, SFR_CLKSEL, 0x00 );
	
	ec2_write_paged_sfr( obj, SFR_OSCICN, 0xc0 );
	ec2_write_paged_sfr( obj, SFR_CCH0LC, 0x3e );
	ec2_write_paged_sfr( obj, SFR_CCH0CN, 0x07 );	// Cache Lock
	
	trx( obj, "\x0b\x02\x04\x00",4,"\x0d",1);
	trx( obj, "\x0d\x05\x80\x12\x02\x28\x00",7,"\x0d",1);
	trx( obj, "\x0d\x05\xc2\x07\x47\x00\x00",7,"\x0d",1);
	trx( obj, "\x0c\x02\xc3\x10",4,"\x00\xff\xff\x0d",4);
	trx( obj, "\x0c\x02\xc3\x10",4,"\x00\xff\xff\x0d",4);
	trx( obj, "\x0B\x02\x01\x00", 4, "\x0D", 1 );	
	
//	printf("Cache control\n");
	// Cache control
	if( obj->dev->has_cache )
		ec2_write_paged_sfr( obj, SFR_CCH0LC, 0x00 );	// Cache Lock
	
//	printf("Oscilator config\n");
	// oscilator config
	uint8_t cur_oscin = ec2_read_paged_sfr( obj, SFR_OSCICN,  0 );
	ec2_write_paged_sfr( obj, SFR_OSCICN, 0x80 );
	
//	printf("ClkSel\n");
	uint8_t cur_clksel = ec2_read_paged_sfr( obj, SFR_CLKSEL, 0 );
	ec2_write_paged_sfr( obj, SFR_CLKSEL, 0x00 );
	
	ec2_core_suspend(obj);
	
	JTAG_0x14( obj, 0x10, 0x00 );
	JTAG_0x16_Len2( obj, 0xc1, 0x40 );
	
	JTAG_unknown_op_0x15(obj);
	JTAG_0x16_Len3( obj, 0x00, 0x80, 0x90 );
	
	JTAG_unknown_op_0x15(obj);
	JTAG_0x16_Len3( obj, 0x00, 0x00, 0x00 );
	
	JTAG_unknown_op_0x15(obj);
	JTAG_0x16_Len3( obj, 0x00, 0x00, 0x00 );
	
	JTAG_0x14( obj, 0x10, 0x00 );
	JTAG_0x16_Len2( obj, 0xc1, 0x40 );
	
	JTAG_unknown_op_0x15(obj);
	JTAG_0x16_Len3( obj, 0x00, 0x80, 0x10 );
	
	JTAG_unknown_op_0x15(obj);
	JTAG_0x16_Len3( obj, 0x00, 0x80, 0xD0 );
	
	JTAG_unknown_op_0x15(obj);
	JTAG_0x16_Len3( obj, 0x00, 0x80, 0x10 );
	
	JTAG_0x14( obj, 0x10, 0x00 );
	JTAG_0x16_Len2( obj, 0xc1, 0x40 );
	
	JTAG_unknown_op_0x15(obj);
	JTAG_0x16_Len3( obj, 0x20, 0xe6, 0x16 );	
	
	JTAG_unknown_op_0x15(obj);
	JTAG_0x16_Len3( obj, 0x20, 0xe6, 0xD6 );	

	JTAG_unknown_op_0x15(obj);
	JTAG_0x16_Len3( obj, 0x20, 0xe6, 0x16 );	
	
	JTAG_0x14( obj, 0x10, 0x00 );
	JTAG_0x16_Len2( obj, 0xc1, 0x40 );
	
	JTAG_unknown_op_0x15(obj);
	JTAG_0x16_Len3( obj, 0x00, 0x80, 0x10 );
	
	JTAG_unknown_op_0x15(obj);
	JTAG_0x16_Len3( obj, 0x00, 0x80, 0xD0 );
	
	JTAG_unknown_op_0x15(obj);
	JTAG_0x16_Len3( obj, 0x00, 0x80, 0x10 );
	
	uint32_t sect_start_addr;
	uint32_t sect_end_addr;
	uint16_t sector_size;
	
	if(!scratchpad)
	{
		// Normal case write to code memory
		
		// F120 does this... T 0d 05 80 12 02 28 00	R 0d	// what is JTAG IR = 0x80?
		trx(obj,"\x0d\x05\x80\x12\x02\x28\x00",7,"\x0d",1);	// without this writes fail on F120
		
		sector_size = obj->dev->flash_sector_size;
		sect_start_addr = sect_addr / obj->dev->flash_sector_size;
		sect_start_addr *= obj->dev->flash_sector_size;
		sect_end_addr = sect_start_addr+obj->dev->flash_sector_size-1;
	}
	else
	{
		// F120 does this... T 0d 05 80 12 02 28 00	R 0d	// what is JTAG IR = 0x80?
		trx(obj,"\x0d\x05\x80\x12\x02\x28\x00",7,"\x0d",1);	// without this writes fail on F120
		
		sector_size = obj->dev->scratchpad_sector_size;
		sect_start_addr = sect_addr / obj->dev->scratchpad_sector_size;
		sect_start_addr *= obj->dev->scratchpad_sector_size;
		sect_end_addr = sect_start_addr+obj->dev->scratchpad_sector_size-1;
	}
	
	// begin actual write now
//	printf("sector = [0x%05x,0x%05x]\n",sect_start_addr, sect_end_addr );
	if(scratchpad)
		jtag_flashcon( obj, 0xa0 );
	else
		jtag_flashcon( obj, 0x20 );
	
	set_flash_addr_jtag( obj, sect_start_addr );	
	// erase flash
	trx( obj, "\x0f\x01\xa5", 3, "\x0d", 1 );
	if(scratchpad)
		jtag_flashcon( obj, 0x82 );
	else
		jtag_flashcon( obj, 0x02 );
	trx( obj, "\x0e\x00", 2, "\xa5", 1 );	// ???
	trx( obj, "\x0e\x00", 2, "\xff", 1 );	// ???
	
	if(scratchpad)
		jtag_flashcon( obj, 0x90 );
	else
		jtag_flashcon( obj, 0x10 );
	set_flash_addr_jtag( obj, sect_start_addr );
	
	uint32_t addr = sect_start_addr;
	char tbuf[0xff];
	if( obj->dbg_adaptor==EC2 )
	{
		while( (sect_end_addr-addr) >= max_block_len )
		{
//			printf("Fragment start_addr = 0x%05x, len = 0x%04x\n",addr,max_block_len );
			memcpy(tbuf,"\x12\x02\x0c\x00",4);
			memcpy(tbuf+4,buf,max_block_len);
			write_port(obj,tbuf,max_block_len+4);
			result |= read_port_ch(obj)==0x0d;
			addr += max_block_len;
			buf += max_block_len;
		}
		if( (sect_end_addr-addr)>0 )
		{
			uint8_t len = (sect_end_addr-addr)+1;
//			printf("Fragment start_addr = 0x%05x, len = 0x%04x\n",addr,len );
			// mop up wats left, it had better be a multiple of 4 for the F120 and start on the correct offset
			tbuf[0] = 0x12;
			tbuf[1] = 0x02;
			tbuf[2] = len;
			tbuf[3] = 0x00;
			memcpy(&tbuf[4],buf,len);
			write_port(obj, tbuf,4+len);
			result &= read_port_ch( obj )==0x0d;
		}
	}
	else
	{
		usleep(1000000);
		// EC3 does it differently.
		// tell the EC2 a sectors worth of data is comming
		tbuf[0] = 0x12;
		tbuf[1] = 0x02;
		tbuf[2] = sector_size & 0xff;
		tbuf[3] = sector_size>>8 & 0xff;
		write_port(obj,tbuf,4);
		// write in the chunks now
		uint16_t offset=0;
		offset=0;
		for( offset=0; offset<sector_size; offset+=max_block_len )
		{
			uint16_t blk = sector_size-offset>max_block_len ? max_block_len
				: sector_size-offset;
			write_port(obj,(char*)buf+offset,blk);
		}
		result &= read_port_ch(obj)==0x0d;
		
	}
	jtag_flashcon( obj, 0x00 );
	trx( obj, "\x0b\x02\x01\x00", 4, "\x0d", 1 );
	trx( obj, "\x03\x02\xB6\x80", 4, "\x0d", 1 );
	trx( obj, "\x03\x02\xB2\x14", 4, "\x0d", 1 );
	return result;
}


/** Read a sector of flash memory.
	Conveniance function, read exactly one sector of flash memory starting at 
	the specified address.
	This function fill convert the supplied address back to a sector start if it
	not exact.

	\param obj	ec2drv object to act on
	\param buf	must be big enough to hold an entire flash sector to be read
	\returns TRUE on success, FALSE on failure
*/
BOOL jtag_read_flash_sector( EC2DRV *obj, uint32_t sect_addr, uint8_t *buf,
							 BOOL scratchpad )
{
	uint32_t sector_size = scratchpad ? 
			obj->dev->scratchpad_sector_size : obj->dev->flash_sector_size;
	uint32_t sect_start_addr = (sect_addr/sector_size)*sector_size;
	uint32_t sect_end_addr = sect_start_addr+sector_size-1;
	uint32_t len = sector_size;
	
	return jtag_read_flash( obj, buf,sect_start_addr, len, scratchpad );
}


/** Read data from the code memory (Flash) of a JTAG connected processor.
	Note that this function contains JTAG operations that are not fully 
	understood but aseem necessary to tead the flash from the F120 but don't
	seem to impact the F020.

	\param obj			ec2drv object to cact on.
	\param buf			Buffer to recieve the read bytes
	\param start_addr	Address to start reading from.
						( banks are treated as a flast address range )
	\param len			Number of bytes to read.
	\returns			TRUE on success, FALSE otherwise.
 */
BOOL jtag_read_flash( EC2DRV *obj, uint8_t *buf,
					  uint32_t start_addr, int len, BOOL scratchpad )
{
	BOOL result = TRUE;
	//ec2_target_halt(obj);
	jtag_halt(obj);		// trx(obj,"\x0B\x02\x01\x00",4,"\x0D ",1);			// suspend processor?
	ec2_write_paged_sfr( obj, SFR_FLSCL, 0x80 );
	ec2_write_paged_sfr( obj, SFR_CLKSEL, 0x00 );
	
	ec2_write_paged_sfr( obj, SFR_OSCICN, 0xc0 );
	
	if( obj->dev->has_cache )
		ec2_write_paged_sfr( obj, SFR_CCH0LC, 0x3e );	// Cache Lock
	
	//====== flash read starts here
	uint8_t cur_page	= ec2_read_paged_sfr( obj, SFR_SFRPAGE, 0 );
	uint8_t cur_flscl	= ec2_read_paged_sfr( obj, SFR_FLSCL, 0 );
	
	// Cache control
	if( obj->dev->has_cache )
		ec2_write_paged_sfr( obj, SFR_CCH0LC, 0x00 );	// Cache Lock
	
	// oscilator config
	uint8_t cur_oscin = ec2_read_paged_sfr( obj, SFR_OSCICN,  0 );
	ec2_write_paged_sfr( obj, SFR_OSCICN, 0x80 );
	
	uint8_t cur_clksel = ec2_read_paged_sfr( obj, SFR_CLKSEL, 0 );
	ec2_write_paged_sfr( obj, SFR_CLKSEL, 0x00 );
	
	//ec2_write_paged_sfr( obj, SFR_SFRPAGE, 0 );		// page 0
	ec2_write_raw_sfr( obj, SFR_SFRPAGE.addr, 0 );			// page 0
	
	trx(obj,"\x0B\x02\x04\x00",4,"\x0D",1); 			//ec2_core_suspend(obj);
	
	// F120 does this... T 0d 05 80 12 02 28 00	R 0d	// what is JTAG IR = 0x80?
	trx(obj,"\x0d\x05\x80\x12\x02\x28\x00",7,"\x0d",1);
	
	//--------------------------------------------------------------------------
	JTAG_0x14( obj, 0x10, 0x00 );
	JTAG_0x16_Len2( obj, 0xc1, 0x40 );
	
	
	JTAG_unknown_op_0x15(obj);
	JTAG_0x16_Len3( obj, 0x00, 0x80, 0x90 );
	
	JTAG_unknown_op_0x15(obj);
	JTAG_0x16_Len3( obj, 0x00, 0x00, 0x00 );
	
	JTAG_unknown_op_0x15(obj);
	JTAG_0x16_Len3( obj, 0x00, 0x00, 0x00 );
	
	JTAG_0x14( obj, 0x10, 0x00 );
	JTAG_0x16_Len2( obj, 0xc1, 0x40 );
	
	JTAG_unknown_op_0x15(obj);
	JTAG_0x16_Len3( obj, 0x00, 0x80, 0x10 );
	
	JTAG_unknown_op_0x15(obj);
	JTAG_0x16_Len3( obj, 0x00, 0x80, 0xD0 );
	
	JTAG_unknown_op_0x15(obj);
	JTAG_0x16_Len3( obj, 0x00, 0x80, 0x10 );
	
	JTAG_0x14( obj, 0x10, 0x00 );
	JTAG_0x16_Len2( obj, 0xc1, 0x40 );
	
	JTAG_unknown_op_0x15(obj);
	JTAG_0x16_Len3( obj, 0x20, 0xe6, 0x16 );	
	
	JTAG_unknown_op_0x15(obj);
	JTAG_0x16_Len3( obj, 0x20, 0xe6, 0xD6 );	

	JTAG_unknown_op_0x15(obj);
	JTAG_0x16_Len3( obj, 0x20, 0xe6, 0x16 );	
	
	JTAG_0x14( obj, 0x10, 0x00 );
	JTAG_0x16_Len2( obj, 0xc1, 0x40 );
	
	JTAG_unknown_op_0x15(obj);
	JTAG_0x16_Len3( obj, 0x00, 0x80, 0x10 );
	
	JTAG_unknown_op_0x15(obj);
	JTAG_0x16_Len3( obj, 0x00, 0x80, 0xD0 );
	
	JTAG_unknown_op_0x15(obj);
	JTAG_0x16_Len3( obj, 0x00, 0x80, 0x10 );
	
//	usleep(10000);
	//--------------------------------------------------------------------------
	
	// setup for readthe actual read
	set_flash_addr_jtag( obj, start_addr );
	
	// FLASH Control
	//	T 0d 05 82 08 01 00 00	R 0d			// FLASHCON = 0x01?	8 = 8 bits
	// normal program memory
	// 82 flash control reg
	if( scratchpad )
		jtag_flashcon(obj,0x81);
	else
		jtag_flashcon(obj,0x01);
			
	int			i;
	uint32_t	addr;
	char cmd[7];	//,acmd[7];
	memset( buf, 0xff, len );
	
	uint16_t block_size = obj->dbg_adaptor==EC3 ? 0x3C : 0x0C;
	char tmp_buf[0x3d];	// room for terminator
//	printf("block size = 0x%04x\n",block_size);
	for( i=0; i<len; i+=block_size )
	{
		addr = start_addr + i;
//		printf("addr=0x%05x\n",addr);
		set_flash_addr_jtag( obj, addr );
		cmd[0] = 0x11;
		cmd[1] = 0x02;
		cmd[2] = (len-i)>=block_size ? block_size : (len-i);
		cmd[3] = 0x00;
		write_port( obj, (char*)cmd, 4 );
		read_port( obj, tmp_buf, cmd[2]+1 );	// +1 for 0x0d terminator
		memcpy(buf+i,tmp_buf,cmd[2]);
		result &= tmp_buf[cmd[2]]==0x0d;
		//read_port( obj, buf+i, cmd[2]+1 ); 
	//	result &= read_port_ch( obj )==0x0d;	// requires ec2 ver 0x13 or newer
	}
	
	jtag_flashcon( obj, 0x00 );				//trx( obj, "\x0D\x05\x82\x08\x00\x00\x00", 7, "\x0D", 1 );
	trx( obj, "\x0B\x02\x01\x00", 4, "\x0D", 1 );	// state ctrl = halt
	trx( obj, "\x03\x02\xB6\x80", 4, "\x0D", 1 );
	trx( obj, "\x03\x02\xB2\x14", 4, "\x0D", 1 );
	return result;
}


/** Write a block of data to flash.
	This routine will erase and rewrite all affected sectors.
	@FIXME this function is doing things to the wrong memory area and sizer when trying to use scratchpad.
	\param save	TRUE causes exsisting data arround that written to be saved, FALSE dosen't save unmodified bytes within the sector
*/
BOOL jtag_write_flash_block( EC2DRV *obj, uint32_t addr,
							 uint8_t *buf, uint32_t len,
							 BOOL save, BOOL scratchpad)
{
	BOOL result = TRUE;
	
	uint16_t sector_size;
	if(scratchpad)
		sector_size = obj->dev->scratchpad_sector_size;
	else
		sector_size = obj->dev->flash_sector_size;
	
	uint32_t end_addr = addr + len - 1;
	uint32_t first_sect_addr = (addr/sector_size)*sector_size;
	uint32_t last_sect_addr = (end_addr/sector_size)*sector_size;
	
	uint16_t num_sects = (last_sect_addr/sector_size) -
						 (first_sect_addr/sector_size);
//	printf("jtag_write_flash_block    addr = 0x%05x, len=0x%05x\n",addr,len);
	// allocate buffer to hold out mirror image
	uint8_t *mirror = malloc( sector_size );
	if(!mirror)
		return FALSE;
	
	uint32_t cur_addr,cur_sect_addr;
	uint32_t cur_sect_end_addr;
	cur_addr = addr;
	uint32_t offset = 0;
	
	// for each sector...
	for( cur_sect_addr = first_sect_addr;
		cur_sect_addr <= last_sect_addr;
		cur_sect_addr += sector_size )
	{
//		printf("Modifying sector @ 0x%05x\n",cur_sect_addr);
		cur_sect_end_addr = cur_sect_addr + sector_size - 1;
		
		if(save)
		{
			// read current sector
			result &= jtag_read_flash_sector( obj, cur_sect_addr, mirror, scratchpad );
		}
		else
			memset(mirror,0xff,sector_size);
		
		// modify with new data
		// must be careful as data write might not be at start of sector
		
		for( cur_addr = cur_sect_addr; cur_addr<=cur_sect_end_addr; cur_addr++ )
		{
//			printf("cur_addr = 0x%05x, addr=0x%05x, value=0x%05x\n",cur_addr, addr,buf[offset]);
			if( (cur_addr>=addr) && (cur_addr<=end_addr) )
			{
//				printf("will write 0x%05x\n",cur_addr-cur_sect_addr);
				mirror[cur_addr-cur_sect_addr] = buf[offset++];
			}
		}
		// mirror should now contan the correct sector data
		// write out the new sector
		result &= jtag_write_flash_sector( obj, cur_sect_addr,mirror,
										   scratchpad );
	}
	free(mirror);
	return result;
}

/** Erase all user code locations in the device including the lock locations.
	This function needs to allow longer timeouts for the reply because the erase can take a while.

	\param obj	EC2DRV object to act on.
	\returns	TRUE on success, FALSE otherwise.
*/
BOOL jtag_erase_flash( EC2DRV *obj )
{
	jtag_halt(obj);
//	ec2_disconnect( obj );
//	ec2_connect( obj, obj->port );
//=== Expirimental code to erase F120 ==========================================
	jtag_flashcon( obj, 0x00 );
	trx( obj, "\x0b\x02\x01\x00",4,"\x0d",1);

	
	ec2_write_paged_sfr( obj, SFR_FLSCL, 0x80 );
	ec2_write_paged_sfr( obj, SFR_CLKSEL, 0x00 );
	
	ec2_write_paged_sfr( obj, SFR_OSCICN, 0xc0 );
	ec2_write_paged_sfr( obj, SFR_CCH0LC, 0x3e );
	ec2_write_paged_sfr( obj, SFR_CCH0CN, 0x07 );	// Cache Lock
	
	trx( obj, "\x0b\x02\x04\x00",4,"\x0d",1);
	trx( obj, "\x0d\x05\x80\x12\x02\x28\x00",7,"\x0d",1);
	trx( obj, "\x0d\x05\xc2\x07\x47\x00\x00",7,"\x0d",1);
	trx( obj, "\x0c\x02\xc3\x10",4,"\x00\xff\xff\x0d",4);
	trx( obj, "\x0c\x02\xc3\x10",4,"\x00\xff\xff\x0d",4);
	trx( obj, "\x0B\x02\x01\x00", 4, "\x0D", 1 );	
	
//		printf("Cache control\n");
	// Cache control
	if( obj->dev->has_cache )
		ec2_write_paged_sfr( obj, SFR_CCH0LC, 0x00 );	// Cache Lock
	
//		printf("Oscilator config\n");
	// oscilator config
	uint8_t cur_oscin = ec2_read_paged_sfr( obj, SFR_OSCICN,  0 );
	ec2_write_paged_sfr( obj, SFR_OSCICN, 0x80 );
	
//		printf("ClkSel\n");
	uint8_t cur_clksel = ec2_read_paged_sfr( obj, SFR_CLKSEL, 0 );
	ec2_write_paged_sfr( obj, SFR_CLKSEL, 0x00 );
	
	ec2_core_suspend(obj);
	
	///  test from read code
	/// It workes, Yes! nice and simple the same code is needed as for the read!
	//--------------------------------------------------------------------------
	JTAG_0x14( obj, 0x10, 0x00 );
	JTAG_0x16_Len2( obj, 0xc1, 0x40 );
	
	
	JTAG_unknown_op_0x15(obj);
	JTAG_0x16_Len3( obj, 0x00, 0x80, 0x90 );
	
	JTAG_unknown_op_0x15(obj);
	JTAG_0x16_Len3( obj, 0x00, 0x00, 0x00 );
	
	JTAG_unknown_op_0x15(obj);
	JTAG_0x16_Len3( obj, 0x00, 0x00, 0x00 );
	
	JTAG_0x14( obj, 0x10, 0x00 );
	JTAG_0x16_Len2( obj, 0xc1, 0x40 );
	
	JTAG_unknown_op_0x15(obj);
	JTAG_0x16_Len3( obj, 0x00, 0x80, 0x10 );
	
	JTAG_unknown_op_0x15(obj);
	JTAG_0x16_Len3( obj, 0x00, 0x80, 0xD0 );
	
	JTAG_unknown_op_0x15(obj);
	JTAG_0x16_Len3( obj, 0x00, 0x80, 0x10 );
	
	JTAG_0x14( obj, 0x10, 0x00 );
	JTAG_0x16_Len2( obj, 0xc1, 0x40 );
	
	JTAG_unknown_op_0x15(obj);
	JTAG_0x16_Len3( obj, 0x20, 0xe6, 0x16 );	
	
	JTAG_unknown_op_0x15(obj);
	JTAG_0x16_Len3( obj, 0x20, 0xe6, 0xD6 );	

	JTAG_unknown_op_0x15(obj);
	JTAG_0x16_Len3( obj, 0x20, 0xe6, 0x16 );	
	
	JTAG_0x14( obj, 0x10, 0x00 );
	JTAG_0x16_Len2( obj, 0xc1, 0x40 );
	
	JTAG_unknown_op_0x15(obj);
	JTAG_0x16_Len3( obj, 0x00, 0x80, 0x10 );
	
	JTAG_unknown_op_0x15(obj);
	JTAG_0x16_Len3( obj, 0x00, 0x80, 0xD0 );
	
	JTAG_unknown_op_0x15(obj);
	JTAG_0x16_Len3( obj, 0x00, 0x80, 0x10 );
	
//=============================================================================
		
		
		
	trx( obj,"\x0b\x02\x03\x00",4,"\x0d",1);
//		trx( obj,"\x0B\x02\x04\x00",4,"\x0D",1);	// CPU core suspend
	trx( obj,"\x0D\x05\x85\x08\x00\x00\x00",7,"\x0D",1);
	trx( obj,"\x0D\x05\x82\x08\x20\x00\x00",7,"\x0D",1);	// erase mode
		
		// we do need the following lines because some processor families like the F04x have
		// both 64K and 32K variants and no distinguishing device id, just a whole family id
	if( obj->dev->lock_type==FLT_RW_ALT )
		set_flash_addr_jtag( obj, obj->dev->lock );	// alternate lock byte families

	if( obj->dev->lock_type==FLT_RW || obj->dev->lock_type==FLT_RW_ALT )
	{
		set_flash_addr_jtag( obj, obj->dev->read_lock );
	}
	else
		set_flash_addr_jtag( obj, obj->dev->lock );

	write_port(obj,"\x0F\x01\xA5",3);
	BOOL r;
	char c;
	r = read_port_tm(obj,&c,1,5000);
	r &= c==0x0d; 
	
	trx( obj, "\x0b\x02\x02\x00",4,"\x0d",1);
	return r;
}

/** Erase a single sector of flash code or scratchpad
*/
BOOL jtag_erase_flash_sector( EC2DRV *obj, uint32_t sector_addr, BOOL scratchpad )
{
	jtag_halt(obj);
//=== Expirimental code to erase F120 ==========================================
	jtag_flashcon( obj, 0x00 );
	trx( obj, "\x0b\x02\x01\x00",4,"\x0d",1);
	
	ec2_write_paged_sfr( obj, SFR_FLSCL, 0x80 );
	ec2_write_paged_sfr( obj, SFR_CLKSEL, 0x00 );
	
	ec2_write_paged_sfr( obj, SFR_OSCICN, 0xc0 );
	ec2_write_paged_sfr( obj, SFR_CCH0LC, 0x3e );
	ec2_write_paged_sfr( obj, SFR_CCH0CN, 0x07 );	// Cache Lock
	
	trx( obj, "\x0b\x02\x04\x00",4,"\x0d",1);
	trx( obj, "\x0d\x05\x80\x12\x02\x28\x00",7,"\x0d",1);
	trx( obj, "\x0d\x05\xc2\x07\x47\x00\x00",7,"\x0d",1);
	trx( obj, "\x0c\x02\xc3\x10",4,"\x00\xff\xff\x0d",4);
	trx( obj, "\x0c\x02\xc3\x10",4,"\x00\xff\xff\x0d",4);
	trx( obj, "\x0B\x02\x01\x00", 4, "\x0D", 1 );	
	
//		printf("Cache control\n");
	// Cache control
	if( obj->dev->has_cache )
		ec2_write_paged_sfr( obj, SFR_CCH0LC, 0x00 );	// Cache Lock
	
//		printf("Oscilator config\n");
	// oscilator config
	uint8_t cur_oscin = ec2_read_paged_sfr( obj, SFR_OSCICN,  0 );
	ec2_write_paged_sfr( obj, SFR_OSCICN, 0x80 );
	
//		printf("ClkSel\n");
	uint8_t cur_clksel = ec2_read_paged_sfr( obj, SFR_CLKSEL, 0 );
	ec2_write_paged_sfr( obj, SFR_CLKSEL, 0x00 );
	
	ec2_core_suspend(obj);
	
	///  test from read code
	/// It workes, Yes! nice and simple the same code is needed as for the read!
	//--------------------------------------------------------------------------
	JTAG_0x14( obj, 0x10, 0x00 );
	JTAG_0x16_Len2( obj, 0xc1, 0x40 );
	
	
	JTAG_unknown_op_0x15(obj);
	JTAG_0x16_Len3( obj, 0x00, 0x80, 0x90 );
	
	JTAG_unknown_op_0x15(obj);
	JTAG_0x16_Len3( obj, 0x00, 0x00, 0x00 );
	
	JTAG_unknown_op_0x15(obj);
	JTAG_0x16_Len3( obj, 0x00, 0x00, 0x00 );
	
	JTAG_0x14( obj, 0x10, 0x00 );
	JTAG_0x16_Len2( obj, 0xc1, 0x40 );
	
	JTAG_unknown_op_0x15(obj);
	JTAG_0x16_Len3( obj, 0x00, 0x80, 0x10 );
	
	JTAG_unknown_op_0x15(obj);
	JTAG_0x16_Len3( obj, 0x00, 0x80, 0xD0 );
	
	JTAG_unknown_op_0x15(obj);
	JTAG_0x16_Len3( obj, 0x00, 0x80, 0x10 );
	
	JTAG_0x14( obj, 0x10, 0x00 );
	JTAG_0x16_Len2( obj, 0xc1, 0x40 );
	
	JTAG_unknown_op_0x15(obj);
	JTAG_0x16_Len3( obj, 0x20, 0xe6, 0x16 );	
	
	JTAG_unknown_op_0x15(obj);
	JTAG_0x16_Len3( obj, 0x20, 0xe6, 0xD6 );	

	JTAG_unknown_op_0x15(obj);
	JTAG_0x16_Len3( obj, 0x20, 0xe6, 0x16 );	
	
	JTAG_0x14( obj, 0x10, 0x00 );
	JTAG_0x16_Len2( obj, 0xc1, 0x40 );
	
	JTAG_unknown_op_0x15(obj);
	JTAG_0x16_Len3( obj, 0x00, 0x80, 0x10 );
	
	JTAG_unknown_op_0x15(obj);
	JTAG_0x16_Len3( obj, 0x00, 0x80, 0xD0 );
	
	JTAG_unknown_op_0x15(obj);
	JTAG_0x16_Len3( obj, 0x00, 0x80, 0x10 );
	
//=============================================================================
		
		
		
	trx( obj,"\x0b\x02\x03\x00",4,"\x0d",1);
//		trx( obj,"\x0B\x02\x04\x00",4,"\x0D",1);	// CPU core suspend
	trx( obj,"\x0D\x05\x85\x08\x00\x00\x00",7,"\x0D",1);
	
	if(scratchpad)
		jtag_flashcon(obj,0xA0);	// erase mode scratchpad
	else
		jtag_flashcon(obj,0x20);	// erase mode code
		
	if( obj->dev->lock_type==FLT_RW || obj->dev->lock_type==FLT_RW_ALT )
	{
		set_flash_addr_jtag( obj, obj->dev->read_lock );
	}
	else
		set_flash_addr_jtag( obj, obj->dev->lock );
	trx( obj,"\x0F\x01\xA5",3,"\x0D",1);		// erase sector
	//ec2_disconnect( obj );
	//ec2_connect( obj, obj->port );
	trx( obj, "\x0b\x02\x02\x00",4,"\x0d",1);
	return TRUE;
}



/** Set flash address register, Internal helper function,
	\param obj ec2 object to act on.
	\param 24 bit address to set the flash pointer to
	Note that flash preamble must be used before this can be used successfully
 */
void set_flash_addr_jtag( EC2DRV *obj, uint32_t addr )
{
	DUMP_FUNC();
	char cmd[7];
	cmd[0] = 0x0d;
	cmd[1] = 0x05;
	cmd[2] = 0x84;				// write flash address
//	cmd[3] = 0x10;				// for F020
//	cmd[3] = 0x11;				// for F120
	// @FIXME detect fproper processor here andadjust
	if(DEVICE_IN_RANGE( obj->dev->unique_id, C8051F120, C8051F133 )) {
		// all devices in the F120 series seem to use this, even though the F13x chips
		//   have only 0x1000 bytes of flash
		cmd[3] = 0x11;
	}
	else {
		if( obj->dev->flash_size > 0x10000 )
			cmd[3] = 0x11;					// number of bits 0x11 (17) for F120, 0x10(16) for others
		else
			cmd[3] = 0x10;
	}
	cmd[4] = addr & 0xFF;			// addr low
	cmd[5] = (addr >> 8) & 0xFF;	// addr middle
	cmd[6] = (addr >> 16) & 0xFF;	// addr top
	trx( obj, cmd, 7, "\x0D", 1 );
}

////////////////////////////////////////////////////////////////////////////////
// Low Level JTAG helper routines
////////////////////////////////////////////////////////////////////////////////


/** Write to the devices JTAG IR Register
	\param obj	ec2drv object to operate on
	\param reg IR register address to write to
	\param num_bits	number of bits of data to use (starting at lsb)
*/
BOOL jtag_write_IR( EC2DRV *obj, uint16_t reg, uint8_t num_bits, uint32_t data )
{
	char cmd[7];
	cmd[0] = 0x0d;
	cmd[1] = 0x05;	// num bytes
	cmd[2] = reg;
	cmd[3] = num_bits;
	cmd[4] = data & 0xff;
	cmd[5] = (data>>8) & 0xff;
	cmd[6] = (data>>16) & 0xff;
	return trx( obj, cmd, 7, "\x0d", 1 );
}

/** Unknown JTAG operation 0x14
	CMD:
		\x14\x2\x??\x??
		reply  1 byte and the 0x0d end of responce byte

	\returns the 1 byte of data returned from the hardware
*/
uint8_t JTAG_0x14( EC2DRV *obj, uint8_t a, uint8_t b )
{
	char cmd[4];
	cmd[0] = 0x14;
	cmd[1] = 0x02;
	cmd[2] = a;
	cmd[3] = b;
	write_port( obj, cmd, 4 ); 
	read_port( obj, cmd, 2 );
	return cmd[0];
}


/** An unknown opperation that occurs frequently.
	Seems to always read 4 then the terminating 0x0d
*/
uint8_t JTAG_unknown_op_0x15( EC2DRV *obj )
{
	char buf[2];
	write_port( obj, "\x15\x02\x18\x00",4);
	read_port( obj, buf, 2);
	return buf[0];
}


uint16_t JTAG_0x16_Len2( EC2DRV *obj, uint8_t a, uint8_t b )
{
	char cmd[10];
	cmd[0] = 0x16;
	cmd[1] = 0x02;
	cmd[2] = a;
	cmd[3] = b;
	write_port( obj, cmd, 4);
	read_port( obj, cmd, 3);
	return cmd[0] | (cmd[1]<<8);
}

uint32_t JTAG_0x16_Len3( EC2DRV *obj, uint8_t a, uint8_t b, uint8_t c )
{
	char cmd[10];
	cmd[0] = 0x16;
	cmd[1] = 0x03;
	cmd[2] = a;
	cmd[3] = b;
	cmd[4] = c;
	write_port( obj, cmd, 5);
	read_port( obj, cmd, 4);
	return cmd[0] | (cmd[1]<<8) | (cmd[1]<<16);
}


/** new JTAG connect function
	will be called by a common auto-detect function
	This split has been to do simplify debugging
 */
BOOL ec2_connect_jtag( EC2DRV *obj, const char *port )
{
	DUMP_FUNC()
			char buf[32];
	// adapter version
	ec2_reset( obj );
	if( obj->dbg_adaptor==EC2 )
	{
		if( !trx( obj,"\x55",1,"\x5A",1 ) )
			return FALSE;
		if( !trx( obj,"\x00\x00\x00",3,"\x03",1) )
			return FALSE;
		if( !trx( obj,"\x01\x03\x00",3,"\x00",1) )
			return FALSE;
	} 
	else if( obj->dbg_adaptor==EC3 )
	{
		if( !trx( obj,"\x00\x00\x00",3,"\x02",1) )	// get version
			return FALSE;
		if( !trx( obj,"\x01\x0c\x00",3,"\x00",1) )	// set sector
			return FALSE;
	}
	write_port( obj, "\x06\x00\x00",3);
	read_port( obj, buf, 1);
	
	printf("Debug adaptor ver = 0x%02x\n",buf[0]);
	ec2_target_reset( obj );
	obj->dev = getDeviceUnique( unique_device_id(obj), 0);
	
	DUMP_FUNC_END();
	return TRUE;
}



void jtag_read_ram( EC2DRV *obj, char *buf, int start_addr, int len )
{
	ec2_read_ram_sfr( obj, buf, start_addr, len, FALSE );	
	char tmp[4];
	write_port( obj,"\x02\x02\x24\x02",4 );
	read_port( obj, &tmp[0], 3);	// 3rd byte is 0x0d terminator
	usleep(10000);
	write_port( obj,"\x02\x02\x26\x02",4 );
	read_port( obj, &tmp[2], 3);	// 3rd byte is 0x0d terminator
	usleep(10000);
	if( start_addr<3 )
	{
		memcpy( &buf[0], &tmp[start_addr], 3-start_addr );
	}
}

void jtag_read_ram_sfr( EC2DRV *obj, char *buf, int start_addr, int len, BOOL sfr )
{
	char cmd[0x40], rbuf[0x40];
	int i;
	memset( buf, 0xff, len );	
	cmd[0] = sfr ? 0x02 : 0x06;
	cmd[1] = 0x02;
	for( i = 0; i<len; i+=0x0C )
	{
		cmd[2] = start_addr+i;
		cmd[3] = len-i >= 0x0C ? 0x0C : len-i;
		write_port( obj, cmd, 0x04 );
//		usleep(10000);	// try to prevent bad reads of RAM by letting the EC2 take a breather
		//read_port( obj, buf+i, cmd[3] );
		read_port( obj, rbuf, cmd[3]+1 );	// +1 for terminator 0x0d
		memcpy(buf+i,rbuf,cmd[3]);
	}
}



/** Write data into the micros RAM (JTAG)			<br>
	cmd  07 addr len a b							<br>
	len is 1 or 2									<br>
	addr is micros data ram location				<br>
	a  = first databyte to write					<br>
	b = second databyte to write					<br>
	
	@todo take improvments for C2 mode and apply to JTAG mode,  factor out common code
	
	\param obj			Object to act on.	
	\param buf			Buffer containing dsata to write to data ram
	\param start_addr	Address to begin writing at, 0x00 - 0xFF
	\param len 			Number of bytes to write, 0x00 - 0xFF
	
	\returns 			TRUE on success, otherwise FALSE
*/
BOOL jtag_write_ram( EC2DRV *obj, char *buf, int start_addr, int len )
{
	assert(obj->mode==JTAG);
	int i=0;
	char cmd[5], tmp[2];
	
	while( (start_addr+i)<3 && ((len-i)>=1) )
	{
		cmd[0] = 0x03;
		cmd[1] = 0x02;
		cmd[2] = 0x24+start_addr+i;
		cmd[3] = buf[i];
		trx( obj, cmd, 4, "\x0D", 1 );
			// printf("write special addr=0x%04x, data=0x%02x\n",(unsigned char)start_addr+i,(unsigned char)buf[i]);
		i++;
	}

	for( ; i<len; i+=2 )
	{
		cmd[0] = 0x07;
		cmd[1] = start_addr + i;
		int blen = len-i;
		if( blen>=2 )
		{
			cmd[2] = 0x02;		// two bytes
			cmd[3] = buf[i];
			cmd[4] = buf[i+1];
			trx( obj, cmd, 5, "\x0d", 1 );
		}
		else
		{
#if 0				
			// single byte write but ec2 only does 2 byte writes correctly.
				// we read the affected bytes and change the first to our desired value
				// then write back
			if( (start_addr + i) < 0xff )
			{
				cmd[0] = 0x07;
				cmd[1] = start_addr + i;
				cmd[2] = 0x02;			// two bytes
				ec2_read_ram( obj, &cmd[3], start_addr+i, 2 );
				cmd[3] = buf[i];		// poke in desired value
				trx( obj, cmd, 5, "\x0d", 1 );
			}
			else
			{
					// expirimental
				cmd[0] = 0x07;
				cmd[1] = start_addr + i-1;
				cmd[2] = 0x02;			// two bytes
				ec2_read_ram( obj, &cmd[3], start_addr+i-1, 2 );
				cmd[4] = buf[i];		// poke in desired value
				trx( obj, cmd, 5, "\x0d", 1 );
			}
				// FIXME seems to be broken if we want to modify the byte at 0xff
#else
			// read back, poke in byte and call write for 2 bytes
			if( (start_addr+i) == 0xFF )
			{
				// must use previous byte
				ec2_read_ram( obj, tmp, start_addr+i-1, 2 );
				tmp[1] = buf[i];	// databyte to write at 0xFF
				ec2_write_ram( obj, tmp, start_addr+i-1, 2 );
			}
			else
			{
								// use following byte
				ec2_read_ram( obj, tmp, start_addr+i, 2 );
				tmp[0] = buf[i];	// databyte to write
				ec2_write_ram( obj, tmp, start_addr+i, 2 );
			}
#endif
		}
	}
	return TRUE;
}

/** write to targets XDATA address space (JTAG mode).
	
	Preamble... trx("\x03\x02\x2D\x01",4,"\x0D",1);	<BR>
													<BR>
	Select page address:							<BR>
	trx("\x03\x02\x32\x00",4,"\x0D",1);				<BR>
	cmd: 03 02 32 addrH								<BR>
	where addrH is the top 8 bits of the address	<BR>
	cmd : 07 addrL len a b							<BR>
	addrL is low byte of address					<BR>
	len is 1 of 2									<BR>
	a is first byte to write						<BR>
	b is second byte to write						<BR>
													<BR>
	closing :										<BR>
	cmd 03 02 2D 00									<BR>

	\param obj			Object to act on.
	\param buf buffer containing data to write to XDATA
	\param start_addr address to begin writing at, 0x00 - 0xFFFF
	\param len Number of bytes to write, 0x00 - 0xFFFF
	
	\returns TRUE on success, otherwise FALSE
*/
BOOL jtag_write_xdata( EC2DRV *obj, char *buf, int start_addr, int len )
{
	int blen, page;
	char start_page	= ( start_addr >> 8 ) & 0xFF;
	char last_page	= ( (start_addr+len-1) >> 8 ) & 0xFF;
	unsigned int ofs=0;

	unsigned int pg_start_addr, pg_end_addr;	// start and end addresses within page
	assert( start_addr>=0 && start_addr<=0xFFFF && start_addr+len<=0x10000 );
	
	for( page = start_page; page<=last_page; page++ )
	{
		pg_start_addr = (page==start_page) ? start_addr&0x00FF : 0x00;	
		pg_end_addr = (page==last_page) ? (start_addr+len-1)-(page<<8) : 0xff;
		blen = pg_end_addr - pg_start_addr + 1;	
//		printf("page = 0x%02x, start = 0x%04x, end = 0x%04x, len = %i, ofs=%04x\n", page,pg_start_addr, pg_end_addr,blen,ofs);
		jtag_write_xdata_page( obj, buf+ofs, page, pg_start_addr, blen );
		ofs += blen;
	}
	return TRUE;
}


/** Read len bytes of data from the target (JTAG mode)
	starting at start_addr into buf

	T 03 02 2D 01  R 0D								<br>
	T 03 02 32 addrH								<br>
	T 06 02 addrL len								<br>
	where len <= 0x0C	(for EC2, longer for ec3)	<br>

	\param obj			Object to act on.
	\param buf			Buffer to recieve data read from XDATA
	\param start_addr	Address to begin reading from, 0x00 - 0xFFFF
	\param len			Number of bytes to read, 0x00 - 0xFFFF
*/
void jtag_read_xdata( EC2DRV *obj, char *buf, int start_addr, int len )
{
	int blen, page;
	char start_page	= ( start_addr >> 8 ) & 0xFF;
	char last_page	= ( (start_addr+len-1) >> 8 ) & 0xFF;
	unsigned int ofs=0;
	unsigned int pg_start_addr, pg_end_addr;	// start and end addresses within page
		
	assert( start_addr>=0 && start_addr<=0xFFFF && start_addr+len<=0x10000 );
	memset( buf, 0xff, len );
	for( page = start_page; page<=last_page; page++ )
	{
		pg_start_addr = (page==start_page) ? start_addr&0x00FF : 0x00;	
		pg_end_addr = (page==last_page) ? (start_addr+len-1)-(page<<8) : 0xff;
		blen = pg_end_addr - pg_start_addr + 1;	
	//		printf("page = 0x%02x, start = 0x%04x, end = 0x%04x, len = %i\n", page,pg_start_addr, pg_end_addr,blen);
		jtag_read_xdata_page( obj, buf+ofs, page, pg_start_addr, blen );
		ofs += blen;
	}
}


void jtag_write_sfr( EC2DRV *obj, uint8_t value, uint8_t addr )
{
	char cmd[4];
	
	cmd[0] = 0x03;
	cmd[1] = 0x02;
	cmd[2] = addr;
	cmd[3] = value;
	trx( obj,cmd,4,"\x0D",1 );
}



/** this function performs the preamble and postamble
 */
BOOL jtag_write_xdata_page( EC2DRV *obj, char *buf, unsigned char page,
						   unsigned char start, int len )
{
	DUMP_FUNC();
	int i;
	char cmd[5];
	char extraValues[2],endByte;
	
	assert(obj->mode==JTAG);
	
	// if write length is odd, go ahead and read last byte before we even
	//  start reading, since reading data in the middle of a write causes
	//  problems
	if (len % 2) {
		ec2_read_xdata( obj, &extraValues[0], (page<<8)+len+start-1, 2 );
		endByte = extraValues[1];
	}
	
	// start writing to XDATA now
	// write preamble
	if(DEVICE_IN_RANGE( obj->dev->unique_id, C8051F020, C8051F023 ))
		trx(obj,"\x03\x02\x2D\x01",4,"\x0D",1);
	else
		trx(obj,"\x03\x02\x2E\x01",4,"\x0D",1);
	
	// select page
	cmd[0] = 0x03;
	cmd[1] = 0x02;

	if(DEVICE_IN_RANGE( obj->dev->unique_id, C8051F020, C8051F023 ))
		cmd[2] = 0x32;
	else
		cmd[2] = 0x31;

	cmd[3] = page;
	trx( obj, (char*)cmd, 4, "\x0D", 1 );
	
	// set constant values
	cmd[0] = 0x07;
	cmd[2] = 2;	// length
	for( i=0; i<len; i+=2 )
	{
		// write position
		cmd[1] = i+start;
		// write new values
		cmd[3] = (char)buf[i];
		cmd[4] = (char)buf[i+1];
		// if it's the last byte, and it's odd, fill in the old
		//  read value for the last byte to even it out
		if( (len-i) <= 1 ) {
			cmd[4] = endByte;
		}
		trx( obj, (char*)cmd, 5, "\x0d", 1 );
	}
	//write postamble
	if(DEVICE_IN_RANGE( obj->dev->unique_id, C8051F020, C8051F023 ))
		trx(obj,"\x03\x02\x2D\x00",4,"\x0D",1);
	else
		trx(obj,"\x03\x02\x2E\x00",4,"\x0D",1);
	return TRUE;
}



void jtag_read_xdata_page( EC2DRV *obj, char *buf, unsigned char page,
						  unsigned char start, int len )
{
	DUMP_FUNC();
	unsigned int i;
	unsigned char cmd[0x0C];

	memset( buf, 0xff, len );	
	assert( (start+len) <= 0x100 );		// must be in one page only
	
	if(DEVICE_IN_RANGE( obj->dev->unique_id, C8051F020, C8051F023 ))
		trx( obj, "\x03\x02\x2D\x01", 4, "\x0D", 1 );
	else
		trx( obj, "\x03\x02\x2E\x01", 4, "\x0D", 1 );

	// select page
	cmd[0] = 0x03;
	cmd[1] = 0x02;
	
	if(DEVICE_IN_RANGE( obj->dev->unique_id, C8051F020, C8051F023 ))
		cmd[2] = 0x32;	// 31 for F120, 32 for F020
	else
		cmd[2] = 0x31;	// 31 for F120, 32 for F020

	cmd[3] = page;
	trx( obj, (char*)cmd, 4, "\x0D", 1 );
	cmd[0] = 0x06;
	cmd[1] = 0x02;
	/// @FIXME shoulden't we begin reading at the desired location
	//	 within the page?
	// read the rest
	for( i=0; i<len; i+=0x3C )
	{
		cmd[2] = (start+i) & 0xFF;
		cmd[3] = (len-i)>=0x3C ? 0x3C : (len-i);
		write_port( obj, (char*)cmd, 4 );
		read_port( obj, buf, cmd[3]+1 );	// +1 for 0x0d terminator
		buf += cmd[3];
	}
	// close out XDATA read process...bug fix for bug #2024032
	if(DEVICE_IN_RANGE( obj->dev->unique_id, C8051F020, C8051F023 ))
		trx( obj, "\x03\x02\x2D\x00", 4, "\x0D", 1 );
	else
		trx( obj, "\x03\x02\x2E\x00", 4, "\x0D", 1 );
}


/** Flash write routine for JTAG mode.
	This version should work with both the F020 and F120
	F020 has 512 byte sectors and the F120 has 1024K sectors

	\param obj	ec2drv object to act on
	\param buf	pointer to buffer containing data to write
	\param start_addr	Address in flash to start writing at
	\param len			number of bytes to write
	\returns 			TRUE on success, FALSE on failure
 */
BOOL jtag_write_flash( EC2DRV *obj, uint8_t *buf, uint32_t start_addr, uint32_t len )
{
	DUMP_FUNC();
//	if(!check_flash_range( obj, start_addr, len )) return FALSE;
	return jtag_write_flash_block( obj, start_addr, buf, len, TRUE, FALSE);
}





/** Start the target running. (JTAG)

	\param obj			Object to act on.
	\returns			TRUE on success, otherwise FALSE
 */
BOOL jtag_target_go( EC2DRV *obj )
{
	if( !trx( obj, "\x0b\x02\x00\x00", 4, "\x0d", 1 ) )
		return FALSE;
	if( !trx( obj, "\x09\x00", 2, "\x0d", 1 ) )
		return FALSE;
	return TRUE;
}


/** Request the target to halt (JTAG).
	This function does not wait for the device to actually halt.
	Call jtag_target_halt_poll( EC2DRV *obj ) until it returns true or
	you timeout.
	
	\param obj			Object to act on.
	\returns			TRUE if command acknowledged, false otherwise
 */
BOOL jtag_target_halt( EC2DRV *obj )
{
	// trx( obj, "\x0B\x02\x02\x00",4,"\x0D",1); // system reset??? is this the right place.  won''t this break debugging modes (run/stop since a reset is bad. test
	// the above should only occur when halt is used as part of an init sequence.
	return trx( obj, "\x0B\x02\x01\x00", 4, "\x0d", 1 );
}


/** Poll the target to determine if the processor has halted.
	The halt may be caused by a breakpoint or the c2_target_halt() command.
	
	For run to breakpoint it is necessary to call this function regularly to
	determine when the processor has actually come accross a breakpoint and
	stopped.
	
	Recommended polling rate every 250ms.

	\returns	TRUE if processor has halted, FALSE otherwise
 */
BOOL jtag_target_halt_poll( EC2DRV *obj )
{
	write_port( obj, "\x13\x00", 2 );
	return read_port_ch( obj )==0x01;	// 01h = stopped, 00h still running
}



/** Rest the target processor (JTAG).
	This reset is a cut down form of the one used by the IDE which seems to 
	read 2 64byte blocks from flash as well.
	\todo investigate if the additional reads are necessary

	\param obj			Object to act on.
*/
BOOL jtag_target_reset( EC2DRV *obj )
{
	BOOL r = TRUE;
	jtag_connect_target (obj);
	//r &= trx( obj, "\x04", 1, "\x0D", 1 );
	
	r &= trx( obj, "\x1A\x06\x00\x00\x00\x00\x00\x00", 8, "\x0D", 1 );
	r &= trx( obj, "\x0B\x02\x02\x00", 4, "\x0D", 1 );	// sys reset
	r &= trx( obj, "\x14\x02\x10\x00", 4, "\x04", 1 );
	r &= trx( obj, "\x16\x02\x01\x20", 4, "\x01\x00", 2 );
	r &= trx( obj, "\x14\x02\x10\x00", 4, "\x04", 1 );
	r &= trx( obj, "\x16\x02\x81\x20", 4, "\x01\x00", 2 );
	r &= trx( obj, "\x14\x02\x10\x00", 4, "\x04", 1 );
	r &= trx( obj, "\x16\x02\x81\x30", 4, "\x01\x00", 2 );
	r &= trx( obj, "\x15\x02\x08\x00", 4, "\x04", 1 );
	r &= trx( obj, "\x16\x01\xE0", 3, "\x00", 1 );
	if (ec2_target_halt (obj))
		r &= trx( obj, "\x03\x02\x00\x00", 4, "\x0D", 1 );
	else
		r = FALSE;
	return r;
}


/** Suspend the target core (JTAG)
	\param obj			Object to act on.
*/
void jtag_core_suspend( EC2DRV *obj )
{
	assert( obj->mode==JTAG );
	trx( obj,"\x0b\x02\x04\x00",4,"\x0d",1 );
}


////////////////////////////////////////////////////////////////////////////////
// Breakpoint support
////////////////////////////////////////////////////////////////////////////////


BOOL jtag_addBreakpoint( EC2DRV *obj, uint8_t bp, uint32_t addr )
{
	// printf("Adding breakpoint using jtag mode\n");
	// set address
	char cmd[7];
	obj->bpaddr[bp] = addr;
	cmd[0] = 0x0D;
	cmd[1] = 0x05;
	cmd[2] = 0x90+bp;	// Breakpoint address register to write
	if(DEVICE_IN_RANGE( obj->dev->unique_id, C8051F120, C8051F133 )) {
		// all devices in the F120 series seem to use this, even though the F13x chips
		//   have only 0x1000 bytes of flash
		cmd[3] = 0x11;		// 17 bits of address
	}
	else {
		if(obj->dev->flash_size<=0x10000)
			cmd[3] = 0x10;		// 16 bits of address
		else
			cmd[3] = 0x11;		// 17 bits of address		
	}
	cmd[4] = addr & 0xFF;
	cmd[5] = (addr>>8) & 0xFF;
	cmd[6] = (addr>>16) & 0xff;
	if( !trx( obj, cmd, 7, "\x0D", 1 ) )
		return FALSE;
	return TRUE;
}


/** Cause changed to the bpmask to take effect JTAG.
	For JTAG this actually re-writes the mask into the device using JTAG.
 */
BOOL jtag_update_bp_enable_mask( EC2DRV *obj )
{
	char cmd[7];
	
	cmd[0] = 0x0D;
	cmd[1] = 0x05;
	cmd[2] = 0x86;
	cmd[3] = 0x10;
	cmd[4] = obj->bp_flags;
	cmd[5] = 0x00;
	cmd[6] = 0x00;
	if( trx( obj, cmd, 7, "\x0D", 1 ) )		// inform EC2
	{
//		dump_bp(obj);
		return TRUE;
	}
	else
		return FALSE;
}
