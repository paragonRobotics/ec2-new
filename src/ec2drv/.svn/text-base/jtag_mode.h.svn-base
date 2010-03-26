/**	Routines to provide support for the JTAG mode.
	This is ued to communicate with processors like the
	C8051F020 and C8051F120 etc

	(C) Ricky White 2006
*/
#ifndef JTAG_MODE_H
#define JTAG_MODE_H
#include <stdint.h>
#include "ec2drv.h"


#define JTAG_IR_EXTEST			0x0000
#define JTAG_IR_SAMPLE_PRELOAD	0x0002
#define JTAG_IR_IDCODE			0x0004
#define JTAG_IR_UNKNOWN_1		0x0080
#define JTAG_IR_FLASH_CTRL		0x0082
#define JTAG_IR_FLASH_DATA		0x0083
#define JTAG_IR_FLASH_ADDR		0x0084
#define JTAG_IR_FLASH_SCALE		0x0085
#define JTAG_IR_BREAKPOINT_0	0x0090
#define JTAG_IR_BREAKPOINT_1	0x0091
#define JTAG_IR_BREAKPOINT_2	0x0092
#define JTAG_IR_BREAKPOINT_3	0x0093
#define JTAG_IR_BYPASS			0xFFFF

void jtag_connect_target( EC2DRV *obj );
uint16_t jtag_device_id( EC2DRV *obj );
uint16_t jtag_unique_device_id( EC2DRV *obj );

BOOL jtag_read_flash_sector( EC2DRV *obj, uint32_t sect_addr, uint8_t *buf,
							  BOOL scratchpad );
BOOL jtag_write_flash_sector( EC2DRV *obj, uint32_t sect_addr, uint8_t *buf,
							  BOOL scratchpad );
BOOL jtag_read_flash( EC2DRV *obj, uint8_t *buf,
						  uint32_t start_addr, int len, BOOL scratchpad );
BOOL jtag_write_flash_block( EC2DRV *obj, uint32_t addr,
							 uint8_t *buf, uint32_t len,
							 BOOL save, BOOL scratchpad);
BOOL jtag_erase_flash( EC2DRV *obj );
BOOL jtag_erase_flash_sector( EC2DRV *obj, uint32_t sector_addr,
							  BOOL scratchpad );
void set_flash_addr_jtag( EC2DRV *obj, uint32_t addr );

BOOL ec2_connect_jtag( EC2DRV *obj, const char *port );

void jtag_read_ram( EC2DRV *obj, char *buf, int start_addr, int len );
void jtag_read_ram_sfr( EC2DRV *obj, char *buf, int start_addr, int len, BOOL sfr );
BOOL jtag_write_ram( EC2DRV *obj, char *buf, int start_addr, int len );
BOOL jtag_write_xdata( EC2DRV *obj, char *buf, int start_addr, int len );
void jtag_read_xdata( EC2DRV *obj, char *buf, int start_addr, int len );
void jtag_write_sfr( EC2DRV *obj, uint8_t value, uint8_t addr );
BOOL jtag_write_flash( EC2DRV *obj, uint8_t *buf, uint32_t start_addr, uint32_t len );

BOOL jtag_target_go( EC2DRV *obj );
BOOL jtag_target_halt( EC2DRV *obj );
BOOL jtag_target_halt_poll( EC2DRV *obj );
BOOL jtag_target_reset( EC2DRV *obj );
void jtag_core_suspend( EC2DRV *obj );

////////////////////////////////////////////////////////////////////////////////
// Breakpoints
////////////////////////////////////////////////////////////////////////////////
BOOL jtag_addBreakpoint( EC2DRV *obj, uint8_t bp, uint32_t addr );
BOOL jtag_update_bp_enable_mask( EC2DRV *obj );



////////////////////////////////////////////////////////////////////////////////
// Low level JTAG helper routines
////////////////////////////////////////////////////////////////////////////////
BOOL jtag_write_IR( EC2DRV *obj, uint16_t reg, uint8_t num_bits, uint32_t data);
BOOL jtag_flashcon( EC2DRV *obj, uint8_t value);
uint8_t JTAG_0x14( EC2DRV *obj, uint8_t a, uint8_t b );
uint8_t JTAG_unknown_op_0x15( EC2DRV *obj );
uint16_t JTAG_0x16_Len2( EC2DRV *obj, uint8_t a, uint8_t b );
uint32_t JTAG_0x16_Len3( EC2DRV *obj, uint8_t a, uint8_t b, uint8_t c );

















#endif
