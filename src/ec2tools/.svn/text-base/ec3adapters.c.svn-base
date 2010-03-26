#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <usb.h>
#include <sys/ioctl.h>

#include "ec2drv.h"

void scan_usb();

int main(int argc, char *argv[])
{
	printf("List of Silicon Labs USB debug adapters found:\n");
	printf("port\t\tDevice\t\t\tManufacturer\t\tFriendly Name\n");
	scan_usb();
	return 0;
}


#define EC3_OUT_ENDPOINT	0x02
#define EC3_IN_ENDPOINT		0x81
#define EC3_PRODUCT_ID		0x8044
#define EC3_VENDOR_ID		0x10c4
void scan_usb()
{
	struct usb_bus *busses;
	struct usb_bus *bus;
	struct usb_device *ec3dev;
	struct usb_dev_handle *ec3;
	char serial_num[255], manufacturer[255], product[255];
	DBG_ADAPTER_INFO *dbg_info;
	
	usb_init();
	usb_find_busses();
	usb_find_devices();
	busses = usb_get_busses(); 

	ec3dev = 0;
	for (bus = busses; bus; bus = bus->next)
	{
		struct usb_device *dev;
		for (dev = bus->devices; dev; dev = dev->next)
		{
			dbg_info =
				ec2_GetDbgInfo(dev->descriptor.idVendor, dev->descriptor.idProduct);

			if( dbg_info )
			{
				ec3 = usb_open(dev);
				if( usb_get_string_simple(ec3, dev->descriptor.iSerialNumber, serial_num, sizeof(serial_num)) <0)
				{
					printf("%s found, ERROR: no permission to access,\n"
						   "please try as root or setup udev to give you access as a user\n",
						  dbg_info->name );
				}
				else
				{
					usb_get_string_simple(ec3, dev->descriptor.iManufacturer, manufacturer, sizeof(manufacturer));
					usb_get_string_simple(ec3, dev->descriptor.iProduct, product, sizeof(product));
							
					usb_release_interface(ec3, 0 );
					usb_close(ec3);
					
					printf("USB:%s\t%s\t%s\t%s\n",
							serial_num,
		 					product,
							manufacturer,
							dbg_info->name );
				}
			}
		}
	}
}
