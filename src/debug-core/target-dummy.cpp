// target-dummy.cpp
// Dummy Target
// sink for all opperations when no real target exsists.
//
// (C) Ricky White 2007
// GPLv2 or later.
////////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <string.h>
#include "target-dummy.h"


using namespace std;

TargetDummy::TargetDummy()
  : Target()
{
}


TargetDummy::~TargetDummy()
{
}


bool TargetDummy::connect()
{
	is_connected_flag = true;
	return is_connected_flag;
}

bool TargetDummy::disconnect()
{
	is_connected_flag = false;
	return is_connected_flag;
}

bool TargetDummy::is_connected()
{
	return is_connected_flag;
}
		
string TargetDummy::port()
{
	return "<sink>";
}

bool TargetDummy::set_port( string port )
{
	return false;
}

string TargetDummy::target_name()
{
	return "<none>";
}

string TargetDummy::target_descr()
{
	return "Command sink, this is not a real target";
}

string TargetDummy::device()
{
	return "sink";
}

bool TargetDummy::command( string cmd )
{
	return false;
}


///////////////////////////////////////////////////////////////////////////////
// Device control
///////////////////////////////////////////////////////////////////////////////

void TargetDummy::reset()
{
	cout << "Resetting target."<<endl;
}

uint16_t TargetDummy::step()
{
	return 0;
}

bool TargetDummy::add_breakpoint(uint16_t addr)
{
	cout << "adding breakpoint to sink device" << endl;
	return true;
}

bool TargetDummy::del_breakpoint(uint16_t addr)
{
	cout << "bool TargetDummy::del_breakpoint(uint16_t addr)" << endl;
	return true;
}

void TargetDummy::clear_all_breakpoints()
{
	cout << "bool TargetDummy::clear_all_breakpoints()" << endl;
}


void TargetDummy::run_to_bp(int ignore_cnt)
{
	cout << "TargetDummy::run_to_bp(int ignore_cnt)" << endl;
}

bool TargetDummy::is_running()
{
	return false;
}

void TargetDummy::stop()
{
	Target::stop();
	cout <<"Stopping....."<<endl;
}


///////////////////////////////////////////////////////////////////////////////
// Memory reads
///////////////////////////////////////////////////////////////////////////////

void TargetDummy::read_data( uint8_t addr, uint8_t len, unsigned char *buf )
{
	memset(buf,0x55,len);
}

/** @DEPRECIATED
*/
void TargetDummy::read_sfr( uint8_t addr, uint8_t len, unsigned char *buf )
{
	memset(buf,0x55,len);
}

void TargetDummy::read_sfr( uint8_t addr,
				uint8_t page,
				uint8_t len,
				unsigned char *buf )
{
	memset(buf,0x55,len);
}


void TargetDummy::read_xdata( uint16_t addr, uint16_t len, unsigned char *buf )
{
	memset(buf,0x55,len);
}

void TargetDummy::read_code( uint16_t addr, int len, unsigned char *buf )
{
	memset(buf,0x55,len);
}

uint16_t TargetDummy::read_PC()
{
	return 0x1234;
}

///////////////////////////////////////////////////////////////////////////////
// Memory writes
///////////////////////////////////////////////////////////////////////////////
void TargetDummy::write_data( uint8_t addr, uint8_t len, unsigned char *buf )
{
}


/** @DEPRECIATED
*/
void TargetDummy::write_sfr( uint8_t addr, uint8_t len, unsigned char *buf )
{
}

void TargetDummy::write_sfr( uint8_t addr,
				uint8_t page,
				uint8_t len,
				unsigned char *buf )
{
}



void TargetDummy::write_xdata( uint16_t addr, uint16_t len, unsigned char *buf )
{
}

void TargetDummy::write_code( uint16_t addr, int len, unsigned char *buf )
{
}

void TargetDummy::write_PC( uint16_t addr )
{
}
