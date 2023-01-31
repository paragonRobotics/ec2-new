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
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include "cmdtarget.h"
#include "target.h"

using namespace std;

void print_target_error(string target_name)
{
  cout << "Error:"<< endl
       << "Target: "<< target_name <<"is not supported" << endl
       << "Please set correct target name and port." << endl
       << endl 
       << "Target:" << endl
       << "For silicon labs debuggers: set target SL51" << endl
       << "For the s51 simulator: set target S51" << endl
       << endl
       << "Port:" << endl
       << "For Serial debuggers: set target port /dev/ttyXXX" <<endl
       << "For USB debuggers: set target port USB" <<endl;
}

bool connect()
{
  string target_name;

  if(gSession.target())
    target_name = gSession.target()->target_name();
  
  if(target_name != "SL51" && target_name != "S51")
    print_target_error(target_name);
  if(gSession.target()->connect())
    cout<<"Connected to "<<gSession.target()->device()<<endl;
  else
    cout<<"Failed to connect to device" <<endl;
  return true;
}

bool CmdConnect::directnoarg()
{
  connect();
  return true;
}

bool CmdDisconnect::directnoarg()
{
	gSession.target()->disconnect();
	return true;
}

bool CmdTarget::direct( string cmd )
{
  //Provide a gateway to communicate with the target driver.
	return gSession.target()->command(cmd);
}

bool CmdTarget::directnoarg( )
{
  return this->help("");
}

bool CmdTarget::help( string cmd)
{
  cout << "Setup and connect/disconnect target device." <<endl
       << endl
       << "set target <TARGET>:    select target, 'SL51' or 'S51'" <<endl
       << "set target port <PORT>: debugger port, USB or '/dev/ttyXXX'" <<endl
       << "set target mode=C2:     set mode to C2" <<endl
       << "set target mode=JTAG:   set mode to JTAG" <<endl
       << endl
       << "connect:                connect to target device" <<endl
       << "set target connect:     same as 'connect' "<<endl
       << endl
       << "disconnect:             disconnect from target device" <<endl
       << "set target disconnect:  same as 'disconnect'" <<endl
       << endl
       << "info target:            display target information" <<endl;
  return true;
}

bool CmdTarget::set( string cmd )
{

	if(cmd == "SL51" || cmd == "S51")
    gSession.SelectTarget(cmd);

	if(cmd.find("port ") == 0)
  {
		gSession.target()->set_port(cmd.substr(5));
  }
	if(cmd == "connect")
  {
    connect();
    return true;
  }
	if(cmd == "disconnect")
		gSession.target()->disconnect();

  if(cmd == "mode=C2" || cmd == "mode=JTAG" || cmd == "mode=AUTO")
	  return gSession.target()->command(cmd);
  
  return true;  
}

//command: info target
bool CmdTarget::info( string cmd )
{
  cout <<"Target  = "<< gSession.target()->target_name() <<endl
       <<"Port    = "<< gSession.target()->port() <<endl
       <<"Device  = "<< gSession.target()->device() <<endl;
  printf("PC      = 0x%04x\n",gSession.target()->read_PC());
	cout <<"Status  = " << (gSession.target()->is_connected() ? 
			   "Connected" : "Disconnected") <<endl;
//  gSession.contextmgr()->dump();
  return true;
}
