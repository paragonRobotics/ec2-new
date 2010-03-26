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
#include <stdlib.h>
#include <string.h>
using namespace std;
#include "linespec.h"
#include "symtab.h"
#include "module.h"
#include <breakpointmgr.h>

LineSpec::LineSpec( DbgSession *session )
	: mSession(session)
{
}


LineSpec::~LineSpec()
{
}

bool LineSpec::set( string linespec )
{
	// ok parse the linespac
	int ofs;
	if( linespec[0] == '*' )
	{
		//cout << "linespec::address"<<endl;
		spec_type	= ADDRESS;
		address		= strtoul( linespec.substr(1).c_str(), 0, 16 );
		//filename	= bp_mgr.current_file();
		
#if 0
		if( !symtab.find_c_file_line( address, filename, line_num ) )
		{
			filename = "???";
			return false;
		}
		return true;
#else
	// new version
	string module;
	LINE_NUM line;
	if( mSession->modulemgr()->get_c_addr( address, module, line ) )
	{
		filename = mSession->modulemgr()->module(module).get_c_file_name();
		line_num = line;
		return true;
	}
	else if( mSession->modulemgr()->get_asm_addr( address, module, line ) )
	{
		filename = mSession->modulemgr()->module(module).get_asm_file_name();
		line_num = line;
		return true;
	}
	else
	{
		cout << "ERROR: linespec does not match a valid line." << endl;
		return false;
	}
		
#endif
	}
	if( (ofs=linespec.find(':'))>0 )
	{
		filename = linespec.substr(0,ofs);
		// filename:function or filename:line number
		if( isdigit(linespec[ofs+1]) )
		{
			// line number
			line_num = strtoul( linespec.substr(ofs+1).c_str(),0,10);
			spec_type = LINENO;
			address = mSession->symtab()->get_addr( filename, line_num );
			return true;
		}
		else
		{
			// file function
			spec_type = FUNCTION;
			function = linespec.substr(ofs+1);
			//address = symtab.get_addr( filename, function );
			if( mSession->symtab()->get_addr(filename,
											function,
											address,
											endaddress ) &&
				mSession->symtab()->find_c_file_line(address,
													filename,
													line_num ) )
				return true;
			else
				return false;
		}
	}
	if( linespec[0]=='+' )
	{
		int32_t offset = strtoul( linespec.substr(1).c_str(), 0, 0 );
		spec_type = PLUS_OFFSET;
		address = mSession->bpmgr()->current_addr() + offset;
		cout << "ERROR: offset suport not fully implemented..."<<endl;
		return true;
	}
	if( linespec[0]=='-' )
	{
		int32_t offset = strtoul( linespec.substr(1).c_str(), 0, 0 );
		address = mSession->bpmgr()->current_addr() + offset;
		spec_type = MINUS_OFFSET;
		cout << "ERROR: offset suport not fully implemented..."<<endl;
		return true;
	}
	if( linespec.length()>0 )
	{
		// text only, must be function name
		spec_type = FUNCTION;
		function = linespec;
		if( mSession->symtab()->get_addr( linespec, address, endaddress ) &&	
			mSession->symtab()->find_c_file_line( address, filename, line_num ) )
			return true;
		else
			return false;
	}
	spec_type = INVALID;
	return false;
}

