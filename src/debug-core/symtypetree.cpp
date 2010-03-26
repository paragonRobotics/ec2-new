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
#include <iostream>
#include <iomanip>
#include "symtypetree.h"
#include "contextmgr.h"
#include "target.h"
#include "memremap.h"
#include "outformat.h"
#include "dbgsession.h"
using namespace std;

SymTypeTree::SymTypeTree( DbgSession *session )
	: mSession(session)
{
	clear();
#if 0
	// Add terminal types to the tree
	m_types.push_back(new SymTypeChar());
	m_types.push_back(new SymTypeUChar());
	m_types.push_back(new SymTypeShort());
	m_types.push_back(new SymTypeUShort());
	m_types.push_back(new SymTypeInt());
	m_types.push_back(new SymTypeUInt());
	m_types.push_back(new SymTypeLong());
	m_types.push_back(new SymTypeULong());
	m_types.push_back(new SymTypeFloat());
	m_types.push_back(new SymTypeSbit());
	/*	
	SymTypeStruct *a = new SymTypeStruct();
	a->set_name("test_1");
	m_types.push_back(a);
	a = new SymTypeStruct();
	a->set_name("astruct");
	m_types.push_back(a);
	
	a->add_member( "q", i );
	a->add_member( "r", i );
	a->add_member( "s", i );
	*/
#endif
}


SymTypeTree::~SymTypeTree()
{
}


/** Clear out the tree and add back the terminal types.
*/
void SymTypeTree::clear()
{
	m_types_scope.clear();
	m_types.clear();

	// Add terminal types to the tree
	m_types.reserve(10);
	m_types.push_back(new SymTypeChar(mSession));
	m_types.push_back(new SymTypeUChar(mSession));
	m_types.push_back(new SymTypeShort(mSession));
	m_types.push_back(new SymTypeUShort(mSession));
	m_types.push_back(new SymTypeInt(mSession));
	m_types.push_back(new SymTypeUInt(mSession));
	m_types.push_back(new SymTypeLong(mSession));
	m_types.push_back(new SymTypeULong(mSession));
	m_types.push_back(new SymTypeFloat(mSession));
	m_types.push_back(new SymTypeSbit(mSession));
}


bool SymTypeTree::add_type( SymType *ptype )
{
	m_types.push_back(ptype);
}

void SymTypeTree::dump()
{
	cout << setw(24) << left <<"Type name"
		<< setw(9)<< left << "Terminal"
		<< setw(8)<< left << "Size"
		<< setw(24)<< left << "Scope"
		<< endl;
	cout << string(80,'=') << endl;
	for( int i=0; i<m_types.size(); i++ )
	{
		cout << setw(24) << left << m_types[i]->name()
			<< setw(9) << left << boolalpha << m_types[i]->terminal()
			<< setw(8) << left <<  m_types[i]->size()
			<< setw(24) << left <<  m_types[i]->file()
			<< endl;
	}
	cout << endl;
}


void SymTypeTree::dump(std::string type_name)
{
	// find type
	for(int i=0; i<m_types.size(); i++)
	{
		cout << "checking "<<m_types[i]->name()<<endl;
		if(m_types[i]->name()==type_name)
		{
			cout << "Dumping type = '"<<type_name<<"'"<<endl;
			cout << m_types[i]->text() << endl;
			return;
		}
	}
	cout << "ERROR Type = '"<<type_name<<"' not found."<<endl;
}

#if 0
/** scan for the requested type name.
	\FIXME need scope and function / file
	\returns true on success, false if not found
*/
bool SymTypeTree::get_type( std::string type_name, SymType **type )
{
	for(int i=0; i<m_types.size(); i++)
	{
		if(match)
		{
			
			return true;
		}
	}
	return false;
}

#endif


/** Search for the specified type is the supplied context.
	Starts at closest scope and works out.
		1) blocks in the current function
		2) File scope
		3) Global scope

	\param name		Name of the type to search for.
	\param context	Context to search for the type
	\returns 		A pointer to the best matching type entry or NULL if not found
*/
SymType *SymTypeTree::get_type( std::string type_name,
								ContextMgr::Context context )
{
	//cout << "looking for '" << type_name << "'" << endl;
	
	/// @FIXME: this a a crude hack that just takes the first matching type and ignores the scope requirments
	for(int i=0; i<m_types.size(); i++)
	{
		if( m_types[i]->name()==type_name )
		{
			return m_types[i];
		}
	}
	
	return 0;	// not found
}


std::string SymTypeTree::pretty_print( SymType *ptype,
							char fmt,
							uint32_t flat_addr,
							std::string subpath )
{
	cout <<"Sorry Print not implemented for this type!"<<endl;
}


////////////////////////////////////////////////////////////////////////////////
// SymType
////////////////////////////////////////////////////////////////////////////////

bool SymType::read_memory( uint32_t flat_addr, uint32_t len, uint8_t *buf )
{
	char area;
	ADDR a = MemRemap::target( flat_addr, area );
	printf("reading from addr= 0x%04x\n",a);
	mSession->target()->read_data( a, len, buf );
	//memset( buf,0xFF,length );	// hack for test
	return true;
}

////////////////////////////////////////////////////////////////////////////////
// SymTypeChar / SymTypeUChar
////////////////////////////////////////////////////////////////////////////////

std::string SymTypeChar::pretty_print( char fmt,
										std::string name,
										uint32_t &addr )
{
	OutFormat of(mSession);
	std::string s;
	if( !name.empty() )
		s += name + '=';
	s += of.print( fmt==0 ? default_format() : fmt, addr, 1 );
	return s;
}


std::string SymTypeUChar::pretty_print( char fmt,
										std::string name,
										uint32_t &addr )
{
	OutFormat of(mSession);
	std::string s;
	if( !name.empty() )
		s += name + '=';
	s += of.print( fmt==0 ? default_format() : fmt, addr, 1 );
	return s;
}


std::string SymTypeInt::pretty_print( char fmt,
										std::string name,
										uint32_t &addr )
{
	OutFormat of(mSession);
	std::string s;
	if( !name.empty() )
		s += name + '=';
	s += of.print( fmt==0 ? default_format() : fmt, addr, size() );
	return s;
}


std::string SymTypeUInt::pretty_print( char fmt,
										std::string name,
										uint32_t &addr )
{
	OutFormat of(mSession);
	std::string s;
	if( !name.empty() )
		s += name + '=';
	s += of.print( fmt==0 ? default_format() : fmt, addr, size() );
	return s;
}

std::string SymTypeLong::pretty_print( char fmt,
										std::string name,
										uint32_t &addr )
{
	OutFormat of(mSession);
	std::string s;
	if( !name.empty() )
		s += name + '=';
	s += of.print( fmt==0 ? default_format() : fmt, addr, size() );
	return s;
}

std::string SymTypeULong::pretty_print( char fmt,
										std::string name,
										uint32_t &addr )
{
	OutFormat of(mSession);
	std::string s;
	if( !name.empty() )
		s += name + '=';
	s += of.print( fmt==0 ? default_format() : fmt, addr, size() );
	return s;
}

std::string SymTypeFloat::pretty_print( char fmt,
										std::string name,
										uint32_t &addr )
{
	OutFormat of(mSession);
	std::string s;
	if( !name.empty() )
		s += name + '=';
	s += of.print( fmt==0 ? default_format() : fmt, addr, size() );
	return s;
}



////////////////////////////////////////////////////////////////////////////////
// SymTypeStruct
////////////////////////////////////////////////////////////////////////////////


int32_t SymTypeStruct::size()
{
	int32_t size=0;
//	for( int i=0; i<m_members.size(); i++)
//		size += m_members[i].pType->size();
	return size;
}


// GDB standard text representation
std::string SymTypeStruct::text()
{
	char buf[255];
	string s;
	
	for(int i=0; i<m_members.size(); i++)
	{
		if( m_members[i].count!=1 )
			snprintf(buf,sizeof(buf),"%s %s[%i]\n",
					m_members[i].type_name.c_str(),
					m_members[i].member_name.c_str(),
					m_members[i].count);
		else
			snprintf(buf,sizeof(buf),"%s %s\n",
					 m_members[i].type_name.c_str(),
					 m_members[i].member_name.c_str() );
		s+=buf;
	}
	return s;
}


/** Add a member to a structure.
	\param name		Name of the member to add.
	\param ptype	pointer to the type to add.
	\param count	Count of number of items if the member is an array.

	This type only stores the names of the associated types.
	The lookup for each must be done manually for now.
*/
void SymTypeStruct::add_member( std::string member_name,
								std::string type_name,
								/*SymType *ptype,*/
								 uint16_t count )
{
	MEMBER m;
	m.member_name	= member_name;
	m.type_name		= type_name;
	m.count			= count;
	m_members.push_back(m);
	cout << "adding: member ='"<<member_name<<"', "<<
			"type = '"<<type_name<<"', "<<
			" count="<<count<<endl;
}
