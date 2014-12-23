#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "c2_mode.h"

uint8_t c2_special_read( EC2DRV *obj, uint8_t sfr)
{
	int i;
	uint8_t cmd[10];
	char buf[5];
	
	cmd[0] = 0x36;
	cmd[1] = sfr & 0xff;	// low byte
	cmd[2] = 1;

	write_port( obj, (char*)cmd, 3 );
	read_port( obj, (char*)buf, 2 );	// +1 for 0x0d

	return buf[0];
}

void c2_special_write( EC2DRV *obj, uint8_t sfr, uint8_t value )
{
	int i;
	uint8_t cmd[10];
	char buf[5];

	cmd[0] = 0x37;
	cmd[1] = sfr & 0xff;	// low byte
	cmd[2] = 1;
	cmd[3] = value & 0xff;
	write_port( obj, (char*)cmd, 4 );
	read_port( obj, (char*)buf, 1 );	// +1 for 0x0d
}

/** Perform opperations necessary before any flash write or erase
	These can be slightly processor specific.

	The first need for this function came about from the C8051F52x/C8051F53x
	needing VDMLVL=1 in VDDMON set before flash writes.

	\param obj Object to act on.
*/ 
static void flash_write_pre( EC2DRV *obj )
{
	uint8_t reg_a0_save;
	BOOL ok;

	if( DEVICE_IN_RANGE( obj->dev->unique_id, C8051F530, C8051F537 ) )
	{
		SFRREG SFR_VDDMON = { 0, 0xff };
		SFRREG SFR_RSTSRC = { 0, 0xef };

		reg_a0_save = ec2_read_raw_sfr( obj, 0xa0, &ok );
		ec2_write_raw_sfr( obj, 0xa0, 0x90 );
		ec2_write_paged_sfr( obj, SFR_VDDMON, 0xe0 );	// VDMLVL = 1
		ec2_write_paged_sfr( obj, SFR_RSTSRC, 0x4a );
		ec2_write_raw_sfr( obj, 0xa0, reg_a0_save ); // restore a0 = 80
	}
	else if( DEVICE_IN_RANGE( obj->dev->unique_id, C8051F340, C8051F347 ) )
	{
		char buf[5];
		
		c2_special_read (obj, 0xbf);
		c2_special_read (obj, 0x8f);
		c2_special_write (obj, 0xbf, 0x01);
		c2_special_read (obj, 0xbf);
		//c2_special_write (obj, 0x8f, 0x00);
		c2_special_write (obj, 0xbf, 0x01);
		c2_special_read (obj, 0xff);
		c2_special_write (obj, 0xff, 0xd8);
		c2_special_read (obj, 0xff);
		c2_special_read (obj, 0xbf);
		c2_special_write (obj, 0xbf, 0x01);
		c2_special_read (obj, 0xa0);
		c2_special_write (obj, 0xa0, 0x90);
		ec2_read_raw_sfr( obj, 0xbf, &ok );
		ec2_read_raw_sfr( obj, 0xef, &ok );
		c2_special_write (obj, 0xa0, 0x80);
		c2_special_write (obj, 0xbf, 0x01);
	}
	else if(( DEVICE_IN_RANGE( obj->dev->unique_id, C8051F920, C8051F921 ))||
		( DEVICE_IN_RANGE( obj->dev->unique_id, C8051F930, C8051F931 ))||
		( DEVICE_IN_RANGE( obj->dev->unique_id, SI1000, SI1031)))
	{
		char buf[5];
		
		c2_special_write (obj, 0xef, 0x48);
		c2_special_write (obj, 0xff, 0x60);
		c2_special_read (obj, 0xa7);
		//c2_special_write (obj, 0x8f, 0x00);
		c2_special_read (obj, 0xff);
		c2_special_read (obj, 0xa0);
		c2_special_write (obj, 0xa0, 0x90);
		ec2_read_raw_sfr( obj, 0xa7, &ok );
		ec2_read_raw_sfr( obj, 0xef, &ok );
		c2_special_write (obj, 0xa0, 0x80);
		c2_special_write (obj, 0xff, 0xe0);
		c2_special_write (obj, 0xef, 0x4a);
	}
}


/** Perform operations necessary after any flash write or erase
	These can be slightly processor specific.

	The first need for this function came about from the C8051F52x/C8051F53x
	needing VDMLVL=0 in VDDMON after flash writes.

	\param obj Object to act on.
*/ 
static void flash_write_post( EC2DRV *obj )
{
	if( DEVICE_IN_RANGE( obj->dev->unique_id, C8051F530, C8051F537 ) )
	{
		SFRREG SFR_VDDMON = { 0, 0xff };
		SFRREG SFR_RSTSRC = { 0, 0xef };
		ec2_write_paged_sfr( obj, SFR_VDDMON, 0xc0 );	// VDMLVL = 0
	}
	else if( DEVICE_IN_RANGE( obj->dev->unique_id, C8051F340, C8051F347 ) )
	{

	}
}


/** Connect to a device using C2 mode.
*/
void c2_connect_target( EC2DRV *obj )
{
	trx(obj,"\x20",1,"\x0d",1);
}

void c2_disconnect_target( EC2DRV *obj )
{
	trx(obj,"\x21",1,"\x0d",1);
}


uint16_t c2_device_id( EC2DRV *obj )
{
	char buf[3];
	// this appeared in new versions of IDE but seems to have no effect for F310	
	// EC2 chokes on this!!!!		trx(obj,"\xfe\x08",2,"\x0d",1);
	write_port( obj,"\x22", 1 );	// request device id (C2 mode)
	read_port( obj, buf, 3 );
	return buf[0]<<8 | buf[1];
}

uint16_t c2_unique_device_id( EC2DRV *obj )
{
	char buf[3];
	uint16_t dev_id;

//	ec2_target_halt(obj);	// halt needed otherwise device may return garbage!
	write_port(obj,"\x23",1);
	read_port(obj,buf,3);
	dev_id = buf[1];
//	print_buf( buf,3);
//	ec2_target_halt(obj);	// halt needed otherwise device may return garbage!
	
	// test code (not sure if this is actually needed
	////////////////////////////////////////////////////////////////////////////
	ec2_read_flash( obj, buf, 0x0000, 1 );
	ec2_read_flash( obj, buf, obj->dev->lock, 1 );
	
	return dev_id;
}


void c2_erase_flash( EC2DRV *obj )
{
	int i;
	DUMP_FUNC();
	// generic C2 erase entire device
	// works for EC2 and EC3
		
	// FIXME the disconnect / connect sequence dosen't work with the EC2 and C2 mode!
	if( obj->dbg_adaptor==EC3 )
	{
		ec2_disconnect( obj );
		ec2_connect( obj, obj->port );
	}
	flash_write_pre(obj);
	write_port( obj, "\x3C",1);			// Erase entire device
	read_port_ch(obj);					// 0x0d
	flash_write_post(obj);
	c2_special_write (obj, 0x8f, 0x00);
	if( obj->dbg_adaptor==EC3 )
	{
		ec2_disconnect( obj );
		ec2_connect( obj, obj->port );
	}
	DUMP_FUNC_END();
}

BOOL c2_erase_flash_sector( EC2DRV *obj, uint32_t sector_addr,
							BOOL scratchpad )
{
	DUMP_FUNC();
	BOOL r;
	char cmd[2];
	flash_write_pre(obj);
	if(( DEVICE_IN_RANGE( obj->dev->unique_id, C8051F920, C8051F921 ))||
		( DEVICE_IN_RANGE( obj->dev->unique_id, C8051F930, C8051F931 ))||
		( DEVICE_IN_RANGE( obj->dev->unique_id, SI1000, SI1031))) {
		if (scratchpad) {
			c2_special_write (obj, 0x8f, 0x04);
		}
		else {
			c2_special_write (obj, 0x8f, 0x00);
		}
	}
	cmd[0] = 0x30;		// sector erase command
	cmd[1] = sector_addr/ obj->dev->flash_sector_size;
	r =  trx( obj, cmd, 2, "\x0d", 1 );
	flash_write_post(obj);
	if(( DEVICE_IN_RANGE( obj->dev->unique_id, C8051F920, C8051F921 ))||
		( DEVICE_IN_RANGE( obj->dev->unique_id, C8051F930, C8051F931 ))||
		( DEVICE_IN_RANGE( obj->dev->unique_id, SI1000, SI1031))) {
		c2_special_write (obj, 0x8f, 0x00);
	}
	DUMP_FUNC_END();
	return r;
}

/*  C2 version of ec2_write_flash
*/
BOOL c2_write_flash( EC2DRV *obj, uint8_t *buf, uint32_t start_addr, int len, BOOL scratchpad )
{
	DUMP_FUNC();
//	if(!check_flash_range( obj, start_addr, len )) return FALSE;
	// preamble
	// ...
	// 2f connect breakdown:
	// T 2f 00 30 08 55 55 55 55 55 55 55 55		R 0d
	//    |  |  |  | +---+--+--+--+--+--+--+--- Data bytes to write
	//    |  |  |  +--------------------------- number of data bytes towrite (8 max, maxtotal cmd length 0x0c)
	//    |  |  +------------------------------ High byte of address to start write
	//    |  +--------------------------------- low byte of address
	//    +------------------------------------ write code memory command
	//
	// for some funny reason the IDE alternates between 8 byte writes and 
	// 4 byte writes, this means the total number of writes cycle is 0x0c the
	// exact same number as the JTAG mode, it looks like they economise code,
	// I this would complicate things, well just do 8 byte writes and then an
	// fragment at the end.  This will need testing throughlerly as it is
	// different to the IDE's action.
	unsigned int i, addr;
	char		 cmd[0x0c];
	BOOL ok;

	flash_write_pre(obj);
	if(( DEVICE_IN_RANGE( obj->dev->unique_id, C8051F920, C8051F921 ))||
		( DEVICE_IN_RANGE( obj->dev->unique_id, C8051F930, C8051F931 ))||
		( DEVICE_IN_RANGE( obj->dev->unique_id, SI1000, SI1031))) {
		if (scratchpad) {
			c2_special_write (obj, 0x8f, 0x04);
		}
		else {
			c2_special_write (obj, 0x8f, 0x00);
		}
	}
	cmd[0] = 0x2f;									// Write code/flash memory cmd	
	for( i=0; i<len; i+=8 )
	{
		addr = start_addr + i;
		cmd[1] = addr & 0xff;						// low byte
		cmd[2] = (addr>>8) & 0xff;					// high byte
		cmd[3] = (len-i)<8 ? (len-i) : 8;
		memcpy( &cmd[4], &buf[i], cmd[3] );
		if( !trx( obj, cmd, cmd[3]+4, "\x0d", 1 ) )
			return FALSE;							// Failure
	}

	// estore origional condition
	flash_write_post(obj);
	if(( DEVICE_IN_RANGE( obj->dev->unique_id, C8051F920, C8051F921 ))||
		( DEVICE_IN_RANGE( obj->dev->unique_id, C8051F930, C8051F931 ))||
		( DEVICE_IN_RANGE( obj->dev->unique_id, SI1000, SI1031))) {
		c2_special_write (obj, 0x8f, 0x00);
	}
	return TRUE;
}


BOOL c2_read_flash( EC2DRV *obj, uint8_t *buf, uint32_t start_addr, int len, BOOL scratchpad )
{
	int i;
	uint8_t cmd[10];
	uint32_t addr;
	
	if(( DEVICE_IN_RANGE( obj->dev->unique_id, C8051F920, C8051F921 ))||
		( DEVICE_IN_RANGE( obj->dev->unique_id, C8051F930, C8051F931 ))||
		( DEVICE_IN_RANGE( obj->dev->unique_id, SI1000, SI1031))) {
		if (scratchpad) {
			c2_special_write (obj, 0x8f, 0x04);
		}
		else {
			c2_special_write (obj, 0x8f, 0x00);
		}
	}
	// C2 mode is much simpler
	//
	// example command 0x2E 0x00 0x00 0x0C
	//				     |    |   |    |
	//					 |    |   |    +---- Length or read up to 0x0C bytes
	//					 |    |   +--------- High byte of address to start at
	//					 |    +----(len-i)--------- Low byte of address
	//					 +------------------ Flash read command
	cmd[0] = 0x2E;
	for( i=0; i<len; i+=0x0c )
	{
		addr = start_addr + i;
		cmd[1] = addr & 0xff;	// low byte
		cmd[2] = (addr >> 8) & 0xff;
		cmd[3] = (len-i) > 0x0c ? 0x0c : (len-i);
		write_port( obj, (char*)cmd, 4 );
		read_port( obj, (char*)buf+i, cmd[3]+1 );	// +1 for 0x0d
	}
	if(( DEVICE_IN_RANGE( obj->dev->unique_id, C8051F920, C8051F921 ))||
		( DEVICE_IN_RANGE( obj->dev->unique_id, C8051F930, C8051F931 ))||
		( DEVICE_IN_RANGE( obj->dev->unique_id, SI1000, SI1031))) {
		c2_special_write (obj, 0x8f, 0x00);
	}
	return TRUE;
}


/** Read from xdata memory on chips that have external memory interfaces and C2

	\param obj			Object to act on.
	\param buf			Buffer to recieve data read from XDATA
	\param start_addr	Address to begin reading from, 0x00 - 0xFFFF
	\param len			Number of bytes to read, 0x00 - 0xFFFF
	
	\returns			TRUE on success, otherwise FALSE
*/
BOOL c2_read_xdata_emif( EC2DRV *obj, char *buf, int start_addr, int len )
{
	// Command format
	//	T 3e LL HH NN
	// where
	//		LL = Low byte of address to start reading from
	//		HH = High byte of address to start reading from
	//		NN = Number of  bytes to read, max 3c for EC3, max 0C? for EC2
	assert( obj->mode==C2 );
	assert( obj->dev->has_external_bus );
	uint16_t block_len_max = obj->dbg_adaptor==EC2 ? 0x0C : 0x3C;
	// read  blocks of upto max block  len
	uint16_t addr = start_addr;
	uint16_t cnt = 0;
	uint16_t block_len;
	char cmd[4];
	while( cnt < len )
	{
		// request the block
		cmd[0] = 0x3e;					// Read EMIF
		cmd[1] = addr & 0xff;			// Low byte
		cmd[2] = (addr & 0xff00) >> 8;	// High byte
		block_len = (len-cnt)>block_len_max ? block_len_max : len-cnt;
		cmd[3] = block_len;
		write_port( obj, cmd, 4 );
		read_port( obj, buf+cnt, block_len+1 );	// +1 for 0x0d terminator ///@FIXME do we need to read one extra here for 0x0d?
		addr += block_len;
		cnt += block_len;
	}
	return TRUE;
}



/** Special version for F350 xdata reads.
	now not using pages as they seem unnecessary based on the latest data
	captures.

	\param obj			Object to act on
	\param buf			Buffer to recieve the read data.
	\param start_addr	Address in xdata to start reading from
	\param len			number of bytes to read.
	\return				TRUE is successful, false if address out of range
*/
BOOL c2_read_xdata_F350( EC2DRV *obj, char *buf, int start_addr, int len )
{
	const SFRREG LOW_ADDR_REG	= { 0x0f, 0xad };
	const SFRREG HIGH_ADDR_REG	= { 0x0f, 0xc7 };
	const uint8_t max_read_len = obj->dbg_adaptor==EC3 ? 0x3c : 0x0c;
	uint8_t sfrpage_save;
			
	// Set start address
	ec2_write_paged_sfr( obj, LOW_ADDR_REG , start_addr&0xff );
	ec2_write_paged_sfr( obj, HIGH_ADDR_REG, (start_addr >> 8)&0xff );
	
	// save SFR page register
	if( obj->dev->has_paged_sfr )
	{
		sfrpage_save = ec2_read_raw_sfr(obj, obj->dev->sfr_page_reg, 0 );
		ec2_write_raw_sfr( obj, obj->dev->sfr_page_reg, 0x0f );
	}

	// read the data
	uint16_t ofs;
	uint8_t cmd[4];
	for(ofs=0; ofs<len; ofs+=max_read_len )
	{
		cmd[0] = 0x2c;
		cmd[1] = 0x84;
		cmd[2] = 0x00;
		cmd[3] = (len-ofs)>=max_read_len ? max_read_len : (len-ofs);
		write_port( obj, (char*)cmd, 4 );
		read_port( obj, (char*)buf+ofs, cmd[3]+1 ); // for 0x0d terminator
	}

	// restore SFR page register
	if( obj->dev->has_paged_sfr )
		ec2_write_raw_sfr( obj, obj->dev->sfr_page_reg, sfrpage_save );
	
	return TRUE;
}



/** write to targets XDATA address space (C2 with internal + external xdata).
	c2 xdata write with emif.

	\param obj			Object to act on.
	\param buf			Buffer containing data to write to XDATA
	\param start_addr	Address to begin writing at, 0x00 - 0xFFFF
	\param len			Number of bytes to write, 0x00 - 0xFFFF
	
	\returns			TRUE on success, otherwise FALSE
*/
BOOL c2_write_xdata_emif( EC2DRV *obj, char *buf, int start_addr, int len )
{
	// Command format
	// data upto 3C bytes, last byte is in a second USB transmission with its own length byte
	// 3f 00 00 3c 5a
	// 3f LL HH NN
	// where
	//		LL = Low byte of address to start writing at.
	//		HH = High byte of address to start writing at.
	//		NN = Number of  bytes to write, max 3c for EC3, max 0C? for EC2	
	
	// read back result of 0x0d
	assert( obj->mode==C2 );
	assert( obj->dev->has_external_bus );	
	uint16_t block_len_max = obj->dbg_adaptor==EC2 ? 0x0C : 0x3C;
	uint16_t block_len;
	uint16_t addr = start_addr;
	uint16_t cnt = 0;
	char cmd[64];
	const char cmd_len = 4;
	BOOL ok=TRUE;
	while( cnt<len )
	{
		cmd[0] = 0x3f;	// Write EMIF
		cmd[1] = addr&0xff;
		cmd[2] = (addr&0xff00)>>8;
		block_len = (len-cnt)>block_len_max ? block_len_max : len-cnt;
		cmd[3] = block_len;
		//memcpy( &cmd[4], buf+addr, block_len );
		memcpy( &cmd[4], buf+cnt, block_len );
		if( block_len==0x3c )
		{
			// split write over 2 USB writes
			write_port( obj, cmd, 0x3f );
			write_port( obj, &cmd[cmd_len+0x3b], 1 );
			ok |= (read_port_ch( obj)=='\x0d');
		}
		else
		{
			write_port( obj, cmd, block_len + cmd_len );
			ok |= (read_port_ch( obj)=='\x0d');
		}
		addr += block_len;
		cnt += block_len;
	}
	return ok;
}


void c2_read_ram( EC2DRV *obj, char *buf, int start_addr, int len )
{
	char tmp[4];
	assert(obj->mode==C2);
	
	ec2_read_ram_sfr( obj, buf, start_addr, len, FALSE );
		/// \TODO we need to do similar to above, need to check out if there is a generic way to handle
		/// the first 2 locations,  should be since the same reason the the special case and sfr reads
		/// are already JTAG / C2 aware.
		// special case, read first 3 bytes of ram
		//T 28 24 02		R 7C 00
		//T 28 26 02		R 00 00
	write_port( obj,"\x28\x24\x02",3 );
	read_port( obj, &tmp[0], 2);
	write_port( obj,"\x28\x26\x02",3 );
	read_port( obj, &tmp[2], 2);
	if( start_addr<3 )
	{
		memcpy( &buf[0], &tmp[start_addr], 3-start_addr );
	}
}


void c2_read_ram_sfr( EC2DRV *obj, char *buf, int start_addr, int len, BOOL sfr )
{
	int i;
	uint8_t cmd[40];
	char block_len = obj->dbg_adaptor==EC2 ? 0x0c : 0x3b;
	memset( buf, 0xff, len );
	cmd[0] = sfr ? 0x28 : 0x2A;		// SFR read or RAM read
	for( i = 0; i<len; i+=block_len )
	{
		cmd[1] = start_addr+i;
		cmd[2] = len-i >= block_len ? block_len : len-i;
		write_port( obj, (char*)cmd, 3 );
		read_port( obj, buf+i, cmd[2] );
	}
}


/** Write data into the micros DATA RAM (C2).
	\param obj			Object to act on.	
	\param buf			Buffer containing dsata to write to data ram
	\param start_addr	Address to begin writing at, 0x00 - 0xFF
	\param len			Number of bytes to write, 0x00 - 0xFF
	
	\returns 			TRUE on success, otherwise FALSE
 */
BOOL c2_write_ram( EC2DRV *obj, char *buf, int start_addr, int len )
{
	// special case for first 2 bytes, related to R0 / R1 I think
	// special case the first 2 bytes of RAM
	// looks like it should be the first 3 bytes
	assert(obj->mode==C2);
	int i=0;
	char cmd[5], tmp[2];
	
	while( (start_addr+i)<=0x02 && ((len-i)>=1) )
	{
		cmd[0] = 0x29;
		cmd[1] = 0x24+start_addr+i;
		cmd[2] = 0x01;	// len
		cmd[3] = buf[i];
		trx( obj, cmd, 4, "\x0D", 1 );
		i++;
	}
		// normal writes
		// 0x2b 0x03 0x02 0x55 0x55
		//  |    |    |    |    |
		//	|    |    |    |    +--- Seconds data byte
		//	|    |    |    +-------- Seconds data byte
		//	|    |    +------------- Number of bytes, must use 2 using 1 or >2 won't work!
		//	|    +------------------ Start address
		//	+----------------------- Data ram write command
	for( ; i<len; i+=2 )
	{
		cmd[0] = 0x2b;				// write data RAM
		cmd[1] = start_addr + i;	// address
		int blen = len-i;
		if( blen>=2 )
		{
			cmd[2] = 0x02;			// two bytes
			cmd[3] = buf[i];
			cmd[4] = buf[i+1];
			trx( obj, cmd, 5, "\x0d", 1 );
		}
		else
		{
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
		}
	}
	return TRUE;
}


/** This method of writing to XDATA is probably used by more than just the F350
	Need to find out which then the name can be changed to be more generic

	\param obj			Object to act on.
	\param buf			Buffer containing data to write to xdata RAM.
	\param start_addr	Address in XDATA to begin writing at.
	\param len			Number of bytes to write.
*/
BOOL c2_write_xdata_F35x( EC2DRV *obj, char *buf, int start_addr, int len )
{
	const SFRREG LOW_ADDR_REG	= { 0x0f, 0xad };
	const SFRREG HIGH_ADDR_REG	= { 0x0f, 0xc7 };
	uint8_t sfrpage_save;
	
	// setup start address as low and high bytes
	ec2_write_paged_sfr( obj, LOW_ADDR_REG , start_addr&0xff );
	ec2_write_paged_sfr( obj, HIGH_ADDR_REG, (start_addr >> 8)&0xff );

	// save SFR page register
	if( obj->dev->has_paged_sfr )
	{
		sfrpage_save = ec2_read_raw_sfr(obj, obj->dev->sfr_page_reg, 0 );
		ec2_write_raw_sfr( obj, obj->dev->sfr_page_reg, 0x0f );
	}

	// each write block begins with 2d 84 00 LL
	// where LL is the number of bytes to write
	// the result is 0x0d on success
	// max transfer size of USB is 0x3C, probably 0x0c for EC2
	
	assert( obj->mode==C2 );
	uint16_t block_len_max = obj->dbg_adaptor==EC2 ? 0x0C : 0x3C;
	uint16_t block_len;
	uint16_t addr = start_addr;
	uint16_t cnt = 0;
	char cmd[64];
	const char cmd_len = 4;
	BOOL ok=TRUE;
	while( cnt<len )
	{
		// 2d 84 00 len + len bytes of data
		cmd[0] = 0x2d;					// Write F35x xdata
		cmd[1] = 0x84;
		cmd[2] = 0x00;
		block_len = (len-cnt)>block_len_max ? block_len_max : len-cnt;
		cmd[3] = block_len;
		memcpy( &cmd[4], buf+cnt, block_len );
		if( block_len==0x3c )
		{
			// split write over 2 USB writes
			write_port( obj, cmd, 0x3f );
			write_port( obj, &cmd[cmd_len+0x3b], 1 );
			ok |= (read_port_ch( obj)=='\x0d');
		}
		else
		{
			write_port( obj, cmd, block_len + cmd_len );
			ok |= (read_port_ch( obj)=='\x0d');
		}
		addr += block_len;
		cnt += block_len;
	}

	// restore SFR page register
	if( obj->dev->has_paged_sfr )
		ec2_write_raw_sfr( obj, obj->dev->sfr_page_reg, sfrpage_save );

	return ok;
}


/** write to targets XDATA address space (C2 with only internal xdata).
	Normal c2 xdata write without emif.

	\param obj			Object to act on.
	\param buf			Buffer containing data to write to XDATA
	\param start_addr	Address to begin writing at, 0x00 - 0xFFFF
	\param len			Number of bytes to write, 0x00 - 0xFFFF
	
	\returns			TRUE on success, otherwise FALSE
*/
BOOL c2_write_xdata( EC2DRV *obj, char *buf, int start_addr, int len )
{
	assert( obj->mode==C2);
	if( obj->dev->has_external_bus )
		return c2_write_xdata_emif( obj, buf, start_addr, len );
	
	if( DEVICE_IN_RANGE( obj->dev->unique_id, C8051F350, C8051F353 ) &&
	  	obj->dbg_adaptor==EC3 )
		return c2_write_xdata_F35x( obj, buf, start_addr, len );
	
	const SFRREG LOW_ADDR_REG	= { 0x0f, 0xad };
	const SFRREG HIGH_ADDR_REG	= { 0x0f, 0xc7 };
	const SFRREG WRITE_BYTE_REG	= { 0x0f, 0x84 };
	
	// T 29 ad 01 00	R 0d
	// T 29 c7 01 00	R 0d
	// T 29 84 01 55	R 0d	// write 1 byte (0x55) at the curr addr then inc that addr
	char cmd[4];
	unsigned int i;
	// EC2  seems to use this method rather the the bulk method used by the EC3.
	// even for the same micro like the F350
	// start addres ad low and high bytes
	ec2_write_paged_sfr( obj, LOW_ADDR_REG , start_addr&0xff );
	ec2_write_paged_sfr( obj, HIGH_ADDR_REG, (start_addr >> 8)&0xff );
	
	// setup write command
	for(i=0; i<len; i++)
	{
		if( !ec2_write_paged_sfr(obj,WRITE_BYTE_REG,buf[i]) )
			return FALSE;
	}
	return TRUE;
}


/** Read len bytes of data from the target (C2)
	8051F310
	8051F350???
	althernate function called from within for EMIF based devices.

	The newer SILABS code for the F35x seems to read large blocks over USB
	rather than byte by byte

	\param obj			Object to act on.
	\param buf			Buffer to recieve data read from XDATA
	\param start_addr	Address to begin reading from, 0x00 - 0xFFFF
	\param len			Number of bytes to read, 0x00 - 0xFFFF
	
	\returns			TRUE on success, otherwise FALSE
*/
BOOL c2_read_xdata( EC2DRV *obj, char *buf, int start_addr, int len )
{
	// C2 dosen't seem to need any paging like jtag mode does
	char cmd[4];
	unsigned int i;

	if( obj->dev->has_external_bus )
		return c2_read_xdata_emif( obj, buf, start_addr, len );
	
	if( DEVICE_IN_RANGE( obj->dev->unique_id, C8051F350, C8051F353 ) &&
		obj->dbg_adaptor==EC3 )
	{
		return c2_read_xdata_F350( obj, buf, start_addr, len );
	}
	
	// code below is for C2 devices that don't have externam memory.
	// T 29 ad 01 10	R 0d	.// low byte of address 10 ( last byte of cmd)
	// T 29 c7 01 01	R 0d	//  high byte of address 01 ( last byte of cmd)
	// T 28 84 01		R 00	// read next byte (once for every byte to be read)

	
	const SFRREG addr_low_sfr = { 0x0f, 0xad };
	const SFRREG addr_high_sfr = { 0x0f, 0xc7 };
	const SFRREG read_ram_sfr = { 0x0f, 0x84 };
	// low byte of start address
	ec2_write_paged_sfr( obj, addr_low_sfr, start_addr&0xff );
	// high byte of start address
	ec2_write_paged_sfr( obj, addr_high_sfr, (start_addr>>8)&0xff );

	// setup read command
	for(i=0; i<len; i++)
	{
		buf[i] = ec2_read_paged_sfr( obj, read_ram_sfr,0 );
	}
	
	return TRUE;
}



void c2_write_sfr( EC2DRV *obj, uint8_t value, uint8_t addr )
{
	char cmd[4];

	cmd[0] = 0x29;
	cmd[1] = addr;
	cmd[2] = 0x01;
	cmd[3] = value;
	trx( obj,cmd,4,"\x0D",1 );
}




////////////////////////////////////////////////////////////////////////////////
// Target state control
////////////////////////////////////////////////////////////////////////////////

/** Start the target running. (C2)

	\param obj			Object to act on.
	\returns			TRUE on success, otherwise FALSE
*/
BOOL c2_target_go( EC2DRV *obj )
{
	assert(obj->mode==C2);
	if( !trx( obj, "\x24", 1, "\x0d", 1 ) )		// run
		return FALSE;
	if( !trx( obj, "\x27", 1, "\x00", 1 ) )		// indicates running
		return FALSE;
	return TRUE;
}


/** Request the target to halt (C2).
	This function does not wait for the device to catually halt.
	Call c2_target_halt_poll( EC2DRV *obj ) until it returns true or
	you timeout.
	
	\param obj			Object to act on.
	\returns			TRUE if command acknowledged, false otherwise
*/
BOOL c2_target_halt( EC2DRV *obj )
{
	return trx( obj, "\x25", 1, "\x0d", 1 );
}


/** Poll the target to determine if the processor has halted.
	The halt may be caused by a breakpoint or the c2_target_halt() command.
	
	For run to breakpoint it is necessary to call this function regularly to
	determine when the processor has actually come accross a breakpoint and
	stopped.
	
	Recommended polling rate every 250ms.

	\param obj			Object to act on.
	\returns			TRUE if processor has halted, FALSE otherwise
*/
BOOL c2_target_halt_poll( EC2DRV *obj )
{
	char buf[2];
	write_port( obj, "\x27", 1 );
	//write_port( obj, "\x27\x00", 2 );
	read_port( obj, buf, 2 );
	return buf[0]==0x01;	// 01h = stopped, 00h still running
}



/** Rest the target processor (C2).
	This reset is a cut down form of the one used by the IDE which seems to 
	read 2 64byte blocks from flash as well.
	\todo investigate if the additional reads are necessary

	\param obj			Object to act on.
*/
BOOL c2_target_reset( EC2DRV *obj )
{
	DUMP_FUNC();
	/// @FIXME put correct C2 target reset code here.
//	Dosen't look like this is needed
//	trx( obj, "\x2a\x00\x03\x20", 4, "\x0d", 1 );
//	trx( obj, "\x29\x24\x01\x00", 4, "\x0d", 1 );
//	trx( obj, "\x29\x25\x01\x00", 4, "\x0d", 1 );
//	trx( obj, "\x29\x26\x01\x3d", 4, "\x0d", 1 );
//	trx( obj, "\x28\x20\x02", 3, "\x00\x00", 2 );
//	trx( obj, "\x2a\x00\x03", 3, "\x03\x01\x00", 3 );
//	trx( obj, "\x28\x24\x02", 3, "\x00\x00", 2 );
//	trx( obj, "\x28\x26\x02", 3, "\x3d\x00", 2 );
	//
	//hmm C2 device reset seems wrong.
		// new expirimental code
//		r &= trx( obj, "\x2E\x00\x00\x01",4,"\x02\x0D",2);
//		r &= trx( obj, "\x2E\xFF\x3D\x01",4,"\xFF",1);
	ec2_set_pc(obj,0x0000);
	DUMP_FUNC_END();
	return TRUE;
}


/** Suspend the target core (C2)
	\param obj			Object to act on.
*/
void c2_core_suspend( EC2DRV *obj )
{
	assert(obj->mode==C2);
	trx( obj,"\x25",1,"\x0d",1 );
}




////////////////////////////////////////////////////////////////////////////////
// Breakpoints
////////////////////////////////////////////////////////////////////////////////
static BOOL ec2_connect_jtag( EC2DRV *obj, const char *port );

BOOL c2_addBreakpoint( EC2DRV *obj, uint8_t bp, uint32_t addr )
{
	printf("C2: Adding breakpoint into position %i, addr=0x%08x\n",bp,addr);
	assert(obj->mode==C2);
	assert(bp<4);
	
	obj->bpaddr[bp] = addr;
	return TRUE;
}

/** Cause changed to the bpmask to take effect
*/
BOOL c2_update_bp_enable_mask( EC2DRV *obj )
{
	c2_write_breakpoints(obj);
			return TRUE;
}

/** Write breakpoints to a C2 device.
	The currently active breakpoints to be written to the device after any 
	change because C2 mode dosen't store the breakpoints.
	@TODO make this an internal function only
	\param obj			Object to act on.
 */
void c2_write_breakpoints( EC2DRV *obj )
{
	DUMP_FUNC();
	char cmd[4];
	int i;

//	for(i=0; i<4;i++)
//	{
//		printf("BP %i Low = 0x%02x\n",i,obj->dev->SFR_BP_L[i]);
//		printf("BP %i High = 0x%02x\n",i,obj->dev->SFR_BP_H[i]);
//	}
	
	if( DEVICE_IN_RANGE( obj->dev->unique_id, C8051F340, C8051F347 ))
	{
		const SFRREG BP_ACTIVE_REG	= { 0x01, 0xe5 };
		uint8_t active_bitmap = 0x00;

		// disable all breakpoints
		ec2_write_paged_sfr(obj, BP_ACTIVE_REG, 0x00);

		for( i=0; i<4; i++ )
		{
			uint8_t low = obj->bpaddr[i]&0xff;
			uint8_t high = (obj->bpaddr[i]>>8)&0xff;
			if( isBPSet( obj, i ) )
				active_bitmap |= 1<<i;
			ec2_write_paged_sfr( obj, obj->dev->SFR_BP_L[i], low );
			ec2_write_paged_sfr( obj, obj->dev->SFR_BP_H[i], high );
			ec2_write_paged_sfr( obj, BP_ACTIVE_REG, active_bitmap );
		}
	}
	else if(( DEVICE_IN_RANGE( obj->dev->unique_id, C8051F920, C8051F921 ))||
		( DEVICE_IN_RANGE( obj->dev->unique_id, C8051F930, C8051F931 ))||
		( DEVICE_IN_RANGE( obj->dev->unique_id, SI1000, SI1031)))
	{
		//printf("New method");
		const SFRREG BP_ACTIVE_REG	= { 0x01, 0xf4};
		uint8_t active_bitmap = 0x00;

		// disable all breakpoints
		ec2_write_paged_sfr(obj, BP_ACTIVE_REG, 0x00);

		for( i=0; i<4; i++ )
		{
			uint8_t low = obj->bpaddr[i]&0xff;
			uint8_t high = (obj->bpaddr[i]>>8)&0xff;
			if( isBPSet( obj, i ) )
				active_bitmap |= 1<<i;
			ec2_write_paged_sfr( obj, obj->dev->SFR_BP_L[i], low );
			ec2_write_paged_sfr( obj, obj->dev->SFR_BP_H[i], high );
			//printf ("try %d\n%d\n",obj->dev->SFR_BP_L[i].page,obj->dev->SFR_BP_H[i].addr);
			ec2_write_paged_sfr( obj, BP_ACTIVE_REG, active_bitmap );
			//printf ("result %d\n",ec2_read_paged_sfr( obj, obj->dev->SFR_BP_L[i],0 ));
		}
	}
	else
	{
		// disable all breakpoints
		for(i=0; i<4;i++)
			ec2_write_paged_sfr(obj,obj->dev->SFR_BP_H[i],0x00);
		
		// the preamble ones are offset by one like below.
		// the normal breakpoints
		for( i=0; i<4; i++ )
		{
			if( isBPSet( obj, i ) )
			{
				uint8_t low = obj->bpaddr[i]&0xff;
				uint8_t high = (obj->bpaddr[i]>>8)&0xff;

				ec2_write_paged_sfr( obj, obj->dev->SFR_BP_L[i], low );
				ec2_write_paged_sfr( obj, obj->dev->SFR_BP_H[i], high | 0x80 );
				// | 80 to enable
			}
		}
	}
}

