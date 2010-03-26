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
#include "outformat.h"
#include <assert.h>
#include <iostream>
#include <sstream>
#include "memremap.h"
#include "symtab.h"
#include "target.h"

using std::string;

OutFormat::OutFormat( DbgSession *session )
	: mTargetEndian(ENDIAN_LITTLE), mSession(session)
{
}


OutFormat::~OutFormat()
{
}

string OutFormat::print( char fmt, uint32_t flat_addr, uint32_t size )
{
	std::ostringstream out;
	using std::endl;
	using std::hex;
	using std::oct;
	uint32_t i, j;
	bool active, flag, one;
	
	switch(fmt)
	{
		case 'x':
			out << showbase << hex << get_uint( flat_addr, size );
			break;
		case 'd': out << (int32_t)get_int( flat_addr, size );			break;
		case 'u': out << dec << get_uint( flat_addr, size );			break;
		case 'o': out << showbase << oct << get_uint( flat_addr, size );break;
		case 't':
			// integer in binary. The letter `t' stands for "two"
			// strips leading zeros
			j = get_uint( flat_addr, size );
			active = false;
			for( i=0; i<size; i++ )
			{
				one = j & ( 1<<(size-i-1) );
				active |= one;
				out << ( one && active ) ? '1' : '0';
			}
			break;
		case 'a':
			// Address
			// prints the address and the nearest preceding symbol
			// (gdb) p/a 0x54320
			// $3 = 0x54320 <_initialize_vx+396>
			out << hex << flat_addr;
			out << " <"
				<< mSession->symtab()->get_symbol_name_closest(flat_addr)
				<< ">";	/// @FIXME add symbol information
			break;		// Print as an address, both absolute in hexadecimal and as an offset from the nearest preceding symbol. You can use this format used to discover where (in what function) an unknown address is located:
		case 'c':
			// Regard as an integer and print it as a character constant.
			// This prints both the numerical value and its character 
			// representation. The character representation is replaced with
			// the octal escape `\nnn' for characters outside the 7-bit ASCII
			// range.
			j = get_uint( flat_addr, size );
			out << j << " '";
			if( j<0x20 || j>0x7e )
			{
				// non printable, use \nnn format
				out << '\\' << showbase << oct << j;
			}
			else
				out << char(j);	
			out << "'";
			break;	// Regard as an integer and print it as a character constant. 
		case 'f':
			// print as floating point
			if( size==4)
			{
				i = get_uint( flat_addr, 4 );
				out << *((float*)&i);
			}
			else
				out << "float not supported for this data type!";
			break;
		case 0:
			// Default format specifier for type
			out << "?";
			break;
		case 's':	// newcdb specific format, string
			j = get_uint( flat_addr, size );
			if( (j<0x20 || j>0x7e) && j!=0 )
				out << '\\' << showbase << oct << j; // use \nnn format
			else
				out << char(j);	
			break;
		default:
			out << "ERROR Unknown format specifier.";
	}
	return out.str();
}

string OutFormat::print( char fmt, uint32_t flat_addr, string type_name )
{
	using std::cout;
	/// @TODO lookup type name and begin rendering the data starting at flat_addr
	cout << "NOT IMPLEMENTED: "
		 << "string OutFormat::print( char fmt, uint32_t flat_addr, string "
		 << "type_name )"
		 << endl;
	return "NOT IMPLEMENTED";
}


uint32_t OutFormat::get_uint( uint32_t flat_addr, char size )
{
	uint32_t	result;
	uint8_t		*buf = new uint8_t[size];
	char		area;
	int			i;
	
	ADDR a = MemRemap::target(flat_addr, area );
	mSession->target()->read_data( a, size, buf );
	
	result = 0;
	if( mTargetEndian==ENDIAN_LITTLE )
	{
		for( i=0; i<size; i++ )
			result = (result<<8) | buf[size-i-1];
	}
	else if( mTargetEndian==ENDIAN_BIG )
	{
		for( i=0; i<size; i++ )
			result = (result<<8) | buf[i];
	}
	else
		assert(1==0);	// unsupported endian type
	return result;	
}


int32_t OutFormat::get_int( uint32_t flat_addr, char size )
{
	uint32_t v = get_uint( flat_addr, size );	// raw bit pattern
	// Sign extend
	int32_t mask = 1 << (size*8 - 1);
	return -(v & mask) | v;
}
