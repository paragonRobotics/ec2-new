#ifndef EC2TYPES_H
#define EC2TYPES_H

#if !BOOL
	typedef uint8_t BOOL;
	#undef TRUE
	#undef FALSE
	#define TRUE	1
	#define FALSE	0
#endif


typedef struct
{
	uint8_t page;		///< SFR Page 0 - 0xff
	uint8_t addr;		///< SFR address 0x80 - 0xff
} SFRREG;

typedef enum { AUTO, JTAG, C2 } EC2_MODE;
typedef enum
{
	FLT_SINGLE,		///< single lock as is F310 etc, uses lock
	FLT_SINGLE_ALT,	///< single lock as is F310 etc, uses lock, alternate is in read_lock
	FLT_RW,			///<Read and write locks eg F020, uses read_lock and write_lock
	FLT_RW_ALT		///<Read and write Locks eg F040, uses read_lock and write_lock additionally lock holds an alternate readlock location since some devices have smaller flash but the same device id!
} FLASH_LOCK_TYPE;


typedef struct 
{
	uint16_t	usb_vendor_id;
	uint16_t	usb_product_id;
	uint16_t	usb_out_endpoint;
	uint16_t	usb_in_endpoint;
	BOOL		has_bootloader;
	char		name[32];
	uint16_t	min_ver;
	uint16_t	max_ver;
} DBG_ADAPTER_INFO;


#endif
