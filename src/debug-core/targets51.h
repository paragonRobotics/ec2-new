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
#ifndef TARGETS51_H
#define TARGETS51_H
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <iostream>
using namespace std;

#include <target.h>

/**	Target for debugging using the s51 simulator

	@author Ricky White <ricky@localhost.localdomain>
*/
class TargetS51 : public Target
{
public:
    TargetS51();
    virtual ~TargetS51();
	virtual bool connect();
	virtual bool disconnect();
	virtual bool is_connected();
	virtual string port();
	virtual bool set_port( string port );
	virtual string target_name();
	virtual string target_descr();
	virtual string device();
	virtual uint32_t max_breakpoints()		{ return 0xFFFF; }

//	virtual bool load_file( string name );
	virtual bool command( string cmd );
	
	// Device control
	virtual void reset();
	virtual uint16_t step();
	virtual bool add_breakpoint(uint16_t addr);
	virtual bool del_breakpoint(uint16_t addr);
	virtual void clear_all_breakpoints();
	virtual void run_to_bp(int ignore_cnt=0);
	virtual bool is_running();
	virtual void stop();
	
	virtual void go();
	virtual bool poll_for_halt();
	
	// memory reads
	virtual void read_data( uint8_t addr, uint8_t len, unsigned char *buf );
	virtual void read_sfr( uint8_t addr, uint8_t len, unsigned char *buf );
	virtual void read_sfr( uint8_t addr, uint8_t page, uint8_t len, unsigned char *buf );
	virtual void read_xdata( uint16_t addr, uint16_t len, unsigned char *buf );
	virtual void read_code( uint16_t addr, int len, unsigned char *buf );
	virtual uint16_t read_PC();
			
	// memory writes
	virtual void write_data( uint8_t addr, uint8_t len, unsigned char *buf );
	virtual void write_sfr( uint8_t addr, uint8_t len, unsigned char *buf );
	virtual void write_sfr( uint8_t addr, uint8_t page, uint8_t len, unsigned char *buf );
	virtual void write_xdata( uint16_t addr, uint16_t len, unsigned char *buf );
	virtual void write_code( uint16_t addr, int len, unsigned char *buf );
	virtual void write_PC( uint16_t addr );
	
protected:
	FILE *simin;		// stream for simulator input
	FILE *simout;		// stream for simulator output
	int sock;			// socket descriptor to comm with simulator
	pid_t simPid;
	bool bConnected;
	bool bRunning;
	// Protected functions
	///////////////////////////////////////////////////////////////////////////
	
	void sendSim( string cmd );
	string recvSim(int timeout_ms );
	string recvSimLine(int timeout_ms );
	void parse_mem_dump( string dump, unsigned char *buf, int len );
	void write_mem( string area, uint16_t addr, uint16_t len, unsigned char *buf );
};

#endif

