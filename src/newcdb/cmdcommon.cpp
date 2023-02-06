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
#include <sstream>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <fstream>
#include <limits>


#include "types.h"
#include "cmdcommon.h"
#include "target.h"
#include "module.h"
#include "cdbfile.h"
#include "breakpointmgr.h"
#include "linespec.h"
#include "contextmgr.h"
#include "newcdb.h"

#include "utils.h"

using namespace std;


/** top level help
*/
bool CmdHelp::parse( string cmd )
{
	if( cmd.compare("help")==0 )
	{
    cout << "Please refer to:" <<endl
         << endl
         << "help target:      setup/connect to target device"<<endl
         << "help breakpoints: add/delete breakpoints" <<endl
         << "help readwrite:   read/write Rn/SFR/bit and memory address" <<endl
         << "help disassemble: disassemble a speficied section" <<endl
         << "help control:     run/step/continue the program" <<endl
         << "help info:        query various information" <<endl
         << "help file:        load and list files" <<endl
				 <<endl;
		return true;
	}
	return false;
}

bool CmdControl::help(string cmd)
{
  cout << "Run/step/continue the program" <<endl
       << endl
       << "Please refer to:"<<endl
       << endl
       << "help run:      to read value of PC" <<endl
       << "help step:     to step one instruction/function" <<endl
       << "help stepi:    to step one ASM instruction" <<endl
       << "help next:     same as 'step'" <<endl
       << "help nexti:    same as 'stepi'" <<endl
       << "help continue: to continue execution from the current address" <<endl;
  return true;
}

bool CmdInfo::help(string cmd)
{
  cout << "Display various information" <<endl
       << endl
       << "info target:     to display target device information" <<endl
       << "info registers:  to list important registers and their values" <<endl
       << "info functions:  to display function information of current project" <<endl
       << "info line:       to display source line information of current context"<<endl
       << "info source:     to display source file of current context"<<endl
       << "info sources:    to list source files of current project" <<endl
       << "print <SYMBOL>:  to display SYMBOL type and addr" <<endl;
  return true;
}


/** cause the target to step one source level instruction
*/
bool CmdStep::directnoarg()
{
  //ASM MODE, run next instruction.
  if(gSession.contextmgr()->get_current().mode == 0)
  {
    ADDR addr = gSession.target()->step();
    gSession.bpmgr()->stopped(addr);
    gSession.contextmgr()->set_context(addr);
    gSession.contextmgr()->dump();
  } else { //C MODE, do loop until hit another c function.
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
	}
	return true;
}

bool CmdStep::help(string cmd)
{
  cout << "Step one instruction/function" <<endl
       << endl
       << "step: "<<endl
       << "- step one instruction for asm source" <<endl
       << "- step one function for c source" <<endl;
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

bool CmdStepi::help(string cmd)
{
  cout << "Step one instruction" <<endl
       << endl
       << "stepi: step one ASM instruction" <<endl;
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
  //ASM MODE, run next instruction.
  if(gSession.contextmgr()->get_current().mode == 0)
  {
    ADDR addr = gSession.target()->step();
    gSession.bpmgr()->stopped(addr);
    gSession.contextmgr()->set_context(addr);
    gSession.contextmgr()->dump();
    return true;
  } else { //C MODE, do loop until hit another c function.
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
  }
	return true;
}

bool CmdNext::help(string cmd)
{
  cout << "As same as 'step', step one instruction/function" <<endl
       << endl
       << "next: "<<endl
       << "- step one instruction for asm source" <<endl
       << "- step one function for c source" <<endl;
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

bool CmdNexti::help(string cmd)
{
  cout << "As same as 'stepi', step one instruction" <<endl
       << endl
       << "stepi: step one ASM instruction" <<endl;
  return true;
}

/**	Continue execution from the current address
	if there is a breakpoint on the current address it is ignored.
	optional parameter specifies a further number of breakpoints to ignore
*/
bool CmdContinue::direct( string cmd )
{
	printf("Continuing.\n");
  if(!is_digit(cmd))
  {
    printf("Wrong argument %s, please specify a correct breakpoint number to run to\n", cmd.c_str());
    return true;
  }
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

bool CmdContinue::help(string cmd)
{
  cout << "Continue execution from the current address" <<endl
       << endl
       << "continue: run to next breakpoint" <<endl
       << "continue [NUM]: run to breakpoint NUM" <<endl;
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

bool CmdRun::help(string cmd)
{
  cout << "Reset target, reload breakpoints and run" <<endl
       << "run:   run program" <<endl;
  return true;
}
/** open a new cdb file for debugging
	all associated files must be in the same directory
*/	
bool CmdFile::direct( string argstr)
{
  if(!gSession.target()->is_connected())
  {
    return true;
  }

  string cdbfilename = trim(argstr);

  if(!cdbfilename.find("."))
  {
    cout <<cdbfilename <<" is illegal cdbfile name." <<endl;
    return true;
  }

  if(access(cdbfilename.c_str(), F_OK ) == -1)
  {
    cout <<cdbfilename<<" not exist."<<endl; 
    return true;
  }
  
  vector<string> fv = split(cdbfilename,'.');
  
  string ihxfilename = fv[0] + ".ihx";


  if(access(ihxfilename.c_str(), F_OK ) == -1)
  {
    cout <<ihxfilename<<" not exist."<<endl; 
    return true;
  }

	gSession.modulemgr()->reset();
	gSession.symtab()->clear();
	gSession.symtree()->clear();	
	gSession.bpmgr()->clear_all();

	// disconnect and reconnect to make sure data is valid (fixes bug where
	//   data reads are wrong after loading new file)
	gSession.target()->disconnect();
	gSession.target()->connect();
	CdbFile cdbfile(&gSession);
	cdbfile.open(cdbfilename);
	gSession.target()->load_file(ihxfilename);

  return true;
}

bool CmdFile::help(string cmd)
{
  cout << "Open cdb file for debugging" <<endl
       << endl
       << "file <filename>.cdb: open <filename>.cdb for debugging" <<endl
       << "- NOTE: All associated files must be in the same directory" <<endl
       << endl
       << "To list source files, refer to:" <<endl
       << "help list"<<endl;
  return true;
}
/** open a new cdb file for debugging, WITHOUT loading the firmware to the device
	all associated files must be in the same directory
*/	
bool CmdDFile::direct( string argstr)
{
  if(!gSession.target()->is_connected())
  {
    cout << "target not connected" << endl;
    return true;
  }
  
  string cdbfilename = trim(argstr);

  if(access(cdbfilename.c_str(), F_OK ) == -1)
  {
    cout <<cdbfilename<<" not exist."<<endl; 
    return true;
  }

	gSession.modulemgr()->reset();
	gSession.symtab()->clear();
	gSession.symtree()->clear();	
	gSession.bpmgr()->clear_all();

	CdbFile cdbfile(&gSession);
	cdbfile.open(cdbfilename);
	return true;
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

bool CmdList::set( string cmd )
{
  if(cmd.find("size ") != 0)
  {
    cout<<"set list size <n>" <<endl;
    return true;
  }

  string num = remove_duplicate_space(cmd).substr(5);
  if(!is_digit(num) || num == "0")
  {
    cout<<"Please specify a valid value of listsize" <<endl;
    return true;
  }

  this->listsize = atoi(num.c_str());
  return true;
}

/*
List specified function or line.
With no argument, lists ten more lines after or around previous listing.
"list -" lists the ten lines before a previous ten-line listing.
One argument specifies a line, and ten lines are listed around that line.
Two arguments with comma between specify starting and ending lines to list.
Lines can be specified in these ways:
  LINENUM, to list around that line in current file,
  FILE:LINENUM, to list around that line in that file,
  FUNCTION, to list around beginning of that function,
  FILE:FUNCTION, to distinguish among like-named static functions.
  *ADDRESS, to list around the line containing that address.
With two args, if one is empty, it stands for ten lines away from
the other arg.

By default, when a single location is given, display ten lines.
This can be changed using "set listsize", and the current value
can be shown using "show listsize".
*/

bool CmdList::direct( string cmd )
{
  string argstr = trim(cmd);
  //FIXME: filter out - + *, not supported now.
  if(argstr == "-" || argstr == "+" || argstr[1] == '*')
  {
    printf("not supported\n");
    return true;
  }
  
  string ls_str = argstr;
  LineSpec ls(&gSession);

  // if it's a digit number and no current list file.
  if(is_digit(argstr)) 
    if(this->current_list_file.filename.empty())
    {
      ContextMgr::Context cur_context = gSession.contextmgr()->get_current();
      ADDR addr;
      //mode 1 for C, 0 for ASM
      if( cur_context.mode == 1 )
        addr = cur_context.addr;
      else
        addr = cur_context.asm_addr;

      //convert it to string
      char addr_str[8];
      sprintf(addr_str,"*0x%x",addr);

      if(!ls.set(addr_str))
        return true;

      ls_str=ls.file()+":"+argstr;
    } else
      ls_str = this->current_list_file.filename+":"+argstr;

  if(!ls.set(ls_str))
    return true;
   
  cout << ls.file() <<ls.line() <<endl; 
  this->current_list_file.filename = ls.file();
  this->current_list_file.linenum = ls.line();

  this->directnoarg();

	return true;
}

bool CmdList::directnoarg()
{
  string filename;
  uint32_t line_num;

  if(!this->current_list_file.filename.empty())
  {
    filename = this->current_list_file.filename; 
    line_num = this->current_list_file.linenum;
  }

  if(this->current_list_file.filename.empty() || gSession.bpmgr()->is_stopped()) 
  {
    gSession.bpmgr()->clear_stopped();

    ContextMgr::Context cur_context = gSession.contextmgr()->get_current();

    ADDR addr;
    //mode 1 for C, 0 for ASM
    if( cur_context.mode == 1 )
      addr = cur_context.addr;
    else
      addr = cur_context.asm_addr;

    //convert it to string
    char addr_str[8];
    sprintf(addr_str,"*0x%x",addr);

    LineSpec ls(&gSession);
    
    if(!ls.set(addr_str))
      return true;

    filename = ls.file();
    line_num = ls.line();

    this->current_list_file.filename = filename;
    this->current_list_file.linenum = line_num;
  }

  if(line_num == 0)
    line_num = 1;

  string line ;
  ifstream file(filename);
  file.seekg(std::ios::beg);
  for(int i=0; i < line_num - 1; ++i){
    file.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
  }
  
  int i = line_num;
  while (!file.fail() && !file.eof() && (i < line_num + this->listsize))
  {
    getline(file, line);
    printf("%4d\t", i);
    cout << line << endl;
    i++;
  }

  if(file.eof())
  {
    printf("End of file '%s'\n", filename.c_str());
  }
 
  file.close();

  // update context.
  this->current_list_file.filename = filename;
  this->current_list_file.linenum = i;

	return true;
}


bool CmdList::help(string cmd)
{
  cout << "List contents of source file" <<endl
       << endl
       << "set list size <N>: display N line one time" <<endl
       << endl
       << "list FUNCTION:       list sources from the start of FUNCTION" <<endl
       << "list FILE:FUNCTION:  list sources from the start of FUNCTION in FILE" <<endl
       << "list FILE:LINE_NUM:  list sources from the LINE_NUM of FILE" <<endl
       << "list *ADDR:          list the corresponding sources of ADDR" <<endl
       << "list:                continue list the current file or the source of current context"<<endl;
  return true; 
}

bool CmdPWD::directnoarg()
{
  char cwd[PATH_MAX];
  if (getcwd(cwd, sizeof(cwd)) != NULL)
    printf("Workding directory '%s'\n", cwd);
	return true;
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
	cout << "Not implement" << endl;
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
  if(!gSession.target()->is_connected())
  {
    return true;
  }

	string sym_name = expr;
	char format = 0;

	// Split 'expr' into spaces. Read out the flag
	std::stringstream ss(expr);
	std::string item;
	while (std::getline(ss, item, ' ')) {
		if (item.length() == 2 && item[0] == '/')
			format = item[1];

		// last token in the symbol name
		sym_name = item;
	}

	int seperator_pos = sym_name.find_first_of(".[");
	if( seperator_pos!=-1 )
		sym_name = sym_name.substr(0,seperator_pos);

	// figure out where we are
	SymTab::SYMLIST::iterator it;
	Symbol::SCOPE scope;

	ContextMgr::Context c = gSession.contextmgr()->get_current();
	if( gSession.symtab()->getSymbol( sym_name, c, it ) )
  {
		it->print(format,expr.substr(expr.find(' ')+1));
    if( (*it).isFunction() )
    {
        printf("function addr = 0x%04x\n", (*it).addr());
    }
	} else
		cout << "No symbol \""<<expr<<"\" in current context."<<endl;

	return true;
}

bool CmdPrint::help(string cmd)
{
  cout << "Print infomation of SYMBOL" <<endl
       << endl
       << "print SYMBOL:  print type and addr of SYMBOL" <<endl;
  return true;
}

bool CmdRegisters::directnoarg()
{
  this->info("");
  return true;
}

bool CmdRegisters::info( string cmd )
{
  if(!gSession.target()->is_connected())
  {
    return true;
  }

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
		printf("PC  : 0x%04x\nRegisterBank %i:\n",
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
		
		printf("PSW : 0x%02x |CY:%i|AC:%i|F0:%i|RS1:%i|RS0:%i|OV:%i|--:%i|P:%i|\n",
			   reg_psw,
			   (reg_psw>>7)&1,	// CY
			   (reg_psw>>6)&1,	// AC
			   (reg_psw>>5)&1,	// F0
			   (reg_psw>>4)&1,	// RS1
			   (reg_psw>>3)&1,	// RS0
			   (reg_psw>>2)&1,	// OV
			   (reg_psw>>1)&1,	// --
			   reg_psw&1);		// P
	}
	return true;
}

bool CmdRegisters::help( string cmd )
{
  cout << "List important registers and their values" <<endl
       << endl
       << "info registers: list PC, RegisterBank, R0-R7, A, B, DPTR, SP, PSW" <<endl;
  return true;
}

bool CmdFunctions::info( string cmd )
{
  gSession.symtab()->dump_functions();
  return true;
}

bool CmdFunctions::help( string cmd )
{
  cout << "Dump all functions and it's start/end address" <<endl
       << endl
       << "info functions:  dump function infomation" <<endl;
  return true;
}

/**  	
	Examples

	(gdb) info line m4_changequote
	Line 895 of "builtin.c" starts at pc 0x634c and ends at 0x6350.
	
	We can also inquire (using *addr as the form for linespec) what source line covers a particular address:

	(gdb) info line *0x63ff
	Line 926 of "builtin.c" starts at pc 0x63e4 and ends at 0x6404.
*/

//TODO, end_addr is always 0x0000
void linespec_with_str(string str)
{
  LineSpec ls(&gSession);
  if( ls.set( str ) )
  {
    // test.c:19
    printf("\032\032%s:%i\n",
         ls.file().c_str(),
         ls.line()
        );
  }
  else
    printf("No line number information available.\n");
}

bool CmdLine::info( string cmd )
{
  if(!gSession.target()->is_connected())
  {
    return true;
  }

  //FIXME
  //
  //when enter breakpoint, 'info line' should show current line.
  //otherwise, say "No line number information available."
	if( cmd.empty() )
	{
    ADDR addr = gSession.contextmgr()->get_current().addr;
    char addr_str[8];
    sprintf(addr_str,"*0x%x",addr);
    linespec_with_str(addr_str);
    return true;
  }
     
  linespec_with_str(cmd);
  return true;
}

bool CmdLine::help(string cmd)
{
  cout << "Show source line information of current context" <<endl
       << endl
       << "info line: show source filename and line number" <<endl;
  return true;
}

/** info files and info target are synonymous; both print the current target
*/
bool CmdFiles::info( string cmd )
{
	cout <<"Not implement"<<endl;	// @TODO put correct pathe in here
	return true;
}

// get it from symtab
// main.asm also listed. 

//Current source file is b.c
//Compilation directory is /home/cjacker
//Located in /home/cjacker/gdb/b.c
//Contains 6 lines.
//Source language is c.
bool CmdSource::info( string cmd )
{
  if( cmd.empty() )
  {
    ADDR addr = gSession.contextmgr()->get_current().addr;
    char addr_str[8];
    sprintf(addr_str,"*0x%x",addr);
  
    LineSpec ls(&gSession);
    if( ls.set( addr_str ) )
    {
      cout <<"Current source file is '"<<ls.file()<<"'"<<endl;       
    }
    return true;
  }
  return false;
}

bool CmdSource::help(string cmd)
{
  cout << "Show source filename of current context" <<endl
       << endl
       << "info source: show source filename of current context" <<endl;
  return true;
}

//list all sources
bool CmdSources::info( string cmd )
{
	if(cmd.empty())
	{
    vector<string> file_map = gSession.symtab()->get_file_map();
    
    int i=0;
    while( i<file_map.size() )
    {
      cout <<file_map[i]<<endl;
      i++;
    }
	  return true;
	}
	return false;
}

bool CmdSources::help(string cmd)
{
  cout << "Show source filenames of current project" <<endl
       << endl
       << "info sources: show all source filenames of current project" <<endl;
  return true;
}




