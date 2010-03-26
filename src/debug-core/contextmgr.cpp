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
#include <stdio.h>
#include "module.h"
#include "symtab.h"
#include "contextmgr.h"
#include "breakpointmgr.h"
#include "target.h"

ContextMgr::ContextMgr( DbgSession *session )
	: mSession(session)
{
}


ContextMgr::~ContextMgr()
{
}

/** Set the current context to that of the specified address.
	\param addr	Address of instruction to update the context to.
*/
void ContextMgr::set_context( ADDR addr )
{
	/// \TODO integrate checks with the breakpoint manager for special breakpoints used to detect entry / exit of all functions in mode 'b'
	string module;
	LINE_NUM line;
	if( mSession->modulemgr()->get_c_addr( addr, module, line ) )
	{
		string file;
		mSession->symtab()->get_c_function( addr, file, cur_context.function );
		cur_context.mode = C;
		cur_context.module = module;
		cur_context.line = line;
		cur_context.c_line = line;
		if(mSession->symtab()->get_c_block_level(
									mSession->modulemgr()->module(module).get_c_file_name(),
									cur_context.c_line,
									cur_context.block,
									cur_context.level) )
			cout <<"found block/level "<<cur_context.block<<endl;
		else
			cout <<"coulden't find block/level, file = '"
					<< mSession->modulemgr()->module(module).get_c_file_name()<<"', "
					<<cur_context.c_line<<endl;
		cur_context.addr = addr;	// @FIXME we need this address to be the address of the c line for mapping but the asm addr for asm pc pointer on ddd
		//cur_context.function
		cur_context.asm_addr = addr;
	}
	else if( mSession->modulemgr()->get_asm_addr( addr, module, line ) )
	{
		cur_context.module = module;
		cur_context.line = line;
		cur_context.addr = addr;
		cur_context.asm_addr = addr;
	}
	else
	{
		cout <<"ERROR: Context corrupt!"<<endl;
		printf("addr = 0x%04x, module='%s', line=%i, pc=0x%04x\n",
			   addr,
			   module.c_str(),
			   line, mSession->target()->read_PC() );
	}
	
	mSession->bpmgr()->stopped(cur_context.addr);
}

/** Dumps the current context in a form parsable by ddd but also
	in a human readable form
*/
void ContextMgr::dump()
{
	printf("PC = 0x%04x\n",cur_context.addr);
	printf("Module:\t%s\n",cur_context.module.c_str());
	printf("Function:\t%s\n",cur_context.function.c_str());
	printf("Line:\t%i\n",cur_context.line);
	printf("Block:\t%i\n",cur_context.block);
	if( cur_context.mode==C )
	{
		printf("\032\032%s:%d:1:beg:0x%08x\n",
				mSession->modulemgr()->module(cur_context.module).get_c_file_name().c_str(),
				cur_context.line,
				cur_context.asm_addr);
	} else if(cur_context.mode==ASM )
	{
		printf("\032\032%s:%d:1:beg:0x%08x\n",
				mSession->modulemgr()->module(cur_context.module).get_asm_file_name().c_str(),
				cur_context.line,	// vas c_line
				cur_context.asm_addr);
	}
	else
		cout <<"INVALID mode"<<endl;
	
#if 0
	fprintf(stdout,"\032\032%s:%d:1:beg:0x%08x\n",
			currCtxt->func->mod->cfullname,
			currCtxt->cline+1,currCtxt->addr);
	else
		fprintf(stdout,"\032\032%s:%d:1:beg:0x%08x\n",
#endif
}

