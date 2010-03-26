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
#ifndef MODULE_H
#define MODULE_H
#include <string>
#include <map>
#include <vector>
#include "types.h"
using namespace std;

/**
Holds all information about a single module

	@author Ricky White <rickyw@neatstuff.co.nz>
*/
class Module
{
public:
	class SrcLine
	{
	public:
		SrcLine()	{ addr = -1; block=0; level=0; }
		ADDR addr;
		uint32_t block, level;	// scope information
		string src;				// actual source line
	};
	
	Module();
    ~Module();
	
	void set_name( string name );
	bool load_c_file( string path );
	bool load_asm_file( string path );
	
	bool set_c_block_level( LINE_NUM line, uint32_t block, uint32_t level );
	bool set_asm_block_level( LINE_NUM line, uint32_t block, uint32_t level );
	void set_c_addr( LINE_NUM line, ADDR addr );
	void set_asm_addr( LINE_NUM line, ADDR addr );
	LINE_NUM get_c_line( ADDR addr );
	LINE_NUM get_asm_line( ADDR addr );
	
	void reset();
	void dump();
	// accessors
	uint32_t get_c_block( uint32_t line );
	uint32_t get_c_level( uint32_t line );
	SrcLine get_c_line( uint32_t line );
	ADDR get_c_addr( LINE_NUM line );
	ADDR get_asm_addr( LINE_NUM line );
	
	SrcLine get_asm_line( uint32_t line );
	const string &get_name()			{ return module_name; }
	const string &get_c_file_path()		{ return c_file_path; }
	const string &get_c_file_name()		{ return c_file_name; }
	const string &get_asm_file_path()	{ return asm_file_path; }
	const string &get_asm_file_name()	{ return asm_file_name; }
	uint32_t get_c_num_lines()			{ return c_src.size(); }
	uint32_t get_asm_num_lines()		{ return asm_src.size(); }
	string get_asm_src( LINE_NUM line )	{ return asm_src[line-1].src; }
	
protected:
	typedef vector<SrcLine> SrcVec;
	typedef map<ADDR,LINE_NUM> AddrMap;
	;
	string		module_name;
	string		c_file_name;
	string		c_file_path;
	SrcVec		c_src;
	AddrMap		c_addr_map;

	string		asm_file_name;
	string		asm_file_path;
	SrcVec		asm_src;
	AddrMap		asm_addr_map;
	
	bool load_file( string path, SrcVec &srcvec );


#if 0
	// old struct from sdcdb
	typedef struct module {
		char *cfullname ;        /* full name Includeing path for the module */
		char *afullname;         /* fullname of assembly file */
		char *name ;             /* name of module */
		char *c_name;            /* c filename     */
		char *asm_name;          /* asm file name  */
		int   ncLines;           /* number of lines in this module */
		int   nasmLines;         /* # of lines in the assembler file */
		srcLine  **cLines;       /* actual source lines */    
		srcLine  **asmLines;     /* actual assembler source lines*/
		set       *cfpoints;     /* set of double line execution points */   
	} module;
#endif
};



/** manages all currently exsisting modules in the project.
*/
class ModuleMgr
{
public:
	ModuleMgr();
	~ModuleMgr();
	void reset();
	Module &add_module( string mod_name );
	Module &module( string mod_name )	{ return add_module(mod_name ); }	// fixme need a variant of this that won't create new entries as this quick hack does.
	bool del_module( string mod_name );
	const void dump();
	bool get_asm_addr( ADDR addr, string &module, LINE_NUM &line );
	bool get_c_addr( ADDR addr, string &module, LINE_NUM &line );
	
protected:
	//void dump_module(const pair<string,Module>& pr);
	typedef map<string,Module> ModMap;
	ModMap mMap;
};

#endif
