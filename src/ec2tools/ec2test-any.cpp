/** ec2readflash utility
 *
 *   Copyright (C) 2005 by Ricky White
 *   rickyw@neatstuff.co.nz
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#include <iostream>
#include <string.h>
#include <getopt.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include "ec2drv.h"
using namespace std;


static EC2DRV obj;
static void (*old_sigint_handler)(int);

void help();
void print_buf( char *buf, int len );
bool test_flash( EC2DRV &obj );
bool test_flash_scratchpad( EC2DRV &obj );
bool test_data_ram( EC2DRV &obj );
bool test_xdata_ram( EC2DRV &obj );
bool test_pc_access( EC2DRV &obj );
bool test_debug( EC2DRV &obj );

void print_test( string name )
{
	cout << endl;
	cout << "Test " << name << endl;
	cout << "-------------------------------------------------------------" << endl;
}

void print_subtest( string name )
{
	cout << "\t"<<name<<" ... "<<flush;
}

void print_result( bool pass )
{
	cout << (pass ? "PASS" : "FAIL") << endl;
}

extern "C" void exit_func(void)
{
	cout << "exiting now" << endl;
	ec2_disconnect(&obj);
	signal(SIGINT,old_sigint_handler);
	cout << "disconnect done" <<endl;
}

static int quick_flag;		// true if quick test only

int main(int argc, char *argv[])
{
	string port;
	
	static int debug=false, help_flag, mode_flag;
	static int disable_flash, disable_data, disable_xdata, disable_pc;
	static int disable_scratch, disable_debug;
	static struct option long_options[] = 
	{
		{"debug", no_argument, &debug, 1},
		{"help", no_argument, &help_flag, 'h'},
		{"mode", required_argument, 0, 'm'},
		{"port", required_argument, 0, 'p'},
		{"disable-flash", no_argument, &disable_flash, 1},
		{"disable-data", no_argument, &disable_data, 1},
		{"disable-xdata", no_argument, &disable_xdata, 1},
		{"disable-pc", no_argument, &disable_pc, 1},
		{"disable-scratch", no_argument, &disable_scratch, 1},
		{"disable-debug", no_argument, &disable_debug, 1},
		{"quick", no_argument, &quick_flag, 1},
		{0, 0, 0, 0}
	};
	int option_index = 0;
	int c, i;

	old_sigint_handler = signal(SIGINT,exit);
	atexit(exit_func);
	
	obj.mode = AUTO;	// default to auto device selection

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

	cout << "Testing device / debugger connected to '"<<port<<"'"<<endl;
	if( !ec2_connect( &obj, port.c_str() ) )
	{
		cout <<"Failed to connect.  Aborting"<<endl;
		ec2_disconnect( &obj);
		return -1;
	}
	
	printf("FOUND:\n");
	printf("device\t: %s\n", obj.dev->name);
	printf("mode\t: %s\n", obj.dev->mode==C2 ? "C2" : "JTAG");
	printf("\n");

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
		printf("We need your help to discover if any sub device id's exsist in the silicon\n\n");
	}

	printf("NOTE some tests may take a few minutes to run so please be patient\n");
	if( obj.dbg_adaptor==EC2DRV::EC2 )
		printf("     The EC2 will be significantly slower than an EC3 device.\n");
	
	if( quick_flag )
		cout << "Quick Mode enabled" << endl;
	
	bool pass = true;
	if(!disable_data)		pass &= test_data_ram( obj );
	if(!disable_xdata)		pass &= test_xdata_ram( obj );
	if(!disable_flash)		pass &= test_flash( obj );
	if(!disable_scratch)	pass &= test_flash_scratchpad( obj );
	if(!disable_pc)			pass &= test_pc_access( obj );
	if(!disable_debug)		pass &= test_debug( obj );
	
	cout <<"Test " << (pass ? "Passed" : "Failed") << endl << endl;
	ec2_disconnect( &obj);
	return pass ? 0 : -1;
}


bool test_data_ram( EC2DRV &obj )
{
	char read_buf[0xff];
	char write_buf[0xff];
	bool pass;
	bool test_pass = true;
	
	print_test("DATA RAM");
	print_subtest("write / read 0x00");
	memset( write_buf, 0, sizeof(write_buf) );
	ec2_write_ram( &obj, write_buf, 0, sizeof(write_buf) );
	memset( read_buf, 0xff, sizeof(read_buf) );
	ec2_read_ram( &obj, read_buf, 0, sizeof(read_buf) );
	if( memcmp( read_buf, write_buf, sizeof(read_buf) )!=0 )
	{
		pass = false;
		print_result(pass);
		print_buf(write_buf,sizeof(write_buf));
		print_buf(read_buf,sizeof(read_buf));
	}
	else
	{
		pass = true;
		print_result(pass);
	}
	test_pass &= pass;
	
	print_subtest("write / read 0xff");
	memset( write_buf, 0xff, sizeof(write_buf) );
	ec2_write_ram( &obj, write_buf, 0, sizeof(write_buf) );
	memset( read_buf, 0xff, sizeof(read_buf) );
	ec2_read_ram( &obj, read_buf, 0, sizeof(read_buf) );
	if( memcmp( read_buf, write_buf, sizeof(read_buf) )!=0 )
	{
		pass = false;
		print_result(pass);
		cout << "Write buffer"<<endl;
		print_buf(write_buf,sizeof(write_buf));
		cout << "Read buffer"<<endl;
		print_buf(read_buf,sizeof(read_buf));
	}
	else
	{
		pass = true;
		print_result(pass);
	}
	test_pass &= pass;

	print_subtest("write / read Random data, sequential addr");
	srand( time(0) );
	for( int i=0; i<sizeof(write_buf); i++)
		write_buf[i] = rand() & 0x00FF;
	ec2_write_ram( &obj, write_buf, 0, sizeof(write_buf) );
	memset( read_buf, 0xff, sizeof(read_buf) );
	ec2_read_ram( &obj, read_buf, 0, sizeof(read_buf) );
	if( memcmp( read_buf, write_buf, sizeof(read_buf) )!=0 )
	{
		pass = false;
		print_result(pass);
		cout << "Write buffer"<<endl;
		print_buf(write_buf,sizeof(write_buf));
		cout << "Read buffer"<<endl;
		print_buf(read_buf,sizeof(read_buf));
	}
	else
	{
		pass = true;
		print_result(pass);
	}
	test_pass &= pass;


	
	// This write / read 0x00 test is needed to get the target dataram into a knowen state
	print_subtest("write / read 0x00");
	memset( write_buf, 0, sizeof(write_buf) );
	ec2_write_ram( &obj, write_buf, 0, sizeof(write_buf) );
	memset( read_buf, 0xff, sizeof(read_buf) );
	ec2_read_ram( &obj, read_buf, 0, sizeof(read_buf) );
	if( memcmp( read_buf, write_buf, sizeof(read_buf) )!=0 )
	{
		pass = false;
		print_result(pass);
		cout << "Write buffer"<<endl;
		print_buf(write_buf,sizeof(write_buf));
		cout << "Read buffer"<<endl;
		print_buf(read_buf,sizeof(read_buf));
	}
	else
	{
		pass = true;
		print_result(pass);
	}
	test_pass &= pass;
	usleep(1000000);
	print_subtest("write / read even addr");
	for( int i=0; i<=0xff; i+=2 )
	{
		write_buf[i] = 0x55;
		ec2_write_ram( &obj, &write_buf[i], i, 1 );
		ec2_read_ram( &obj, read_buf, 0, sizeof(read_buf) );
		if( memcmp( read_buf, write_buf, sizeof(read_buf) )!=0 )
		{
			pass = false;
			print_result(pass);
			cout << "Write buffer"<<endl;
			print_buf(write_buf,sizeof(write_buf));
			cout << "Read buffer"<<endl;
			print_buf(read_buf,sizeof(read_buf));
			printf("Failed at 0x%02x\n",i);
			cout << "rereading..." << endl;
			ec2_read_ram( &obj, read_buf, 0, sizeof(read_buf) );
			print_buf(read_buf,sizeof(read_buf));
		}
	}
	if(pass)
		print_result(pass);
	test_pass &= pass;
	print_subtest("write / read odd addr");
	for( int i=1; i<=0xff; i+=2 )
	{
		write_buf[i] = 0xAA;
		ec2_write_ram( &obj, &write_buf[i], i, 1 );
		ec2_read_ram( &obj, read_buf, 0, sizeof(read_buf) );
		if( memcmp( read_buf, write_buf, sizeof(read_buf) )!=0 )
		{
			pass = false;
			print_result(pass);
			cout << "Write buffer"<<endl;
			print_buf(write_buf,sizeof(write_buf));
			cout << "Read buffer"<<endl;
			print_buf(read_buf,sizeof(read_buf));
		}
	}
	if(pass)
		print_result(pass);
	test_pass &= pass;
	
	// This write / read 0x00 test is needed to get the target dataram into a knowen state
	print_subtest("write / read 0x00");
	memset( write_buf, 0, sizeof(write_buf) );
	ec2_write_ram( &obj, write_buf, 0, sizeof(write_buf) );
	memset( read_buf, 0xff, sizeof(read_buf) );
	ec2_read_ram( &obj, read_buf, 0, sizeof(read_buf) );
	if( memcmp( read_buf, write_buf, sizeof(read_buf) )!=0 )
	{
		pass = false;
		print_result(pass);
		cout << "Write buffer"<<endl;
		print_buf(write_buf,sizeof(write_buf));
		cout << "Read buffer"<<endl;
		print_buf(read_buf,sizeof(read_buf));
	}
	else
	{
		pass = true;
		print_result(pass);
	}
	test_pass &= pass;


	print_subtest("write / read Random data, random addr");
	srand( time(0) );
	int NUM_RW_OPPS = quick_flag ? 50 : 500;
	int addr;
	char data;
	memset( write_buf, 0x00, sizeof(write_buf) );
	for(int i=0;i<NUM_RW_OPPS; i++)
	{
		addr = rand() & 0xff;
		data = rand() & 0xff;
//		printf("addr=0x%02x, data=0x%02x\n",(unsigned char)addr,(unsigned char)data);
		write_buf[addr] = data;
		ec2_write_ram(&obj, &data, addr, 1 );
		ec2_read_ram( &obj, read_buf, 0, sizeof(read_buf) );
		if( memcmp( read_buf, write_buf, sizeof(read_buf) )!=0 )
		{
			pass = false;
			print_result(pass);
			printf("\tfail at addr=0x%02x\n",addr);
			print_result(pass);
			cout << "Write buffer"<<endl;
			print_buf(write_buf,sizeof(write_buf));
			cout << "Read buffer"<<endl;
			print_buf(read_buf,sizeof(read_buf));
			break;
		}
		else
			pass = true;
	}
	if(pass) print_result(pass);
	test_pass &= pass;
	return test_pass;
}

bool test_xdata_ram( EC2DRV &obj )
{
	int size = obj.dev->internal_xram_size;
	char *read_buf = new char[size];
	char *write_buf = new char[size];
	bool pass;
	bool test_pass = true;
	
	if(size==0)
		return true;	// no XRAM on this device
	print_test("XDATA RAM - onchip");
#if 1
	print_subtest("write / read 0x00");
	memset( write_buf, 0, size );
	ec2_write_xdata( &obj, write_buf, 0, size );
	memset( read_buf, 0xff, size );
	ec2_read_xdata( &obj, read_buf, 0, size );
	if( memcmp( read_buf, write_buf, size )!=0 )
	{
		pass = false;
		print_result(pass);
		print_buf(write_buf,size);
		print_buf(read_buf,size);
	}
	else
	{
		pass = true;
		print_result(pass);
	}
	test_pass &= pass;
	
	print_subtest("write / read 0xff");
	memset( write_buf, 0xff, size );
	ec2_write_xdata( &obj, write_buf, 0, size );
	memset( read_buf, 0xff, size );
	ec2_read_xdata( &obj, read_buf, 0, size );
	if( memcmp( read_buf, write_buf, size )!=0 )
	{
		pass = false;
		print_result(pass);
		cout << "Write buffer"<<endl;
		print_buf(write_buf,size);
		cout << "Read buffer"<<endl;
		print_buf(read_buf,size);
	}
	else
	{
		pass = true;
		print_result(pass);
	}
	test_pass &= pass;
	
	print_subtest("write / read Random data, sequential addr");
	srand( time(0) );
	for( int i=0; i<size; i++)
		write_buf[i] = rand() & 0x00FF;
	ec2_write_xdata( &obj, write_buf, 0, size );
	memset( read_buf, 0xff, size );
	ec2_read_xdata( &obj, read_buf, 0, size );
	if( memcmp( read_buf, write_buf, size )!=0 )
	{
		pass = false;
		print_result(pass);
		cout << "Write buffer"<<endl;
		print_buf(write_buf,size);
		cout << "Read buffer"<<endl;
		print_buf(read_buf,size);
	}
	else
	{
		pass = true;
		print_result(pass);
	}
	test_pass &= pass;

#endif	
	// This write / read 0x00 test is needed to get the target dataram into a knowen state
	print_subtest("write / read 0x00");
	memset( write_buf, 0, size);
	ec2_write_xdata( &obj, write_buf, 0, size);
	memset( read_buf, 0xff, size);
	ec2_read_xdata( &obj, read_buf, 0, size);
	if( memcmp( read_buf, write_buf, size)!=0 )
	{
		pass = false;
		print_result(pass);
		cout << "Write buffer"<<endl;
		print_buf(write_buf,size);
		cout << "Read buffer"<<endl;
		print_buf(read_buf,size);
	}
	else
	{
		pass = true;
		print_result(pass);
	}
	test_pass &= pass;

	print_subtest("write / read Random data, random addr");
	srand( time(0) );
	int NUM_RW_OPPS = quick_flag ? 100 : 500;
	int BURST_SIZE = quick_flag ? 100 : 50;
	int addr;
	char data;
	memset( write_buf, 0x00, size );
	for(int i=0;i<NUM_RW_OPPS/BURST_SIZE; i++)
	{
		printf("\x08%i",i);
		fflush(stdout);
		for(int burst=0; burst<BURST_SIZE; burst++)
		{
			do { addr = rand();} while(addr >= size);
			data = rand() & 0xff;
			write_buf[addr] = data;		// why was this commented out
			ec2_write_xdata( &obj, &data, addr, 1 );	// why was this commented out
		}
		ec2_read_xdata( &obj, read_buf, 0, size );
		if( memcmp( read_buf, write_buf, size )!=0 )
		{
			pass = false;
			print_result(pass);
			printf("\tfail at addr=0x%02x\n",addr);
			print_result(pass);
			cout << "Write buffer"<<endl;
			print_buf(write_buf,size);
			cout << "Read buffer"<<endl;
			print_buf(read_buf,size);
			exit(-1);
			break;
		}
		else
			pass = true;
	}
	printf("\x08 ");
	if(pass) print_result(pass);
	test_pass &= pass;
	return test_pass;
}


bool test_flash( EC2DRV &obj )
{
	uint8_t read_buf[0x1FFFF];
	uint8_t write_buf[0x1FFFF];
	bool pass;
	bool test_pass = true;
	int size;
	
	/// @FIXME this is wrong and should be included in device.c...
	if( obj.dev->flash_reserved_bottom==-1 )
	{
		size = obj.dev->flash_size-2;	// room for locks
	}
	else
	{
		size = obj.dev->flash_reserved_bottom-2;
		// we only bother testing below the reserved area since some devices 
		// don't have reserved areas.
	}
#if 1
	printf("flash_size = 0x%04x\n",obj.dev->flash_size);
	printf("reserved bottom = 0x%08x\n",obj.dev->flash_reserved_bottom);
	printf("\tTop User address = 0x%08x\n",size);
			
	print_test("FLASH");
	printf("\tTop address = 0x%04x\n",obj.dev->flash_size);
	printf("\tTop User address = 0x%04x\n",size);
	printf("\tsector size = %u bytes\n",obj.dev->flash_sector_size);
	printf("\tlock addr = 0x%04x\n",obj.dev->lock);
	printf("\tread lock addr = 0x%04x\n",obj.dev->read_lock);
	printf("\twrite lock addr = 0x%04x\n",obj.dev->write_lock);
	printf("\n");
#endif
	print_subtest("Erase Flash");
	ec2_erase_flash( &obj );
	print_result(TRUE);
	print_subtest("Checking Erase worked");
	ec2_read_flash( &obj, read_buf, 0x0000, size );
	pass = true;
	for(int i=0; i<size; i++)
	{
		if( (unsigned char)read_buf[i] != 0xff )
		{
			printf("Stopped @ 0x%04x, 0x%02x  ",i,(unsigned char)read_buf[i]);
			pass = false;
			break;
		}
	}
	print_result(pass);
	test_pass &= pass;

	print_subtest("Write / Read all Flash, ec2_write_flash_auto_erase (random data)" );
	for( int addr=0; addr<size; addr++ )
		write_buf[addr] = rand()&0x00FF;
	ec2_write_flash_auto_erase( &obj, write_buf, 0x0000, size );
	ec2_read_flash( &obj, read_buf, 0x0000, size );
	if( memcmp( write_buf, read_buf, size )!=0 )
	{
		pass = false;
		print_result(pass);
		cout << "Write buffer"<<endl;
		print_buf((char*)write_buf,size);
		cout << "Read buffer"<<endl;
		print_buf((char*)read_buf,size);
		for(int i=0; i<size;i++)
		{
			if(write_buf[i]!=read_buf[i])
			{
				printf("mismatch at 0x%04x, [0x%02x,0x%02x]\n",
									   i,write_buf[i],read_buf[i]);
			}
		}
		
	}
	else
	{
		pass = true;
		print_result(pass);
	}
	test_pass &= pass;
	
	print_subtest("Write block in middle, ec2_write_flash_auto_keep" );
	// cross sector boundatires and write into exsisting flash image
	for( int addr=0x420; addr<0x420+600; addr++ )
		write_buf[addr] = rand()&0x00FF;
	ec2_write_flash_auto_keep( &obj, write_buf+0x420, 0x420, 600 );
	ec2_read_flash( &obj, read_buf, 0x0000, size );
	if( memcmp( write_buf, read_buf, size )!=0 )
	{
		pass = false;
		print_result(pass);
		cout << "Write buffer"<<endl;
		print_buf((char*)write_buf,size);
		cout << "Read buffer"<<endl;
		print_buf((char*)read_buf,size);
		for(int i=0; i<size;i++)
		{
			if(write_buf[i]!=read_buf[i])
			{
				printf("mismatch at 0x%04x, [0x%02x,0x%02x]\n",
					   i,write_buf[i],read_buf[i]);
			}
		}
	}
	else
	{
		pass = true;
		print_result(pass);
	}
	test_pass &= pass;

	/// @TODO modify this test to support different sector sizes.
	if( obj.dev->flash_sector_size==512 )
	{
		print_subtest("Write / Read chunk Flash, ec2_write_flash_auto_erase (random data)" );
		// blank arround the sectors in question
		// affected sectors at 0x200, 0x400
		memset( write_buf+0x200, 0xff, 1024 );	// mimic what will be read back
		for( int addr=0x220; addr<0x220+0x300; addr++ )
			write_buf[addr] = rand()&0x00FF;
		ec2_write_flash_auto_erase( &obj, write_buf+0x220, 0x220, 0x300 );
		ec2_read_flash( &obj, read_buf, 0x0000, size );
		if( memcmp( write_buf, read_buf, size )!=0 )
		{
			pass = false;
			print_result(pass);
			cout << "Write buffer"<<endl;
			print_buf((char*)write_buf,size);
			cout << "Read buffer"<<endl;
			print_buf((char*)read_buf,size);
			for(int i=0; i<size;i++)
			{
				if(write_buf[i]!=read_buf[i])
				{
					printf("mismatch at 0x%04x, [0x%02x,0x%02x]\n",
						   i,write_buf[i],read_buf[i]);
				}
			}

		}
		else
		{
			pass = true;
			print_result(pass);
		}
		test_pass &= pass;
	}
	
	print_subtest("Erase Flash");
	ec2_erase_flash( &obj );
	ec2_read_flash( &obj, read_buf, 0x0000, size );
	pass = true;
	for(int i=0; i<size; i++)
	{
		if( (unsigned char)read_buf[i] != 0xff )
		{
			printf("Stopped @ 0x%04x, 0x%02x  ",i,(unsigned char)read_buf[i]);
			pass = false;
			break;
		}
	}
	print_result(pass);
	test_pass &= pass;

	return test_pass;
}


/** Test the scratchpad memory on devices that have it.
*/
bool test_flash_scratchpad( EC2DRV &obj )
{
	bool test_pass=true;
	uint32_t addr;
	printf("dss %d",obj.dev->has_scratchpad);
	if( !obj.dev->has_scratchpad )
		return TRUE;
	uint8_t *tbuf = (uint8_t*)malloc( obj.dev->scratchpad_len );
	uint8_t *rbuf = (uint8_t*)malloc( obj.dev->scratchpad_len );
	if(!tbuf||!rbuf)
	{
		free(tbuf);
		free(rbuf);
		return false;
	}
	print_test("Testing FLASH scratchpad access\n");
	print_subtest("Erasing Scratchpad Area");
	ec2_erase_flash_scratchpad( &obj );
	//printf("\tCheck scratchpad erased ... ");
	ec2_read_flash_scratchpad( &obj, rbuf, 0, obj.dev->scratchpad_len  );
	memset( tbuf, 0xff, obj.dev->scratchpad_len );
	if( memcmp( rbuf, tbuf, obj.dev->scratchpad_len )!=0 )
	{
		print_result(FALSE);
		test_pass=false;
	}
	else
		print_result(TRUE);
	
	print_subtest("Check random data write, all");
	for( addr=0; addr<obj.dev->scratchpad_len; addr++ )
		tbuf[addr] = rand()&0x00FF;
	ec2_write_flash_scratchpad_merge( &obj, tbuf, 0, obj.dev->scratchpad_len );
	memset( rbuf, 0xff, obj.dev->scratchpad_len  );
	ec2_read_flash_scratchpad( &obj, rbuf, 0, obj.dev->scratchpad_len );
	if( memcmp( rbuf, tbuf, obj.dev->scratchpad_len )!=0 )
	{
		print_result(FALSE);
		cout << "Write buffer"<<endl;
		print_buf((char*)tbuf,obj.dev->scratchpad_len);
		cout << "Read buffer"<<endl;
		print_buf((char*)rbuf,obj.dev->scratchpad_len);

		test_pass=false;
	}
	else
		print_result(TRUE);
	
	print_subtest("Check write in middle");
	tbuf[42] = 0x55;
	tbuf[43] = 0x5a;
	tbuf[44] = 0xa5;
	tbuf[45] = 0xaa;
	tbuf[46] = 0x00;
	ec2_write_flash_scratchpad_merge( &obj, &tbuf[42], 42, 5 );
	ec2_read_flash_scratchpad( &obj, rbuf, 0, obj.dev->scratchpad_len );
	if( memcmp( rbuf, tbuf, obj.dev->scratchpad_len )!=0 )
	{
		print_result(FALSE);
		test_pass=false;
	}
	else
		print_result(TRUE);
	
	print_subtest("Erasing scratchpad");
	ec2_erase_flash_scratchpad( &obj );
	ec2_read_flash_scratchpad( &obj, rbuf, 0, obj.dev->scratchpad_len );
	memset(tbuf,0xff,obj.dev->scratchpad_len);
	if( memcmp( rbuf, tbuf, obj.dev->scratchpad_len )!=0 )
	{
		print_result(FALSE);
		test_pass=false;
	}
	else
		print_result(TRUE);
	return test_pass;
}




// Program counter access
bool test_pc_access( EC2DRV &obj )
{
	bool pass;
	bool test_pass=true;
	print_test("Program Counter Access");
	print_subtest("PC = 0x0000");
	ec2_set_pc( &obj, 0x0000 );
	pass = ec2_read_pc( &obj )==0x0000;
	print_result(pass);
	test_pass &= pass;
	
	print_subtest("PC = 0x0110");
	ec2_set_pc( &obj, 0x0110 );
	pass = ec2_read_pc( &obj )==0x0110;
	print_result(pass);
	test_pass &= pass;

	print_subtest("PC = random");
	const int NUM_TESTS = 100;
	srand( time(0) );
	pass = true;
	for(int i=0; i<NUM_TESTS; i++)
	{
		uint16_t addr = rand() & 0xff;
		ec2_set_pc( &obj, addr );
		pass = ec2_read_pc( &obj )==addr;
		if(!pass)
		{
			printf("addr=0x%04x ",addr);
			pass = false;
			break;
		}
	}
	print_result(pass);
	test_pass &= pass;
	return test_pass;
}


void print_buf( char *buf, int len )
{
	while( len-- !=0 )
		printf("%02X ",(unsigned char)*buf++);
	printf("\n");
}


void help()
{
	printf("ec2test-any\n"
			"syntax:\n"
			"\tec2test-any --port=/dev/ttyS0 \n"
			"\tec2test-any --port=USB\n"
			"\tec2test-any --port=USB:serialnum\n"
			"\n"
			"Options:\n"
			"\t--debug               Dump debug trace of bytes sent/received\n"
			"\t--port <device>   	Specify serial port to connect to EC2 on or USB for a use device\n"
			"\t--mode                specify the mode of the debug interface.\n"
			"\t                      auto / jtag / c2 with auto being the default\n"
			"\n"
			"\t--disable-data        Disable data/idata ram tests\n"
			"\t--disable-xdata       Disable xdata tests\n"
			"\t--disable-flash       Disable Flash memory tests\n"
			"\t--disable-pc          Disable Program counter tests\n"
			"\t--disable-scratch     Disable flash scratchpad tests\n"
			"\t--disable-debug       Disable target debug tests\n"
			"\t--quick               Speed up some tests at the expense of accuracy\n"
			"\n"
			"\t--debug               Turn on ec2drv debug tracing\n"
			"\t--help                Display this help\n"
			"\n");
}




/** Test the debug features.
	This function first programs a small test program into the device's
	flash then proceds to test run/halt/step etc
	\param obj		Object to act on
	\returns		TRUE on pass, FALSE on failure
*/
bool test_debug( EC2DRV &obj )
{
	uint8_t program[] =
	{
		0x74, 0x20,			// 0x0000	start: MOV A,#20h
		0xF8,				// 0x0002	MOV R0,A		BP_3
		0x00,				// 0x0003	loop1: NOP
		0x00,				// 0x0004	NOP
		0x00,				// 0x0005	NOP				BP_0
		0x00,				// 0x0006	NOP
		0x00,				// 0x0007	NOP				BP_1
		0x00,				// 0x0008	NOP				BP_2
		0x00,				// 0x0009	NOP
		0x00,				// 0x000a	NOP
		0xD8,uint8_t(-10),	// 0x000b	DJNZ R0, loop1
		0x02, 0x00, 0x00	// 0x000d	LJMP start (0x0000)
	};

	const uint32_t BP_0 = 0x0005;
	const uint32_t BP_1 = 0x0007;
	const uint32_t BP_2 = 0x0008;
	const uint32_t BP_3 = 0x0002;
	bool test_pass=true, pass;
	
	print_test("Debug operations");
	print_subtest("Load program");
	pass = ec2_write_flash_auto_erase( &obj, program, 0x0000, sizeof(program) );
	print_result(pass);
	test_pass &= pass;
	
	print_subtest("Reset Target");
	ec2_target_reset(&obj);
	pass = ec2_read_pc(&obj)==0x0000;
	print_result(pass);
	test_pass &= pass;
	
	
	ec2_clear_all_bp( &obj );
	print_subtest("Add single breakpoint");
	pass = ec2_addBreakpoint( &obj, BP_0 );		// first breakpoint
	print_result(pass);
	test_pass &= pass;

	print_subtest("Run to BP");
	ec2_target_go(&obj);

	usleep(1000000);	// allow time to reach breakpoint
	pass = ec2_target_halt_poll(&obj);
	pass &= ec2_read_pc(&obj)==BP_0;
	print_result(pass);
	test_pass &= pass;
	
	print_subtest("Run to BP again");
	ec2_target_go(&obj);
	usleep(100000);	// allow time to reach breakpoint
	pass = ec2_target_halt_poll(&obj);
	pass &= ec2_read_pc(&obj)==BP_0;
	print_result(pass);
	test_pass &= pass;
	
	print_subtest("Add second breakpoint");
	pass = ec2_addBreakpoint( &obj, BP_1 );		// second breakpoint
	print_result(pass);
	test_pass &= pass;
	
	print_subtest("Test with 2 BP's");
	//ec2_target_reset(&obj);	/// @FIXME I think targetreset kills the breakpoints!
	ec2_set_pc(&obj,0x0000);	/// @FIXME this should be part of target reset!
	ec2_target_go(&obj);
	usleep(1000000);	// allow time to reach breakpoint
	pass = ec2_target_halt_poll(&obj);
	pass &= ec2_read_pc(&obj)==BP_0;
	ec2_target_go(&obj);
	usleep(1000000);	// allow time to reach breakpoint
	pass &= ec2_target_halt_poll(&obj);
	pass &= ec2_read_pc(&obj)==BP_1;
	ec2_target_go(&obj);
	usleep(1000000);	// allow time to reach breakpoint
	pass = ec2_target_halt_poll(&obj);
	pass &= ec2_read_pc(&obj)==BP_0;
	ec2_target_go(&obj);
	usleep(1000000);	// allow time to reach breakpoint
	pass &= ec2_target_halt_poll(&obj);
	pass &= ec2_read_pc(&obj)==BP_1;
	print_result(pass);
	test_pass &= pass;
	
	print_subtest("Add Third breakpoint");
	pass = ec2_addBreakpoint( &obj, BP_2 );		// third breakpoint
	print_result(pass);
	test_pass &= pass;
	
	print_subtest("Test with 3 BP's");
	ec2_set_pc(&obj,0x0000);	/// @FIXME this should be part of target reset!
	ec2_target_go(&obj);
	usleep(100000);	// allow time to reach breakpoint
	pass = ec2_target_halt_poll(&obj);
	pass &= ec2_read_pc(&obj)==BP_0;
	ec2_target_go(&obj);
	usleep(100000);	// allow time to reach breakpoint
	pass &= ec2_target_halt_poll(&obj);
	pass &= ec2_read_pc(&obj)==BP_1;
	ec2_target_go(&obj);
	usleep(100000);	// allow time to reach breakpoint
	pass &= ec2_target_halt_poll(&obj);
	pass &= ec2_read_pc(&obj)==BP_2;
	ec2_target_go(&obj);
	usleep(100000);	// allow time to reach breakpoint
	pass &= ec2_target_halt_poll(&obj);
	pass &= ec2_read_pc(&obj)==BP_0;
	ec2_target_go(&obj);
	usleep(100000);	// allow time to reach breakpoint
	pass &= ec2_target_halt_poll(&obj);
	pass &= ec2_read_pc(&obj)==BP_1;
	ec2_target_go(&obj);
	usleep(100000);	// allow time to reach breakpoint
	pass &= ec2_target_halt_poll(&obj);
	pass &= ec2_read_pc(&obj)==BP_2;
	print_result(pass);
	test_pass &= pass;
	
	
	print_subtest("Add forth breakpoint");
	pass = ec2_addBreakpoint( &obj, BP_3 );
	print_result(pass);
	test_pass &= pass;
	
	print_subtest("Test with 4 BP's");
	
	ec2_set_pc(&obj,0x0000);	/// @FIXME this should be part of target reset!
	ec2_target_go(&obj);
	usleep(100000);	// allow time to reach breakpoint
	pass = ec2_target_halt_poll(&obj);
	pass &= ec2_read_pc(&obj)==BP_3;
	ec2_target_go(&obj);
	usleep(100000);	// allow time to reach breakpoint
	pass &= ec2_target_halt_poll(&obj);
	pass &= ec2_read_pc(&obj)==BP_0;
	ec2_target_go(&obj);
	usleep(100000);	// allow time to reach breakpoint
	pass &= ec2_target_halt_poll(&obj);
	pass &= ec2_read_pc(&obj)==BP_1;
	ec2_target_go(&obj);
	usleep(100000);	// allow time to reach breakpoint
	pass &= ec2_target_halt_poll(&obj);
	pass &= ec2_read_pc(&obj)==BP_2;
	ec2_target_go(&obj);
	usleep(100000);	// allow time to reach breakpoint
	pass &= ec2_target_halt_poll(&obj);
	pass &= ec2_read_pc(&obj)==BP_0;
	ec2_target_go(&obj);
	usleep(100000);	// allow time to reach breakpoint
	pass &= ec2_target_halt_poll(&obj);
	pass &= ec2_read_pc(&obj)==BP_1;
	ec2_target_go(&obj);
	usleep(100000);	// allow time to reach breakpoint
	pass &= ec2_target_halt_poll(&obj);
	pass &= ec2_read_pc(&obj)==BP_2;
	print_result(pass);
	test_pass &= pass;
	
	print_subtest("Delete 2 breakpoints BP_1 and BP_3");
	pass = ec2_removeBreakpoint( &obj, BP_1 );
	pass &= ec2_removeBreakpoint( &obj, BP_3 );
	print_result(pass);
	test_pass &= pass;
	
	print_subtest("Test with 2 BP's, BP_0 and BP_2");
	//ec2_target_reset(&obj);	/// @FIXME I think targetreset kills the breakpoints!
	ec2_set_pc(&obj,0x0000);	/// @FIXME this should be part of target reset!
	ec2_target_go(&obj);
	usleep(1000000);	// allow time to reach breakpoint
	pass = ec2_target_halt_poll(&obj);
	pass &= ec2_read_pc(&obj)==BP_0;
	ec2_target_go(&obj);
	usleep(1000000);	// allow time to reach breakpoint
	pass &= ec2_target_halt_poll(&obj);
	pass &= ec2_read_pc(&obj)==BP_2;
	ec2_target_go(&obj);
	usleep(1000000);	// allow time to reach breakpoint
	pass = ec2_target_halt_poll(&obj);
	pass &= ec2_read_pc(&obj)==BP_0;
	ec2_target_go(&obj);
	usleep(1000000);	// allow time to reach breakpoint
	pass &= ec2_target_halt_poll(&obj);
	pass &= ec2_read_pc(&obj)==BP_2;
	print_result(pass);
	test_pass &= pass;
	
	print_subtest("Remove all breakpoints");
	ec2_clear_all_bp(&obj);
	ec2_set_pc(&obj,0x0000);	/// @FIXME this should be part of target reset!
	ec2_target_go(&obj);
	usleep(10000000);			// allow time to reach breakpoint if still present
	pass &= !ec2_target_halt_poll(&obj);	// only pass if still running
	print_result(pass);
	test_pass &= pass;
		
	print_subtest("Halt a running processor");
	pass &= ec2_target_halt(&obj);
	print_result(pass);
	test_pass &= pass;	
	
	print_subtest("Stepping the target");
	ec2_set_pc(&obj,0x0000);	/// @FIXME this should be part of target reset!
	uint32_t addr;
	pass = true;
	addr = ec2_step(&obj);
	pass &= (addr==ec2_read_pc(&obj)) && (addr==0x0002);
	addr = ec2_step(&obj);
	pass &= (addr==ec2_read_pc(&obj)) && (addr==0x0003);
	addr = ec2_step(&obj);
	pass &= (addr==ec2_read_pc(&obj)) && (addr==0x0004);
	addr = ec2_step(&obj);
	pass &= (addr==ec2_read_pc(&obj)) && (addr==0x0005);
	addr = ec2_step(&obj);
	pass &= (addr==ec2_read_pc(&obj)) && (addr==0x0006);
	addr = ec2_step(&obj);
	pass &= (addr==ec2_read_pc(&obj)) && (addr==0x0007);
	addr = ec2_step(&obj);
	pass &= (addr==ec2_read_pc(&obj)) && (addr==0x0008);
	addr = ec2_step(&obj);
	pass &= (addr==ec2_read_pc(&obj)) && (addr==0x0009);
	addr = ec2_step(&obj);
	pass &= (addr==ec2_read_pc(&obj)) && (addr==0x000a);
	addr = ec2_step(&obj);
	pass &= (addr==ec2_read_pc(&obj)) && (addr==0x000b);
	addr = ec2_step(&obj);
	pass &= (addr==ec2_read_pc(&obj)) && (addr==0x0003);	// loop1
	addr = ec2_step(&obj);
	pass &= (addr==ec2_read_pc(&obj)) && (addr==0x0004);
	print_result(pass);
	test_pass &= pass;

	return test_pass;
}
