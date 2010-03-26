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
#ifndef CDBFILE_H
#define CDBFILE_H
#include <string>
#include "symtab.h"
#include "symtypetree.h"
#include "dbgsession.h"

using namespace std;


/**
Load a cdb file into the symbol table


	@author Ricky White <ricky@localhost.localdomain>
*/
class CdbFile
{
public:
    CdbFile( DbgSession *session );
    ~CdbFile();
	bool open( string filename );
	bool parse_record( string line );
	
protected:
	int		parse_type_chain_record( string line );
	bool	parse_type_chain_record( string line, Symbol &sym, int &pos  );
	bool	parse_linker( string line );
	bool	parse_level_block_addr( string line, Symbol &sym, int &pos, bool bStartAddr=true );
	bool	parse_scope_name( string data, Symbol &sym, int &pos );
	bool	parse_type( string line );
	bool	parse_type_member( string line, int &spos, SymTypeStruct *t  );
	bool	parse_symbol_record( string line, int &spos, SymTypeStruct *t  );
	bool	parse_struct_member_dcl( string line,
									 int &spos,
									 std::string name,
									 SymTypeStruct *t );
	string	cur_module;
	string	cur_file;
	DbgSession *mSession;

};

#endif
