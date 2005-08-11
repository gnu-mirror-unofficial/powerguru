// 
//   Copyright (C) 2005 Free Software Foundation, Inc.
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

// This is generated by autoconf
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdarg.h>
#include <signal.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <cstdio>
#include <iostream>

#include "dejagnu.h"
#include "tcpip.h"
#include "console.h"

using namespace std;
extern LogFile dbglogfile;
static void usage (const char *);

int
main(int argc, char *argv[])
{
    int c;
    string filespec;
    bool client;
    bool daemon;
    string hostname;
    string user;
    string passwd;

    client = true;
    daemon = false;
    hostname = "localhost";
    
    while ((c = getopt (argc, argv, "hvdcm:u:p:")) != -1) {
      switch (c) {
      case 'h':
        usage (argv[0]);
        break;
        
      case 'v':
        dbglogfile.set_verbosity();
        break;
        
      case 'm':
        hostname = strdup(optarg);
        break;
        
        // Specify database user name.
      case 'u': 
        user = strdup(optarg);
        break;
        
        // Specify database user password.
      case 'p':
        passwd = strdup(optarg);
        break;   
        
      case 'c':
        client = true;
        daemon = false;
        break;   
        
      case 'd':
        client = false;
        daemon = true;
        break;   
        
      default:
        usage (argv[0]);
        break;
      }
    }
    
    // get the file name from the command line
    if (optind < argc) {
        filespec = argv[optind];
        dbglogfile << "Will use \"" << filespec << "\" for test " << endl;
    }

    Tcpip tcpip;
    Console con;
    int ch;

    tcpip.toggleDebug(true);

    // Open a console for user input
    con.Open();

    char *buffer;
      
    buffer = (char *)new char[300];
    memset(buffer, 0, 300);
      
    // If we are in client mode, connect to the server
    if (client) {
      if (tcpip.createNetClient(hostname)) {
        dbglogfile << "Connected to server at " << hostname.c_str() << endl;
      } else {
        dbglogfile << "ERROR: Couldn't create connection to server" << hostname  << endl;
      }
    }
    
    // If we are in daemon mode, wait for the remote server to connect to us
    // as a client. This is for when the server is behind a firewall.
    if (daemon) {
      if (tcpip.createNetServer()) {
        dbglogfile << "New server started for remote client." << endl;
      } else {
        dbglogfile << "ERROR: Couldn't create a new server" << endl;
      }      

      if (tcpip.newNetConnection(true)) {
        dbglogfile << "New connection started for remote client." << endl;
      } else {
        dbglogfile << "ERROR: Couldn't create a new connection!" << endl;
        exit (1);
      }
    }
    
    tcpip.writeNet("\r<powerguru><version>0.2</version></powerguru>\n");
      
    while ((ch = con.Getc()) != 'q') {
      if (ch > 0){                // If we have something, process it
        //con.Putc (ch);          // echo inputted character to screen
        
        switch (ch) {
          // Toggle the DTR state, which is as close as we get to
          // flow control.
        case 'Q':
        case 'q':
          tcpip.writeNet("quit");
          exit(0);
          break;
        case '?':
          con.Puts("PowerGuru client\n");
          con.Puts("\t? - help\r\n");
          con.Puts("\tq - Quit\r\n");
          con.Puts("\tQ - Quit\r\n");
          sleep(2);
        default:
          break;
        };
      }

      int bytes = tcpip.readNet(buffer, 300, 0);
#if 1
      if (bytes > 0) {
        con.Puts(buffer);
        con.Puts("\r\n");
        if (strncmp(buffer, "quit", 4) == 0) {
          exit(0);
        } 
      }
#endif
      con.Close();
    }
}

static void
usage (const char *prog)
{
    cerr << "This is a simple cmmand line for the PowerGuru daemon." << endl;
    cerr << "Usage: pguru: [h] filename" << endl;
    //    cerr << "Usage: " << prog << ": [h] filename" << endl;
    cerr << "-h\tHelp" << endl;
    cerr << "-v\tVerbose output" << endl;
    cerr << "-d\tDaemon Mode" << endl;
    cerr << "-c\tClient Mode (default)" << endl;
    cerr << "-m\tRemote Machine (localhost)" << endl;
    cerr << "-u\tRemote Machine user" << endl;
    cerr << "-p\tRemote Machine password" << endl;
    exit (-1);
}
