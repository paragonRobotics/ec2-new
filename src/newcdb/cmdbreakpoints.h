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
#ifndef CMDBREAKPOINTS_H
#define CMDBREAKPOINTS_H

#include "parsecmd.h"

/**
	@author Ricky White <ricky@localhost.localdomain>
*/
class CmdBreakpoints : public CmdTemplate
{
public:
	CmdBreakpoints()	{ name="BREAKPoints"; }
  virtual bool help( string cmd );
	virtual bool info( string cmd );
};

class CmdBreak : public CmdTemplate
{
public:
	CmdBreak()	{ name="BReak"; }
	virtual bool direct( string cmd );
	virtual bool directnoarg();
	virtual bool help( string cmd );
};

class CmdTBreak : public CmdTemplate
{
	public:
		CmdTBreak()	{ name="TBreak"; }
		virtual bool direct( string cmd );
		virtual bool directnoarg();
		virtual bool help( string cmd );
};


class CmdClear : public CmdTemplate
{
	public:
		CmdClear()	{ name="CLear"; }
		virtual bool direct( string cmd );
		virtual bool directnoarg();
		virtual bool help( string cmd );
};

class CmdDelete : public CmdTemplate
{
	public:
		CmdDelete()	{ name="DElete"; }
		virtual bool direct( string cmd );
		virtual bool help( string cmd );
};

class CmdDisable : public CmdTemplate
{
	public:
		CmdDisable()	{ name="DIsable"; }
		virtual bool direct( string cmd );
		virtual bool help( string cmd );
};

class CmdEnable : public CmdTemplate
{
	public:
		CmdEnable()	{ name="ENable"; }
		virtual bool direct( string cmd );
		virtual bool help( string cmd );
};

#endif
