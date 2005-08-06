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

// This sets up a console for user interaction, and defaults to using
// standard in.

#include <string>
#include <cstdio>

#include "err.h"
#include "log.h"
#include "console.h"

#include <fcntl.h>
#include <termios.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>


using namespace std;

void signal_handler (int);

Console::Console (void) {
    state = Console::UNSET;
    //    Open ("stdin");
}

Console::~Console (void) {
  Close();
  inchannel.filespec.erase();
  outchannel.filespec.erase();
  state = Console::UNSET;
}

void
Console::Open (void) {
  // Open a console for the end user to type at. We default to stdio (cin)
  //  string str = "stdin";
  
  OpenInChannel();
  
  //  str = "stdout";

  // Open a the output channel for the console. We default to stdio (cout)
  OpenOutChannel();
}

void
Console::OpenInChannel (string channel) {
  if (channel == "stdin") {
    inchannel.fhandle = stdin;
    inchannel.filespec = "stdin";
    state = Console::OPEN;
  }

#if 1
  signal (SIGINT, signal_handler);
  signal (SIGQUIT, signal_handler);
#else
  struct sigaction saio;

  // FIXME: we want to trap ^C on the console, but this doesn't seem to work
  saio.sa_handler = signal_handler;
  saio.sa_mask = 0;
  saio.sa_flags = 0;
  saio.sa_restorer = 0;

  sigaction(SIGIO, &saio, 0);
#endif

  fcntl(fileno(inchannel.fhandle), F_SETOWN, getpid());

  MakeRaw(fileno(inchannel.fhandle));
}

void
Console::OpenInChannel (void) {
  inchannel.fhandle = stdin;
  inchannel.filespec = "stdin";
  state = Console::OPEN;

#if 1
  signal (SIGINT, signal_handler);
  signal (SIGQUIT, signal_handler);
#else
  struct sigaction saio;

  // FIXME: we want to trap ^C on the console, but this doesn't seem to work
  saio.sa_handler = signal_handler;
  saio.sa_mask = 0;
  saio.sa_flags = 0;
  saio.sa_restorer = 0;

  sigaction(SIGIO, &saio, 0);
#endif

  fcntl(fileno(inchannel.fhandle), F_SETOWN, getpid());

  MakeRaw(fileno(inchannel.fhandle));
}

void
Console::OpenOutChannel (string channel) {
  if (channel == "stdout") {
    outchannel.fhandle = stdout;
    outchannel.filespec = "stdout";
    state = Console::OPEN;
  }
  //    MakeRaw(fileno(outchannel.fhandle));
}

void
Console::OpenOutChannel (void) {
  outchannel.fhandle = stdout;
  outchannel.filespec = "stdout";
  state = Console::OPEN;
}

// Reset the input channel to be where it was when we started
void
Console::Close (void) {
  state = Console::CLOSED;
  Reset();
}

void
Console::Reset (void) {
  tcsetattr(fileno(inchannel.fhandle), TCSANOW, &oldtty);
}

void
Console::MakeRaw (int x) {
  termios newtty;
  int flag;

  tcgetattr(x, &oldtty);
  newtty = oldtty;
  newtty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL|IXON);
  newtty.c_oflag &= ~OPOST;
  newtty.c_lflag &= ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN);
  newtty.c_cflag &= ~(CSIZE|PARENB);
  newtty.c_cflag |= CS8;
  tcsetattr(x, TCSANOW, &newtty);
  
  flag = fcntl(x, F_GETFL, 0);
  fcntl(x, F_SETFL, flag | O_NDELAY);
}

void
Console::SendEOL (void) {
  Putc('\r');
  Putc('\n'); 
}

void
signal_handler (int sig)
{
  dbglogfile << "Got a " << sig << " from the console" << endl;
  exit(sig);
}
