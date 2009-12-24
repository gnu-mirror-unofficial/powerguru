// 
// Copyright (C) 2005, 2006, 2007, 2008, 2009 Free Software Foundation, Inc.
// 
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

#include <cstdlib>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <signal.h>
#include <unistd.h>

// #include "datasource.h"

using namespace std;

#include "err.h"
#include "log.h"
#include "menuitem.h"

static const char hexchars[]="0123456789abcdef";

MenuItem pollout;


// This is the prefix for all output to the LCD display.
const char *prefix = "\r\n  ";


// These are for the User Menus. Each list must be terminated by a "" (NULL)
// pointer.
MenuItem::MenuItem()
{
  headidx = 0;
  itemidx = 0;
  type = NONE;
}

// Constructors for the MenuItem class.
MenuItem::MenuItem(string lab, int x)
{
  //  DEBUGLOG_REPORT_FUNCTION;
  union mval zz;
  zz.intval = 0;
  SetItem(lab, x, 0, zz, MENUHEAD);
}

MenuItem::MenuItem(string lab, int x, int y, string abbrev)
{
  //  DEBUGLOG_REPORT_FUNCTION;
  union mval zz;
  zz.intval = 0;
  SetItem(lab, x, y, zz, MENUITEM);
}

MenuItem::MenuItem(string lab, int x, int y, bool z, string abbrev)
{
  //  DEBUGLOG_REPORT_FUNCTION;
  union mval zz;
  zz.boolval = z;
  SetItem(lab, x, y, zz, BOOL);
}

MenuItem::MenuItem(string lab, int x, int y, int z, string abbrev)
{
  //  DEBUGLOG_REPORT_FUNCTION;
  union mval zz;
  zz.intval = z;
  SetItem(lab, x, y, zz, INT);
}

MenuItem::MenuItem(string lab, int x, int y, float z, string abbrev)
{
  //  DEBUGLOG_REPORT_FUNCTION;

  union mval zz;
  zz.floatval = z;
  SetItem(lab, x, y, zz, FLOAT);
}

MenuItem::MenuItem(string lab, int x, int y, time_t z, string abbrev)
{
  //  DEBUGLOG_REPORT_FUNCTION;

  union mval zz;
  zz.timeval = z;
  SetItem(lab, x, y, zz, TIME);
}

// Set the item's data
void
MenuItem::SetRange(int x, int y)
{
  minrange.intval = x;
  maxrange.intval = y;
}

void
MenuItem::SetRange(float x, float y)
{
  minrange.floatval = x;
  maxrange.floatval = y;
}

void
MenuItem::SetItem(string lab, int x, int y, union mval z, enum datatype dtype)
{
  SetItem(lab, x, 0, z, dtype, "");
}

void
MenuItem::SetItem(string lab, int x, int y, union mval z, enum datatype dtype, string abbrev)
{
  //  DEBUGLOG_REPORT_FUNCTION;

  label = lab;
  headidx = x;
  itemidx = y;
  value = z;
  cmdname = abbrev;
  type = dtype;
}

// This sets a menu header
void
MenuItem::SetItem(string lab, int x)
{
  //  DEBUGLOG_REPORT_FUNCTION;
  union mval zz;
  zz.intval = 0;
  SetItem(lab, x, 0, zz, MENUHEAD);
}

// These all set a menu item
void
MenuItem::SetItem(string lab, int x, int y)
{
  //  DEBUGLOG_REPORT_FUNCTION;
  union mval zz;
  zz.intval = 0;
  SetItem(lab, x, y, zz, INFO);
}

void
MenuItem::SetItem(string lab, int x, int y, string abbrev)
{
  //  DEBUGLOG_REPORT_FUNCTION;
  union mval zz;
  zz.intval = 0;
  SetItem(lab, x, y, zz, MENUITEM, abbrev);
}

void
MenuItem::SetItem(string lab, int x, int y, bool z)
{
  SetItem(lab, x, y, z, "");
}

void
MenuItem::SetItem(string lab, int x, int y, bool z, std::string abbrev)
{
  //  DEBUGLOG_REPORT_FUNCTION;
  union mval zz;
  zz.boolval = z;
  SetItem(lab, x, y, zz, BOOL, abbrev);  
}


void
MenuItem::SetItem(string lab, int x, int y, int z)
{
  union mval zz;
  zz.intval = z;
  SetItem(lab, x, y, zz, INT, "");
}

void
MenuItem::SetItem(string lab, int x, int y, int z, string abbrev)
{
  //  DEBUGLOG_REPORT_FUNCTION;
  union mval zz;
  zz.intval = z;
  SetItem(lab, x, y, zz, INT, abbrev);
}

void
MenuItem::SetItem(string lab, int x, int y, float z)
{
  //  DEBUGLOG_REPORT_FUNCTION;

  union mval zz;
  zz.floatval = z;
  SetItem(lab, x, y, zz, FLOAT, "");
}

void
MenuItem::SetItem(string lab, int x, int y, float z, string abbrev)
{
  //  DEBUGLOG_REPORT_FUNCTION;

  union mval zz;
  zz.floatval = z;
  SetItem(lab, x, y, zz, FLOAT, abbrev);
}

void
MenuItem::SetItem(string lab, int x, int y, time_t z, string abbrev)
{
  //  DEBUGLOG_REPORT_FUNCTION;

  union mval zz;
  zz.timeval = z;
  SetItem(lab, x, y, zz, TIME, abbrev);
}

void
MenuItem::Dump(void)
{
  //  DEBUGLOG_REPORT_FUNCTION;

#if 0
  string str0[] = {
    "NONE",
    "MENUHEAD",
    "MENUITEM",
    "TIME",
    "DATE",
    "FLOAT",
    "INT",
    "EOL",
    "BOOL",
    "CLOCK",
    "ENUM"
  };
#endif

  if (type <= ENUM) {
    dbglogfile << " Label is: \"" << label << "\"" << endl;
  }
  dbglogfile << "\tHeader Index: "   << headidx;
  dbglogfile << ", Item Index: " << itemidx;

  if (cmdname.size() > 0)
    dbglogfile << ", Alias is: \"" << cmdname << "\"";
  dbglogfile << endl;
  
  switch (type) {
  case NONE:
    dbglogfile << "\tUnitialized data type." << endl;
    break;
  case INFO:
    dbglogfile << "\tInformational Message." << endl;
    break;
  case MENUHEAD:
    dbglogfile << "\tMenu Heading" << endl;
    break;
  case MENUITEM:
    dbglogfile << "\tMenu Item: "<< endl;
    break;
  case BOOL:
    if (value.boolval == false)
      dbglogfile << "\tBoolean value is: " << "OFF" ;
    else
      dbglogfile << "\tBoolean value is: " << "ON" ;      
    dbglogfile << endl;
    break;
  case INT:
    dbglogfile << "\tInteger value is: " << value.intval << endl;
    break;
  case FLOAT:
    dbglogfile << "\tFIXME: Float value is: " << (int)value.floatval << endl;
    break;
  case TIME:
    dbglogfile << "\tFIXME: Time value is: " << (int)value.timeval << endl;
    break;
  case EOL:
  case CLOCK:
  case ENUM:
  case DATE:
    dbglogfile << "\tERROR: unsupported type! " << value.intval << endl;
    break;
  default:
    dbglogfile << "Data Type out of range";
    break;
  };
  
  
#if 0
  if (type == BOOL)
    dbglogfile << "\tBoolean Value is: " << value.intval << " and is: " << str0[type] << "\r" << endl;
  else
    dbglogfile << "BOOL shit!\r\n";
#endif
}

std::string &
MenuItem::SetDisplay(std::string &label)
{
  DEBUGLOG_REPORT_FUNCTION;

  ostringstream oss;
  
  oss << prefix;
  oss << label;
  oss.fill(FILLCHAR);
  oss.width(HEADIDX - label.size());
  
  display = oss.str();

  return display;
}

std::string &
MenuItem::SetDisplay(std::string &label, int x)
{
  DEBUGLOG_REPORT_FUNCTION;
  ostringstream oss;

  oss.clear();
  
  oss << prefix;
  oss << label;
  oss.fill(FILLCHAR);
  oss.width(INTIDX - label.size() - 1);
  oss << FILLCHAR;
  oss.fill('0');
  oss.width(2);
  oss << x;
  display = oss.str();

  pollout = *this;
  
  return display;
}

std::string &
MenuItem::SetDisplay(int x)
{
  return SetDisplay(label, x);
}

std::string &
MenuItem::SetDisplay(std::string &label, bool x)
{
  DEBUGLOG_REPORT_FUNCTION;
  ostringstream oss;
  
  oss.clear();
  oss << prefix;
  oss << label;
  oss.fill(FILLCHAR);
  oss.width(BOOLIDX - label.size());

  pollout = *this;

  if (x)
    oss << "YES";
  else
    oss << "NO";
  
  display = oss.str();
  
  return display;
}

std::string &
MenuItem::SetDisplay(std::string &label, float x)
{
  DEBUGLOG_REPORT_FUNCTION;
  ostringstream oss;
  
  oss.clear();
  oss << prefix;
  oss << label;
  oss.fill(FILLCHAR);
  oss.precision(3);
  oss.width(FLTIDX - label.size());
  oss << x;
  display = oss.str();
  
  pollout = *this;

  return display;
}

std::string &
MenuItem::SetDisplay(float x)
{
  return SetDisplay(label, x);
}

std::string &
MenuItem::SetDisplay(time_t x)
{
  return SetDisplay(label, x);
}

std::string
&MenuItem::SetDisplay(std::string &label, time_t x)
{
  DEBUGLOG_REPORT_FUNCTION;

  ostringstream oss;
  
  oss.clear();
  oss << prefix;
  oss << label;
  oss.fill(' ');
  oss.width(TIMEIDX - label.size());
  oss << x;
  display = oss.str();

  return display;
}


