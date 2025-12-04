#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <stdio.h>
#include <iterator>
#include <sstream>
#include <regex>
#include "utils.h"

bool is_digit(string s)
{
  // Iterate over string
  int n = s.length();
  for (int i = 0; i < n; i++)
  {
    if (!isdigit(s[i]))
        return false;
  }
  return true;
}

// A string is considered a hexadecimal encoding if it has:
//   - A (case insensitive) prefix of '0x' or suffix of 'h', and
//   - all digits are within the set {[0,9],[A-F]}.
bool is_hex(string s)
{
  bool result = false;

  if ((s.rfind("0x", 0) == 0) || (s.rfind("0X", 0) == 0))
  {
    result = true;
    s.erase(0, 2);
  }

  if (toupper(s.back()) == 'H')
  {
    result = true;
    s.pop_back();
  }

  // Walk each character in s[], checking that they are
  // within the hex digit set.
  int n = s.length();
  for (int i = 0; i < n; i++)
  {
    if (!isxdigit(s[i]))
      return false;
  }

  return result;
}


long hex_to_num(string s)
{
  if ((s.rfind("0x", 0) == 0) || (s.rfind("0X", 0) == 0))
    s.erase(0, 2);

  if (toupper(s.back()) == 'H')
    s.pop_back();

  long val = 0;
  int n = s.length();

  for (int i = 0; i < n; i++)
  {
    char ch = toupper(s[i]);
    if (ch >= '0' && ch <= '9')
      ch = ch - '0';
    else if (ch >= 'A' && ch <= 'F')
      ch = ch - 'A' + 10;
    val = (val << 4) | (ch & 0xf);
  }

  return val;
}


string remove_duplicate_space(const string &s)
{
    regex reg(R"(\s+)");
    return regex_replace(s, reg, " ");
}

vector<string> split(const string &s, char delim) {
  stringstream ss(s);
  string item;
  vector<string> elems;
  while (getline(ss, item, delim)) {
    elems.push_back(item);
    // elems.push_back(std::move(item)); // if C++11 (based on comment from @mchiasson)
  }
  return elems;
}

bool str_icase_equal(const string& a, const string& b)
{
    unsigned int sz = a.size();
    if (b.size() != sz)
        return false;
    for (unsigned int i = 0; i < sz; ++i)
        if (tolower(a[i]) != tolower(b[i]))
            return false;
    return true;
}

// trim from left
std::string& ltrim(std::string& s, const char* t)
//std::string& ltrim(std::string& s, const char* t = " \t\n\r\f\v")
{
    s.erase(0, s.find_first_not_of(t));
    return s;
}

// trim from right
//std::string& rtrim(std::string& s, const char* t = " \t\n\r\f\v")
std::string& rtrim(std::string& s, const char* t)
{
    s.erase(s.find_last_not_of(t) + 1);
    return s;
}

// trim from left & right
//std::string& trim(std::string& s, const char* t = " \t\n\r\f\v")
std::string& trim(std::string& s, const char* t)
{
    return ltrim(rtrim(s, t), t);
}
