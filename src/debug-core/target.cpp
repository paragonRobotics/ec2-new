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
#include <iostream>
#include <string.h>
#include "target.h"
#include "ihex.h"

using namespace std;

Target::Target()
	: force_stop(false)
{
}


Target::~Target()
{
}

void Target::print_buf_dump( char *buf, int len )
{
	const int PerLine = 16;
	int i, addr;

	for( addr=0; addr<len; addr += PerLine )
	{
		printf("%04x\t",(unsigned int)addr);
		// print each hex byte		
		for( i=0; i<PerLine; i++ )
			printf("%02x ",(unsigned int)buf[addr+i]&0xff);
		printf("\t");
		for( i=0; i<PerLine; i++ )
			putchar( (buf[addr+i]>='0' && buf[addr+i]<='z') ? buf[addr+i] : '.' );
		putchar('\n');
	}
}

/** Default implementation, load an intel hex file and use write_code to place
	it in memory
*/
bool Target::load_file( string name )
{
	uint16_t start, end;
	char *buf = new char[0x10000];
	memset(buf,0x00,0x10000);
	cout << "Loading file '"<<name<<"'"<<endl;
	if( buf && ihex_load_file( name.c_str(), buf, &start, &end) )
	{
		print_buf_dump( buf, end-start );
		write_code( start, end-start+1, (unsigned char*)&buf[start] );
		delete buf;
		return true;
	}
	delete buf;
	return false;
}

void Target::stop()
{
	force_stop = true;
}

bool Target::check_stop_forced()
{
	if( force_stop )
	{
		force_stop = false;
		return true;
	}
	return false;
}





/** derived calsses must call this function to ensure the cache is updated.
*/
void Target::write_sfr( uint8_t addr,
						uint8_t page,
	  					uint8_t len,
						unsigned char *buf )
{
	SFR_PAGE_LIST::iterator it;
	it = cache_get_sfr_page(page);
	
	if( it!=mCacheSfrPages.end() )
	{
		// update values in cache
		memcpy( (*it).buf+(addr-0x80), buf, len );
	}
}

void Target::invalidate_cache()
{
	mCacheSfrPages.clear();
}

void Target::read_sfr_cache(uint8_t addr,
							uint8_t page,
							uint8_t len,
							unsigned char *buf )
{
	SFR_PAGE_LIST::iterator it;
	it = cache_get_sfr_page(page);
	
	if( it==mCacheSfrPages.end() )
	{
		// not in cache, read it and cache it.
		SFR_CACHE_PAGE page_entry;
		page_entry.page = page;
		read_sfr( 0x80, page_entry.page, 128, page_entry.buf );
		mCacheSfrPages.push_back(page_entry);
		memcpy( buf, page_entry.buf+(addr-0x80), len );
	}
	else
	{
		// in cache
		memcpy( buf, (*it).buf+(addr-0x80), len );
	}
}

