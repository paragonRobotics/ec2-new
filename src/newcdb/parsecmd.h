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
#ifndef PARSECMD_H
#define PARSECMD_H
#include <list>
#include <string>
#include <vector>
#include "types.h"
#include "newcdb.h"
using namespace std;

/**
Base clase for all command parsers

	@author Ricky White <ricky@localhost.localdomain>
*/
class ParseCmd
{
public:
	typedef list<ParseCmd*> List;
	ParseCmd();
	~ParseCmd();
	virtual bool parse( string cmd )=0;
	
	void Tokenize( const string& str,
				  vector<string>& tokens,
				  const string& delimiters = " ");
	bool match( const string &token, const string &mask );
};

class CmdShowSetInfoHelp : public ParseCmd
{
public:
	CmdShowSetInfoHelp();
	~CmdShowSetInfoHelp();
	virtual bool parse( string cmd );
	
protected:
	string name;
	virtual int compare_name( string s );
	static const ParseCmd *cmds[];
	virtual bool help(string cmd )			{ return false; }
	virtual bool set( string cmd )			{ return false; }
	virtual bool show( string cmd )			{ return false; }
	virtual bool info( string cmd )			{ return false; }
	virtual bool direct( string cmd )		{ return false; }
	virtual bool directnoarg()				{ return false; }
};

#endif
