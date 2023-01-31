#ifndef UTIL_H
#define UTIL_H
#include <string.h>

using namespace std;

bool is_digit(string);
bool is_hex(string);
long hex_to_num(string);
string remove_duplicate_space(const string &);
vector<string> split(const string &, char);

bool str_icase_equal(const string&, const string&);

// trim from left
std::string& ltrim(std::string& s, const char* t = " \t\n\r\f\v");
// trim from right
std::string& rtrim(std::string& s, const char* t = " \t\n\r\f\v");
// trim from left & right
std::string& trim(std::string& s, const char* t = " \t\n\r\f\v");

static map<string, uint8_t> reg_map = {
  /*
  depend on reg bank setting.
  {"R0",    0x00},
  {"R1",    0x01},
  {"R2",    0x02},
  {"R3",    0x03},
  {"R4",    0x04},
  {"R5",    0x05},
  {"R6",    0x06},
  {"R7",    0x07},
  */

  {"P0",    0x80},
  {"SP",    0x81},
  {"DPL",   0x82},
  {"DPH",   0x83},
  {"PCON",  0x87},
  {"TCON",  0x88},
  {"TMOD",  0x89},
  {"TL0",   0x8A},
  {"TL1",   0x8B},
  {"TH0",   0x8C},
  {"TH1",   0x8D},
  {"P1",    0x90},
  {"SCON",  0x98},
  {"SBUF",  0x99},
  {"P2",    0xA0},
  {"IE",    0xA8},
  {"P3",    0xB0},
  {"IP",    0xB8},
  {"PSW",   0xD0},
  {"ACC",   0xE0},
  {"A",     0xE0},
  {"B",     0xF0},
};

static map<string, pair<uint8_t, uint8_t>> bit_map = {
  // port 0
  {"P0.0",  make_pair(0x80, 0)}, 
  {"P0.1",  make_pair(0x80, 1)}, 
  {"P0.2",  make_pair(0x80, 2)}, 
  {"P0.3",  make_pair(0x80, 3)}, 
  {"P0.4",  make_pair(0x80, 4)}, 
  {"P0.5",  make_pair(0x80, 5)}, 
  {"P0.6",  make_pair(0x80, 6)}, 
  {"P0.7",  make_pair(0x80, 7)},
 
  // port 1
  {"P1.0",  make_pair(0x90, 0)}, 
  {"P1.1",  make_pair(0x90, 1)}, 
  {"P1.2",  make_pair(0x90, 2)}, 
  {"P1.3",  make_pair(0x90, 3)}, 
  {"P1.4",  make_pair(0x90, 4)}, 
  {"P1.5",  make_pair(0x90, 5)}, 
  {"P1.6",  make_pair(0x90, 6)}, 
  {"P1.7",  make_pair(0x90, 7)}, 

  // port 2
  {"P2.0",  make_pair(0xA0, 0)}, 
  {"P2.1",  make_pair(0xA0, 1)}, 
  {"P2.2",  make_pair(0xA0, 2)}, 
  {"P2.3",  make_pair(0xA0, 3)}, 
  {"P2.4",  make_pair(0xA0, 4)}, 
  {"P2.5",  make_pair(0xA0, 5)}, 
  {"P2.6",  make_pair(0xA0, 6)}, 
  {"P2.7",  make_pair(0xA0, 7)},
 
  // port 3
  {"P3.0",  make_pair(0xB0, 0)}, 
  {"P3.1",  make_pair(0xB0, 1)}, 
  {"P3.2",  make_pair(0xB0, 2)}, 
  {"P3.3",  make_pair(0xB0, 3)}, 
  {"P3.4",  make_pair(0xB0, 4)}, 
  {"P3.5",  make_pair(0xB0, 5)}, 
  {"P3.6",  make_pair(0xB0, 6)}, 
  {"P3.7",  make_pair(0xB0, 7)}, 

  // TCON
  {"TCON.0",  make_pair(0x88, 0)}, 
  {"TCON.1",  make_pair(0x88, 1)}, 
  {"TCON.2",  make_pair(0x88, 2)}, 
  {"TCON.3",  make_pair(0x88, 3)}, 
  {"TCON.4",  make_pair(0x88, 4)}, 
  {"TCON.5",  make_pair(0x88, 5)}, 
  {"TCON.6",  make_pair(0x88, 6)}, 
  {"TCON.7",  make_pair(0x88, 7)},
 
  {"IT0",     make_pair(0x88, 0)}, 
  {"IE0",     make_pair(0x88, 1)}, 
  {"IT1",     make_pair(0x88, 2)}, 
  {"IE1",     make_pair(0x88, 3)}, 
  {"TR0",     make_pair(0x88, 4)}, 
  {"TF0",     make_pair(0x88, 5)}, 
  {"TR1",     make_pair(0x88, 6)}, 
  {"TF1",     make_pair(0x88, 7)}, 

  // SCON 
  {"SCON.0",  make_pair(0x98, 0)}, 
  {"SCON.1",  make_pair(0x98, 1)}, 
  {"SCON.2",  make_pair(0x98, 2)}, 
  {"SCON.3",  make_pair(0x98, 3)}, 
  {"SCON.4",  make_pair(0x98, 4)}, 
  {"SCON.5",  make_pair(0x98, 5)}, 
  {"SCON.6",  make_pair(0x98, 6)}, 
  {"SCON.7",  make_pair(0x98, 7)},
  
  {"RI",      make_pair(0x98, 0)}, 
  {"TI",      make_pair(0x98, 1)}, 
  {"RB8",     make_pair(0x98, 2)}, 
  {"TB8",     make_pair(0x98, 3)}, 
  {"REN",     make_pair(0x98, 4)}, 
  {"SM2",     make_pair(0x98, 5)}, 
  {"SM1",     make_pair(0x98, 6)}, 
  {"SM0",     make_pair(0x98, 7)},

  //IE
  {"IE.0",    make_pair(0xA8, 0)}, 
  {"IE.1",    make_pair(0xA8, 1)}, 
  {"IE.2",    make_pair(0xA8, 2)}, 
  {"IE.3",    make_pair(0xA8, 3)}, 
  {"IE.4",    make_pair(0xA8, 4)}, 
  {"IE.5",    make_pair(0xA8, 5)}, 
  {"IE.6",    make_pair(0xA8, 6)}, 
  {"IE.7",    make_pair(0xA8, 7)},

  {"EX0",     make_pair(0xA8, 0)}, 
  {"ET0",     make_pair(0xA8, 1)}, 
  {"EX1",     make_pair(0xA8, 2)}, 
  {"ET1",     make_pair(0xA8, 3)}, 
  {"ES",      make_pair(0xA8, 4)}, 
  {"EA",      make_pair(0xA8, 7)},
  //IP 
  {"IP.0",    make_pair(0xB8, 0)}, 
  {"IP.1",    make_pair(0xB8, 1)}, 
  {"IP.2",    make_pair(0xB8, 2)}, 
  {"IP.3",    make_pair(0xB8, 3)}, 
  {"IP.4",    make_pair(0xB8, 4)}, 
  {"IP.5",    make_pair(0xB8, 5)}, 
  {"IP.6",    make_pair(0xB8, 6)}, 
  {"IP.7",    make_pair(0xB8, 7)},

  {"PX0",     make_pair(0xB8, 0)}, 
  {"PT0",     make_pair(0xB8, 1)}, 
  {"PX1",     make_pair(0xB8, 2)}, 
  {"PT1",     make_pair(0xB8, 3)}, 
  {"PS",      make_pair(0xB8, 4)}, 

  //PSW
  {"PSW.0",   make_pair(0xD0, 0)}, 
  {"PSW.1",   make_pair(0xD0, 1)}, 
  {"PSW.2",   make_pair(0xD0, 2)}, 
  {"PSW.3",   make_pair(0xD0, 3)}, 
  {"PSW.4",   make_pair(0xD0, 4)}, 
  {"PSW.5",   make_pair(0xD0, 5)}, 
  {"PSW.6",   make_pair(0xD0, 6)}, 
  {"PSW.7",   make_pair(0xD0, 7)},

  {"P",       make_pair(0xD0, 0)}, 
  {"F1",      make_pair(0xD0, 1)}, 
  {"OV",      make_pair(0xD0, 2)}, 
  {"RS0",     make_pair(0xD0, 3)}, 
  {"RS1",     make_pair(0xD0, 4)}, 
  {"F0",      make_pair(0xD0, 5)}, 
  {"AC",      make_pair(0xD0, 6)}, 
  {"CY",      make_pair(0xD0, 7)},
  {"C",       make_pair(0xD0, 7)},
};

#endif
