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

class CmdHelp : public ParseCmd
{
	public:
		virtual bool parse( string cmd );
};

// meta class to provide help msg for Control group 
class CmdControl : public CmdTemplate
{
public:
  CmdControl() { name = "control"; }
  virtual bool help( string cmd);
};

// meta class to provide help msg for Info group
class CmdInfo : public CmdTemplate
{
public:
  CmdInfo() { name = "information"; }
  virtual bool help( string cmd);
};

class CmdPrompt : public CmdTemplate
{
	public:
		CmdPrompt()	{ name="Prompt"; }
		virtual bool set( string cmd );
};

class CmdStep : public CmdTemplate
{
	public:
		CmdStep()	{ name="Step"; }
		bool directnoarg();
	  virtual bool help( string cmd );
};

class CmdStepi : public CmdTemplate
{
	public:
		CmdStepi()	{ name="STEPI"; }
		bool directnoarg();
	  virtual bool help( string cmd );
};

class CmdNext : public CmdTemplate
{
	public:
		CmdNext()	{ name="Next"; }
		bool directnoarg();
	  virtual bool help( string cmd );
};

class CmdNexti : public CmdTemplate
{
	public:
		CmdNexti()	{ name="NEXTI"; }
		bool directnoarg();
	  virtual bool help( string cmd );
};


class CmdContinue : public CmdTemplate
{
	public:
		CmdContinue()	{ name="Continue"; }
		bool direct( string cmd );
		bool directnoarg();
	  virtual bool help( string cmd );
};

class CmdFile : public CmdTemplate
{
	public:
		CmdFile()	{ name="file"; }
		bool direct( string cmd);
	  virtual bool help( string cmd );
};

class CmdDFile : public CmdTemplate
{
	public:
		CmdDFile()	{ name="dfile"; }
		bool direct( string cmd);
};

class CmdList : public CmdTemplate
{
	public:
    typedef struct
    {
      std::string filename;
      uint32_t linenum;
    } CurrentFile;

		CmdList()	{ name="list"; listsize = 20;}
		bool direct( string cmd );
		bool directnoarg();
		bool set(string cmd);
	  virtual bool help( string cmd );
    CurrentFile current_list_file;
    uint listsize;
};

class CmdPWD : public CmdTemplate
{
	public:
		CmdPWD()	{ name="pwd"; }
		bool directnoarg();
};

class CmdFiles : public CmdTemplate
{
	public:
		CmdFiles()	{ name="files"; }
		bool info( string cmd );
};

class CmdSource : public CmdTemplate
{
	public:
		CmdSource()	{ name="source"; }
		bool info( string cmd );
	  virtual bool help( string cmd );
};

class CmdSources : public CmdTemplate
{
	public:
		CmdSources()	{ name="sources"; }
		bool info( string cmd );
	  virtual bool help( string cmd );
};

class CmdLine : public CmdTemplate
{
	public:
		CmdLine()		{ name="Line"; }
		bool info( string cmd );
	  virtual bool help( string cmd );
};

class CmdFunctions : public CmdTemplate
{
  public:
    CmdFunctions()   { name="functions"; }
    bool info( string cmd );
	  virtual bool help( string cmd );
};

class CmdRun : public CmdTemplate
{
	public:
		CmdRun()		{ name="Run"; }
		bool directnoarg();
	  virtual bool help( string cmd );
};

class CmdStop : public CmdTemplate
{
	public:
		CmdStop()		{ name="Stop"; }
		bool directnoarg();
};

class CmdFinish : public CmdTemplate
{
	public:
		CmdFinish()		{ name="Finish"; }
		bool directnoarg();
};

class CmdPrint : public CmdTemplate
{
public:
	CmdPrint()	{ name="Print"; }
	virtual bool direct( string cmd );
	virtual bool help( string cmd );
};

class CmdRegisters : public CmdTemplate
{
	public:
		CmdRegisters()	{ name="Registers"; }
    virtual bool directnoarg(); 
		virtual bool info( string cmd );
		virtual bool help( string cmd );
};

#endif

