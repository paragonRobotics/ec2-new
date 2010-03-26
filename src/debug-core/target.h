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
#ifndef TARGET_H
#define TARGET_H
#include <stdint.h>
#include <string>
#include <list>
using namespace std;
/**
Base class for all target implementations


	@author Ricky White <ricky@localhost.localdomain>
*/
class Target
{
public:
    Target();
    virtual ~Target();
	virtual bool connect()=0;
	virtual bool disconnect()=0;
	virtual bool is_connected()=0;
	virtual bool command( string cmd )		{ return false; }
	virtual string port()=0;
	virtual bool set_port( string port )=0;
	virtual string target_name()=0;
	virtual string target_descr()=0;
	virtual string device()=0;
	
	/** \returns Maximum number of breakpoints supported by the target.
	*/
	virtual uint32_t max_breakpoints()=0;
	
	///////////////////////////////////////////////////////////////////////////
	// device control
	///////////////////////////////////////////////////////////////////////////
	
	/// Reset the target device.
	virtual void reset()=0;
	
	/// cause the target to step 1 assembly instruction.
	virtual uint16_t step()=0;
	
	/** Add a breakpoint.
		\param addr	address to place the breakpoint at
		\returns true=success, false=failure
	*/
	virtual bool add_breakpoint(uint16_t addr)=0;
	
	/** Remove a breakpoint.
		\param addr	of breakpoint to remove
		\returns true=success, false=failure
	 */
	virtual bool del_breakpoint(uint16_t addr)=0;
	
	/** Clear all breakpoints currently set in the target
	*/
	virtual void clear_all_breakpoints()=0;
	
	/** Run to breakpoint.
	*/
	virtual void run_to_bp(int ignore_cnt=0)=0;
	
	/** poll while running to determin if the target has stopped or been stopped
	*/
	virtual bool is_running()=0;
	
	/** Stop the target running
	*/
	virtual void stop();
	
	/** Stop the target running
		This is a temporary version until newcdb is updated.
	*/
	virtual void stop2()	{ stop(); }
	
	// Start target running but don't hang arround.  poll with poll_for_halt until that returns true
	virtual void go()
	{
	}
	
	/** Poll this regularly to determine if the target had halted.
		You should only do this when you expect the target to be running in the
		first place to avoid confusing hardware targets like the SL driver.
	*/
	virtual bool poll_for_halt()
	{
	}

	
	// memory reads
	virtual void read_data( uint8_t addr, uint8_t len, unsigned char *buf )=0;
	virtual void read_sfr( uint8_t addr, uint8_t len, unsigned char *buf )=0;
	virtual void read_sfr( uint8_t addr, uint8_t page, uint8_t len, unsigned char *buf )=0;
	virtual void read_xdata( uint16_t addr, uint16_t len, unsigned char *buf )=0;
	virtual void read_code( uint16_t addr, uint16_t len, unsigned char *buf )=0;
	virtual uint16_t read_PC()=0;
	
	// memory writes
	virtual void write_data( uint8_t addr, uint8_t len, unsigned char *buf )=0;
	virtual void write_sfr( uint8_t addr, uint8_t len, unsigned char *buf )=0;
	virtual void write_sfr( uint8_t addr, uint8_t page,uint8_t len, unsigned char *buf );
	virtual void write_xdata( uint16_t addr, uint16_t len, unsigned char *buf )=0;
	virtual void write_code( uint16_t addr, uint16_t len, unsigned char *buf )=0;
	virtual void write_PC( uint16_t addr )=0;

	/** load an intel hex file into the target
	*/
	virtual bool load_file( string name );

	
	/** Special function to allow target->stop() to stop processes outside of the target such as a cont or runaway step of until operation.
	if a force_stop has been requested and the target han't handle it this function will return true and will clear the flag within the target module.  the caller must then stop the current operation.
	*/
	virtual bool check_stop_forced();
	
	/** utility function to print a buffer as an HEX and ASCII dump
	*/
	void print_buf_dump( char *buf, int len );
	
////////////////////////////////////////////////////////////////////////////////
// Read Caching functions used for all targets but can be overridden if desired
////////////////////////////////////////////////////////////////////////////////
	
	virtual void invalidate_cache();
	
	/** Read an SFR from the cach.  if this register isn't in the cache then
		read the entire SFR page into the cache.
	*/
	virtual void read_sfr_cache( uint8_t addr, uint8_t page, uint8_t len, unsigned char *buf );
	
	
	
protected:
	bool force_stop;
		
	typedef struct
	{
		int page;
		unsigned char buf[128];
	} SFR_CACHE_PAGE;
	typedef std::list<SFR_CACHE_PAGE> SFR_PAGE_LIST;
	SFR_PAGE_LIST mCacheSfrPages;
	
	SFR_PAGE_LIST::iterator cache_get_sfr_page( int page )
	{
		SFR_PAGE_LIST::iterator it;
		for( it=mCacheSfrPages.begin(); it!=mCacheSfrPages.end(); it++ )
		{
			if( (*it).page==page )
				return it;
		}
		return mCacheSfrPages.end();
	}
};

#endif
