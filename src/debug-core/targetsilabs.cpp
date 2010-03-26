/***************************************************************************
 *   Copyright (C) 2006 by Ricky White   *
 *   ricky@localhost.localdomain   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include <iostream>
#include <pthread.h>
#include <stdio.h>
#include "targetsilabs.h"
#include "ec2drv.h"

using namespace std;

TargetSiLabs::TargetSiLabs()
  :	Target(),
	running(false),
	debugger_port("/dev/ttyS0"),
	is_connected_flag(false)
{
	obj.mode=AUTO;
}


TargetSiLabs::~TargetSiLabs()
{
	running = false;
	pthread_join( run_thread, NULL );	// wait for thread to stop
}


bool TargetSiLabs::connect()
{
	if( ec2_connect( &obj, debugger_port.c_str() ) )
	{
		is_connected_flag = true;
		return true;
	}
	else
		return false;
}

bool TargetSiLabs::disconnect()
{
	if( is_connected() )
	{
		ec2_disconnect( &obj );
		is_connected_flag = false;
		return true;
	}
	return false;
}

bool TargetSiLabs::is_connected()
{
	return is_connected_flag;
}
		
string TargetSiLabs::port()
{
	return debugger_port;
}

bool TargetSiLabs::set_port( string port )
{
	debugger_port = port;
	return true;
}

string TargetSiLabs::target_name()
{
	return "SL51";
}

string TargetSiLabs::target_descr()
{
	return "Silicon Laboritories Debug adapter EC2 / EC3";
}

string TargetSiLabs::device()
{
	running = false;
	return	obj.dev ? obj.dev->name : "unknown";
}

bool TargetSiLabs::command( string cmd )
{
	if( cmd=="special" )
	{
		char buf[19];
		ec2_read_ram_sfr( &obj, buf, 0x20, sizeof(buf), true );
		cout << "Special register area:" << endl;
		print_buf_dump(buf, sizeof(buf));
		return true;
	}
	else if(cmd=="mode=C2")
	{
		obj.mode=C2;
		cout << "MODE = C2" << endl;
		return true;
	}
	else if(cmd=="mode=JTAG")
	{
		obj.mode=JTAG;
		cout << "MODE = JTAG" << endl; return true;
	}
	else if(cmd=="mode AUTO")
	{
		obj.mode=AUTO;
		cout << "MODE = AUTO" << endl;
		return true;
	}
	return false;
}


///////////////////////////////////////////////////////////////////////////////
// Device control
///////////////////////////////////////////////////////////////////////////////

void TargetSiLabs::reset()
{
	cout << "Resetting target."<<endl;
	ec2_target_reset( &obj );
}

uint16_t TargetSiLabs::step()
{
	force_stop = false;
	return ec2_step( &obj );
}

bool TargetSiLabs::add_breakpoint(uint16_t addr)
{	cout << "adding breakpoint to silabs device" << endl;
	ec2_addBreakpoint( &obj, addr );
	return true;
}

bool TargetSiLabs::del_breakpoint(uint16_t addr)
{
	cout << "bool TargetSiLabs::del_breakpoint(uint16_t addr)" << endl;
	return ec2_removeBreakpoint( &obj, addr );
}

void TargetSiLabs::clear_all_breakpoints()
{
	ec2_clear_all_bp( &obj );
}


void TargetSiLabs::run_to_bp(int ignore_cnt)
{
	cout << "starting a run now..."<<endl;
	running = TRUE;
	force_stop = false;
	int i=0;
	
	do
	{
		//ec2_target_run_bp( &obj, &running );
		ec2_target_go(&obj);
		while( !ec2_target_halt_poll( &obj ) )
		{
			usleep(250);
			if(!running)
			{
				ec2_target_halt(&obj);
				return;		// someone stopped us early
			}
		}		
	}
	while( (i++)!=ignore_cnt );
}


/** Start the target running.
	You must now call poll_for_halt to determin when the target has halted.
	calling stop will cause the target to be halted.
*/
void TargetSiLabs::go()
{
	ec2_target_go(&obj);
}


/** Call after starting the target running to determnin if the traget has halted
	at a breakpoint or for some other reason.
*/
bool TargetSiLabs::poll_for_halt()
{
	return ec2_target_halt_poll( &obj );
}


bool TargetSiLabs::is_running()
{
	return running;
}

void TargetSiLabs::stop()
{
	Target::stop();
	cout <<"Stopping....."<<endl;
	running = FALSE;
}

void TargetSiLabs::stop2()
{
	Target::stop();
	cout <<"Stopping....."<<endl;
	ec2_target_halt_no_wait(&obj);
}

///////////////////////////////////////////////////////////////////////////////
// Memory reads
///////////////////////////////////////////////////////////////////////////////

void TargetSiLabs::read_data( uint8_t addr, uint8_t len, unsigned char *buf )
{
	ec2_read_ram( &obj, (char*)buf, addr, len );
}

/** @DEPRECIATED
*/
void TargetSiLabs::read_sfr( uint8_t addr, uint8_t len, unsigned char *buf )
{
	for( uint16_t offset=0; offset<len; offset++ )
		ec2_read_sfr( &obj, (char*)&buf[offset], addr+offset );
}

void TargetSiLabs::read_sfr( uint8_t addr,
				uint8_t page,
				uint8_t len,
				unsigned char *buf )
{
	BOOL ok;
	SFRREG sfr_reg;
	for( uint16_t offset=0; offset<len; offset++ )
	{
		sfr_reg.addr = addr+offset;
		sfr_reg.page = page;
		buf[offset] = ec2_read_paged_sfr( &obj, sfr_reg, &ok );
	}
}


void TargetSiLabs::read_xdata( uint16_t addr, uint16_t len, unsigned char *buf )
{
	ec2_read_xdata( &obj, (char*)buf, addr, len );
}

void TargetSiLabs::read_code( uint16_t addr, int len, unsigned char *buf )
{
	ec2_read_flash( &obj, buf, addr, len );
}

uint16_t TargetSiLabs::read_PC()
{
	return ec2_read_pc( &obj );
}

///////////////////////////////////////////////////////////////////////////////
// Memory writes
///////////////////////////////////////////////////////////////////////////////
void TargetSiLabs::write_data( uint8_t addr, uint8_t len, unsigned char *buf )
{
	ec2_write_ram( &obj, (char*)buf, addr, len );
}


/** @DEPRECIATED
*/
void TargetSiLabs::write_sfr( uint8_t addr, uint8_t len, unsigned char *buf )
{
	for( uint16_t offset=0; offset<len; offset++ )
		ec2_write_sfr( &obj, buf[offset], addr+offset );
}

void TargetSiLabs::write_sfr( uint8_t addr,
				uint8_t page,
				uint8_t len,
				unsigned char *buf )
{
	Target::write_sfr( addr, page, len, buf );
	BOOL ok;
	SFRREG sfr_reg;

	for( uint16_t offset=0; offset<len; offset++ )
	{
		sfr_reg.page = page;
		sfr_reg.addr = addr+offset;
		buf[offset] = ec2_write_paged_sfr( &obj, sfr_reg, buf[offset] );
	}
}



void TargetSiLabs::write_xdata( uint16_t addr, uint16_t len, unsigned char *buf )
{
	ec2_write_xdata( &obj, (char*)buf, addr, len );
}

void TargetSiLabs::write_code( uint16_t addr, int len, unsigned char *buf )
{
	cout << "Writing to flash with auto erase as necessary" << endl;
	printf("\tWriting %d bytes at 0x%04x\n",len,addr);
	// also erase scratchpad, since we may be using that for storage
	cout << "Erasing scratchpad";
	ec2_erase_flash_scratchpad( &obj );
	if( ec2_write_flash_auto_erase( &obj, buf, (int)addr, len ) )
		cout << "Flash write successful." << endl;
	else
		cout << "ERROR: Flash write Failed." << endl;
}

void TargetSiLabs::write_PC( uint16_t addr )
{
	ec2_set_pc( &obj, addr );
}
