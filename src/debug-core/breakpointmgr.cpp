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
#include <vector>
#include <stdio.h>
#include "target.h"
#include "breakpointmgr.h"
#include "symtab.h"
#include "linespec.h"
#include "dbgsession.h"
using namespace std;

BreakpointMgr::BreakpointMgr( DbgSession *session )
	: mSession(session)
{
}


BreakpointMgr::~BreakpointMgr()
{
}

	
/** set a normal breakpoint at the specified address
	\param addr	address to set breakpoint on
	\returns assigned bp id
*/
BP_ID BreakpointMgr::set_bp( ADDR addr, bool temporary )
{
	if( add_target_bp(addr) )
	{
		BP_ENTRY ent;
		ent.id		  = next_id();
		ent.addr  	  = addr;
		ent.bTemp 	  = temporary;
		ent.bDisabled = false;
		bplist.push_back(ent);
		return ent.id;
	}
	return BP_ID_INVALID;
}

BP_ID BreakpointMgr::set_bp( string file, LINE_NUM line )
{
	// @TODO lookup address, try and findout what it is, if we can't find it we should fail, since we know which files are involved from the start (nothing is dynamic)
	if( add_target_bp( 0x1234 ) )
	{
		BP_ENTRY ent;
		ent.id		  = next_id();
		ent.addr  	  = 0x1234;
		ent.bTemp 	  = false;
		ent.bDisabled = false;
		ent.file	  = file;
		ent.line	  = line;
		bplist.push_back(ent);
		return ent.id;
	}
	return BP_ID_INVALID;
}

// @FIXME: shoulden't we remove file and line from the entry and have a generic what field that can be set approprieatly when a breakpoint is created of modified
/** Set a temporary breakpoint.
	Temporary breakpoints will be deleted after the target stops on them
	\param addr	address to set breakpoint on
	\returns assigned BP id, BP_ID_INVALID on failure.
*/
BP_ID BreakpointMgr::set_temp_bp( ADDR addr )
{
	if( add_target_bp(addr) )
	{
		BP_ENTRY ent;
		ent.id		  = next_id();
		ent.addr  	  = addr;
		ent.bTemp 	  = true;
		ent.bDisabled = false;
		bplist.push_back(ent);
		return ent.id;
	}
	return BP_ID_INVALID;
}

/** Set a temporary breakpoint.
	Temporary breakpoints will be deleted after the target stops on them
	\param addr	address to set breakpoint on
	\returns assigned BP id, BP_ID_INVALID on failure.
*/
BP_ID BreakpointMgr::set_temp_bp( string file, unsigned int line )
{
	// @TODO lookup address, try and findout what it is, if we can't find it we should fail, since we know which files are involved from the start (nothing is dynamic)
	if( add_target_bp( 0x1234 ) )
	{
		BP_ENTRY ent;
		ent.id		  = next_id();
		ent.addr  	  = 0x1234;
		ent.bTemp 	  = true;
		ent.bDisabled = false;
		ent.file	  = file;
		ent.line	  = line;
		bplist.push_back(ent);
		return ent.id;
	}
	return BP_ID_INVALID;
}



/** Clear all breakpoints.
*/
void BreakpointMgr::clear_all()
{
	cout << "Clearing all breakpoints." << endl;
	bplist.clear();
	cout << "Clearing all breakpoints in target." << endl;
	mSession->target()->clear_all_breakpoints();
}

void BreakpointMgr::reload_all()
{
	BP_LIST::iterator it;
	cout << "Reloading breakpoints into the target." << endl;

	std::vector<ADDR> loaded;
	std::vector<ADDR>::iterator lit;

	mSession->target()->clear_all_breakpoints();
	if( bplist.size()>0 )
	{
		for( it=bplist.begin(); it!=bplist.end(); ++it )
		{
			// only reload if we haven't already loaded a bp at that address.
			//lit = loaded.find( (*it).addr );
			for(lit=loaded.begin();lit!=loaded.end(); ++lit)
			{
				if( (*it).addr==*(lit) )	break;
			}
			//lit = find(loaded.begin(),loaded.end),(*it).addr==addr);
			if( lit == loaded.end() )
			{
				mSession->target()->add_breakpoint( (*it).addr );
				loaded.push_back((*it).addr);
			}
		}
	}
}

void BreakpointMgr::dump()
{
	BP_LIST::iterator it;
	
	if( bplist.size()>0 )
	{
		printf("Num Type           Disp Enb Address            What\n");
		for( it=bplist.begin(); it!=bplist.end(); ++it )
		{
			printf("%-4ibreakpoint     %-5s%-4c0x%04x             %s\n",
				   (*it).id,
				   (*it).bTemp ? "del " : "keep",
				   (*it).bDisabled ? 'n' : 'y',
				   (*it).addr,
				   (*it).what.c_str()
				  );
		}
	}
	else
		cout << "No breakpoints or watchpoints." << endl;
}

/** find the lowest unused breakpoint number and return it
	This version is ineffificent but it will work 
	for the numbe of breakpoints we expect.
*/
int BreakpointMgr::next_id()
{
	int lowest;
	bool ok;

	BP_LIST::iterator it;
	for( lowest=1; lowest<0xffff; lowest++)
	{
		ok=true;
		for( it=bplist.begin(); it!=bplist.end(); ++it )
		{
			if( (*it).id==lowest )
			{
				ok=false;
				break;
			}
		}
		if( ok )
			return lowest;
	}
}


/** Sets a breakpoint based on a what field
	formats:
		break file.c:123		add breakpoint to file.c at line 123
		break main				add a breakpoint at the start of the main function
		break 0x1234			add abreakpoint at 0x1234

  user break point location specification can be of the following
       forms
       a) <nothing>        - break point at current location
       b) lineno           - number of the current module
       c) filename:lineno  - line number of the given file
       e) filename:function- function X in file Y (useful for static functions)
       f) function         - function entry point
       g) *addr            - break point at address 

*/
BP_ID BreakpointMgr::set_breakpoint( string cmd, bool temporary )
{
	BP_ENTRY ent;
	LineSpec ls(mSession);
	if(cmd.length()==0)
	{
		return set_bp( cur_addr );	//  add breakpoint at current location
	}
	else if( ls.set(cmd) )
	{
		// valid linespec
		switch( ls.type() )
		{
			case LineSpec::LINENO:
//				ADDR addr = symtab.get_addr( ls.file(), ls.line() );
				if( ls.addr() !=-1 )
				{
					ent.addr	= ls.addr();
					ent.what	= cmd;
					ent.bTemp	= temporary;
					ent.id		= next_id();
					ent.bDisabled = false;
					if( add_target_bp(ent.addr) )
					{
						printf("Breakpoint %i at 0x%04x: file %s, line %i.\n",
							   ent.id,
							   ent.addr,
							   ls.file().c_str(),
							   ls.line());
						   
						bplist.push_back(ent);
						return ent.id;
					}
				}
				return BP_ID_INVALID;
				//return true;	// don't print bad command, was correctly formatted just no addr
				break;
			case LineSpec::FUNCTION:
				ent.addr	= ls.addr();
				ent.what	= cmd;
				ent.bTemp	= temporary;
				ent.id		= next_id();
				ent.bDisabled = false;
				if( add_target_bp(ent.addr) )
				{
					printf("Breakpoint %i at 0x%04x: file %s, line %i.\n",
						   ent.id,
						   ent.addr,
						   ls.file().c_str(),
						   ls.line() );
					bplist.push_back(ent);
					return ent.id;
				}
				break;
			case LineSpec::PLUS_OFFSET:
				ent.addr	= ls.addr();
				ent.what	= cmd;
				ent.bTemp	= temporary;
				ent.id		= next_id();
				ent.bDisabled = false;
				if( add_target_bp(ent.addr) )
				{
					printf("Breakpoint %i at 0x%04x: file %s, line %i.\n",
						   ent.id,
						   ent.addr,
						   ls.file().c_str(),
						   ls.line() );
					bplist.push_back(ent);
					return ent.id;
				}
				break;
			case LineSpec::MINUS_OFFSET:
				ent.addr	= ls.addr();
				ent.what	= cmd;
				ent.bTemp	= temporary;
				ent.id		= next_id();
				ent.bDisabled = false;
				if( add_target_bp(ent.addr) )
				{
					printf("Breakpoint %i at 0x%04x: file %s, line %i.\n",
						   ent.id,
						   ent.addr,
						   ls.file().c_str(),
						   ls.line() );
					bplist.push_back(ent);
					return ent.id;
				}
				break;
			case LineSpec::ADDRESS:
				ent.addr	= ls.addr();
				ent.what	= cmd;
				ent.bTemp	= temporary;
				ent.id		= next_id();
				ent.bDisabled = false;
				if( add_target_bp(ent.addr) )
				{
					printf("Breakpoint %i at 0x%04x: file %s, line %i.\n",
						   ent.id,
						   ent.addr,
						   ls.file().c_str(),
						   ls.line() );
					bplist.push_back(ent);
					return ent.id;
				}
				break;
			default:
				return BP_ID_INVALID;	// invalid linespec
		}
	}
	return BP_ID_INVALID;
}


/** call when execution stops to check of a temporary breakpoint caused it
	if so this will delete the breakpoint, freeing its resources up
	\param addr Address where the target stopped
 */
void BreakpointMgr::stopped( ADDR addr )
{
	// scan for matching address in breakpoint list.
	BP_LIST::iterator it;

	for( it=bplist.begin(); it!=bplist.end(); ++it)
	{
		if( (*it).addr == addr )
		{
			// inform user we stopped on a breakpoint
			cout <<"Stopped on breakpoint #"<<(*it).id<<endl;
			if( (*it).bTemp )
			{
				// remove temporary breakpoints
				bplist.erase(it);
				it = bplist.begin();		// safer, we start again after deleting an object, slow but reliable
				del_target_bp(addr);
			}
		}
	}
}


bool BreakpointMgr::clear_breakpoint_id( BP_ID id  )
{
	BP_LIST::iterator it;
	for( it=bplist.begin(); it!=bplist.end(); ++it)
	{
		if( (*it).id==id)
		{
			ADDR addr = (*it).addr;
			bplist.erase(it);
			del_target_bp(addr);
			return true;
		}
	}
	return false;
}

bool BreakpointMgr::clear_breakpoint_addr( ADDR addr )
{
	BP_LIST::iterator it;
	for( it=bplist.begin(); it!=bplist.end(); ++it)
	{
		if( (*it).addr==addr )
		{
			bplist.erase(it);
			del_target_bp(addr);
			return true;
		}
	}
	return false;
}

bool BreakpointMgr::clear_breakpoint( string cmd )
{
	LineSpec ls(mSession);
	if(cmd.length()==0)
	{
		return set_bp( cur_addr );	//  add breakpoint at current location
	}
	else if( ls.set(cmd) )
	{
		// valid linespec
		switch( ls.type() )
		{
			case LineSpec::LINENO:
			case LineSpec::FUNCTION:
			case LineSpec::PLUS_OFFSET:
			case LineSpec::MINUS_OFFSET:
			case LineSpec::ADDRESS:
				return clear_breakpoint_addr( ls.addr() );
			default:
				return false;
		}
	}
	return false;
}


string BreakpointMgr::current_file()
{
	return "NOT IMPLEMENTED: BreakpointMgr::current_file()";
}


bool BreakpointMgr::enable_bp( BP_ID id )
{
	BP_LIST::iterator it;
	for( it=bplist.begin(); it!=bplist.end(); ++it)
	{
		if( (*it).id==id )
		{
			if( add_target_bp( (*it).addr ) )
			{
				(*it).bDisabled = false;
				return true;
			}
			else
			{
				return false;
			}
		}
	}
	return false;
}

bool BreakpointMgr::disable_bp( BP_ID id )
{
	BP_LIST::iterator it;
	for( it=bplist.begin(); it!=bplist.end(); ++it)
	{
		if( (*it).id==id )
		{
			(*it).bDisabled = true;
			return del_target_bp( (*it).addr );
		}
	}
	return false;
}


bool BreakpointMgr::active_bp_at( ADDR addr )
{
	BP_LIST::iterator it;
	for( it=bplist.begin(); it!=bplist.end(); ++it)
	{
		if(	(*it).addr==addr && 
			!(*it).bDisabled )
			return true;
	}
	return false;	// no breakpoint at the specified address.
}

// for this to work the bplist entry must be added after this call.
bool BreakpointMgr::add_target_bp( ADDR addr )
{
	if( !active_bp_at(addr) )
	{
		return mSession->target()->add_breakpoint(addr);
	}
	printf("BP already active at this address\n");
	return true;	// already a bp at this address in target.
}

// for this to work the bplist entry must be removed before this 
// function is called.
bool BreakpointMgr::del_target_bp( ADDR addr )
{
	if( !active_bp_at(addr) )
	{
		return mSession->target()->del_breakpoint(addr);
	}
	return true;	// already a bp at this address in target.
}



bool BreakpointMgr::get_bp_file_line( BP_ID id, string &file, int &line )
{
	BP_LIST::iterator it;
	for( it=bplist.begin(); it!=bplist.end(); ++it)
	{
		if( (*it).id==id )
		{
			file = (*it).file;
			line = (*it).line;
			return true;
		}
	}
	return false;
}
