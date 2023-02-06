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
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <stdio.h>
#include <iterator>
#include <sstream>
#include <regex>
#include "types.h"
#include "module.h"
#include "symtab.h"
#include "linespec.h"
#include "cmddisassemble.h"
#include "memremap.h"
#include "target.h"
#include "newcdb.h"
#include "utils.h"

static bool print_asm_line( ADDR start, ADDR end, string function );


/** Disassemble commend
	disassemble [startaddr [endaddress]]
*/
bool CmdDisassemble::direct( string cmd )
{
	vector<string> tokens;
	vector<string>::iterator it;
	Tokenize(cmd, tokens);
	ADDR start=-1, end=-1;
	
	if( tokens.size()==1 )
	{
    if(is_hex(tokens[0]))
    {
		  // start only
		  start = strtoul(tokens[0].c_str(),0,0);
		  /// @FIXME: need a way to get a symbols address, given the symbol and module and vice versa, give an address and get a symbol
		  string file, func;
		  gSession.symtab()->get_c_function( start, file, func );
		  cout << "Dump of assembler code for function "<<func<<":" << endl;
		  print_asm_line( start, end, func );
		  cout << "End of assembler dump." << endl;
    } else {
      SymTab::SYMLIST::iterator it;
      ContextMgr::Context c = gSession.contextmgr()->get_current();
      if( gSession.symtab()->getSymbol( tokens[0], c, it ) ) 
      {
        print_asm_line((*it).addr(), (*it).endAddr(), tokens[0]);
      } else
        cout << "No symbol \""<<tokens[0]<<"\" in current context."<<endl;
    }
		return true;
	}
	else if( tokens.size()==2 )
	{
    if(!is_hex(tokens[0]) || !is_hex(tokens[1]))
    {
      cout << "please specify start and end address in hex form." <<endl;
      return true;
    }
		// start and end
		start = strtoul(tokens[0].c_str(),0,0);
		end = strtoul(tokens[1].c_str(),0,0);
//		printf("start=0x%04x, end=0x%04x\n",start,end);
		string file, func;
		gSession.symtab()->get_c_function( start, file, func );
		cout << "Dump of assembler code for function "<<func<<":" << endl;
		print_asm_line( start, end, func );
		cout << "End of assembler dump." << endl;
		return true;
	}
	else
		return false;
}

string str_replace_with(string subject, const string& search,
                          const string& replace) {
    size_t pos = 0;
    while((pos = subject.find(search, pos)) != std::string::npos) {
         subject.replace(pos, search.length(), replace);
         pos += replace.length();
    }
    return subject;
}

//FIXME, where is LABEL
static bool print_asm_line( ADDR start_addr, ADDR end, string function )
{
	uint32_t asm_lines;
	ADDR delta;
	ADDR sym_addr;
	ADDR last_addr;
	string sym_name;
	bool printedLine=false;
	
	string module;
	LINE_NUM line;

  ADDR start = start_addr;
  while(start >= start_addr && start <= end)
  {
	  gSession.modulemgr()->get_asm_addr( start, module, line );
	  Module &m = gSession.modulemgr()->module(module);
	  asm_lines = m.get_asm_num_lines();
	  last_addr = start+1;
	  sym_addr = start;
	  sym_name.clear();

	  sym_addr = start;
	  sym_name = function;
	  int32_t i,j;

	  for ( j=0, i=1; i <= asm_lines; i++ ) 
	  {
	  	if ( start >= 0 && m.get_asm_addr(i) < start)
	  	{
        //hack for asm label
        if(printedLine) {
          string asm_src_line = m.get_asm_src(i);
          string line = remove_duplicate_space(trim(asm_src_line));
          //remove possible comments
          if(line.find(';') <line.length())
            line.erase(line.find(';'));

          if(!line.empty() && line.compare(line.size()-1, 1, ":") == 0)
            printf("%20c\t%s\n", ' ', line.c_str());
        }
        //end hack
	  		continue;
	  	}
	  	if ( end >= 0 && m.get_asm_addr(i) > end)
	  	{
	  		continue;
	  	}
	  	if( !function.empty() )
	  	{
	  		ADDR sfunc,efunc;
	  		gSession.symtab()->get_addr( function, sfunc, efunc );
	  		if( m.get_asm_addr(i) < sfunc ||
	  			m.get_asm_addr(i) > efunc )
	  			continue;
	  	}
	  	delta = m.get_asm_addr(i) - sym_addr;
	  	if ( delta >= 0 )
	  	{
	  		j++;
	  		last_addr = m.get_asm_addr(i);
        
        //printf("%s\n",gSession.symtab()->get_symbol_name(last_addr).c_str());
	  		printf( "0x%04x <%s", last_addr, sym_name.c_str() );
	  		printf( "+%5d", delta );
	  		printf( ">:\t%s\n", m.get_asm_src(i).c_str() );
	  		printedLine=true;
	  	} 
	  }
    //update start address and continue.
    start = start+last_addr+1;  
    //hack for asm label
    printedLine = false;
  }
	return printedLine;
}

bool CmdDisassemble::help(string cmd)
{
  cout << "Disassemble a specified section of memory." <<endl
       << endl
       << "disassemble FUNC:  disassemble 'FUNC' function" <<endl
       << endl
       << "disassemble START_ADDR:" <<endl
       << "- disassemble section from START_ADDR to the end of symbol/function" <<endl
       << endl
       << "disassemble START_ADDR END_ADDR:" <<endl
       << "- disassemble section from START_ADDR to END_ADDR" <<endl;
  return true;
}
