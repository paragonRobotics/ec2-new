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
#include "dbgsession.h"
#include "target.h"
#include "symtab.h"
#include "symtypetree.h"
#include "breakpointmgr.h"
#include "module.h"
#include "targets51.h"
#include "targetsilabs.h"
#include "target-dummy.h"
#include <iostream>
#include <stdint.h>


const uint64_t ec2debugcore_version = EC2_PACK_VER(0,1,6);

/** Build a debug session.
	if no objects are passed in then the default ones are used.
*/
DbgSession::DbgSession(
				SymTab	*dbg_symtab,
				SymTypeTree *dbg_symtypetree,
				ContextMgr *dbg_contextmgr,
				BreakpointMgr *dbg_bpmgr,
				ModuleMgr *dbg_modulemgr )
	: 	mTarget(0)
{
	std::cout << "====================== DbgSession Constructor =========================" << endl;
	mSymTab = dbg_symtab ? dbg_symtab : new SymTab(this);
	mSymTree = dbg_symtypetree ? dbg_symtypetree : new SymTypeTree(this);
	mContextMgr = dbg_contextmgr ? dbg_contextmgr : new ContextMgr(this);
	mBpMgr = dbg_bpmgr ? dbg_bpmgr : new BreakpointMgr(this);
	mModuleMgr = dbg_modulemgr ? dbg_modulemgr : new ModuleMgr();
	cout <<"constructor this="<<this<<endl;
	Target *t;
	TargetDummy();
	mTarget = add_target( new TargetDummy() );
	add_target( new TargetS51() );
	add_target( new TargetSiLabs() );
}

DbgSession::~DbgSession()
{
}


/** Select the specified target driver.
	This fill clear all exsisting data so ytou will need to reload files
*/
bool DbgSession::SelectTarget( std::string name )
{
	TargetMap::iterator i = mTargetMap.find(name);
	if( i == mTargetMap.end() )
		return false;	// failure
	if( target() )
	{
		cout << "current target "<<target()->target_name()<<endl;
		if( target()->is_connected() )
		{
			mBpMgr->clear_all();
			// clean disconnect
			if(mTarget) target()->stop();
			if(mTarget) target()->disconnect();
		}
		// clear out the data structures.
		mSymTab->clear();
		mSymTree->clear();
		//mContextMgr->clear()	@FIXME contextmgr needs a clear or reset
		mModuleMgr->reset();
	}
	
	// select new target
	mTarget = (*i).second;
	cout << "selecting target "<<target()->target_name()<<endl;
	
	return true;
}


Target *DbgSession::add_target( Target *t )
{
	mTargetMap[t->target_name()] = t;

	TargetInfo ti;
	ti.name = t->target_name();
	ti.descr = t->target_descr();
	mTargetInfoVec.push_back(ti);

	return t;
}

