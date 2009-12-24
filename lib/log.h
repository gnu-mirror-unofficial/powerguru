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

#ifndef __LOG_H__
#define __LOG_H__

#include "config.h"
#include "err.h"

#ifdef __cplusplus

#include <time.h>

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/xmlreader.h>

#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>

// the default name for the debug log
#define DEFAULT_LOGFILE	"powerguru-dbg.log"
#define TIMESTAMP_LENGTH 24             // timestamp length
#define TIMESTAMP_FORMAT "%Y-%m-%d %H:%M:%S     " // timestamp format

extern std::ostream& stampon(std::ostream& x);
extern std::ostream& stampoff(std::ostream& x);
extern std::ostream& timestamp(std::ostream& x);
extern std::ostream& datetimestamp(std::ostream& x);

// Print a number in binary:
class Verbose {
  int level;
public:
  Verbose(int l) { level = l; }
  friend std::ostream& operator<<(std::ostream&, Verbose&);
};

// Get the current timestamp
std::string timestamp();

// This is a basic file logging class
class LogFile { // public std::ofstream {
public:
  LogFile (void);
  LogFile (const char *);
  ~LogFile(void) {
    if (state == OPEN) {
      //      flush();
      //      state = CLOSED;
      Close();
    }
  }
  enum file_state {
    CLOSED,
    OPEN,
    INPROGRESS,
    IDLE
  } state;
  
  file_state GetState (void) { return state; }
  LogFile& operator << (ErrCond&);
  LogFile& operator << (int x);
  LogFile& operator << (long x);
  LogFile& operator << (unsigned int x);
  // These both resolve to an unsigned int.
  // LogFile& operator << (size_t x);
  // LogFile& operator << (time_t x);
  LogFile& operator << (float x);
  LogFile& operator << (double &x);
  LogFile& operator << (bool x);
  LogFile& operator << (void *);
  LogFile& operator << (const char *);
  LogFile& operator << (const xmlChar *);
  LogFile& operator << (std::string );
  std::ostream& operator << (std::ostream & (&)(std::ostream &));
  const char *GetEntry(void);
  
  retcode_t Open(const char *);
  retcode_t Close(void);
  // accessors for the verbose level
  void set_verbosity (void) {
    verbose++;
    //        note (3, "Verbosity now set to %d", verbose);
  }
  void set_verbosity (int x) {
    verbose = x;
    //        note (3, "Verbosity now set to %d", verbose);
  }

  void SetStamp (bool b) {
    stamp = b;
  }
  bool GetStamp (void) {
    return stamp;
  }
  //std::string gettimestamp();
  
private:
  static std::ofstream console;
  std::ofstream outstream;
  static int verbose;
  bool stamp;
  std::string logentry;
  friend std::ostream & operator << (std::ostream &os, LogFile& e);
};

extern LogFile dbglogfile;

struct __Host_Function_Report__ {
    const char *func;

    __Host_Function_Report__(void) {
      if (dbglogfile.GetState() == LogFile::OPEN)
	dbglogfile  << "enter" << std::endl;
    }

    __Host_Function_Report__(char *_func) {
        func = _func;
	dbglogfile << func << " enter" << std::endl;
    }

    __Host_Function_Report__(const char *_func) {
        func = _func;
	dbglogfile << func << " enter" << std::endl;
    }

    ~__Host_Function_Report__(void) {
	dbglogfile << func << " return" << std::endl;
    }
};

#define DEBUGLOG_REPORT_FUNCTION	\
    __Host_Function_Report__ __host_function_report__( __PRETTY_FUNCTION__)

#define DEBUGLOG_REPORT_RETURN

unsigned char *
ascify_buffer (unsigned char *p, const unsigned char *s, int x);

#else
#define DEBUGLOG_REPORT_FUNCTION printf("%s entered\n", __FUNCTION__)
#define DEBUGLOG_REPORT_RETURN printf("%s return\n", __FUNCTION__)

// __cplusplus
#endif

// __LOG_H__
#endif


