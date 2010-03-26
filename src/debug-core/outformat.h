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
#ifndef OUTFORMAT_H
#define OUTFORMAT_H
#include <stdint.h>
#include <string>
#include "dbgsession.h"

/** Output Format for printing values
	Manages the pringing of various values given their specifiers as per the
	print format options.

	@author Ricky White <rickyw@neatstuff.co.nz>
*/
class OutFormat
{
public:
	OutFormat( DbgSession *session );
    ~OutFormat();

	typedef enum { ENDIAN_BIG, ENDIAN_LITTLE } ENDIAN;
	
	void set_endian( ENDIAN e );

	/** Print a memory location in GDB output format as specified.
		\param fmt			Format character as per gdb print
		\param flat_addr	Lowest address of object to print.
		\param size			Number of bytes
		\returns 			The formatted string.
	*/
	std::string print( char fmt, uint32_t flat_addr, uint32_t size );

	/** print the data at the specified address  as the specified type.
		\param fmt format to use when rendering the data '' results in default
		\param flat_addr atter to starrt reading the data from.
		\param type_name type name used to render the data.
		\returns The formatted string containing the rendered data.
	*/
	std::string print( char fmt, uint32_t flat_addr, std::string type_name );
	
private:
	DbgSession	*mSession;
	ENDIAN mTargetEndian;
	
	/** Read an unsigned integer from the device starting at the spcififed 
		address.
		The endian flag is obayed and size is the number of bytes.
	*/
	uint32_t get_uint( uint32_t flat_addr, char size );
	
	/** Read an signed integer from the device starting at the spcififed 
	address.
	The endian flag is obayed and size is the number of bytes.
	 */
	int32_t get_int( uint32_t flat_addr, char size );

};

#endif
