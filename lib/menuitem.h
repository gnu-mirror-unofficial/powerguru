// 
//   Copyright (C) 2005 Free Software Foundation, Inc.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

#ifndef __MENUITEM_H__
#define __MENUITEM_H__

#include <map>
#include <vector>
#include <string>
#include <sstream>

#include "err.h"
#include "log.h"

const int LCDLEN = 38;

const int TIMEIDX = 16;
const int FLTIDX = 38;
const int HEADIDX = 33;
const int INTIDX = 36;
const int BOOLIDX = 37;

// This normally defaults to a space, but we explicitly set it to we can
// use something else when debugging so it's easier to count spaces in strings.
const char FILLCHAR = ' ';

// Each menu item has associated data, and it's location indexes into which
// item number of which header this is.
union mval {
  float  floatval;
  int    intval;
  bool   boolval;
  time_t timeval;
};

class MenuItem 
{
public:
  MenuItem();
  ~MenuItem() { };

  enum datatype
    { NONE, INFO, MENUHEAD, MENUITEM, TIME, DATE, FLOAT, INT, EOL, BOOL, CLOCK, ENUM };
  MenuItem(std::string label, int x); // make a menuheader label
  // make a menuitem label
  MenuItem(std::string label, int x, int y, std::string abbrev);
  MenuItem(std::string label, int x, int y, bool z, std::string abbrev);
  //  MenuItem(std::string label, int x, int y, enum datatype z);
  MenuItem(std::string label, int x, int y, int z, std::string abbrev);
  MenuItem(std::string label, int x, int y, float z, std::string abbrev);
  MenuItem(std::string label, int x, int y, time_t z, std::string abbrev);

  // Set the data
  void SetItem(std::string label, int x, int y, union mval val, enum datatype dtype);
  void SetItem(std::string label, int x, int y, union mval val, enum datatype dtype, std::string);

  void SetRange(int x, int y);
  void SetRange(float x, float y);
  void SetItem(std::string label, int x); // make a menuheader label
  // make a menuitem label
  void SetItem(std::string label, int x, int y);
  void SetItem(std::string label, int x, int y, std::string abbrev);
  void SetItem(std::string label, int x, int y, bool z);
  void SetItem(std::string label, int x, int y, bool z, std::string abbrev);
  //  void SetItem(std::string label, int x, int y, enum datatype z);
  void SetItem(std::string label, int x, int y, int z);
  void SetItem(std::string label, int x, int y, int z, std::string abbrev);
  void SetItem(std::string label, int x, int y, float z);
  void SetItem(std::string label, int x, int y, float z, std::string abbrev);
  void SetItem(std::string label, int x, int y, time_t z, std::string abbrev);
  
  std::string &GetLabel(void)   { return label; }
  std::string &GetAlias(void)   { return cmdname; }
  int GetHeaderIndex(void)      { return headidx; }
  int GetItemIndex(void)        { return itemidx; }
  

  // Overload so we can extract the various data types
  union mval GetValue(union mval *x) { return (*x = value); }
  int GetInt(void)             { return value.intval; }
  float GetFloat(void)         { return value.floatval; }
  bool GetBool(void)           { return value.boolval; }      
  bool GetValue(bool *x)       { return (*x = value.boolval); }
  int GetValue(int *x)         { return (*x = value.intval); }
  float GetValue(float *x)     { return (*x = value.floatval); }
  time_t GetValue(time_t *x)   { return (*x = value.timeval); }
  void SetValue(int x)         { value.intval = x; }
  void SetValue(float x)       { value.floatval = x; }
  void SetValue(time_t x)      { value.timeval = x; }
  enum datatype GetType(void)  { return type; }
  int GetIntMin (void)         { return minrange.intval; }
  int GetIntMax (void)         { return maxrange.intval; }
  float GetFloatMin (void)       { return minrange.floatval; }
  float GetFloatMax (void)       { return maxrange.floatval; }
  void Dump(void);
  
  std::string &SetDisplay(std::string &label);
  std::string &SetDisplay(std::string &label, int x);
  std::string &SetDisplay(int x);
  std::string &SetDisplay(std::string &label, float x);
  std::string &SetDisplay(float x);
  std::string &SetDisplay(std::string &label, bool x);
  std::string &SetDisplay(bool x);
  std::string &SetDisplay(time_t x);
  std::string &SetDisplay(std::string &label, time_t x);
  std::string &GetDisplay(void)   { return display; }

 private:
  std::string cmdname;          // this is an abbreviation used for
                                // the command line interface
  std::string label;
  std::string display;
  int headidx;
  int itemidx;
  enum datatype type;
  union mval value;
  union mval minrange;
  union mval maxrange;
};

// end of __MENUITEM_H__
#endif
