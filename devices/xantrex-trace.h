// 
// Copyright (C) 2005, 2006, 2007, 2008, 2009, 2010, 2011
//      Free Software Foundation, Inc.
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

#ifndef  __XANTREXUI_H__
#define __XANTREXUI_H__

#ifdef __cplusplus

#include <cstring>
#include <vector>
#include <map>
#include "err.h"
#include <console.h>

#include "serial.h"
#include "menuitem.h"
#include "database.h"

class XantrexUI : public Serial
{
 public:
  typedef enum {USERMENU, SETUPMENU} menu_e;
  enum inverter_e {OFF, SRCH, ON, CHG};
  enum invertsetup_e {FLOAT, SELL, SLT, LBX};
  enum battery_e {NICAD, LEADACID, GEL};
  enum genauto_e {UNINITIALIZED, RUNNING, IDLE};

  XantrexUI(void);
  ~XantrexUI(void);
  
  // Dump all internal data to the console
  void Dump (void);
  void DumpAliases(void);
  
  // This selects which inverter we want to control
  std::string &SelectInverter(int x);

  // Read from the serial port
  std::string &ReadSerial(void);

  // Write to the serial port
  std::string &WriteSerial(const char *buf, int nbytes);

  // These methods all go with the Xantrex display buttons
  std::string &MenuHeadingMinus(void);
  std::string &MenuHeadingPlus(void);
  std::string &MenuItemMinus(void);
  std::string &MenuItemPlus(void);
  std::string &SetPointMinus(void);
  std::string &SetPointPlus(void);
  std::string &Inverter(void);
  std::string &Generator(void);
  std::string &SetupMenu(void);
  std::string &LedStatus(void);
  std::string &Version(void);
  std::string &SetTerminalMode(void);
  
  std::vector<meter_data_t *> PollMeters(int loops);
  //std::map<std::string, float> *PollMeters(int loops);
  
  // This method takes the data as outputted by the Xantrex inverter,
  // and munges it to be a standard form with whitespace compressed,
  // newlines & carriage returns stripped out, etc... This is so
  // we can analyse the string data in a consistant fashion.
  std::string &CleanUpData(std::string &str);

  meter_data_t *exportMeterData(meter_data_t *data);
  
#if 0
  std::string &UserMenu(void);
  // These methods are for switching menu modes
  retcode_t GotoSetupMenu(void);
  retcode_t GotoUserMenu(void);
#endif
  // This method takes us to the beginning of the menu headers and items
  // This corresponds then to being in the Inverter Mode header of the
  // User menu.
  retcode_t GotoMenuStart(void);
  std::string &GotoMenuItem(int mh, int mi);
  
  // These methods manipulate values
  std::string &GetValue (std::string &x);
  std::vector<std::string> *GetValues (std::string &str);
  retcode_t SetFloatValue(float x);
  float GetFloatValue(std::string &x);
  retcode_t SetIntValue(int x);
  int GetIntValue(std::string &x);
  retcode_t SetBoolValue(bool x);
  bool GetBoolValue(std::string &x);
  std::string GetLabel(std::string &x);
  // This method populates the menu item data to default values as per
  // the Xantrex manual.
  void SetDefaultValues (void);

  // This method tries to match the display with a menu item
  // to verify we're where we think we are...
  MenuItem &Match(std::string &str);
  MenuItem &GetItem(int x, int y);
  MenuItem &GetItem() { return GetItem(menuheading, menuitem); }
  std::string GetLabel();

  retcode_t xantrex_main(Console& con);
 private:
  std::vector< std::vector< MenuItem > > _items;
  int menuheading;
  int menuitem;
  //  Serial uart;  
};

extern "C" {
#else

  // Default object for C code. 
  XantrexUI ui;
  
  /* This is the C API for this class. */
  
  const char *menu_heading_minus(void);
  const char *menu_heading_plus(void);
  const char *menu_item_minus(void);
  const char *menu_item_mlus(void);
  const char *set_point_minus(void);
  const char *set_point_plus(void);
  
  const char *inverter(void);
  const char *generator(void);
  const char *setup_menu(void);
  const char *led_status(void);
  const char *version(void);
  const char *set_terminal_mode(void);
  
#endif
#ifdef __cplusplus
}
#endif

// end of __XANTREXUI_H__
#endif

// local Variables:
// mode: C++
// indent-tabs-mode: nil
// End:
