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
#include <stdlib.h>
#include "cmdbreakpoints.h"
#include "breakpointmgr.h"
#include "target.h"
#include "newcdb.h"
#include "utils.h"

using namespace std;

// Show breakpoints infomation
// 'info breakpoints'
bool CmdBreakpoints::info( string cmd )
{
	gSession.bpmgr()->dump();
	return true;
}

bool CmdBreakpoints::help( string cmd )
{
  cout << "Breakpoints related operations." <<endl
       << endl
       << "info breakpoints:  show breakpoints status."<<endl
       << endl
       << "Please refer to:"<<endl
       << "help break:        to add a breakpoint" <<endl
       << "help tbreak:       to add a temporary breakpoint" <<endl
       << "help clear:        to clear a breakpoint at FUNC or LINENUM" <<endl
       << "help delete:       to delete breakpoints by numbers" <<endl
       << "help enable:       to enable a breakpoint by number" <<endl
       << "help disable:      to disable a breakpoint by number" <<endl;
  return true;
}

//  Add a breakpoint
//	formats:
//	  break               add breakpoint at current location
//		break file.c:123		add breakpoint to file.c at line 123
//		break main				  add a breakpoint at the start of the main function
//		break *0x15			    add abreakpoint at 0x15
//
//  user break point location specification can be of the following
//       forms
//       a) <nothing>        - break point at current location
//       b) lineno           - number of the current module
//       c) filename:lineno  - line number of the given file
//       e) filename:function- function X in file Y (useful for static functions)
//       f) function         - function entry point
//       g) *addr            - break point at address 

bool CmdBreak::direct( string cmd )
{
	return gSession.bpmgr()->set_breakpoint( cmd )!=BP_ID_INVALID;
}

bool CmdBreak::directnoarg()
{
	cout <<"Adding a breakpoint at the current location"<<endl;
	return gSession.bpmgr()->set_bp( gSession.target()->read_PC(), false )!=BP_ID_INVALID;
}


bool CmdBreak::help( string cmd )
{
  cout << "Set breakpoint at specified line or function." <<endl
       << "break:                   add breakpoint at currect location" <<endl
       << "break FILE:LINE_NUMBER:  add breakpoint to FILE at LINE_NUMBER" <<endl
       << "break FUNCTION:          add breakpoint at the start of FUNCTION" <<endl
       << "break FILE:FUNCTION:     add breakpoint at the start of FUNCTION of FILE" <<endl
       << "break *ADDR:             add breakpoint at ADDR" <<endl;
	return true;
}

bool CmdTBreak::direct( string cmd )
{
	return gSession.bpmgr()->set_breakpoint( cmd, true )!=BP_ID_INVALID;
}

bool CmdTBreak::directnoarg()
{
	cout <<"Adding a temporary breakpoint at the current location"<<endl;
	return gSession.bpmgr()->set_bp( gSession.target()->read_PC(), true )!=BP_ID_INVALID;
}

bool CmdTBreak::help( string cmd )
{
  cout << "Set a temporary breakpoint at specified line or function." <<endl
       << "tbreak: add a temprory breakpoint at currect location" <<endl;
	return true;
}

bool CmdClear::direct( string cmd )
{
	return gSession.bpmgr()->clear_breakpoint( cmd );
}

bool CmdClear::directnoarg()
{
	return gSession.bpmgr()->clear_breakpoint_addr( gSession.target()->read_PC() );
}

bool CmdClear::help( string cmd)
{
  cout << "Clear breakpoint at specified line or function." <<endl
       << "clear:                   clear breakpoint at currect location" <<endl
       << "clear FILE:LINE_NUMBER:  clear breakpoint to FILE at LINE_NUMBER" <<endl
       << "clear FUNCTION:          clear breakpoint at the start of FUNCTION" <<endl
       << "clear FILE:FUNCTION:     clear breakpoint at the start of FUNCTION of FILE" <<endl
       << "clear *ADDR:             clear breakpoint at ADDR" <<endl
       << endl
       << "Refer to 'delete' for delete breakpoints by numbers" <<endl;

	return true;
}

bool CmdDelete::direct( string cmd )
{
	vector <string> token;
	Tokenize( cmd, token );

	if( token.size()==0 )
		return false;
	
	for( int i=0; i<token.size(); i++ )
	{
    if(!is_digit(token[i]))
    {
      cout << "Wrong breakpoint number: " << token[i] <<endl;
      continue;
    }
		if(gSession.bpmgr()->clear_breakpoint_id( strtoul( token[i].c_str(),
															0, 10 ) ) )
    {
      cout << "Breakpoint " <<token[i]<<" deleted"<<endl;
    } else {
      cout <<"Delete breakpoint failed: "<<token[i]<<endl;
      continue;
    }
	}
	return true;
}

bool CmdDelete::help( string cmd )
{
  cout << "Delete breakpoints by breakpoint numbers." <<endl
       << "delete NUM1 NUM2 NUM3 ...: delete breakpoints by numbers"<<endl;
  return true;
}

bool CmdEnable::direct( string cmd )
{
  if(!is_digit(cmd))
  {
    cout << "Wrong breakpoint number: " << cmd <<endl;
    return true;
  }

	gSession.bpmgr()->enable_bp( strtoul( cmd.c_str(), 0, 10 ) );
	return true;
	
}

bool CmdEnable::help ( string cmd )
{
  cout << "Enable a breakpoint by breakpoint number." <<endl
       << "enable NUM: enable a breakpoint by number"<<endl;
  return true;
}

bool CmdDisable::direct( string cmd )
{
  if(!is_digit(cmd))
  {
    cout << "Wrong breakpoint number: " << cmd <<endl;
    return true;
  }

	gSession.bpmgr()->disable_bp( strtoul( cmd.c_str(), 0, 10 ) );
	return true;
}

bool CmdDisable::help( string cmd )
{
  cout << "Disable a breakpoint by breakpoint number." <<endl
       << "disable NUM: disable a breakpoint by number"<<endl;
  return true;
}
