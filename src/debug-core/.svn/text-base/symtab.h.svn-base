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
#ifndef SYMTAB_H
#define SYMTAB_H
#include <list>
#include <vector>
#include "types.h"
#include "symbol.h"
#include "contextmgr.h"
#include "memremap.h"

using namespace std;

/**	Symbol tables for ec2cdb
	Consists of:
		- main symbol table
		- file / line number to address mapping
	
	@author Ricky White <ricky@localhost.localdomain>
*/
class SymTab{
public:
    SymTab( DbgSession *session );
    ~SymTab();
	typedef list<Symbol> SYMLIST;
	
	/** clear all tables, get read for the load of a new cdb file
	*/
	void clear();
	
	void addSymbol( Symbol sym );
	
	/** Obtain direct access to the symbol to modify it.
		Searches for th matching symbol.
		@FIXME dosen't seem safe, i think we need to compare more!, function name is necessary with local scope etc...  what about levels
		
		\depreciated
		\returns interator pointing at matching symbol, otherwise end()
	*/
	bool getSymbol( string file, Symbol::SCOPE scope, string name, SYMLIST::iterator &it);
	
	
	/** Looks for a sumbol that is accessable from the specified context.
		search order
		1) SCOPE_LOCAL
		2) SCOPE_FILE
		3) SCOPE_GLOBAL
	
		\param name		Name of the symbol to search for
		\param context	Context to search for symbol
		\param it 		iterator pointing at the symbol in the symbol table
		if true is returned this is a valid symbol.
		\returns 		true if symbol is found, false otherwise.
	*/
	bool getSymbol( string name,
					ContextMgr::Context context,
					SYMLIST::iterator &it);

	
	
	/** Compare two symbol objects (scope/name/level/block only)
		This is used to determin if an S entry is the same as a F entry etc.
		\param sym1	First symbol object to compare
		\param sym2	Second symbol object to compare
		\returns true if match otherwise false.
	*/
	bool compare( Symbol &sym1, Symbol &sym2 );
	
	/** comparea the scope/name/level/block of the supplied symbol and looks
		it up in the symbol table.
		If it is found it is returned otherwise a new entry is made and filled
		in with the scope/name/level/block which is then returned
		@TODO: need to handle file scope vars correctly.
		\returns 0 on error, otherwise a pointer to a symbol
	*/
	Symbol *getSymbol( Symbol sym );
	
	
	/** get a symbol given its location in memory.
		Exact matches only.
		\param addr	Address to look for symbol at
	*/
	string get_symbol_name( FLAT_ADDR addr );
	
	/** get a symbol given its location in memory.
		\param addr	Address to look for symbol at
		\returns the name of the sysbol at the address or the closest preceeding symbol name.
	*/
	string get_symbol_name_closest( FLAT_ADDR addr );


	void dump();
	void dump_symbols();
	void dump_c_lines();
	void dump_asm_lines();
	void dump_functions();

	///////////////////////////////////////////////////////////////////////////
	// address from functions
	///////////////////////////////////////////////////////////////////////////
	/** Get the address that relates to a specific line number in a file
		\returns >=0 address, -1 = failure
	*/
	int32_t get_addr( string file, int line_num );
	/** Get the address of the start of a function in a file
		\returns >=0 address, -1 = failure
	 */
	bool get_addr( string file, string function, int32_t &addr, int32_t &endaddr );
	
	
	/** Get the address of the start of a function (any file)
		\returns >=0 address, -1 = failure
	 */
	int32_t get_addr( string function );
	bool get_addr( string function, int32_t &addr, int32_t &endaddr );
	
	///////////////////////////////////////////////////////////////////////////
	// Find file location based on address
	///////////////////////////////////////////////////////////////////////////
	//bool find_c_file_line( uint16_t addr, string &file, int &line_num );
	bool find_c_file_line( ADDR addr, string &file, LINE_NUM &line_num );
	bool find_asm_file_line( uint16_t addr, string &file, int &line_num );
	
	///////////////////////////////////////////////////////////////////////////
	// Adding file entries
	///////////////////////////////////////////////////////////////////////////
	bool add_c_file_entry( string name, int line_num, int level, int block, uint16_t addr );
	bool add_asm_file_entry( string name, int line_num, uint16_t addr );
	bool add_function_file_entry( string file_name, string func_name, int line_num, uint16_t addr );
	bool add_function_file_entry( string file_name, int line_num, uint16_t addr );

	///////////////////////////////////////////////////////////////////////////
	// reverse lookups from address.
	///////////////////////////////////////////////////////////////////////////
	bool get_c_line( ADDR addr, string &module, LINE_NUM &line  );

	bool get_c_function( ADDR addr,
						 string &file,
						 string &func);
	bool get_c_block_level( string file,
							LINE_NUM line,
							BLOCK &block,
							LEVEL &level );

protected:
	SYMLIST m_symlist;
	typedef struct
	{
		int		file_id;
		string	function;
		int		line_num;
		int		level;
		int		block;
		int16_t	addr;
	} FILE_ENTRY;
	typedef list<FILE_ENTRY> FILE_LIST;
	FILE_LIST	c_file_list;
	FILE_LIST	asm_file_list;
	
	typedef vector<string>	FILE_VEC;
	FILE_VEC	file_map;
	int file_id(string filename);
	string file_name( int id );
	
	
	typedef struct
	{
		string	name;
		int		file_id;
		int32_t	start_addr;
		int32_t	end_addr;
		int		line_num;
	} FUNC_ENTRY;
	typedef list<FUNC_ENTRY> FUNC_LIST;
//	FUNC_LIST	func_list;
	DbgSession *mSession;
};

#endif
