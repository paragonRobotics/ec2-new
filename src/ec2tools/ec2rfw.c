/**	ec2rfw.c
	Liscence GPL V2 or later
	Initial version Kolja Waschk
	Ricky White added getopt options for common operations
*/
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>
#include <signal.h>
#include "ec2drv.h"

#define BUFLEN 512	/* Use sector size */

EC2DRV ec2obj;

void exit_func(void)
{
	printf("Exiting now\n");
	ec2_disconnect(&ec2obj);
	printf("Disconnect done\n");
}



int main(int argc, char *argv[])
{
	int i,j,out;
	unsigned char buf[BUFLEN];
	int debug=0, help=0, fw_only=0;
	char *port=0, *image_name=0;
	
	struct option long_options[] = 
	{
		{"debug", no_argument, &debug, 1},
		{"help", no_argument, &help, 1},
		{"firmware", no_argument, &fw_only, 1},
		{"port", required_argument, 0, 'p'},
		{"out", required_argument, 0, 'o'},
		{0, 0, 0, 0}
	};

	signal(SIGINT,exit);
	atexit(exit_func);
	
	int option_index = 0;
	int c;
	
	ec2obj.mode = AUTO;	// default to auto device selection
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
			case 'o':	// Output file
				image_name = optarg;
				break;
			default:
				printf("unexpected option = \"%s\"\n",optarg);
				exit(-1);
				break;
		}
	};
	if( debug)
		ec2obj.debug = TRUE;
	
	if(!port)
	{
		fprintf(stderr,"Error --port must be specified\n");
		exit(-1);
	}
	
	if(!image_name)
	{
		printf("optind = %i, argc=%i\n",optind,argc);
		if( (optind+1)==argc )
			image_name = argv[optind];
		else
		{
			fprintf(stderr,"Error you must specify an output filename\n");
			exit(-1);
		}
	}
	
	if(help)
	{
		printf(	"ec2readfw\n"
				"Syntax:\n"
				"\tport     - specify port eg --port=/dev/ttyS0 or --port=USB\n"
				"\tfirmware - Reads only the dubugger firmware if this flag is\n" "\t           set otherwise the entire contents of the debuggers\n"
				"\t           code memory is read.\n"
				"\tout      - Specify the output filename --out=file.bin\n"
				"\tdebug    - Dump all communications with the EC2 / EC3\n"
				"\n");
	}

	out=open(image_name,O_CREAT|O_TRUNC|O_WRONLY, 0644);
	if(out<0) { perror(image_name); return -1; };

	if( !ec2_connect_fw_update( &ec2obj, port ) )
		exit(-1);

	uint16_t start_addr, end_addr;
	switch(ec2obj.dbg_adaptor)
	{
		case EC2:
			start_addr = fw_only ? 0x400 : 0x0000;
			end_addr = fw_only ? 0x2000 : 0x7E00; 
			break;
		case EC3:
			start_addr = fw_only ? 0x1600 : 0x0000;
			end_addr = fw_only ? 0x3C00 : 0x3E00;
			break;
		default:
			fprintf(stderr,"Error Unasupported debug adaptor, can't read code\n");
			exit(-1);
	}
	uint16_t addr;
	for( addr=start_addr; addr<end_addr; addr+=BUFLEN )
	{
		printf("Reading %i bytes at 0x%04X\n", BUFLEN, addr);
		for(j=0;j<BUFLEN;j++)
		{
			buf[j] = boot_read_byte(&ec2obj, addr+j);
		}
		write(out, buf, BUFLEN);
		if(fw_only)
		{
			boot_select_flash_page(&ec2obj,addr/512);
			if( boot_local_calc_page_cksum(buf) !=	boot_calc_page_cksum(&ec2obj) )
			{
				fprintf(stderr,"Error - Bad CRC, retrying\n");
			//	addr -= 512;
			}
		}
	}
	
	close(out);

	ec2_disconnect( &ec2obj );

	return EXIT_SUCCESS;
}


void progress( uint8_t percent )
{
	printf("%i\n",percent);
}


 	  	 
