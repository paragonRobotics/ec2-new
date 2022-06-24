#include <stdio.h>
#include "devices.h"

static DEVICE unknown_dev = { "Unknown",-1,255,AUTO,0,8196,512,FALSE };
extern DEVICE devices[];

// Pick up the device exactly match thedevice id and derivative_id
DEVICE *getDeviceByIDAndDerivativeID( uint8_t id, uint8_t derivative_id, uint8_t rev)
{
	int i=0;
	
	//printf("id=0x%02x, derivative_id=0x%02x\n",id, derivative_id);
	
	do
	{
		if( (devices[i].id==id) && (devices[i].derivative_id==derivative_id) )
			return &devices[i];
	} while( devices[++i].name[0] != 0 );
	
	return &unknown_dev;
}

BOOL device_in_range( DEVICE *dev, uint16_t start_id, uint16_t end_id )
{
	uint16_t unique_id = dev->id << 8 | dev->derivative_id;
	return (unique_id >= start_id && unique_id <= end_id);
}
