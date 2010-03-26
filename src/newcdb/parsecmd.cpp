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
using namespace std;
#include "types.h"
#include "parsecmd.h"
#include "cmdcommon.h"



ParseCmd::ParseCmd()
{
}


ParseCmd::~ParseCmd()
{
}


void ParseCmd::Tokenize( const string& str,
						 vector<string>& tokens,
						 const string& delimiters )
{
    // Skip delimiters at beginning.
	string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
	string::size_type pos     = str.find_first_of(delimiters, lastPos);

	while (string::npos != pos || string::npos != lastPos)
	{
        // Found a token, add it to the vector.
		tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
		lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
		pos = str.find_first_of(delimiters, lastPos);
	}
}

bool ParseCmd::match( const string &token, const string &mask )
{
	return token.compare(mask)==0;
}

const ParseCmd *CmdShowSetInfoHelp::cmds[] =
{
	new CmdVersion()
};



CmdShowSetInfoHelp::CmdShowSetInfoHelp()
{
	name = "help";
//	mdlist.push_back( new CmdHelp() );
	//cmdlist.push_back( new CmdVersion() );
}

CmdShowSetInfoHelp::~CmdShowSetInfoHelp()
{
}

bool CmdShowSetInfoHelp::parse( string cmd )
{
	enum { SET, SHOW, INFO, HELP } mode;
	int ofs;
	if( cmd.find("set ")==0 )
	{
		cmd = cmd.substr(4);
		mode = SET;
	}
	else if( cmd.find("show ")==0 )
	{
		cmd = cmd.substr(5);
		mode = SHOW;
	}
	else if( cmd.find("info ")==0 )
	{
		cmd = cmd.substr(5);
		mode = INFO;
	}
	else if( cmd.find("help ")==0 )
	{
		cmd = cmd.substr(5);
		mode = HELP;
	}
//	else if( cmd.find(name+" ")==0 )
	else if( (ofs=compare_name( cmd ))!=-1)
	{
		if( ofs<cmd.length() )
		{
//			cout <<"MATCH + space"<<endl;
			cmd = cmd.substr( ofs+1 );
			return direct( cmd );
		}
		else
		{
//			cout <<"MATCH"<<endl;
			return directnoarg();
		}
	}
	else
		return false;
	// is it for us?
	if( (ofs=compare_name(cmd))!=-1 )
	{
		cmd = cmd.substr(ofs);
		if( cmd[0]==' ' )
			cmd = cmd.substr(1);	// we mnay get sub commands starting with a space.
//		cout <<"mode ["<<cmd<<"]"<<endl;
		switch(mode)
		{
			case SET:	return set(cmd);
			case SHOW:	return show(cmd);
			case INFO:	return info(cmd);
			case HELP:	return help(cmd);
		}
	}
	return false;
}

/** Compares s to the name associated with this command
	parsing stops at first space or '/'
	\returns -1 if no match otherwise the length of characters of the tag
*/
int CmdShowSetInfoHelp::compare_name( string s )
{
	// upper case letters in name at the start represent the shortest form of the command supported.
	// we will only match is at least thase are correct and any following characters match
	int i=0;
	if( s.length()==0 )
		return -1;		// no match
	// match first part
	while( isupper(name[i]) )
	{
		if( i>=s.length() )
			return -1;		// no match
		if( tolower(name[i])!=tolower(s[i]) )
			  return -1;	// no match
		i++;
	}
	for( ; i<s.length(); i++)
	{
		if( s[i]==' ')
			return i;
 		if( s[i]=='/' )
			return i-1;
		if( tolower(name[i])!=tolower(s[i]) )
			return -1;
	}
	return s.length();
}

