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

#ifndef __CONSOLE_H__
#define __CONSOLE_H__

// This sets up a console for user interaction, and defaults to using
// standard in.

#include <cstring>
#include <vector>
#include <cstdio>
#include <termios.h>
#include <boost/system/error_code.hpp>
using namespace boost::system;

class Console {
public:
    Console (void);
    ~Console (void);

    enum state_e { UNSET, OPEN, RAW, CLOSED };

    void openCon (void);
    void openInChannel (std::string channel);
    void openInChannel (void);
    void openOutChannel (std::string channel);
    void openOutChannel (void);
    void closeCon (void);
    void resetCon (void);
  
    void sendEOL (void);
  
    void makeRaw (int);
    void makeRaw (void) { makeRaw (fileno(inchannel.fhandle)); }
    void makeRaw (FILE * x) { makeRaw (fileno(x)); }

    // get a byte from the console
    int getcCon (void) { return getcCon (inchannel.fhandle); }
    int getcCon (FILE *x) { return getc(x); }

    // write a byte to the console
    int putcCon (int x) { return putcCon (x, outchannel.fhandle); }
    int putcCon (int x, FILE *y)  { return putc(x, y); }

    // write a string to the console
    int putsCon (std::string out) { return putsCon (out.c_str(), outchannel.fhandle); }
    int putsCon (std::string out, FILE *y)  { return fputs(out.c_str(), y); }
 
    // Accessors

    // Set/Get the name of the device we are using for the console
    std::string GetFileSpec (void) { return inchannel.filespec; }
    void SetFileSpec (std::string x) { inchannel.filespec = x; }

    // Set/Get the FILE pointer
    FILE *GetFileHandle(void) { return inchannel.fhandle; }
    void SetFileHandle (FILE *x) { inchannel.fhandle = x; }

    // Set/Get the state of the console
    state_e GetState (void) { return state; }
    void SetState (state_e x) { state = x; }

private:
    struct channel {
        FILE *fhandle;
        std::string filespec;
    };
    struct channel inchannel;
    struct channel outchannel;
    state_e state;
    termios oldtty;
};

// __CONSOLE_H__
#endif

// local Variables:
// mode: C++
// indent-tabs-mode: nil
// End:
