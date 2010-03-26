#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <getopt.h>
#include "ec2drv.h"


// foward declarations
void print_buf( char *buf, int len );
void progress( uint8_t percent );

EC2DRV ec2obj;
struct sighandler_t *old_sigint_handler;

void exit_func(void)
{
	printf("Exiting now\n");
	ec2_disconnect(&ec2obj);
	printf("Disconnect done\n");
}

int main(int argc, char *argv[])
{
	int i,in;
	char *port;
	char *image_name;
	char ec2fw[16384];
	int debug=FALSE, do_xor=FALSE, help=FALSE;
	char blocks[20];
	BOOL use_block_list = FALSE;
	FILE *block_file;
	
	struct option long_options[] = 
	{
		{"debug", no_argument, &debug, 1},
		{"help", no_argument, &help, 1},
		{"xor", no_argument, &do_xor, 'x'},
		{"port", required_argument, 0, 'p'},
		{"blocks", required_argument, 0, 'b'},
		{"image", required_argument, 0, 'i'},
		{0, 0, 0, 0}
	};
	int option_index = 0;
	int c;

	signal(SIGINT,exit);
	atexit(exit_func);
	
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
			case 'i':	// image file
				image_name = optarg;
				break;
			case 'b':	// block file
				// open the block file and build the block list
				use_block_list = TRUE;
				block_file = fopen( optarg, "r" );
				int j=0;
				char line[10];
				while( fgets(line,sizeof(line),block_file) )
					blocks[j++] = strtoul(line,0,0);
				fclose(block_file);
				break;
			default:
				printf("unexpected option\n");
				break;
		}
	};
	if( debug)
		ec2obj.debug = TRUE;
	
	if(help)
	{
		printf(	"ec2-update-fw\n"
				"Syntax:\n"
				"\tport     - specify port eg --port=/dev/ttyS0 or --port=USB\n"
				"\txor      - specify that the image must be xored with 0x55 before upload\n"
				"\tblock    - Name the file containing a list of blocks for a\n"
				"\t           scrambled file  eg --blocks=ec2blocks.txt\n"
				"\tdebug    - Dump all communications with the EC2 / EC3\n"
				"\n");
	}
	
	
	in = open( image_name, O_RDONLY );
	if( in!=-1 )
	{
		i = read( in, ec2fw, 16384);
		printf("%i bytes read\n",i);

		ec2_connect_fw_update( &ec2obj, port );
		printf("Updating %s Firmware\n", 
			   (ec2obj.dbg_adaptor==EC2 ? "EC2":"EC3") );
		ec2obj.progress_cbk = &progress;
		printf("Firmware update ");
		BOOL pass;
		if(use_block_list)
			pass = ec2_write_firmware( &ec2obj, ec2fw, i, do_xor, blocks );
		else
			pass = ec2_write_firmware( &ec2obj, ec2fw, i, do_xor, 0 );

		printf("%s\n\n", pass ? "PASSED" : "FAILED" );
		
		close(in);
		ec2_disconnect( &ec2obj );
		ec2_connect( &ec2obj, port );
		ec2_disconnect( &ec2obj );
	}
	return EXIT_SUCCESS;
}


void progress( uint8_t percent )
{
	printf("%i\n",percent);
}

