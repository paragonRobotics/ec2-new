/** EC2DRV Driver Library
  *
  *
  *   Copyright (C) 2005 by Ricky White
  *   rickyw@neatstuff.co.nz
  *
  *   This program is free software; you can redistribute it and/or modify
  *   it under the terms of the GNU General Public License as published by
  *   the Free Software Foundation; either version 2 of the License, or
  *   (at your option) any later version.
  *
  *   This program is distributed in the hope that it will be useful,
  *   but WITHOUT ANY WARRANTY; without even the implied warranty of
  *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  *   GNU General Public License for more details.
  *
  *   You should have received a copy of the GNU General Public License
  *   along with this program; if not, write to the
  *   Free Software Foundation, Inc.,
  *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
  */
#ifndef EC2_H
#define EC2_H
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
#include "ec2types.h"
	
//#define FUNC_TRACE
#ifdef FUNC_TRACE
#define DUMP_FUNC()		printf("Function = %s\n",__PRETTY_FUNCTION__ );
#define DUMP_FUNC_END()	printf("End Function = %s\n",__PRETTY_FUNCTION__ );
#else
#define DUMP_FUNC()
#define DUMP_FUNC_END()
#endif	

#include "devices.h"

/**	Object for an EC2.
	Create one of these for every EC you wish to use
*/
typedef struct
{
	// public settings
	EC2_MODE		mode;				///< Communication method used to communicate with the target chip.
	DEVICE		*dev;
	enum  { EC2, EC3 }	dbg_adaptor;	/// set before calling connect so choose your adaptor type
	BOOL			debug;				///< true to enable debugging on an object, false otherwise
	uint8_t			progress;			///< % complete, check from an alternative thread or use callback
	void (*progress_cbk)(uint8_t percent);	///< called on significant progress update interval
	char 			port[255];			///< Holds a copy of the port used to communicate with the debugger
	
	BOOL connected;					///< True when actually connected
	// private settings
	uint8_t			boot_ver;
	int				fd;				///< file descriptor for com port
	uint8_t			bp_flags;		///< mirror of EC2 breakpoint byte
	uint32_t		bpaddr[4];		///< breakpoint addresses
	struct usb_dev_handle	*ec3;	
	DBG_ADAPTER_INFO	*dbg_info;	///< Information about the partucular debugger, flags etc
} EC2DRV;



uint16_t ec2drv_version();
BOOL ec2_connect( EC2DRV *obj, const char *port );
BOOL ec2_connect_fw_update( EC2DRV *obj, char *port );
void ec2_disconnect( EC2DRV *obj );
void ec2_reset( EC2DRV *obj );
void ec2_read_sfr( EC2DRV *obj, char *buf, uint8_t addr );
void ec2_write_sfr( EC2DRV *obj, uint8_t value, uint8_t addr );
void ec2_read_ram( EC2DRV *obj, char *buf, int start_addr, int len );
void ec2_read_ram_sfr( EC2DRV *obj, char *buf, int start_addr, int len, BOOL sfr );
BOOL ec2_write_ram( EC2DRV *obj, char *buf, int start_addr, int len );
BOOL ec2_write_xdata( EC2DRV *obj, char *buf, int start_addr, int len );
void ec2_read_xdata( EC2DRV *obj, char *buf, int start_addr, int len );

BOOL ec2_read_flash( EC2DRV *obj, uint8_t *buf, uint32_t start_addr, int len );
BOOL ec2_read_flash_scratchpad( EC2DRV *obj, uint8_t *buf, uint32_t start_addr, int len );

BOOL ec2_write_flash( EC2DRV *obj, uint8_t *buf, uint32_t start_addr, int len );
BOOL ec2_write_flash_auto_erase( EC2DRV *obj, uint8_t *buf, uint32_t start_addr, int len );
BOOL ec2_write_flash_auto_keep( EC2DRV *obj, uint8_t *buf, uint32_t start_addr, int len );
BOOL ec2_write_flash_scratchpad( EC2DRV *obj, uint8_t *buf, uint32_t start_addr, int len );
BOOL ec2_write_flash_scratchpad_merge( EC2DRV *obj, uint8_t *buf, uint32_t start_addr, int len );
BOOL ec2_erase_flash_scratchpad( EC2DRV *obj );
BOOL ec2_erase_flash_scratchpad_sector( EC2DRV *obj, uint32_t sector_addr );
void ec2_erase_flash_sector( EC2DRV *obj, uint32_t sector_addr );
void ec2_erase_flash( EC2DRV *obj );
BOOL ec2_target_go( EC2DRV *obj );
uint16_t ec2_target_run_bp( EC2DRV *obj, BOOL *bRunning );
BOOL ec2_target_halt( EC2DRV *obj );
void ec2_target_halt_no_wait( EC2DRV *obj );
BOOL ec2_target_halt_poll( EC2DRV *obj );

BOOL ec2_target_reset( EC2DRV *obj );
uint16_t ec2_step( EC2DRV *obj );
void read_active_regs( EC2DRV *obj, char *buf );
uint16_t ec2_read_pc( EC2DRV *obj );
void ec2_set_pc( EC2DRV *obj, uint16_t addr );
BOOL ec2_addBreakpoint( EC2DRV *obj, uint32_t addr );
BOOL ec2_removeBreakpoint( EC2DRV *obj, uint32_t addr );
void ec2_clear_all_bp( EC2DRV *obj );
//BOOL ec2_write_firmware( EC2DRV *obj, char *image, uint16_t len);
BOOL ec2_write_firmware( EC2DRV *obj, char *image, uint16_t len,
						 BOOL do_xor,
						 char *blockmap );

DBG_ADAPTER_INFO *ec2_GetDbgInfo( uint16_t usb_vendor_id,
								  uint16_t usb_product_id );

uint8_t flash_lock_byte( EC2DRV *obj );
uint8_t flash_read_lock( EC2DRV *obj );
uint8_t flash_write_erase_lock( EC2DRV *obj );
uint16_t unique_device_id( EC2DRV *obj );

BOOL isBPSet( EC2DRV *obj, int bpid );

// Paged SFR support
uint8_t ec2_read_paged_sfr(EC2DRV *obj, SFRREG sfr_reg, BOOL *ok );
BOOL ec2_write_paged_sfr(EC2DRV *obj, SFRREG sfr_reg, uint8_t value);
uint8_t ec2_read_raw_sfr(EC2DRV *obj, uint8_t addr, BOOL *ok );
BOOL ec2_write_raw_sfr(EC2DRV *obj, uint8_t addr, uint8_t value );



// temporary exports until the jtag split is finalised
BOOL trx( EC2DRV *obj, char *txbuf, int txlen, char *rxexpect, int rxlen );
BOOL write_port_ch( EC2DRV *obj, char ch );
BOOL write_port( EC2DRV *obj, char *buf, int len );
int read_port_ch( EC2DRV *obj );
BOOL read_port( EC2DRV *obj, char *buf, int len );
BOOL read_port_tm( EC2DRV *obj, char *buf, int len, uint32_t ms );

void set_flash_addr_jtag( EC2DRV *obj, uint32_t addr );
void ec2_core_suspend( EC2DRV *obj );
#ifdef __cplusplus
}
#endif

#endif
