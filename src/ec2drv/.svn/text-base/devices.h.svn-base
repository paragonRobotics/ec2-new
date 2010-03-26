#ifndef EC2_DEVICES
#define EC2_DEVICES
#include <stdint.h>
//#include "ec2drv.h"
#include "ec2types.h"



typedef struct
{
	char		name[32];
	uint8_t 	id;			// family
	uint8_t 	unique_id;	// unique id, 1:1 map with chip name
	uint8_t 	rev;		// -1 is any matching device id
	uint32_t	flash_size;
	uint16_t	flash_sector_size;
	uint16_t	internal_xram_size;
	BOOL		has_external_bus;
	BOOL		tested;		// TRUE if ec2dev developers are happy the device support has been tested and is complete
	FLASH_LOCK_TYPE	lock_type;
	uint32_t	read_lock;
	uint32_t	write_lock;
	uint16_t	lock;			// single
	int32_t		flash_reserved_bottom;	// -1 = invalid
	int32_t		flash_reserved_top;	// -1 = invalid
	BOOL		has_scratchpad;
	int32_t		scratchpad_start;	// -1 = invalid
	int32_t		scratchpad_len;		// -1 = invalid
	uint32_t	scratchpad_sector_size;
	BOOL		has_paged_sfr;
	uint8_t		sfr_page_reg;
	int16_t		USB_FIFO_size;
	BOOL		has_cache;
	EC2_MODE	mode;	
	
	// C2 Specific values, fill with 0 for JTAG devices
	// C2 Uses undocumented SFR registers for breakpoint control
	// High byte registers also have the active bit
#if 0
	const uint8_t SFR_BP_L[4];		// SFR low addr part of Breakpoint
	const uint8_t SFR_BP_H[4];		// SFR high addr part of bp + active bit
#else
	// new version, each entry is a SFRREG
	const SFRREG SFR_BP_L[4];		// SFR low addr part of Breakpoint
	const SFRREG SFR_BP_H[4];		// SFR high addr part of bp + active bit
#endif
} DEVICE;

DEVICE *getDevice( uint8_t id, uint8_t rev );
DEVICE *getDeviceUnique( uint8_t unique_id, uint8_t rev );


#define DEVICE_IN_RANGE( uid, start_uid, end_uid )	\
	(((uid)>=(start_uid)) && ((uid)<=(end_uid)))


#include "device_enum.h"

#endif
