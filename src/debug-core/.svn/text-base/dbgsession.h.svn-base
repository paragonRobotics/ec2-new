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
#ifndef DBGSESSION_H
#define DBGSESSION_H

#include <map>
#include <string>
#include <assert.h>
#include <iostream>
#include <vector>
#include <stdint.h>
class Target;
class SymTab;
class SymTypeTree;
class ContextMgr;
class BreakpointMgr;
class ModuleMgr;


extern const uint64_t ec2debugcore_version;
#define EC2DEBUG_MAJOR()	ec2debugcore_version>>32 & 0xFFFF
#define EC2DEBUG_MINOR()	(ec2debugcore_version>>16) & 0xFFFF
#define EC2DEBUG_BUILD()	ec2debugcore_version & 0xFFFF

#define EC2_PACK_VER( major, minor, build) \
	(((uint64_t)major&0xFFFF)<<32 | ((uint64_t)minor&0xFFFF)<<16 | ((uint64_t)build&0xFFFF))

#define EC2_CHECK_VER( major, minor, build) \
	( EC2_PACK_VER( major, minor, build ) == ec2debugcore_version )


/**
This class holds data about a single debug session

	@author Ricky White <rickyw@neatstuff.co.nz>
*/
class DbgSession
{
public:
	DbgSession( SymTab	*dbg_symtab = 0,
				SymTypeTree *dbg_symtypetree = 0,
				ContextMgr *dbg_contextmgr = 0,
				BreakpointMgr *dbg_bpmgr = 0,
				ModuleMgr *dbg_modulemgr = 0 );
    ~DbgSession();

	Target *target()			{ return mTarget; }
	SymTab *symtab()			{ assert(mSymTab);		return mSymTab; }
	SymTypeTree *symtree()		{ assert(mSymTree);		return mSymTree; }
	ContextMgr	*contextmgr()	{ assert(mContextMgr);	return mContextMgr; }
	BreakpointMgr *bpmgr()		{ assert(mBpMgr);		return mBpMgr; }
	ModuleMgr *modulemgr()		{ assert(mModuleMgr);	return mModuleMgr; }

	bool SelectTarget( std::string name );
	typedef std::map<std::string,Target*>	TargetMap;

	typedef struct
	{
		std::string name;
		std::string descr;
	} TargetInfo;
	typedef std::vector<TargetInfo> TargetInfoVec;
	TargetInfoVec get_target_info()	{ return mTargetInfoVec; }


private:
	Target			*mTarget;

	SymTab			*mSymTab;
	SymTypeTree		*mSymTree;
	ContextMgr		*mContextMgr;
	BreakpointMgr	*mBpMgr;
	ModuleMgr		*mModuleMgr;

	TargetMap		mTargetMap;
	TargetInfoVec	mTargetInfoVec;

	Target *add_target( Target *t );
};

#endif
