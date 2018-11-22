// 
// Copyright (C) 2005, 2006 - 2018
//       Free Software Foundation, Inc.
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

#include <cstring>
#include <unistd.h>

#include "log.h"
#include "err.h"
#include "fakeuart.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <termios.h>
#include <iostream>
#include <iomanip>
#include <cstring>
#include <fstream>

extern LogFile dbglogfilefile;

FakeUart::FakeUart(void)
{
  _uartfd = 0;
  name = DEFAULT_FAKEUART;
  state = IDLE;
}

FakeUart::~FakeUart(void)
{
    dbglogfile << "Closing fake uart " << name << std::endl;

  // Don't try to close a device that isn't open
  if (state == OPEN) {
    close(_uartfd);
  }
}

retcode_t
//FakeUart::Open(string &filespec, ErrCond &err)
FakeUart::Open(ErrCond &err)
{
    DEBUGLOG_REPORT_FUNCTION;

    std::string filespec;
    int i,j, flag;
    int master = -1;
    struct termios newtty, oldtty;
    const char *letterpart = "pqrstuvwxyzPQRST";
    const char *numberpart = "0123456789abcdef";

    filespec = "/dev/ptmx";
    dbglogfile << "Looking for a pty: " << filespec.c_str() << std::endl;
    if ((master = open(filespec.c_str(), O_RDWR | O_NONBLOCK)) < 0) {
        if (errno == ENOENT) {
            std::cerr << "ERROR: no more pty's available";
            return ERROR;
        }
    } else {
        dbglogfile << "Opening pty master: " << filespec << std::endl; 
    }
    
    // Search for a free pty
    for(i=0; i<16 && master <= 0; i++) {
        for(j=0; j<16 && master <= 0; j++) {
            filespec[8] = letterpart[i];
            filespec[9] = numberpart[j];
            dbglogfile << "Opening pty: " << filespec.c_str() << std::endl;
            if ((master = open(filespec.c_str(), O_RDWR | O_NONBLOCK)) < 0) {
                if (errno == ENOENT) {
                    std::cerr << "ERROR: no more pty's available";
                    return ERROR;
                } else {
                    dbglogfile << "Opened pty: " << filespec << std::endl;
                }
            }
        } // for j loop
    } // for i loop

    if ((master < 0) && (i == 16) && (j == 16)) {
        // failed to found an available pty
        std::cerr << "ERROR: couldn't open a pty";
        return(ERROR);
    }

    state = OPEN;
  
    // create the name of the slave pty
    //filespec[5] = 't';
    filespec = ptsname(master);

    dbglogfile << "Opened " << ptsname(master) << " with file descriptor " << master << std::endl;

    grantpt(master);
    unlockpt(master);

    _uartfd = master;
    uartStream = fdopen(_uartfd, "w+");

    tcgetattr(master, &oldtty);
    newtty = oldtty;
    newtty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL|IXON);
    newtty.c_oflag &= ~OPOST;
    newtty.c_lflag &= ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN);
    newtty.c_cflag &= ~(CSIZE|PARENB);
    newtty.c_cflag |= CS8;
    newtty.c_cc[VMIN] = 1;
    newtty.c_cc[VTIME] = 1;

    tcsetattr(master, TCSANOW, &newtty);

    //  fcntl(master, F_SETFL, flag | O_NDELAY);

    // return(master);
    return SUCCESS;
}

retcode_t
FakeUart::Close(std::string &filespec, ErrCond &err)
{
    DEBUGLOG_REPORT_FUNCTION;

    if (state == OPEN)
        close(_uartfd);
  
    _uartfd = 0;
    state = CLOSED;
    return SUCCESS;
}

int
FakeUart::Read(unsigned char *buf, int nbytes, ErrCond &err)
{
    DEBUGLOG_REPORT_FUNCTION;

    return read (_uartfd, buf, nbytes);
}

#if 0
int
FakeUart::Write(std::string &str, ErrCond &err)
{
    DEBUGLOG_REPORT_FUNCTION;

    if (!str.empty()) {
        dbglogfile << "Writing " << str << " to uart FD: " << _uartfd << std::endl;
        //write(4, str.c_str(), str.size());
        //write (_uartfd, str.c_str(), str.size());
    }
}

int
FakeUart::Write(const unsigned char *buf, int nbytes, ErrCond &err)
{
    DEBUGLOG_REPORT_FUNCTION;

    //write (_uartfd, buf, nbytes);
}
#endif

#if 0
int
FakeUart::GetChar(char *buf, ErrCond &err)
{
    //  DEBUGLOG_REPORT_FUNCTION;

    read (_uartfd, buf, 1);
  
    return (int)*buf;
}

ostream &operator<<(ostream &os, FakeUart &l) {
    return os << l.GetName();
}

FakeUart&
FakeUart::operator << (int x) {

    dbglogfile << x;

    if (state == OPEN)
        write (_uartfd, (char *)&x, sizeof(int)); 
}

FakeUart& 
FakeUart::operator << (std::string &s) {
    dbglogfile << s;
  
    if (state == OPEN)
        *this << s.c_str();
}

FakeUart& 
FakeUart::operator << (const char *c) {
    dbglogfile << c;
  
    if (state == OPEN) {
        write (_uartfd, c, strlen(c));
    }
}
#endif

// Accessors
inline void
FakeUart::SetName (std::string &newname)
{
    name = newname;
}

void 
FakeUart::SetState (state_e s)
{
    state = s;
}

FakeUart::state_e
FakeUart::GetState (void)
{
    return state;
}

void 
FakeUart::SendEOL (void)
{
    Putc('\r');
    Putc('\n');  
}

// local Variables:
// mode: C++
// indent-tabs-mode: nil
// End:
