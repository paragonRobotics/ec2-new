/***************************************************************************
 *   Copyright (C) 2023 by Cjacker
 *   cjacker@foxmail.com 
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
#ifndef CMDREADWRITE_H
#define CMDREADWRITE_H
#include "parsecmd.h"

// meta class to provide help info for readX/writeX
class CmdReadwrite : public CmdTemplate
{
public:
  CmdReadwrite() { name = "readwrite"; }
  virtual bool help( string cmd);
};

class CmdReadpc : public CmdTemplate
{
public:
  CmdReadpc()  { name="Readpc"; }
  bool directnoarg();
  virtual bool help( string cmd);
};

class CmdReadcode : public CmdTemplate
{
public:
  CmdReadcode()  { name="Readcode"; }
  bool direct( string cmd );
  virtual bool help( string cmd);
};

class CmdReadxdata : public CmdTemplate
{
public:
  CmdReadxdata()  { name="Readxdata"; }
  bool direct( string cmd );
  virtual bool help( string cmd);
};

class CmdReaddata : public CmdTemplate
{
public:
  CmdReaddata()  { name="Readdata"; }
  bool direct( string cmd );
  virtual bool help( string cmd);
};

class CmdReadbit : public CmdTemplate
{
public:
  CmdReadbit()  { name="Readbit"; }
  bool direct( string cmd );
  virtual bool help( string cmd);
};

class CmdReadregister : public CmdTemplate
{
public:
  CmdReadregister()  { name="Readregister"; }
  bool direct( string cmd );
  virtual bool help( string cmd);
};

class CmdReadpsfr : public CmdTemplate
{
public:
  CmdReadpsfr()  { name="Readpsfr"; }
  bool direct( string cmd );
  virtual bool help( string cmd);
};

class CmdWritepc : public CmdTemplate
{
public:
  CmdWritepc()  { name="Writepc"; }
  bool direct( string cmd );
  virtual bool help( string cmd);
};

class CmdWritexdata : public CmdTemplate
{
public:
  CmdWritexdata()  { name="Writexdata"; }
  bool direct( string cmd );
  virtual bool help( string cmd);
};

class CmdWritedata : public CmdTemplate
{
public:
  CmdWritedata()  { name="Writedata"; }
  bool direct( string cmd );
  virtual bool help( string cmd);
};

class CmdWritepsfr : public CmdTemplate
{
public:
  CmdWritepsfr()  { name="Writepsfr"; }
  bool direct( string cmd );
  virtual bool help( string cmd);
};

class CmdWritebit : public CmdTemplate
{
public:
  CmdWritebit()  { name="Writebit"; }
  bool direct( string cmd );
  virtual bool help( string cmd);
};

class CmdWriteregister : public CmdTemplate
{
public:
  CmdWriteregister()  { name="Writeregister"; }
  bool direct( string cmd );
  virtual bool help( string cmd);
};

#endif
