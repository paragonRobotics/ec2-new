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
#include <algorithm>
#include <iostream>
#include <string.h>
#include <ctype.h>
#include <boost/format.hpp>
#include <boost/tokenizer.hpp>
#include <stdio.h>
#include "types.h"
#include "cmdcommon.h"
#include "target.h"
#include "module.h"
#include "cdbfile.h"
#include "breakpointmgr.h"
#include "linespec.h"
#include "contextmgr.h"
#include "newcdb.h"

using namespace std;
using boost::format;
using boost::io::group;
//using namespace boost::tokenizer;


bool CmdVersion::show( string cmd )
{
	if(cmd.length()==0)
	{
		cout << "\nVersion 0.1 (jelly)\n"
			 << "Compiled on "<<__DATE__<<" at "<<__TIME__<<"\n"
			 << endl;
		return true;
	}
	return false;
}

bool CmdWarranty::show( string cmd )
{
	if(cmd.length()==0)
	{	
		cout << "                            NO WARRANTY\n"
		"\n"
		"  11. BECAUSE THE PROGRAM IS LICENSED FREE OF CHARGE, THERE IS NO WARRANTY\n"
		"FOR THE PROGRAM, TO THE EXTENT PERMITTED BY APPLICABLE LAW.  EXCEPT WHEN\n"
		"OTHERWISE STATED IN WRITING THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES\n"
		"PROVIDE THE PROGRAM \"AS IS\" WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED\n"
		"OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF\n"
		"MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE ENTIRE RISK AS\n"
		"TO THE QUALITY AND PERFORMANCE OF THE PROGRAM IS WITH YOU.  SHOULD THE\n"
		"PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY SERVICING,\n"
		"REPAIR OR CORRECTION.\n"
		"\n"
		"  12. IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING\n"
		"WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MAY MODIFY AND/OR\n"
		"REDISTRIBUTE THE PROGRAM AS PERMITTED ABOVE, BE LIABLE TO YOU FOR DAMAGES,\n"
		"INCLUDING ANY GENERAL, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING\n"
		"OUT OF THE USE OR INABILITY TO USE THE PROGRAM (INCLUDING BUT NOT LIMITED\n"
		"TO LOSS OF DATA OR DATA BEING RENDERED INACCURATE OR LOSSES SUSTAINED BY\n"
		"YOU OR THIRD PARTIES OR A FAILURE OF THE PROGRAM TO OPERATE WITH ANY OTHER\n"
		"PROGRAMS), EVEN IF SUCH HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE\n"
		"POSSIBILITY OF SUCH DAMAGES.\n"
		"\n";
		return true;
	}
	return false;
}

bool CmdCopying::show( string cmd )
{
//	string s = getenv("_");
	if( cmd.length()==0 )
	{
		execl("/usr/bin/less","less","copying",NULL);
		return true;
	}
	return false;	
}

/** top level help
*/
bool CmdHelp::parse( string cmd )
{
	if( cmd.compare("help")==0 )
	{
		printf("Help\n\n");
		cout << "List of classes of commands:\n"
				"\n"
				"breakpoints -- Making program stop at certain points\n"
				"data -- Examining data\n"
				"files -- Specifying and examining files\n"
				"running -- Running the program\n"
				"stack -- Examining the stack\n"
				"status -- Status inquiries\n"
				<<endl;
		return true;
	}
	return false;
}


/** Redirect commands directly to the target driver
	Ideaslly not much should be done through this interface in the interest of
	portability to other targer devices
*/
bool CmdTarget::direct( string cmd )
{
	return gSession.target()->command( cmd );
}

bool CmdTarget::set( string cmd )
{
	if( cmd.find("port ")==0 )
	{
		cout << "set port '" <<cmd.substr(5)<<"'"<< endl;
		gSession.target()->set_port(cmd.substr(5));
		return true;
	}
	else if( cmd.find("device ")==0 )
	{
		cout << "set device"<<cmd<< endl;
		return true;
	}
	else if( cmd.compare("connect")==0 )
	{
		if( gSession.target() )
		{
			gSession.target()->connect();
			return true;
		}
		else
		{
			cout	<< "ERROR you must select a target first." << endl
					<< "for silicon labs debuggers		     set target SL51" << endl
					<< "for the s51 simulator			     set target S51" 	<< endl
					<< endl;
			return false;
		}
	}
	else if( cmd.compare("disconnect")==0 )
	{
		gSession.target()->disconnect();
		return true;
	}
	else
	{
		if( gSession.SelectTarget(cmd) )
		{
			// disconnect from current target and select new one
			// Don't connect yet, user probably needs to setup ports before cmmanding a connect
			return true;
		}
	}
	return false;
}

bool CmdTarget::info( string cmd )
{
	if( cmd.find("port")==0 )
	{
		cout <<"port = \""<<"/dev/ttyS0"<<"\"."<<endl;
		return true;
	}
	else if( cmd.find("device")==0 )
	{
		cout <<"device = \""<<"80C51"<<"\"."<<endl;
		return true;
	}
	else if( cmd.length()==0 )
	{
		cout <<"Target = '"<<gSession.target()->target_name()<<"'\t"
				<<"'"<<gSession.target()->target_descr()<<"'"<<endl;
		cout <<"Port = '"<<gSession.target()->port()<<"'"<<endl;
		cout <<"Device = '"<<gSession.target()->device()<<"'"<<endl;
		printf("PC = 0x%04x\n",gSession.target()->read_PC());
		
		gSession.contextmgr()->dump();
		return true;
	}
	return false;
}

bool CmdTarget::show( string cmd )
{
	if( cmd.compare("connect")==0 )
	{
		cout << (gSession.target()->is_connected() ? 
				"Connected." : "Disconnected.") << endl;
		return true;
	}
	return false;
}


/** cause the target to step one source level instruction
*/
bool CmdStep::directnoarg()
{
	string module;
	LINE_NUM line;
	ADDR addr;
	// keep stepping over asm instructions until we hit another c function
	do
	{
		addr = gSession.target()->step();
		gSession.bpmgr()->stopped(addr);
		gSession.contextmgr()->set_context(addr);
	}
	while( !gSession.modulemgr()->get_c_addr( addr, module, line ) && 
			   !gSession.target()->check_stop_forced());
	gSession.contextmgr()->dump();
	return true;
}

/** cause the target to step one assembly level instruction
 */
bool CmdStepi::directnoarg()
{
	ADDR addr = gSession.target()->step();
	gSession.bpmgr()->stopped(addr);
	gSession.contextmgr()->set_context(addr);
	gSession.contextmgr()->dump();
	return true;
}

/** Continue to the next source line in the current (innermost) stack frame.
	This is similar to step, but function calls that appear within the line of
	code are executed without stopping.
	Execution stops when control reaches a different line of code at the
	original stack level that was executing when you gave the next command.
	This command is abbreviated n.

	@FIXME: change from step implementation to proper next
*/
bool CmdNext::directnoarg()
{
	string module;
	LINE_NUM line;
	ADDR addr;
	// keep stepping over asm instructions until we hit another c function
	do
	{
		addr = gSession.target()->step();
		gSession.bpmgr()->stopped(addr);
		gSession.contextmgr()->set_context(addr);
	}
	while( !gSession.modulemgr()->get_c_addr( addr, module, line ) );
	gSession.contextmgr()->dump();
	return true;
}

/** Execute one machine instruction, but if it is a function call, proceed until
	the function returns.

	@FIXME: change from stepi implementation to proper nexti
*/
bool CmdNexti::directnoarg()
{
	ADDR addr = gSession.target()->step();
	gSession.bpmgr()->stopped(addr);
	gSession.contextmgr()->set_context(addr);
	gSession.contextmgr()->dump();
	return true;
}


/**	Continue execution from the current address
	if there is a breakpoint on the current address it is ignored.
	optional parameter specifies a further number of breakpoints to ignore
*/
bool CmdContinue::direct( string cmd )
{
	printf("Continuing.\n");
	int i = strtoul( cmd.c_str(), 0, 0);
	
	gSession.target()->run_to_bp(i);
	gSession.bpmgr()->stopped(gSession.target()->read_PC());
	gSession.contextmgr()->set_context( gSession.target()->read_PC() );
	gSession.contextmgr()->dump();
	return true;
}

/**	Continue execution from the current address and stop at next breakpoint
*/
bool CmdContinue::directnoarg()
{
	printf("Continuing.\n");
	gSession.target()->run_to_bp();
//	bp_mgr.stopped( target->read_PC() );
	gSession.contextmgr()->set_context( gSession.target()->read_PC() );
	gSession.contextmgr()->dump();
	return true;
}

/** Reset the target abnd reload the breakpoints as necessary
*/
bool CmdRun::directnoarg()
{
	gSession.target()->stop();
	gSession.target()->disconnect();
	gSession.target()->connect();	
	gSession.target()->reset();
	gSession.bpmgr()->reload_all();
	
	if(gSession.bpmgr()->set_breakpoint("main",true)==BP_ID_INVALID)
		cout <<" failed to set main breakpoint!"<<endl;

	gSession.target()->run_to_bp();
	ADDR addr = gSession.target()->read_PC();
	gSession.bpmgr()->stopped(addr);
	gSession.contextmgr()->set_context(addr);
	gSession.contextmgr()->dump();
	return true; 
}


/** open a new cdb file for debugging
	all associated files must be in the same directory
*/	
bool CmdFile::direct( string cmd)
{
	gSession.modulemgr()->reset();
	gSession.symtab()->clear();
	gSession.symtree()->clear();	
	gSession.bpmgr()->clear_all();

	// disconnect and reconnect to make sure data is valid (fixes bug where
	//   data reads are wrong after loading new file)
	gSession.target()->disconnect();
	gSession.target()->connect();
	CdbFile cdbfile(&gSession);
	cdbfile.open( cmd+".cdb" );
	return gSession.target()->load_file(cmd+".ihx");
}


/** list a section of the program
	list linenum
list function
list
list -
list linespec
list first,last
list ,last
list first,
list +
list -

linespec:
	number
	+offset
	-offset
	filename:number 
	function
	filename:function 
	*address
*/
bool CmdList::direct( string cmd )
{
	cout <<"NOT implemented ["<<cmd<<"]"<<endl;
	return true;
}

bool CmdList::directnoarg()
{
	cout <<"NOT implemented"<<endl;
	return true;
}


bool CmdPWD::directnoarg()
{
	printf("Working directory %s.\n","dir here");	// @TODO replace "dir here with current path"
	return true;
}


/** info files and info target are synonymous; both print the current target
*/
bool CmdFiles::info( string cmd )
{
	cout <<"Symbols from \"/home/ricky/projects/ec2cdb/debug/src/test\"."<<endl;	// @TODO put correct pathe in here
	return true;
}


bool CmdSource::info( string cmd )
{
	if(cmd.length()==0)
	{
		cout << "Source files for which symbols have been read in:"<<endl<<endl;
		cout <<"test.c, test.asm"<<endl;
		return true;
	}
	return false;
}

bool CmdSources::info( string cmd )
{
	if(cmd.length()==0)
	{
		cout <<"Current source file is test.c"<<endl;
		cout <<"Located in test.c"<<endl;
		cout <<"Contains 11 lines."<<endl;
		cout <<"Source language is c."<<endl;
		return true;
	}
	return false;
}


/**  	
	Examples

	(gdb) info line m4_changequote
	Line 895 of "builtin.c" starts at pc 0x634c and ends at 0x6350.
	
	We can also inquire (using *addr as the form for linespec) what source line covers a particular address:

	(gdb) info line *0x63ff
	Line 926 of "builtin.c" starts at pc 0x63e4 and ends at 0x6404.
*/
bool CmdLine::info( string cmd )
{
//	if( cmd.find(' ')>=0 || cmd.length()==0 )
//		return false;	// cmd must be just one word
	if( cmd.empty() )
	{
		/// @FIXME need a current context for this one...
		return true;
	}
	LineSpec ls(&gSession);
	
	if( ls.set( cmd ) )
	{
		printf("Line %i of \"%s\" starts at pc 0x%04x and ends at 0x%04x.\n",
				ls.line(),
				ls.file().c_str(),
				ls.addr(),
				ls.end_addr()
			  );
		// test.c:19:1:beg:0x000000f8
		printf("\032\032%s:%i:%i:beg:0x%08x\n",
			   ls.file().c_str(),
			   ls.line(),
			   1,				// what should this be?
			   ls.addr()
			  );
		return true;
	}
	return false;
}


extern string prompt;
bool CmdPrompt::set( string cmd )
{
	prompt = cmd[cmd.length()-1]==' ' ? cmd : cmd+" ";
	return true;
}

bool CmdStop::directnoarg()
{
	cout << "Stopping target" << endl;
	gSession.target()->stop();
	return true;
}

bool CmdFinish::directnoarg()
{
	cout << "Finishing current function" << endl;
	// @fixme set a breakpoint at the end of the current function
	//bp_mgr.set_breakpoint(
	return true;
}

/** `print EXPR'
	`print /F EXPR'
		EXPR is an expression (in the source language).  By default the
		value of EXPR is printed in a format appropriate to its data type;
		you can choose a different format by specifying `/F', where F is a
		letter specifying the format; see  Output formats Output
		Formats.

	`print'
	`print /F'
		If you omit EXPR, GDB displays the last value again (from the
		"value history";  Value history Value History.).  This
		allows you to conveniently inspect the same value in an
		alternative format.
	
		A more low-level way of examining data is with the `x' command.  It
	examines data in memory at a specified address and prints it in a
	specified format.   Examining memory Memory.
	
		If you are interested in information about types, or about how the
	fields of a struct or a class are declared, use the `ptype EXP' command
	rather than `print'.   Examining the Symbol Table Symbols.


	\NOTE expr currently must be a variable name.  indexing into arrays is not yet supported, just displaying the entire array.

	\param expr	expression to display
*/
bool CmdPrint::direct( string expr )
{
	string sym_name = expr;
	char format = 0;


	// split up into a list
	typedef boost::tokenizer<boost::char_separator<char> > 
			tokenizer;
	boost::char_separator<char> sep(" \t");
	tokenizer tokens(expr, sep);
	int cnt=0;
	for( tokenizer::iterator it = tokens.begin();
			it != tokens.end(); ++it )
	{
		cnt++;
		if( (*it)[0]=='/' && it->length()==2  )
		{
			format = (*it)[1];
		}
		// last token in the symbol name
		sym_name = (*it);
	}

	int seperator_pos = sym_name.find_first_of(".[");
	if( seperator_pos!=-1 )
		sym_name = sym_name.substr(0,seperator_pos);

	// figure out where we are
	SymTab::SYMLIST::iterator it;
	Symbol::SCOPE scope;

	ContextMgr::Context c = gSession.contextmgr()->get_current();
	if( gSession.symtab()->getSymbol( sym_name, c, it ) )
		it->print(format,expr.substr(expr.find(' ')+1));
	else
		cout << "No symbol \""<<expr<<"\" in current context."<<endl;

	return true;
}

bool CmdRegisters::info( string cmd )
{
	if( cmd.length()==0 )
	{
		/*
		info registers
				PC  : 0x0031  RegisterBank 0:
				R0-7: 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
				ACC : 0x00 0 .
				B   : 0x00 0 .
				DPTR: 0x0000 0
				SP  : 0x07
				PSW : 0x00 | CY : 0 | AC : 0 | OV : 0 | P : 0
		*/
		unsigned char reg_psw, reg_acc, reg_b, reg_dpl, reg_dph, reg_sp;
		unsigned char reg_set[8], reg_bank;
		uint16_t reg_dptr;
		gSession.target()->read_sfr(0xd0,1,&reg_psw);
		reg_bank = (reg_psw>>3)&0x03;
		printf("PC  : 0x%04x  RegisterBank %i:\n",
			   gSession.target()->read_PC(), reg_bank );
		
		// dump the regs
		gSession.target()->read_data( reg_bank*8, 8, reg_set );
		printf("R0-7:");
		for(int i=0; i<8;i++)
			printf(" 0x%02x",reg_set[i]);
		printf("\n");
		
		// ACC
		gSession.target()->read_sfr(0xe0,1,&reg_acc);
		printf("ACC : 0x%02x %i %c\n", reg_acc,reg_acc, isprint(reg_acc) ? reg_acc : '.'  );
		
		// B
		gSession.target()->read_sfr(0xf0,1,&reg_b);
		printf("B   : 0x%02x %i %c\n", reg_b,reg_b,isprint(reg_b) ? reg_b : '.' );
		
		// DPTR
		gSession.target()->read_sfr(0x82,1,&reg_dpl);
		gSession.target()->read_sfr(0x83,1,&reg_dph);
		reg_dptr = (uint16_t(reg_dph)<<8) | reg_dpl;
		printf("DPTR: 0x%04x %i\n", reg_dptr, reg_dptr );
		
		// SP
		gSession.target()->read_sfr(0x81,1,&reg_sp);
		printf("SP  : 0x%02x\n", reg_sp );
		
		printf("PSW : 0x%02x | CY : %i | AC : %i | OV : %i | P : %i\n",
			   reg_psw,
			   (reg_psw>>7)&1,	// CY
			   (reg_psw>>6)&1,	// AC
			   (reg_psw>>2)&1,	// OV
			   reg_psw&1);		// P
		return true;
	}
	return false;
}
