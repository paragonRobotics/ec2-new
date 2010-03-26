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
#ifndef BREAKPOINTMGR_H
#define BREAKPOINTMGR_H
#include <list>
#include "types.h"
#include "dbgsession.h"
using namespace std;

/**
Manager class for breakpoints
handles temporary and normal breakpoints
supports emulator limits such as limited number of hardware breakpoints etc

	@author Ricky White <ricky@localhost.localdomain>
*/
class BreakpointMgr
{
public:
    BreakpointMgr( DbgSession *session );
    ~BreakpointMgr();
	
	/** set a normal breakpoint at the specified address
		\param addr	address to set breakpoint on
		\returns true if the breakpoint was set, otherwise false
	*/
	BP_ID set_bp( ADDR addr, bool temporary=false );
	
	BP_ID set_bp( string file, unsigned int line );

	/** Set a temporary breakpoint.
		Temporary breakpoints will be deleted after the target stops on them
		\param addr	address to set breakpoint on
		\returns true if the breakpoint was set, otherwise false
	*/
	BP_ID set_temp_bp( ADDR addr );

	BP_ID set_temp_bp( string file, unsigned int line );
	
	bool enable_bp( BP_ID id );
	bool disable_bp( BP_ID id );
	
	/** Clear all breakpoints.
		Both software and harrdware copies.
	*/
	void clear_all();
	
	/** clear all breakpoints in the target then reload from our copy.
	*/
	void reload_all();
	
	/** Dump a list of all breakpoints to the console
	*/
	void dump();
	
	BP_ID set_breakpoint( string cmd, bool temporary=false );
	bool clear_breakpoint( string cmd );
	bool clear_breakpoint_id( BP_ID id  );
	bool clear_breakpoint_addr( ADDR addr );
	
	/** indicates to the breakpoint manager that we have stopped.
		This is needed to temporary breakpoints can be modified as necessary
		\param addr The attreas at which the target stopped
	*/
	void stopped( ADDR addr);

	/** returns the file we are currently stopped in
		used by LineSpec
	*/
	string current_file();
	
	ADDR	current_addr()	{ return cur_addr; }

	/** Returns true if there is a currently active breakpoint at the specified
		address.  Mainly for use internally to ensure the only 1 active bp in
		target per address rule.

		\param addr	Address to check for active breakpoint on.
		\returns	True if there is an active breakpoint at the specified address.
	*/
	bool active_bp_at( ADDR addr );

	
	/** Gets the file name and line number for the supplied BP id and
		returns the primary file be it C or ASM it thats all there is
	
		\param id	BP id to get and line number for.
		\param file	File containing the breakpoint (output).
		\param line	Line number of the file  above where the BP resides.
		\returns true on success, false on failurew (invalid BPID)
	*/
	bool get_bp_file_line( BP_ID id, string &file, int &line );
	
protected:
	DbgSession *mSession;
	typedef struct
	{
		BP_ID		id;				///< id trackng number
		ADDR		addr;			///< Address of BP
		bool		bTemp;			///< true if a temporary BP
		bool 		bDisabled;		///< Indicates if the breaakpoint is currently disabled
		string		file;
		LINE_NUM	line;
		string		what;			///< identification of the breakpoint (file/function/line/addr etc)
	} BP_ENTRY;
	typedef list<BP_ENTRY> BP_LIST;
	BP_LIST	bplist;
	ADDR			cur_addr;		///< address we last stopped at,  this reflects the address we are currently at at any point where we are stopped
	
	int next_id();

	bool add_target_bp( ADDR addr );
	bool del_target_bp( ADDR addr );
};


#endif
