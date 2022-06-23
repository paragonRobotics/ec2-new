#ifndef EC2_DEVICES
#define EC2_DEVICES
#include <stdint.h>
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
DEVICE *getDeviceByIDAndDerivativeID( uint8_t id, uint8_t unique_id, uint8_t rev );


#include "ec2drv.h"

// this still can NOT cover all EFM8 devices. for example, device id of EFM8SB2 is 0x16.
// but until now, EFM8SB2 device information is not present in device_table.csv,
// so it will not cause problem now.
#define IS_EFM8( obj ) \
	(device_id((obj))>>8 == 0x25 || device_id((obj))>>8 == 0x28 || device_id((obj))>>8 >= 32)

// filter out EFM8 for 'duplicated' Unique ID, this is a workaround and hope to touch original codes minimum.
// 'DEVICE_IN_RANGE' was called many times to handle special situation for specific device, 
// it's better not touch it now.
#define DEVICE_IN_RANGE( obj, start_uid, end_uid )	\
	(!IS_EFM8(obj) && ((obj->dev->unique_id)>=(start_uid)) && ((obj->dev->unique_id)<=(end_uid)))

#include "device_enum.h"

#endif
