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

#include <string>
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
#include <string>
#include <fstream>

using namespace std;

extern LogFile dbglogfilefile;

FakeUart::FakeUart(void)
{
  uartfd = 0;
  name = DEFAULT_FAKEUART;
  state = IDLE;
}

FakeUart::~FakeUart(void)
{
  dbglogfile << "Closing fake uart " << name << endl;

  // Don't try to close a device that isn't open
  if (state == OPEN) {
    close(uartfd);
  }
}

retcode_t
//FakeUart::Open(string &filespec, ErrCond &Err)
FakeUart::Open(ErrCond *Err)
{
  DEBUGLOG_REPORT_FUNCTION;

  string filespec;
  int i,j, flag;
  int master = -1;
  struct termios newtty, oldtty;
  char *letterpart = "pqrstuvwxyzPQRST";
  char *numberpart = "0123456789abcdef";

  filespec = "/dev/ptmx";
  dbglogfile << "Opening " << filespec << endl;
  if (filespec.size() > 0) {
    if ((master = open(filespec.c_str(), O_RDWR | O_NONBLOCK)) < 0) {
      if (errno == ENOENT) {
	cerr << "ERROR: no more pty's available";
	return ERROR;
      }
    }
  }

  // filespec = "/dev/pty??";
  filespec = "/dev/ptmx";
  
  // Search for a free pty
  for(i=0; i<16 && master <= 0; i++) {
    for(j=0; j<16 && master <= 0; j++) {
      filespec[8] = letterpart[i];
      filespec[9] = numberpart[j];
      if ((master = open(filespec.c_str(), O_RDWR | O_NONBLOCK)) < 0) {
	if (errno == ENOENT) {
	  cerr << "ERROR: no more pty's available";
	  return ERROR;
	}
      }
    } // for j loop
  } // for i loop

  if ((master < 0) && (i == 16) && (j == 16)) {
    // failed to found an available pty
    cerr << "ERROR: couldn't open a pty";
    return(ERROR);
  }

  state = OPEN;
  
  // create the name of the slave pty
  //filespec[5] = 't';
  filespec = ptsname(master);

  dbglogfile << "Opened " << ptsname(master) << " with file descriptor " << master << endl;

  grantpt(master);
  unlockpt(master);

  uartfd = master;
  uartStream = fdopen(uartfd, "w+");

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
FakeUart::Close(string &filespec, ErrCond &Err)
{
  DEBUGLOG_REPORT_FUNCTION;

  if (state == OPEN)
    close(uartfd);
  
  uartfd = 0;
  state = CLOSED;
  return SUCCESS;
}

int
FakeUart::Read(unsigned char *buf, int nbytes, ErrCond &Err)
{
  // DEBUGLOG_REPORT_FUNCTION;

  return read (uartfd, buf, nbytes);
}

int
FakeUart::Write(unsigned char *buf, int nbytes, ErrCond &Err)
{
  //  DEBUGLOG_REPORT_FUNCTION;

  write (uartfd, buf, nbytes);
}

#if 0
int
FakeUart::GetChar(char *buf, ErrCond &Err)
{
  //  DEBUGLOG_REPORT_FUNCTION;

  read (uartfd, buf, 1);
  
  return (int)*buf;
}

ostream &operator<<(ostream &os, FakeUart &l) {
    return os << l.GetName();
}

FakeUart&
FakeUart::operator << (int x) {

  dbglogfile << x;

  if (state == OPEN)
    write (uartfd, (char *)&x, sizeof(int)); 
}

FakeUart& 
FakeUart::operator << (string &s) {
  dbglogfile << s;
  
  if (state == OPEN)
    *this << s.c_str();
}

FakeUart& 
FakeUart::operator << (const char *c) {
  dbglogfile << c;
  
  if (state == OPEN) {
    write (uartfd, c, strlen(c));
  }
}
#endif

// Accessors
inline void
FakeUart::SetName (string &newname)
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
