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
#ifndef LINESPEC_H
#define LINESPEC_H
#include "types.h"
#include "dbgsession.h"

/**
	@author Ricky White <ricky@localhost.localdomain>
*/
class LineSpec
{
public:
    LineSpec( DbgSession *session );
    ~LineSpec();
	
	typedef enum
	{
		LINENO,
		FUNCTION,
		PLUS_OFFSET,
		MINUS_OFFSET,
		ADDRESS,
		INVALID
	} TYPE;
	bool set( string linespec );
	
	
	TYPE		type()			{ return spec_type; }
	string		file()			{ return filename; }
	LINE_NUM	line()			{ return line_num; }
	string 		func()			{ return function; }
	ADDR		addr()			{ return address; }
	ADDR		end_addr()		{ return endaddress; }
	
protected:
	DbgSession 	*mSession;
	ADDR		address;		///< -1 = invalid, +ve or 0 is an address
	ADDR		endaddress;		///< -1 = invalid, +ve or 0 is an address
	string		filename;
	string		function;
	LINE_NUM	line_num;
	TYPE		spec_type;

};

#endif
