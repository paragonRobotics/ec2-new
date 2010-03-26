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
#include <assert.h>
#include <fstream>
#include <iostream>
#include "types.h"
#include "module.h"


Module::Module()
{
	reset();
}


Module::~Module()
{
}

/** Set the name of the module.
	\param name	Name of the module.
*/
void Module::set_name( string name )
{
	module_name = name;
}

/** Load the c file of the module.
	\param path	The path to the file to load including the filename and ext.
	\returns	true on success, false on failure
*/
bool Module::load_c_file( string path )
{
	c_file_path = path;
	c_file_name = path.substr( path.rfind('/')+1 );
	return load_file( path, c_src );
}

/** Load the asm file of the module.
	\param path	The path to the file to load including the filename and ext.
	\returns	true on success, false on failure
 */
bool Module::load_asm_file( string path )
{
	asm_file_path = path;
	asm_file_name = path.substr( path.rfind('/')+1 );
	return load_file( path, asm_src );
}

/** Load either a c or asm file into the moduel definition.
	The internal vector will have entries modified as necessary,  it may already
	have entires for each line if they were setup as the symbol table is loaded
	if that occurs first.

	\param path The path/filename of the file to load.
	\param srcvec	The vector to recieve the file.
	\returns true on success, false on failure
*/
bool Module::load_file( string path, SrcVec &srcvec )
{
	uint16_t	MAX_LINE_LEN = 4096;
	char		buf[MAX_LINE_LEN];
	ifstream	file;
	SrcLine		src_line;
	uint32_t i;
	i=1;

	file.open( path.c_str(), ios::in );
	if(file)
	{
		while( !file.eof() )
		{
			file.getline( buf, MAX_LINE_LEN );
			if( i > (srcvec.size()) )
				srcvec.resize( i+1 );
			srcvec[i-1].src = buf;
			i++;
		}
		file.close();
		return true;
	}
	else
	{
		cout << "ERROR: couldent open \"" << path << "\"" << endl;
		return false;
	}
}


uint32_t Module::get_c_block( LINE_NUM line )
{
	assert( line <= c_src.size() );
	return c_src[line-1].block;
}

uint32_t Module::get_c_level( LINE_NUM line )
{
	assert( line <= c_src.size() );
	return c_src[line-1].level;
}

Module::SrcLine Module::get_c_line( LINE_NUM line )
{
	assert( line <= c_src.size() );
	return c_src[line-1];
}

Module::SrcLine Module::get_asm_line( LINE_NUM line )
{
	assert( line <= c_src.size() );
	return asm_src[line-1];
}


/** Reset all internal module data structures.
	this allows the module to be reused.
*/
void Module::reset()
{
	c_file_name.clear();
	c_file_path.clear();
	c_src.clear();
	c_addr_map.clear();
	
	asm_file_name.clear();
	asm_file_path.clear();
	asm_src.clear();
	asm_addr_map.clear();
}


/** Set the block and level of the line in the c file (scope).
	\param line		L number ine of the source file the set the block/level for.
	\param block	Block number to set for the line.
	\param level	Level to set for the line.
	\returns		true on success, false on failure.
*/
bool Module::set_c_block_level( LINE_NUM line, uint32_t block, uint32_t level )
{
	if( line > (c_src.size()-1) )
		c_src.resize( line );

	// note c_str[0] <--> line 1
	c_src[line-1].block = block;
	c_src[line-1].level = level;
	return true;
}

void Module::set_c_addr( LINE_NUM line, ADDR addr )
{
	if( line > (c_src.size()-1) || c_src.size()==0 )
		c_src.resize( line );
	c_src[line-1].addr = addr;
	c_addr_map[addr] = line;
}

void Module::set_asm_addr( LINE_NUM line, ADDR addr )
{
	if( line > (asm_src.size()-1) || asm_src.size()==0 )
		asm_src.resize( line );
	int size = asm_src.size();
	asm_src[line-1].addr = addr;
	asm_addr_map[addr] = line;
}

void Module::dump()
{
	SrcVec::iterator it;
	for( it=c_src.begin(); it!=c_src.end(); ++it )
	{
		ADDR a = (*it).addr;
		if(a==-1)
			printf("\t\t[%s]\n",(*it).src.c_str());
		else
			printf("0x%08x\t[%s]\n",(*it).addr,(*it).src.c_str());
	}
	for( it=asm_src.begin(); it!=asm_src.end(); ++it )
	{
		ADDR a = (*it).addr;
		if(a==-1)
			printf("\t\t[%s]\n",(*it).src.c_str());
		else
			printf("0x%08x\t[%s]\n",(*it).addr,(*it).src.c_str());
	}

}

ADDR Module::get_c_addr( LINE_NUM line )
{
	return c_src[line-1].addr;
}

ADDR Module::get_asm_addr( LINE_NUM line )
{
	return asm_src[line-1].addr;
}

LINE_NUM Module::get_c_line( ADDR addr )
{
	AddrMap::iterator it = c_addr_map.find(addr);
	if( it != c_addr_map.end() )
	{
		// found it.
		return (*it).second;
	}
	return LINE_INVALID;
}

LINE_NUM Module::get_asm_line( ADDR addr )
{
	AddrMap::iterator it = asm_addr_map.find(addr);
	if( it != asm_addr_map.end() )
	{
		// found it.
		return (*it).second;
	}
	return LINE_INVALID;
}



///////////////////////////////////////////////////////////////////////////////
// Modules manager.
///////////////////////////////////////////////////////////////////////////////

ModuleMgr::ModuleMgr()
{
}

ModuleMgr::~ModuleMgr()
{
}

void ModuleMgr::reset()
{
	mMap.clear();
}

Module &ModuleMgr::add_module( string mod_name )
{
	ModMap::iterator it;
	it = mMap.find( mod_name );
	if( it==mMap.end() )
	{
		// add new entry.
		mMap[mod_name].set_name( mod_name );
	}
	return mMap[mod_name];
}

bool ModuleMgr::del_module( string mod_name )
{
	mMap.erase( mod_name );
}


/** dump the contants of a single module to the console
*/
void dump_module(const pair<string,Module>& pr)
{
	Module *m = (Module*)&pr.second;
	cout<< "Module: "
		<< m->get_name() << ", "
		<< m->get_c_num_lines() << " c lines, "
		<< m->get_asm_num_lines() << " asm lines"
		<< endl;
	
//	m->dump();
}

/** Dump a list of all modules loaded.
	Used for debugging only.
*/
const void ModuleMgr::dump()
{
//	for_each( mMap.begin(), mMap.end(), dump_module );
}


/** scan all modules looking for the speciifed address.
	\param[in]	addr	Address to search for.
	\param[out] module	Receives the module name.
	\param[out]	line	Received the line number
	\returns true on success, false on filure (not found)
*/
bool ModuleMgr::get_asm_addr( ADDR addr, string &module, LINE_NUM &line )
{
	ModMap::iterator it;
	
	// for each module
	for( it=mMap.begin(); it!=mMap.end(); ++it )
	{
		line = (*it).second.get_asm_line(addr);
		if( line != LINE_INVALID )
		{
			module = (*it).second.get_name();
			return true;
		}
	}
	return false;
}

bool ModuleMgr::get_c_addr( ADDR addr, string &module, LINE_NUM &line )
{
	ModMap::iterator it;
	
	// for each module
	for( it=mMap.begin(); it!=mMap.end(); ++it )
	{
		line = (*it).second.get_c_line(addr);
		if( line != LINE_INVALID )
		{
			module = (*it).second.get_name();
			return true;
		}
	}
	return false;
}
