// 
// Copyright (C) 2005, 2006-2018 Free Software Foundation, Inc.
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

#ifndef __FAKEUART_H__
#define __FAKEUART_H__

#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <iostream>
#include <fstream>

#define DEFAULT_FAKEUART "/dev/ptypc"

class FakeUart {
public:
  enum state_e {IDLE, OPEN, INPROGRESS, CLOSED};
  FakeUart(void);
  ~FakeUart(void);
  retcode_t Open(void);
  retcode_t Open(const std::string &filespec);
  retcode_t Close(const std::string &filespec);

  int Read(unsigned char *, int);
  //int Write(const unsigned char *, int);
  int Write(const std::string &str);
  void SendEOL(void);

   // get a byte from the console
  int Getc (void) { return Getc (uartStream); }
  int UnGetc (int ch) { return UnGetc (ch, uartStream); }
  int Getc (FILE *x) { return getc(x); }
  int UnGetc (int ch, FILE *x) { return ungetc(ch, x); }

  // write a byte to the console
  int Putc (int x) { return Putc (x, uartStream); }
  int Putc (int x, FILE * y) { return putc(x, y); }

  FakeUart &operator << (int);
  FakeUart &operator << (const char *);
  FakeUart &operator << (const std::string &);
  //  ostream&  operator << (ostream & (&)(ostream &));

  void SetName (const std::string &name);
  std::string &GetName (void) { return name; }


  state_e GetState (void);
  void SetState (state_e);

  FILE *GetStreamHandle(void) { return uartStream; }

  int GetFileDescriptor(void) { return _uartfd; }


private:
  std::string name;
  int _uartfd;
  FILE *uartStream;
  state_e state;
};

#endif
