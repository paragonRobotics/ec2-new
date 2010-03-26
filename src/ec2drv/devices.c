#include <stdio.h>
#include "devices.h"

static DEVICE unknown_dev = { "Unknown",-1,255,AUTO,0,8196,512,FALSE };
extern DEVICE devices[];

// Pick the closest device and return it.
DEVICE *getDevice( uint8_t id, uint8_t rev )
{
	int i = 0;
	printf("id=0x%02x, rev=%02x\n",id, rev);
	do
	{
		if( (devices[i].id==id)&&(devices[i].rev==255) )
			return &devices[i];
		else if( (devices[i].id==id)&&(devices[i].rev==rev) )
			return &devices[i];
	} while( devices[++i].name[0] != 0 );
	
	return &unknown_dev;
}


DEVICE *getDeviceUnique( uint8_t unique_id, uint8_t rev )
{
	int i=0;
	printf("unique_id=0x%02x, rev=%02x\n",unique_id, rev);
	do
	{
		if( (devices[i].unique_id==unique_id)&&(devices[i].rev==255) )
			return &devices[i];
		else if( (devices[i].unique_id==unique_id)&&(devices[i].rev==rev) )
			return &devices[i];
	} while( devices[++i].name[0] != 0 );
	
	return &unknown_dev;
}

