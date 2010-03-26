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
#ifndef CMDMAINTENANCE_H
#define CMDMAINTENANCE_H
#include <string>
#include "parsecmd.h"

/** Implement matinance commands.
	These commands are mainly dfor debugging and testing newcdb.

	@author Ricky White <rickyw@neatstuff.co.nz>
*/
class CmdMaintenance : public CmdShowSetInfoHelp
{
public:
	CmdMaintenance()	{ name="Maintenance"; }
	bool direct( string cmd );
};

#endif
