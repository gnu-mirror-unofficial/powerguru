// 
// Copyright (C) 2005, 2006, 2007, 2008, 2009, 2010 Free Software Foundation, Inc.
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

// This is generated by autoconf
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <fstream>

#include "err.h"
#include "log.h"
#include "rc.h"

using namespace std;
using namespace rcinit;
  
RCinitFile::RCinitFile()
{
}

RCinitFile::~RCinitFile()
{
}

// Look for a config file in the likely places.
retcode_t
RCinitFile::load_files()
{
  char *home;
  string loadfile;
  
  // Check the default system location
  loadfile = "/etc/powerguru/config";
  parse_file(loadfile);

  // Check the default config location
  loadfile = "/usr/local/etc/powerguru/config";
  parse_file(loadfile);
  
  // Check the users home directory
  home = getenv("HOME");
  if (home) {
    loadfile = home;
    loadfile += "/.powerguru/config";
    return parse_file(loadfile);
  }
  
  return ERROR;
}

// Parse the config file and set the variables.
retcode_t
RCinitFile::parse_file(string filespec)
{
  struct stat stats;
  string action;
  string variable;
  string value;
  ifstream in;

  dbglogfile << "Seeing if " << filespec << " exists." << endl;
  if (filespec.size() == 0) {
    return ERROR;
  }
  
  if (stat(filespec.c_str(), &stats) == 0) {
    in.open(filespec.c_str());
    
    if (!in) {
      dbglogfile << "ERROR: Couldn't open file: " << filespec << endl;
      return ERROR;
    }

    // Read in each line and parse it
    while (!in.eof()) {
      // Get the first token
      in >> action;
      // Ignore comment lines
      if (action == "#" ) {
        dbglogfile << "Ignoring comment line " << endl;
        continue;
      }
      
      in >> variable >> value;
      //      dbglogfile << action << variable << value << endl;

      if (action == "set") {
        if (variable == "dbhost") {
          dbglogfile << "Database host configured to be " << value << endl;
          _dbhost = value;
        }
        if (variable == "dbuser") {
          dbglogfile << "Database user configured to be " << value << endl;
          _dbuser = value;
        }
        if (variable == "dbpasswd") {
          dbglogfile << "Database password configured to be " << value << endl;
          _dbpasswd = value;
        }
        if (variable == "dbname") {
          dbglogfile << "Database name configured to be " << value << endl;
          _dbname = value;
        }
        if (variable == "devmode") {
          dbglogfile << "Device mode configured to be " << value << endl;
          _devmode = value;
        }
        if (variable == "device") {
          dbglogfile << "Device is " << value << endl;
          _device = value;
        }
      }
    }
  } else {
    if (in) {
      in.close();
    }
    return ERROR;
  }  

  if (in) {
    in.close();
  }
  return SUCCESS;
}

// Write the changed settings to the config file
retcode_t
RCinitFile::update_file(string filespec)
{
  cerr << __PRETTY_FUNCTION__ << "ERROR: unimplemented!" << endl;
  return ERROR;
}




