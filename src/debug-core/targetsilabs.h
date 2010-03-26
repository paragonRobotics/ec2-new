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
#ifndef TARGETSILABS_H
#define TARGETSILABS_H
#include <stdint.h>
#include <pthread.h>
#include "target.h"
#include "ec2drv.h"

/**
Wrapper for the ec2 / EC3 debug adapters from Silicon Labs

	@author Ricky White <ricky@localhost.localdomain>
*/
class TargetSiLabs : public Target
{
public:
    TargetSiLabs();
    virtual ~TargetSiLabs();
	virtual bool connect();
	virtual bool disconnect();
	virtual bool is_connected();
	virtual string port();
	virtual bool set_port( string port );
	virtual string target_name();
	virtual string target_descr();
	virtual string device();
	virtual uint32_t max_breakpoints()		{ return 4; }
//	virtual bool load_file( string name );
	virtual bool command( string cmd );	

	// device control
	virtual void reset();
	virtual uint16_t step();
	virtual bool add_breakpoint(uint16_t addr);
	virtual bool del_breakpoint(uint16_t addr);
	virtual void clear_all_breakpoints();
	virtual void run_to_bp(int ignore_cnt=0);
	virtual bool is_running();
	virtual void stop();
	/** Stop the target running
		This is a temporary version until newcdb is updated.
	*/
	virtual void stop2();

	
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
	string	debugger_port;		// port the device is connected to.
	EC2DRV	obj;
	pthread_t	run_thread;		///< thread that manages a running target.
	volatile BOOL		running;
	static void *run_thread_func( void *ptr );
	bool is_connected_flag;
};

#endif
