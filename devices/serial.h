//
// Copyright (C) 2005, 2006, 2007, 2008, 2009, 2010, 2011, 2012, 2013
//               2014, 2015, 2016, 2017, 2018, 2019
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

/// \copyright GNU Public License.
/// \file serial.h Header file for serial port manipulation class

#ifndef __SERIAL_H__
#define __SERIAL_H__

// This is generated by autoconf
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

// System include files
#include <cstring>
#include <vector>
#include <fstream>
#include <ostream>
#include <iostream>
#include <cstdio>

#include <termios.h>
#include <boost/system/error_code.hpp>
using namespace boost::system;

//#define DEFAULT_UART "/dev/pts/12";
#define DEFAULT_UART "/dev/ttyS0";

extern const char *serial_speeds[];

class Serial {
public:
    ~Serial (void);
    Serial (void);

    // Set the terminal to be in RAW mode
    boost::system::error_code SetRaw (void);
    boost::system::error_code SetDTR (void);
    boost::system::error_code SetDTR (bool value);
    boost::system::error_code SetBlocking(bool mode);    

    // Set the baud rate
    boost::system::error_code SetBaud (int x);

    int GetFD() {
        return _uartfd;
    }
    
    
    // These methods operate on the serial settings
    void DumpTtyState (void);    
    termios *SetTtyState (termios *tty);
    termios *GetTtyState (void);

    // These methods operate on the serial port
    boost::system::error_code Open   (std::string &name);
    boost::system::error_code Close  (void);
    int Read   (char *buf, int nbytes);
    int Write  (const char *buf, int nbytes) const;

    boost::system::error_code Flush  (void);

    // These methods do byte oriented I/O on the serial port
    int Getc (void);
    int UnGetc (int ch);
    int Putc (int x);

    Serial& operator << (Serial &);
    Serial& operator << (char const *str);
    Serial& operator << (std::string &str);

    // This is so we can print ourselves
    friend std::ostream & operator << (std::ostream &os, Serial& s);
    std::ostream& operator << (std::ostream & (&)(std::ostream &));
private:
    std::ifstream  serialin;
    std::ofstream  serialout;
    int            _uartfd;
    FILE           *uartfile;
    termios        origtty;
    termios        currenttty;
};

// __SERIAL_H__
#endif

// local Variables:
// mode: C++
// indent-tabs-mode: nil
// End:
