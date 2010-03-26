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
#ifndef CMDCOMMON_H
#define CMDCOMMON_H
#include "parsecmd.h"

/**
	@author Ricky White <ricky@localhost.localdomain>
*/
class CmdVersion : public CmdShowSetInfoHelp
{
public:
	CmdVersion()	{ name="version"; }
	virtual bool show( string cmd );
};

class CmdWarranty : public CmdShowSetInfoHelp
{
	public:
		CmdWarranty()	{ name="warranty"; }
		virtual bool show( string cmd );
		virtual bool info( string cmd )		{ return show(cmd); }
};

class CmdCopying : public CmdShowSetInfoHelp
{
	public:
		CmdCopying()	{ name="COPying"; }
		virtual bool show( string cmd );
		virtual bool info( string cmd )		{ return show(cmd); }
};


class CmdHelp : public ParseCmd
{
	public:
		virtual bool parse( string cmd );
};


/** Provide a gateway to communicate with the target driver.
	for the simulator this allows execution of simulator commands
	for Si51 isallows resetting the emulator etc
*/
class CmdTarget : public CmdShowSetInfoHelp
{
public:
	CmdTarget()	{ name="Target"; }
	virtual bool direct( string cmd );
	virtual bool set( string cmd );
	virtual bool info( string cmd );
	virtual bool show( string cmd );
};

class CmdPrompt : public CmdShowSetInfoHelp
{
	public:
		CmdPrompt()	{ name="PRompt"; }
		virtual bool set( string cmd );
};


class CmdStep : public CmdShowSetInfoHelp
{
	public:
		CmdStep()	{ name="Step"; }
		bool directnoarg();
};

class CmdStepi : public CmdShowSetInfoHelp
{
	public:
		CmdStepi()	{ name="STEPI"; }
		bool directnoarg();
};

class CmdNext : public CmdShowSetInfoHelp
{
	public:
		CmdNext()	{ name="Next"; }
		bool directnoarg();
};

class CmdNexti : public CmdShowSetInfoHelp
{
	public:
		CmdNexti()	{ name="NEXTI"; }
		bool directnoarg();
};


class CmdContinue : public CmdShowSetInfoHelp
{
	public:
		CmdContinue()	{ name="Continue"; }
		bool direct( string cmd );
		bool directnoarg();
};

class CmdFile : public CmdShowSetInfoHelp
{
	public:
		CmdFile()	{ name="file"; }
		bool direct( string cmd);
};

class CmdList : public CmdShowSetInfoHelp
{
	public:
		CmdList()	{ name="list"; }
		bool direct( string cmd );
		bool directnoarg();
};

class CmdPWD : public CmdShowSetInfoHelp
{
	public:
		CmdPWD()	{ name="pwd"; }
		bool directnoarg();
};

class CmdFiles : public CmdShowSetInfoHelp
{
	public:
		CmdFiles()	{ name="files"; }
		bool info( string cmd );
};

class CmdSource : public CmdShowSetInfoHelp
{
	public:
		CmdSource()	{ name="source"; }
		bool info( string cmd );
};

class CmdSources : public CmdShowSetInfoHelp
{
	public:
		CmdSources()	{ name="sources"; }
		bool info( string cmd );
};

class CmdLine : public CmdShowSetInfoHelp
{
	public:
		CmdLine()		{ name="Line"; }
		bool info( string cmd );
};

class CmdRun : public CmdShowSetInfoHelp
{
	public:
		CmdRun()		{ name="Run"; }
		bool directnoarg();
};

class CmdStop : public CmdShowSetInfoHelp
{
	public:
		CmdStop()		{ name="Stop"; }
		bool directnoarg();
};

class CmdFinish : public CmdShowSetInfoHelp
{
	public:
		CmdFinish()		{ name="Finish"; }
		bool directnoarg();
};

class CmdPrint : public CmdShowSetInfoHelp
{
public:
	CmdPrint()	{ name="Print"; }
	virtual bool direct( string cmd );
};

class CmdRegisters : public CmdShowSetInfoHelp
{
	public:
		CmdRegisters()	{ name="Registers"; }
		virtual bool info( string cmd );
};

#endif

