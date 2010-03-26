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
#ifndef CONTEXTMGR_H
#define CONTEXTMGR_H
#include <string>
#include "types.h"
#include "dbgsession.h"

/** This class manages the context tracking of the debugger.

It has two modes:

	a) no tracking, just holds current state.
	b) full traching, keeps track of current stack frame etc.

mode 'a' is useful for external debuggers with limited numbers of breakpoints
whereas mode'b'dbg_target requires very large numbers of breakpoints and is only suitable
for use with simulators.

	@author Ricky White <rickyw@neatstuff.co.nz>
*/
class ContextMgr
{
public:
	typedef enum { ASM, C } MODE;
	typedef struct
	{
		std::string	module;
		ADDR		addr;	// address of current c line
		ADDR		asm_addr;
		LINE_NUM	line;		///< @depreciated
		LINE_NUM	c_line;
		LINE_NUM	asm_line;
		MODE		mode;
		BLOCK		block;
		LEVEL		level;
		std::string	function;
	} Context;

	ContextMgr( DbgSession *session );
    ~ContextMgr();
	void dump();
	void set_context( ADDR addr );
	Context get_current()				{ return cur_context; }

protected:
	DbgSession *mSession;
	Context cur_context;
};

#endif
