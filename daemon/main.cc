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

#include <net-snmp/net-snmp-config.h>
#ifdef HAVE_CONFIG_H
# undef PACKAGE_VERSION
# undef PACKAGE_TARNAME
# undef PACKAGE_STRING
# undef PACKAGE_NAME
# undef PACKAGE_BUGREPORT
// This is generated by autoconf.
# include "config.h"
#endif

#include <string>
#include <vector>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <sstream>
#include <signal.h>
#include <errno.h>

#ifdef HAVE_GETOPT_H
#include <getopt.h>
#else
extern int optind;
extern char *optarg;
#endif

// local header files
#include "log.h"
#include "err.h"
#include "xantrex-trace.h"
#include "outbackpower.h"
#include "console.h"
#include "menuitem.h"
#include "database.h"
#include "snmp.h"
#include "rc.h"
#include "tcpip.h"
#include "msgs.h"
#include "xml.h"

using namespace std;
using namespace rcinit;
using namespace outbackpower;

static void usage (const char *);
extern LogFile dbglogfile;
const int INBUFSIZE = 1024;

void
alarm_handler2 (int sig);

int curses = 0;

int
main(int argc, char *argv[]) {
    int c, i;
    ErrCond Err;
    string item, str;
    const char *filespec;
    MenuItem ti;
    string hostname;
    bool console;
    bool setitem;
    bool getitem;
    bool echo;
    bool monitor;
    bool poll;
    bool outbackmode;
    bool xantrexmode;
    bool use_db;
    bool snmp;
    bool background;
    bool daemon;
    bool client;
    retcode_t   ret;

    Database pdb;
    //    XantrexUI ui;
    //    Console con;

    if (argc == 1) {
      //usage(argv[0]);
    }

    // scan for the two main standard GNU options
    for (c=0; c<argc; c++) {
      if (strcmp("--help", argv[c]) == 0) {
        usage(argv[0]);
        exit(0);
      }
      if (strcmp("--version", argv[c]) == 0) {
        cerr << "PowerGuru version: " << VERSION << endl;
        exit(0);
      }
    }

    // Set the option flags to default values. We do it this way to
    // shut up GCC complaining they're not used.
    console = false;
    setitem = false;
    getitem = false;
    echo = false;
    monitor = false;
    poll = false;
    outbackmode = false;
    xantrexmode = false;
    use_db = true;
    snmp = false;
    background = false;
    daemon = true;
    client = false;
    hostname = "localhost";
    
    // Load the database config variable so they can be overridden by
    // the command line arguments.
    RCinitFile config;
    config.load_files();
    if (config.dbhostGet().size() > 0) {
      pdb.dbHostSet(config.dbhostGet());
    }
    if (config.dbnameGet().size() > 0) {    
      pdb.dbNameSet(config.dbnameGet());
    }
    if (config.dbuserGet().size() > 0) {    
      pdb.dbUserSet(config.dbuserGet());
    }
    if (config.dbpasswdGet().size() > 0) {    
      pdb.dbPasswdSet(config.dbpasswdGet());
    }

    if (config.deviceGet().size() > 0) {    
      filespec = (char *)config.deviceGet().c_str();
    }
    
    filespec = DEFAULT_UART;
    // Process the command line arguments.
    while ((c = getopt (argc, argv, "d:ahvm:cexnob:pu:w:rj")) != -1) {
      switch (c) {
      case 'p':
        poll = true;
        xantrexmode = true;     // FIXME: force xantrex mode for now
	break;

#if 0
      case 'i':
        ui.Dump();
        exit(0);
	break;

      case 'a':
        ui.DumpAliases();
        exit(0);
        break;

      case 'i':
	item = strdup(optarg);
        monitor = true;
	break;
#endif
        
      case 'd':
	filespec = strdup(optarg);
	break;

      case 'h':
	usage (argv[0]);
	break;

      case 'x':
        //console = true;
        xantrexmode = true;
	break;

      case 'o':
        outbackmode = true;
	break;

      case 'r':
        background = true;
	break;

#ifdef USE_SNMP
      case 'j':
        snmp = true;
        daemon = false;
        client = false;
	break;
#endif
      case 'e':
        echo = true;
	break;
        
        // Don't use the database. The default is to use the database.
      case 'n':
        use_db = false;
        break;

        // Specify database host machine.
      case 'm':
        pdb.dbHostSet(optarg);
        break;
        // Specify database name.
      case 'b':
        pdb.dbNameSet(optarg);
        break;
        
        // Specify database user name.
      case 'u':
        pdb.dbUserSet(optarg);
        break;
        
        // Specify database user password.
      case 'w':
        pdb.dbPasswdSet(optarg);
        break;

      case 'c':
        client = true;
        daemon = false;
        break;

      case 'v':
        // verbosity++;
	dbglogfile.set_verbosity();
	dbglogfile << "Verbose output turned on" << endl;
	break;
	
      default:
	usage (argv[0]);
        break;
      }
    }

    // Open the network connection to the database.
    if (use_db) {
      if (!pdb.openDB()) {
        dbglogfile << "ERROR: Couldn't open database!" << endl;
        exit(1);
      }
    }

    // Start the SNMP daemon support.
#ifdef USE_SNMP
    if (snmp) {
      SnmpDaemon agent;
      agent.master(false);
    }
#endif

    //

        
    // Talk to an Outback Power Systems device
    if (outbackmode) {
      Console con;
      // Open a console for user input
      con.Open();
    
      con.Puts("PowerGuru - Outback Mode\r\n");
      //outback outdev("/dev/pts/7");
      outback outdev(filespec);
      if (poll) {
        // outdev.poll();
      } else {
        if (outdev.main(con, pdb) == ERROR) {
          dbglogfile << "ERROR: Main Loop exited with an error!" << endl;
        }
        
      }
      con.Reset();
      con.Close();
      exit(0);
    }

    if (xantrexmode) {
      XantrexUI ui;
      Console con;
      // Open a console for user input
      con.Open();
      if (poll) {
        // Open the serial port
        try {
          ui.Open(filespec);
        }
        catch (ErrCond catch_err) {
          cerr << catch_err << endl;
          exit(1);
        }
        //
        for (i=0; i<1000; i++) {        
          //display = ui.MenuHeadingPlus();
#if 0
          ch = con.Getc();
          switch (ch) {
            // Toggle the DTR state, which is as close as we get to
            // flow control.
          case 'Q':
          case 'q':
            return SUCCESS;
            break;
          case '?':
            con.Puts("PowerGuru - Outback Mode\r\n");
            con.Puts("\t? - help\r\n");
            con.Puts("\tq - Quit\r\n");
            con.Puts("\tQ - Quit\r\n");
            sleep(2);
          default:
            break;
          };
#endif
          
          vector<meter_data_t *> data = ui.PollMeters(1);
//           ui.MenuHeadingPlus();
//           ui.MenuHeadingMinus();          
          pdb.queryInsert(data);
#if 0
          for (i=0; i<data->size(); i++) {
            //cout << "Inverter/Charger amps: " << data[i]->inverter_amps << endl;
            cout << "Input amps AC: " << data[i]->input_amps << endl;
            cout << "Load  amps AC: " << data[i]->load_amps << endl;
            cout << "Battery actual volts DC: " << data[i]->actual_volts << endl;
            cout << "Battery TempComp volts DC: " << data[i]->tempcomp_volts << endl;
            cout << "Inverter volts AC: " << data[i]->inverter_volts << endl;
            cout << "Grid (AC1) volts AC: " << data[i]->ac1 << endl;
            cout << "Generator (AC2) volts AC: " << data[i]->ac2 << endl;
            cout << "Read Frequency Hertz: " << data[i]->hertz << endl;
            //pdb.queryInsert(data[i]);
            //delete data[i];
          }
#endif
          //sleep(1);
          cout << endl;
        }
      }
      con.Reset();
      con.Close();
      exit(0);
    }

    if (use_db) {
      if (!pdb.closeDB()) {
        dbglogfile << "ERROR: Couldn't open database!" << endl;
        exit(1);
      }
    }
    
    // Network daemon/client mode. Normally we're a network daemon that
    // responses to requests by a remote client. Many house networks
    // are behind a firewall,so the daemon can also connect to a
    // publically accessible host to establish the connection the
    // other direction.
    if (daemon || client) {
      
      Msgs msg;
      msg.toggleDebug(true);
      
      // Make a client connection
      if (client == true) {
        msg.init(hostname);
      }
      
      // Start as a daemon
      if (daemon == true) {
        msg.init(true);
      }
      //msg.methodsDump();          // FIXME: debugging crap
      
      //msg.print_msg(msg.status((meter_data_t *)0));
      
      if (client) {
        msg.writeNet(msg.metersRequestCreate(Msgs::BATTERY_VOLTS));
      }
      
      //      msg.cacheDump();
      
      XML xml;
      unsigned int i;

      vector<const xmlChar *> messages;
      //const xmlChar *messages[200];
        
      bool loop = true;
      while (loop) {
        ret = msg.anydata(messages);
        if (ret == ERROR) {
          dbglogfile << "ERROR: Got error from socket " << endl;
          msg.closeNet();
          // wait for the next connection
          if ((ret = msg.newNetConnection(true))) {
            dbglogfile << "New connection started for remote client."
                       << msg.remoteIP().c_str()
                       << msg.remoteName().c_str() << endl;
            ret = SUCCESS;        // the error has been handled
            continue;
          }
        }
        if (messages.size() == 0) {
          dbglogfile << "ERROR: client socket shutdown! " << endl;
        }
        for (i=0; i < messages.size(); i++) {
          cerr << "Got (" << messages.size() << ") messages " << messages[i] << endl;
          string str = (const char *)messages[i];
          delete messages[i];
          if (msg.findTag("command")) {
            cerr << "Got command message!" << endl;
          }
          if (xml.parseXML(str) == ERROR) {
            continue;
          }
        }
        messages.clear();
      }
    }
}


// signal handler for displaying item values
void
alarm_handler2 (int sig)
{
  DEBUGLOG_REPORT_FUNCTION;
  ostringstream oss;
  struct sigaction  act;
#if 0
  int ch;
  struct errcond Err;
  
#if 1
  // If there is keyboard input, stop looking for the old
  // output.
  ch = con.Getc();
  if (ch > 0) {
    alarm(0);
    con.Ungetc(ch);
    return;
  }
#else
  datasrc.RecvPacket((unsigned char *)&ch, 1, Err);
  dbglogfile << " CH is " << ch << endl;
  
  if (ch > 0) {
    alarm(0);
    con.Ungetc(ch);
    return;
  }
#endif
#endif
  
  act.sa_handler = alarm_handler2;
  sigaction (SIGALRM, &act, NULL);
  
  alarm(1);
}

static void
usage (const char *prog)
{
  cerr <<"This program implements a command line interface" << endl; 
  cerr << "for an inverter or charge controller" << endl;
  cerr << "Usage: " << prog << " [sglvphmdcx]" << endl;

  // enable SNMP daemon mode
  cerr << "SNMP Mode:" << endl;
  cerr << "\t-j\t\t\t\tEnable SNMP agent mode" << endl;
  cerr << "\t-r\t\t\t\trun in the background as a daemon." << endl;

  // Display the End User options
  cerr << "User Options:" << endl;
  // cerr << "\t-s [heading:item or name]\tSet Item value" << endl;
  // cerr << "\t-g [heading:item or name]\tGet Item value" << endl;
  cerr << "\t-p\t\t\t\tPoll the Meters" << endl;
  cerr << "\t-l\t\t\t\tLogfile name" << endl;
  cerr << "\t-v\t\t\t\tVerbose mode" << endl;
  cerr << "\t-d [filespec]\t\t\tSpecify Serial Port" << endl;
  cerr << "\t-h\t\t\t\tHelp (this display)" << endl;
  cerr << "\t-a\t\t\t\tDisplay Command names" << endl;

  // Display the Maintainer options
  cerr << "Maintainer Options:" << endl;
  cerr << "\t-m head:item\t\t\tMenu Item index" << endl;
  cerr << "\t-d\t\t\t\tDump internal data" << endl;
  cerr << "\t-x\t\t\t\tXantrex Console mode" << endl;
  cerr << "\t-o\t\t\t\tOutback Console mode" << endl;
  cerr << "\t-e\t\t\t\tEcho Input Mode" << endl;
  
  // Display the Database options
  cerr << "Database Options:" << endl;
  cerr << "\t-m hostname\t\t\tSpecify Database hostname or IP" << endl;
  cerr << "\t-u username\t\t\tSpecify User Name" << endl;
  cerr << "\t-b dbname\t\t\tSpecify the Database" << endl;
  cerr << "\t-w password\t\t\tSpecify the password" << endl;

  exit (-1);
}

