/***************************************************************************
 *   Copyright (C) 2005 by Ricky White   *
 *   rickyw@neatstuff.co.nz   *
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
using namespace std;
#include "cmdmaintenance.h"
#include "module.h"
#include "symtab.h"
#include "symtypetree.h"
#include "newcdb.h"

/** This command provides similar functionality to that of GDB
*/
bool CmdMaintenance::direct( string cmd )
{
	vector <string> tokens;
	Tokenize( cmd, tokens );
	
	if( tokens.size()==0 )
		return false;
	
	string s = *tokens.begin();
	if( (tokens.size()>1) && match(s,"dump") )
	{
		s = tokens[1];
		
		if( match(s,"modules") && tokens.size()==2 )
		{
			gSession.modulemgr()->dump();
		}
		else if( match(s,"module") && tokens.size()==3 )
		{
			cout <<" dumping module '"<<tokens[2]<<"'"<<endl;
			gSession.modulemgr()->module(tokens[2]).dump();
		}
		else if( match(s,"symbols") && tokens.size()==2 )
		{
			gSession.symtab()->dump();
		}
		else if( match(s,"types") && tokens.size()==2 )
		{
			gSession.symtree()->dump();
		}
		else if( match(s,"type") && tokens.size()==3 )
		{
			gSession.symtree()->dump(tokens[2]);
		}
		else
			return false;
		return true;
	}
	return false;
}

