#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <iostream>
#include <string>
#include <signal.h>
#include <getopt.h>

#include "ec2drv.h"

using namespace std;

void help()
{
	cout <<"ec2device\n"
		<<"options:\n"
		<<"\t--port		Specify the device/port the debug adatper is connected to (USB for EC3)\n"
		<<"\t--debug		Enable debugging trace\n"
		<< endl;
}


static EC2DRV obj;
static sighandler_t old_sigint_handler;
extern "C" void exit_func(void)
{
	cout << "exiting now" << endl;
	ec2_disconnect(&obj);
	signal(SIGINT,old_sigint_handler);
	cout << "disconnect done" <<endl;
}


int main(int argc, char *argv[])
{
	string port;
	
	static int debug=0, help_flag;
	static struct option long_options[] = 
	{
		{"debug", no_argument, &debug, 1},
		{"help", no_argument, &help_flag, 'h'},
		{"mode", required_argument, 0, 'm'},
		{"port", required_argument, 0, 'p'},
		{0, 0, 0, 0}
	};
	int option_index = 0;
	int c;
	
	old_sigint_handler = signal(SIGINT,exit);
	atexit(exit_func);
	
	obj.mode = AUTO;	// default to auto device selection
	obj.debug = FALSE;
	while(1)
	{
		c = getopt_long (argc, argv, "", long_options, &option_index);
		if( c==-1)
			break;
		switch(c)
		{
			case 0:		// set a flag, nothing to do
				break;
				case 'p':	// port
					port = optarg;
					break;
					case 'm':	// mode to use, JTAG / C2 / AUTO
						if( strcasecmp( optarg, "AUTO" )==0 )
							obj.mode = AUTO;
						else if( strcasecmp( optarg, "JTAG" )==0 )
							obj.mode = JTAG;
						else if( strcasecmp( optarg, "C2" )==0 )
							obj.mode = C2;
						else
						{
							printf("Error: unsupported mode, supported modes are AUTO / JTAG/ C2.\n");
							exit(-1);
						}
						break;
			default:
				printf("unexpected option\n");
				break;
		}
	};
	if( help_flag || port.length()==0 )
	{
		help();
		return  help_flag ? EXIT_SUCCESS : EXIT_FAILURE;
	}
	obj.debug = debug;
	if( ec2_connect( &obj, port.c_str() ) )
	{
		printf("FOUND:\n\n");
		printf("device\t: %s\n", obj.dev->name);
		printf("mode\t: %s\n", obj.dev->mode==C2 ? "C2" : "JTAG");
		
		printf("Flash size = %i bytes\n",obj.dev->flash_size);
		printf("Internal xram size = %i bytes\n",obj.dev->internal_xram_size);
		printf("External bus = %s\n", obj.dev->has_external_bus ? "Yes" : "No");
		
		switch(obj.dev->lock_type)
		{
		case FLT_RW:
			printf("Read lock addr = 0x%04x\n",obj.dev->read_lock);
			printf("Write lock addr = 0x%04x\n",obj.dev->write_lock);
			break;
		case FLT_SINGLE:
			printf("Single lock addr = 0x%04x\n",obj.dev->lock);
			break;
		default:
			printf("Unsupported lock type = 0x%02x\n",obj.dev->lock_type);
			break;
		}
		
		printf("Flash sector size = %i bytes\n",obj.dev->flash_sector_size);
		printf("Flash reserved top = 0x%04x\n",obj.dev->flash_reserved_top);
		printf("Flash reserved bottom = 0x%04x\n",obj.dev->flash_reserved_bottom);
		printf("Has Scratchpad = %s\n",obj.dev->has_scratchpad ? "Yes" : "No" );
		
		if(obj.dev->has_scratchpad)
		{
			printf("Scratchpad start addr = 0x%04x\n",obj.dev->scratchpad_start);
			printf("Scratchpad length = 0x%04x\n",obj.dev->scratchpad_len);
			printf("Scratchpad sector size = %i bytes\n",
				   obj.dev->scratchpad_sector_size);
		}
		printf("Has paged SFR = %s\n",obj.dev->has_paged_sfr ? "Yes" : "No" );
		printf("USB FIFO size = %i bytes\n",obj.dev->USB_FIFO_size);
		
		printf("\n");
		printf("Tested = %s\n", obj.dev->tested ? "Yes" : "No");
		printf("\n");
	}
	else
	{
		printf("ERROR: coulden't communicate with the EC2 debug adaptor\n");
		exit(-1);
	}
	
	if( !obj.dev->tested )
	{
		printf("Support for this device has not been fully tested or may not be complete\n");
		printf("You can be of assistance, please visit:\n"); printf("http://sourceforge.net/tracker/?atid=775284&group_id=149627&func=browse\n");
		printf("Please feel free to report your succcess / failure.\n");
		printf("If there are any issues we can probably resolve them with your help since we don't have boards for every supported processor.\n");
	}
	else
	{
		printf("If the above does not match your processor exactly, please contact us at :\n"); printf("http://sourceforge.net/tracker/?atid=775284&group_id=149627&func=browse\n");
		printf("We need your help to discover if any sub device id's exsist in the silicon\n");
	}
	ec2_disconnect( &obj );
	exit(0);
}
