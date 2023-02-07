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
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <stdio.h>
#include <iterator>
#include <sstream>
#include <regex>
#include "types.h"
#include "module.h"
#include "symtab.h"
#include "linespec.h"
#include "memremap.h"
#include "target.h"
#include "newcdb.h"
#include "utils.h"

#include "cmdreadwrite.h"

bool CmdReadwrite::help(string cmd)
{
  cout << "Read/Write from/to Rn/SFR registers and" <<endl 
       << "internal/external Data/Code[only read] spaces."<<endl
       << endl
       << "Please refer to:"<<endl
       << endl
       << "help readpc:             to read value of PC" <<endl
       << "help readbit:            to read value of bits" <<endl
       << "help readregister:       to read values of Rn and SFR registers" <<endl
       << "help readpsfr:           to read values of paged SFR" <<endl
       << "help readdata:           to read values of internal data space" <<endl
       << "help readxdata:          to read values of external data space" <<endl
       << "help readcode:           to read values of code space" <<endl
       << endl
       << "help writepc:            to write to PC" <<endl
       << "help writebit:           to write value to a bit addr" <<endl
       << "help writeregister:      to write value to a Rn/SFR register" <<endl
       << "help writepsfr:          to write value to paged SFR" <<endl
       << "help writedata:          to write value to internal data addr" <<endl
       << "help writexdata:         to write value to external data addr" <<endl;
  return true;
}

bool CmdReadpc::directnoarg()
{
  printf("PC=0x%04x\n", gSession.target()->read_PC());
  return true;
}

bool CmdReadpc::help(string cmd)
{
  cout << "Read the value of Program Counter" <<endl 
       << "readpc:    read the value of PC" <<endl;
  return true;
}

// Read raw code from code space
// cmd: readcode start_addr [n_byte]
bool CmdReadcode::direct(string argstr)
{
  vector<string> argv = split(remove_duplicate_space(argstr), ' ');

  if(argv.size() > 2) {
    printf("Wrong arguments, 'readcode start_addr [n_byte]'");
    return true;
  }  

  uint16_t start_addr = 0x00;
  uint16_t n_byte = 1; 

  if(!is_hex(argv[0])) {
    printf("Wrong code address: %s\n", argv[0]);
    return true;
  } else {
    long hex = hex_to_num(argv[0]);

    if(hex > 0xffff) {
      printf("Max valid code address is 0xffff(64k)\n");
      return true;
    } else {
      start_addr = (uint16_t)hex & 0xffff; 
    }
  }

  if(argv.size() == 2) {
    if(is_hex(argv[1]))
      n_byte = hex_to_num(argv[1]);
    else if(is_digit(argv[1]))
      n_byte = atoi(argv[1].c_str());
    else {
      printf("Wrong byte number: %s\n", argv[1].c_str());
      return true;
    }
  }
 
  if(n_byte > 0xffff - start_addr)
    n_byte = 0xffff - start_addr;

  if(n_byte == 0)
    n_byte = 1;
  
  unsigned char *buffer = (unsigned char *)malloc(n_byte);
  memset(buffer, 0, n_byte);

	gSession.target()->read_code( start_addr, n_byte, buffer);

  for( int i = 0; i<n_byte; i++ ) {
  	printf("0x%02x ", buffer[i]);
    if(i != 0 && i%8 == 0)
      printf("\n");
  }
  printf("\n");

  free(buffer);
  
  return true; 
}
bool CmdReadcode::help(string cmd)
{
  cout << "Read n bytes from code space starting at addr" <<endl
       << "readcode start_addr [nbyte]:" <<endl
       << "- read 'nbyte' raw codes from 'start_addr'" <<endl
       << "- 'nbyte' can be omitted for 1 byte" <<endl;
  return true;
}

// cmd: readxdata start_addr [b_nyte]
bool CmdReadxdata::direct(string argstr)
{
  vector<string> argv = split(remove_duplicate_space(argstr), ' ');

  if(argv.size() > 2) {
    printf("Wrong arguments, 'readxdata start_addr [n_byte]'");
    return true;
  }  

  uint16_t start_addr = 0x00;
  uint16_t n_byte = 1; 

  if(!is_hex(argv[0])) {
    printf("Wrong xdata address: %s\n", argv[0]);
    return true;
  } else {
    long hex = hex_to_num(argv[0]);

    if(hex > 0xffff) {
      printf("Max valid xdata address is 0xffff(64k)\n");
      return true;
    } else {
      start_addr = (uint16_t)hex & 0xffff; 
    }
  }

  if(argv.size() == 2) {
    if(is_hex(argv[1]))
      n_byte = hex_to_num(argv[1]);
    else if(is_digit(argv[1]))
      n_byte = atoi(argv[1].c_str());
    else {
      printf("Wrong byte number: %s\n", argv[1].c_str());
      return true;
    }
  }
 
  if(n_byte > 0xffff - start_addr)
    n_byte = 0xffff - start_addr;

  if(n_byte == 0)
    n_byte = 1;
  
  unsigned char *buffer = (unsigned char *)malloc(n_byte);
  memset(buffer, 0, n_byte);

	gSession.target()->read_xdata( start_addr, n_byte, buffer);

  for( int i = 0; i<n_byte; i++ ) {
  	printf("0x%02x ", buffer[i]);
    if(i != 0 && i%8 == 0)
      printf("\n");
  }
  printf("\n");

  free(buffer);
  
  return true; 
}

bool CmdReadxdata::help(string cmd)
{
  cout << "Read n bytes from xdata space starting at addr" <<endl
       << "readxdata start_addr [nbyte]:" <<endl
       << "- read 'nbyte' from xdata space at 'start_addr'" <<endl
       << "- 'nbyte' can be omitted for 1 byte" <<endl;
  return true;
}

// cmd: readdata start_addr [n_byte]
bool CmdReaddata::direct(string argstr)
{
  vector<string> argv = split(remove_duplicate_space(argstr), ' ');

  if(argv.size() > 2) {
    printf("Wrong arguments, 'readdata start_addr [n_byte]'");
    return true;
  }  

  uint8_t start_addr = 0x00;
  uint8_t n_byte = 1; 

  if(!is_hex(argv[0])) {
    printf("Wrong data address: %s\n", argv[0]);
    return true;
  } else {
    long hex = hex_to_num(argv[0]);

    if(hex > 0xff) {
      printf("Max valid data address is 0xff(256 byte)\n");
      return true;
    } else {
      start_addr = (uint16_t)hex & 0xff; 
    }
  }

  if(argv.size() == 2) {
    if(is_hex(argv[1]))
      n_byte = hex_to_num(argv[1]);
    else if(is_digit(argv[1]))
      n_byte = atoi(argv[1].c_str());
    else {
      printf("Wrong byte number: %s\n", argv[1].c_str());
      return true;
    }
  }
 
  if(n_byte > 0xff - start_addr)
    n_byte = 0xff - start_addr;

  if(n_byte == 0)
    n_byte = 1;
  
  unsigned char *buffer = (unsigned char *)malloc(n_byte);
  memset(buffer, 0, n_byte);

	gSession.target()->read_data( start_addr, n_byte, buffer);

  for( int i = 0; i<n_byte; i++ ) {
  	printf("0x%02x ", buffer[i]);
    if(i != 0 && i%8 == 0)
      printf("\n");
  }
  printf("\n");

  free(buffer);
  
  return true; 
}

bool CmdReaddata::help(string cmd)
{
  cout << "Read n bytes from data space starting at addr" <<endl
       << "readdata start_addr [nbyte]:" <<endl
       << "- read 'nbyte' from data space at 'start_addr'" <<endl
       << "- 'nbyte' can be omitted for 1 byte" <<endl;
  return true;
}

pair<uint8_t, uint8_t> bit_addr(string bitstring)
{
  //use dot notation: byte_addr.offset
  uint8_t byte_addr = 0x00;
  uint8_t offset = 0x00;

  map<string, pair<uint8_t,uint8_t>>::iterator it;

    // look up bit_map first
  it = bit_map.find(bitstring);
  if(it != bit_map.end()) {
    string bit_name = it->first;
    pair<uint8_t, uint8_t> p = it->second;
    byte_addr = p.first;
    offset = p.second;
    return make_pair(byte_addr, offset);
  // bit number notation
  } else if(is_hex(bitstring)) {
  	long hex = hex_to_num(bitstring);
  	if(hex > 0xff)
  	{
			printf("%s - Not valid bit number\n", bitstring.c_str());
      return make_pair(0, 0);
  	}

  	uint8_t bit_num = hex & 0xff;

  	if(bit_num >= 0x80) {
  	  byte_addr = bit_num & 0xf8;
  	  offset = bit_num & 0x07;
      return make_pair(byte_addr, offset);
    //ram addr: 0x20 - 0x2f
  	} else if(bit_num < 0x80) {
  	  byte_addr = (int)(bit_num/8) + 0x20;
  	  offset = (uint8_t)(bit_num - (byte_addr-0x20)*8);
      return make_pair(byte_addr, offset);
  	} else {
			printf("%s - Not bit addressable\n", bitstring);
      return make_pair(0, 0);
  	}
  }
    // bit dot notation
  else if(bitstring.find("."))
  {
      //verify it
    vector<string> bv = split(bitstring,'.');
    if(!is_hex(bv[0]))
    {
      printf("%s - wrong bit addr in dot notation.\n", bitstring);
      return make_pair(0, 0);
    }
    
    uint8_t byte_part = hex_to_num(bv[0]); 
    if(byte_part & 0x7 != 0)
    {
      printf("%s - Not bit addressable.\n", bitstring);
      return make_pair(0, 0);
    }
    
    if(!is_digit(bv[1]))
    {
      printf("%s - wrong bit addr in dot notation.\n", bitstring);
      return make_pair(0, 0);
    }

    uint8_t offset_part = atoi(bv[1].c_str()); 
    if(offset_part > 7)
    {
      printf("%s : wrong bit addr in dot notation.\n", bitstring);
      return make_pair(0, 0);
    }

    byte_addr = byte_part;
    offset = offset_part;
    if(byte_addr >= 0x80 || (byte_addr >= 0x20 && byte_addr <= 0x2f)) {
      return make_pair(byte_addr, offset);
    } else
      return make_pair(0, 0);
  } 
  return make_pair(0, 0);
}

//cmd: readbit bit1 bit2 ...
//by bitname, bit number notation, bit dot notation.

bool CmdReadbit::direct(string argstr)
{
  //convert it to uppercase from map lookup.
  transform(argstr.begin(), argstr.end(), argstr.begin(), ::toupper);

  vector<string> bitv = split(remove_duplicate_space(argstr), ' ');

  //use dot notation: byte_addr.offset
  uint8_t byte_addr = 0x00;
  uint8_t offset = 0x00;

  map<string, pair<uint8_t,uint8_t>>::iterator it;

  for (int i = 0; i < bitv.size(); i++)
  {
    // look up bit_map first
    it = bit_map.find(bitv[i]);
    if(it != bit_map.end()) {
      string bit_name = it->first;
      pair<uint8_t, uint8_t> p = it->second;
      byte_addr = p.first;
      offset = p.second;
			//all byte_addr from bit_map is SFR bits
			uint8_t value;
      gSession.target()->read_sfr( byte_addr, 1, &value );
      uint8_t bit_value = value>>offset & 1;
      printf("%s(0x%02X.%d)\t= %i\n", bit_name.c_str(), byte_addr, offset, bit_value);
    // bit number notation
    } else if(is_hex(bitv[i])) {
  	  long hex = hex_to_num(bitv[i]);
  	  if(hex > 0xff)
  	  {
				printf("%s - Not valid bit number\n", bitv[i].c_str());
				continue;
  	  }

  	  uint8_t bit_num = hex & 0xff;

  	  if(bit_num >= 0x80) {
  	    byte_addr = bit_num & 0xf8;
  	    offset = bit_num & 0x07;

				uint8_t value;
        gSession.target()->read_sfr( byte_addr, 1, &value );
        uint8_t bit_value = value>>offset & 1;
        printf("0x%02X.%d(%s)\t= %i\n", byte_addr, offset, bitv[i].c_str(), bit_value);
      //ram addr: 0x20 - 0x2f
  	  } else if(bit_num < 0x80) {
  	    byte_addr = (int)(bit_num/8) + 0x20;
  	    offset = (uint8_t)(bit_num - (byte_addr-0x20)*8);

				uint8_t value;
        gSession.target()->read_data( byte_addr, 1, &value );
        uint8_t bit_value = value>>offset & 1;
        printf("0x%02X.%d(%s)\t= %i\n", byte_addr, offset, bitv[i].c_str(), bit_value);
  	  } else {
				printf("%s - Not bit addressable\n", bitv[i].c_str());
				continue;
  	  }
  	}
    // bit dot notation
    else if(bitv[i].find("."))
    {
      //verify it
      vector<string> bv = split(bitv[i],'.');
      if(!is_hex(bv[0]))
      {
        printf("%s - wrong bit addr in dot notation.\n", bitv[i].c_str());
				continue;
      }
      
      uint8_t byte_part = hex_to_num(bv[0]); 
      if(byte_part & 0x7 != 0)
      {
        printf("%s - Not bit addressable.\n", bitv[i].c_str());
				continue;
      }
      
      if(!is_digit(bv[1]))
      {
        printf("%s - wrong bit addr in dot notation.\n", bitv[i].c_str());
				continue;
      }

      uint8_t offset_part = atoi(bv[1].c_str()); 
      if(offset_part > 7)
      {
        printf("%s : wrong bit addr in dot notation.\n", bitv[i].c_str());
				continue;
      }

      byte_addr = byte_part;
      offset = offset_part;

      uint8_t value;
      if(byte_addr >= 0x80) {
        gSession.target()->read_sfr( byte_addr, 1, &value );
        uint8_t bit_value = value>>offset & 1;
        printf("0x%02X.%d\t= %i\n", byte_addr, offset, bit_value);
      }
      else if(byte_addr >= 0x20 && byte_addr <= 0x2f) 
			{
        uint8_t value;
        gSession.target()->read_data( byte_addr, 1, &value );
        uint8_t bit_value = value>>offset & 1;
        printf("0x%02X.%d\t = %i\n", byte_addr, offset, bit_value);
      }
			else
			{
        printf("%s : Not bit addressable.\n", bitv[i].c_str());
				continue;
			}
    } 
	}
    
  return true;
}

bool CmdReadbit::help(string cmd)
{
  cout << "Read the values of bits" <<endl
       << "readbit BIT1 BIT2 BIT3 ..." <<endl
       << "- support standard bit name, such as 'C', 'P0.0'" <<endl
       << "- support bit addr by dot notation, such as '0x80h.0'" <<endl
       << "- support bit addr by number notation, such as '0x80h'" <<endl;
  return true;
}

// Read registers and SFRs
// cmd: readregister reg1 reg2 reg3 ... 
// by name or by addr.
bool CmdReadregister::direct(string argstr)
{
  transform(argstr.begin(), argstr.end(), argstr.begin(), ::toupper);

  vector<string> regv = split(remove_duplicate_space(argstr), ' ');

  map<string , uint8_t>::iterator it;

  for (int i = 0; i < regv.size(); i++)
  {
    // look up reg_map first
    it = reg_map.find(regv[i]);
    if(it != reg_map.end()) {
      string reg_name = it->first;
      uint8_t addr = it->second;
      uint8_t value;
      //all map element is SFR.
		  gSession.target()->read_sfr(addr, 1, &value);
      printf("%4s(0x%02X)\t= 0x%02X\n", reg_name.c_str(), addr, value);  
    } 
    // for DPTR
    else if (str_icase_equal(regv[i], "dptr")) 
    {
			uint16_t dptr;
      // DPH
			uint8_t dph;
      gSession.target()->read_sfr((uint8_t)0x83, 1, &dph);
      // DPL
			uint8_t dpl;
      gSession.target()->read_sfr((uint8_t)0x82, 1, &dpl);
			dptr = uint16_t(dph)<<8 | dpl;
      printf("DPTR(0x83 0x82)\t= 0x%04X\n", dptr);  
    }
    //process Rn register
    else if (str_icase_equal(regv[i], "r0") ||
             str_icase_equal(regv[i], "r1") ||
             str_icase_equal(regv[i], "r2") ||
             str_icase_equal(regv[i], "r3") ||
             str_icase_equal(regv[i], "r4") ||
             str_icase_equal(regv[i], "r5") ||
             str_icase_equal(regv[i], "r6") ||
             str_icase_equal(regv[i], "r7"))
    {
      // setup R0-R7 addr according to reg bank.
      uint8_t reg_psw, reg_bank;
      gSession.target()->read_sfr(0xd0, 1, &reg_psw);
      reg_bank = (reg_psw>>3)&0x03;
      //printf("RegisterBank %i:\n",reg_bank);
      uint8_t r0_addr = reg_bank*8;
      // get register number. 
      uint8_t n = regv[i][1] - '0';
      uint8_t value = 0x00; 
      gSession.target()->read_data(r0_addr + n, 1, &value );
      printf("%2s(0x%02X)\t= 0x%02X\n", regv[i].c_str(), r0_addr + n, value);  
    }
    // for addr in hex format.
    else
    {
      if(!is_hex(regv[i])) {
        printf("%s - Wrong Register address\n", regv[i].c_str());
        continue;
 			}

      long hex = hex_to_num(regv[i]);
   
      if(hex > 0xff) {
        printf("%s - Wrong Register address\n", regv[i].c_str());
        continue;
      }

      uint8_t addr = hex & 0xff;
      uint8_t value;

      //if SFR
      if(addr >= 0x80 && addr <= 0xff)
        gSession.target()->read_sfr( addr, 1, &value );
      //if Rn
      else if(addr <= 0x20)
        gSession.target()->read_data( addr, 1, &value );

      printf("0x%02X\t= 0x%02X\n", addr, value);  
    }
  }

  return true;
}

bool CmdReadregister::help(string cmd)
{
  cout << "Read the values of Rn/SFR registers of current SFR_PAGE(if has)." <<endl
       << "readregister R1 R2 R3 ..." <<endl
       << "- support standard register/SFR names, such as 'R0', 'PSW'" <<endl
       << "- support register/SFR addr, such as '0x80h'" <<endl
       << "- register bank will be considered when reading Rn value" <<endl
       << endl
       << "To read DPTR value, you can use:" <<endl
       << "readregister DPTR or readregister DPH DPL" <<endl
       << endl
       << "To read Rn value of other register bank, please consider to use:" <<endl
       << "readdata 'addr' or readregister 'addr'"<<endl
       << endl
       << "Refer to datasheet for SFR_PAGE infomation" <<endl;
  return true;
}

bool CmdReadpsfr::direct(string cmd)
{
  transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);

  vector<string> regv = split(remove_duplicate_space(cmd), ' ');

  if(regv.size() < 2) 
  {
    printf("readpsfr page_number sfr1 sfr2 sfr3 ...\n");
    return true;
  }

  //page number
	if(!is_hex(regv[0]))
  {
		printf("Please specify sfrpage value as first arg\n");
    printf("- For sfrpage, please check the datasheet\n");
    return true;
	}
  
  long hex = hex_to_num(regv[0]);
  if(hex > 0xff)
  {
    printf("Wrong sfr page number: %s\n", regv[0].c_str());
    return true;
  }
   
  uint8_t page_number = hex & 0xff;

  map<string , uint8_t>::iterator it;

  for (int i = 1; i < regv.size(); i++)
  {
    // look up reg_map first
    it = reg_map.find(regv[i]);
    if(it != reg_map.end()) {
      string reg_name = it->first;
      uint8_t addr = it->second;
      uint8_t value;
		  gSession.target()->read_sfr(addr, page_number, 1, &value);
      printf("%4s(0x%02X)\t= 0x%02X\n", reg_name.c_str(), addr, value);  
    } 
    // for DPTR
    else if (str_icase_equal(regv[i], "dptr")) 
    {
			uint16_t dptr;
      // DPH
			uint8_t dph;
      gSession.target()->read_sfr((uint8_t)0x83, page_number, 1, &dph);
      // DPL
			uint8_t dpl;
      gSession.target()->read_sfr((uint8_t)0x82, page_number, 1, &dpl);

			dptr = uint16_t(dph)<<8 | dpl;
      printf("DPTR(0x83 0x82)\t= 0x%04X\n", dptr);  
    }
    // for addr in hex format.
    else
    {
      if(!is_hex(regv[i])) {
        printf("%s - Wrong Register address\n", regv[i].c_str());
        continue;
      }

      long hex = hex_to_num(regv[i]);

      //only for paged SFR 
      if(hex > 0xff || hex < 0x80) {
        printf("%s - Wrong Register address\n", regv[i].c_str());
        continue;
      }

      uint8_t addr = hex & 0xff;
      uint8_t value;
      gSession.target()->read_sfr( addr,page_number, 1, &value );
      printf("0x%02X\t= 0x%02X\n", addr, value);
    }
  }
  return true;
}

bool CmdReadpsfr::help(string cmd)
{
  cout << "Read the values of paged SFR (if device has SFR_PAGE support)." <<endl
       << "readpsfr PAGE_NUM SFR1 SFR2 SFR3 ..." <<endl
       << "- support standard SFR names, such as 'P0', 'PSW'" <<endl
       << "- support SFR addr, such as '0x80h'" <<endl
       << endl
       << "To read DPTR value, you can use:" <<endl
       << "readpsfr DPTR or readpsfr DPH DPL" <<endl
       << endl
       << "Refer to datasheet for SFR_PAGE infomation" <<endl;
  return true;
}

bool CmdWritepc::direct(string pcstr)
{
  if(!is_hex(pcstr))
  {
    printf("Wrong argument format, plese specify a hex value\n");
    return true;
  }
  
  long pc_hex = hex_to_num(pcstr);
  if(pc_hex > 0xffff)
  {
    printf("Range of PC is from 0x0000 to 0xffff\n");
    return true;
  }
  
  uint16_t pc = pc_hex & 0xffff;
  
  printf("Setting PC to 0x%04x\n", pc);
  gSession.target()->write_PC(pc);
  return true; 
}

bool CmdWritepc::help(string cmd)
{
  cout << "Write value to Program Counter. "<<endl
       << "writepc VALUE: write 'VALUE' to PC" <<endl
       << "Range of 'VALUE' is from 0x0000 to 0xffff" <<endl;
  return true;
}
//write a bit
//

bool CmdWritebit::direct(string argstr)
{
  if(argstr.find("=") == 0)
  {
    printf("Wrong argument format, 'writebit ADDR=<0|1>\n'");
    return true;
  }

  vector<string> bev = split(argstr, '=');

  if (bev.size() != 2)
  {
    printf("Wrong argument format, 'writebit ADDR=<0|1>\n'");
    return true;
  }

  string target = bev[0];
  string val = bev[1];

  if(val != "0" && val != "1")
  {
    printf("Wrong argument format, value should be 0 or 1\n");
    return true;
  }

  pair<uint8_t, uint8_t> bp = bit_addr(bev[0]);
  uint8_t byte = bp.first;
  uint8_t offset = bp.second;
  uint8_t value = (uint8_t)(val.c_str()[0] -'0');

  //illegal
  if(byte == 0)
    return true;
  
  uint8_t tv = 0x00;

  //sfr
  if(byte >= 0x80)
  {
    gSession.target()->read_sfr( byte, 1, &tv );
    tv = value == 0? tv&value<<offset:tv|value<<offset;
    gSession.target()->write_sfr( byte, 1, &tv );
  } else if(byte >= 0x20 && byte <= 0x2f) {
    gSession.target()->read_data( byte, 1, &tv );
    tv = value == 0? tv&(0xff&value<<offset):tv|value<<offset;
    gSession.target()->write_data( byte, 1, &tv );
  }
  return true;
}

bool CmdWritebit::help(string cmd)
{
  cout << "Write value to bit. "<<endl
       << "writebit BIT=0|1: write 0 or 1 to 'BIT'"<<endl
       << "- support standard bit name, bit addr by dot notation and bit addr by number" <<endl;
  return true;
}

// simple, addr=value
bool CmdWritedata::direct(string cmd)
{
  if(cmd.find("=")==0)
  {
    printf("Wrong format, 'writedata ADDR=VALUE'\n");
    return true;
  }

  vector<string> regv = split(cmd, '=');

  if (regv.size() != 2)
  {
    printf("Wrong format, 'writedata ADDR=VALUE'\n");
    return true;
  }

  string target = regv[0];
  string val = regv[1];

  if(!is_hex(val)||!is_hex(target))
  {
    printf("Wrong value format, please specify hex addr and value.\n");
    return true;
  }

  long hex_target = hex_to_num(target);
  long hex_value = hex_to_num(val);

  if(hex_target >= 0xff || hex_value >= 0xff)
  {
    printf("Range of addr and value should be 0x00 to 0xff\n");
    return true;
  }

  uint8_t addr = hex_target&0xff;
  uint8_t value = hex_value&0xff;

	gSession.target()->write_data(addr,1,&value);
  return true;
}

bool CmdWritedata::help(string cmd)
{
  cout << "Write value to addr of data space."<<endl
       << "writedata ADDR=VALUE: write 1 byte 'VALUE' to 'ADDR' of data space"<<endl
       << "- ADDR and VALUE should in hex form" <<endl;
  return true;
}


bool CmdWritexdata::direct(string cmd)
{
  if(cmd.find("=")==0)
  {
    printf("Wrong format, 'writexdata ADDR=VALUE'\n");
    return true;
  }

  vector<string> regv = split(cmd, '=');

  if (regv.size() != 2)
  {
    printf("Wrong format, 'writexdata ADDR=VALUE'\n");
    return true;
  }

  string target = regv[0];
  string val = regv[1];

  if(!is_hex(val)||!is_hex(target))
  {
    printf("Wrong value format, please specify a hex addr and hex value.\n");
    return true;
  }

  long hex_target = hex_to_num(target);
  long hex_value = hex_to_num(val);

  if(hex_target > 0xffff)
  {
    printf("Range of addr should be 0x00 to 0xffff\n");
    return true;
  }

  if(hex_value > 0xff)
  {
    printf("Range of value should be 0x00 to 0xff\n");
    return true;
  }


  uint16_t addr = hex_target&0xffff;
  uint8_t value = hex_value&0xff;

	gSession.target()->write_xdata(addr,1,&value);
  return true;
}

bool CmdWritexdata::help(string cmd)
{
  cout << "Write value to addr of external data space."<<endl
       << "writexdata ADDR=VALUE: write 1 byte 'VALUE' to 'ADDR' of xdata space"<<endl
       << "- ADDR and VALUE should in hex form" <<endl;
  return true;
}

bool CmdWriteregister::direct(string cmd)
{
  transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);
 
  //FIXME: need remove all space in cmd.
  if(cmd.find("=")==0)
  {
    printf("Wrong format, 'writeregister R/SFR/ADDR=VALUE'\n");
    return true;
  }

  vector<string> regv = split(cmd, '=');

  if (regv.size() != 2)
  {
    printf("Wrong format, 'writeregister R/SFR/ADDR=VALUE'\n");
    return true;
  }

  string target = regv[0];
  string val = regv[1]; 

  if(!is_hex(val))
  {
    printf("Wrong value format, please specify a hex numer.\n");
    return true;
  }
 
  long hex = hex_to_num(val);

  if(hex >= 0xffff || 
    (hex > 0xff && target!="DPTR"))
  {
    printf("range of value should be 0x00 to 0xff\n");
    return true;
  }
  

  if(target == "DPTR")
  {
    uint16_t dptr = hex&0xffff;
    uint8_t dph = dptr&0xff00 >> 8;
    uint8_t dpl = (uint8_t)dptr&0x00ff;
    gSession.target()->write_sfr(0x83, 1, &dph);
    gSession.target()->write_sfr(0x82, 1, &dph);
    return true;
  }

  uint8_t value = hex&0xff;

  // first, lookup reg table.
  map<string , uint8_t>::iterator it= reg_map.find(target);
  if (it != reg_map.end()) {
      uint8_t addr = it->second;
      //all map element is SFR.
		  gSession.target()->write_sfr(addr, 1, &value);
      return true;
  }
  //process Rn register
  else if (target == "R0" ||
           target == "R1" ||
           target == "R2" ||
           target == "R3" ||
           target == "R4" ||
           target == "R5" ||
           target == "R6" ||
           target == "R7")
  {
    // R0-R7 addr according to reg bank.
    uint8_t reg_psw, reg_bank;
    gSession.target()->read_sfr(0xd0, 1, &reg_psw);
    reg_bank = (reg_psw>>3)&0x03;
    //printf("RegisterBank %i:\n",reg_bank);
    uint8_t r0_addr = reg_bank*8;
    // get register number. 
    uint8_t n = target[1] - '0';

    gSession.target()->write_data(r0_addr + n, 1, &value );
  }
    // for addr in hex format.
  else
  {
    if(!is_hex(target)) {
      printf("%s - Wrong Register address\n", target.c_str());
      return true;
 		}

    long hex = hex_to_num(target);
   
    if(hex > 0xff) {
      printf("%s - Wrong Register address\n", target.c_str());
      return true;
    }

    uint8_t addr = hex & 0xff;
      //SFR
    if(addr >= 0x80 && addr <= 0xff)
      gSession.target()->write_sfr( addr, 1, &value );
    else if(addr <= 0x20)
      gSession.target()->write_data( addr, 1, &value );
  }

  return true;
}

bool CmdWriteregister::help(string cmd)
{
  cout << "Write value to Rn/SFR register."<<endl
       << "writeregister Rn=VALUE:    write 'VALUE' to R0-R7"<<endl
       << "writeregister SFR=VALUE:   write 'VALUE' to standard SFR name"<<endl
       << "writeregister ADDR=VALUE:  write 'VALUE' to ADDR in hex form"<<endl
       << "writeregister DPTR=VALUE:  write 2 bytes 'VALUE' to DPTR"<<endl;
  return true;
}

bool CmdWritepsfr::direct(string cmd)
{
	string argstr = remove_duplicate_space(cmd);

//  transform(argstr.begin(), argstr.end(), argstr.begin(), ::toupper);

  //FIXME: need remove all space in cmd.
  if(argstr.find("=")==0)
  {
    printf("Wrong format, 'writepsfr PAGE_NUM ADDR=VALUE'\n");
    return true;
  }

	vector<string> argv = split(argstr, ' ');

	string page_num = argv[0];

  //page number
  if(!is_hex(page_num))
  {
    printf("Please specify sfrpage value as first arg\n");
    printf("- For sfrpage, please check the datasheet\n");
    return true;
  }

  long hex = hex_to_num(page_num);
  if(hex > 0xff)
  {
    printf("Wrong sfr page number: %s\n", page_num.c_str());
    return true;
  }

  uint8_t page_number = hex & 0xff;

	string equ = argv[1];

  vector<string> regv = split(equ, '=');

  if (regv.size() != 2)
  {
    printf("Wrong format, 'writepsfr PAGE_NUM ADDR=VALUE'\n");
    return true;
  }

  string target = regv[0];
  string val = regv[1];
  if(!is_hex(val)||!is_hex(target))
  {
    printf("Wrong value format, please specify a hex addr and hex value.\n");
    return true;
  }

  long hex_target = hex_to_num(target);
  long hex_value = hex_to_num(val);

  if(hex_target > 0xff)
  {
    printf("Range of addr should be 0x00 to 0xff\n");
    return true;
  }

  if(hex_value > 0xff)
  {
    printf("Range of value should be 0x00 to 0xff\n");
    return true;
  }


  uint8_t addr = hex_target&0xff;
  uint8_t value = hex_value&0xff;

	gSession.target()->read_sfr(addr, page_number, 1, &value);
  return true;	
}

bool CmdWritepsfr::help(string cmd)
{
  cout << "Write value to paged SFR register."<<endl
       << "writepsfr PAGE_NUM ADDR=VALUE: write 'VALUE' to 'ADDR' of SFR_PAGE"<<endl
       << "- usually, most of standard SFR is on all sfr pages,"<<endl
       << "- writepsfr only support write to 'ADDR' instead of standard SFR name."<<endl;
  return true;
}

