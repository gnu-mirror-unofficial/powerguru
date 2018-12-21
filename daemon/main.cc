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

#ifdef HAVE_CONFIG_H
// This is generated by autoconf.
# include "config.h"
#endif

#include <cstring>
#include <vector>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <sstream>
#include <signal.h>
#include <errno.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

#ifdef HAVE_GETOPT_H
#include <getopt.h>
#else
extern int optind;
extern char *optarg;
#endif

// local header files
#include "log.h"
#ifdef BUILD_XANTREX
include "xantrex-trace.h"
#endif
#ifdef BUILD_OUTBACK
#include "outbackpower.h"
#include "msgs.h"
#endif
#ifdef BUILD_OWNET
#include "ownet.h"
#endif
#include "menuitem.h"
#include "console.h"
#include "database.h"
#include "snmp.h"
#include "rc.h"
#include "tcpip.h"
#include "xml.h"
#include "serial.h"
#include "commands.h"

using namespace std;
using namespace rcinit;

static void usage (const char *);
extern LogFile dbglogfile;
const int INBUFSIZE = 1024;

void
alarm_handler2 (int sig);

int curses = 0;

extern void client_handler(Tcpip &net);
extern void ownet_handler(Ownet &);
extern void outback_handler(Ownet &);
extern void xantrex_handler(Ownet &);

std::mutex queue_lock;
std::queue <XML> tqueue;
std::condition_variable queue_cond;

int
main(int argc, char *argv[])
{
    int c, i;
    string item, str;
    const char *filespec;
    MenuItem ti;
    string hostname;
    bool poll;
    bool use_db;
    bool snmp;
    bool daemon;
    bool client;
    bool echo;
#if 0
    bool setitem;
    bool getitem;
    bool monitor;
    bool outbackmode;
    bool xantrexmode;
    bool background;
#endif
    retcode_t   ret;
    std::condition_variable alldone;
#if defined(HAVE_MARIADB) || defined(HAVE_POSTGRESQL)
    Database pdb;
#endif
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
    poll = false;
    daemon = true;
    client = false;
    echo = false;
    use_db = true;
    snmp = false;
#if 0
    setitem = false;
    getitem = false;
    monitor = false;
    outbackmode = false;
    xantrexmode = false;
    background = false;
    hostname = "localhost";
#endif
    // Load the database config variable so they can be overridden by
    // the command line arguments.
    RCinitFile config;
    config.load_files();
    if (config.deviceGet().size() > 0) {    
        filespec = (char *)config.deviceGet().c_str();
    }
    
    filespec = DEFAULT_UART;
    // Process the command line arguments.
    while ((c = getopt (argc, argv, "d:ahvm:cexnob:pu:w:rj")) != -1) {
        switch (c) {
          case 'p':
              poll = true;
              //xantrexmode = true;     // FIXME: force xantrex mode for now
              break;

          case 'd':
              filespec = strdup(optarg);
              break;

          case 'h':
              usage (argv[0]);
              break;

          case 'o':
              //outbackmode = true;
              break;

          case 'r':
              //background = true;
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
#if defined(HAVE_MARIADB) && defined(HAVE_POSTGRESQL)
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
#endif
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

    // Start the SNMP daemon support.
#ifdef USE_SNMP
    if (snmp) {
        SnmpDaemon agent;
        agent.master(false);
    }
#endif

    dbglogfile << "PowerGuru - 1 Wire Mode" << std::endl;
    Tcpip net;
    if (net.createNetServer(DEFAULTPORT) == ERROR) {
        std::cerr << "ERROR: Couldn't create a network server!" << std::endl;
        exit(-1);
    }

    std::thread client_thread (client_handler, std::ref(net));
#ifdef BUILD_OWNET
    Ownet ownet("localhost:4303");
    std::thread ownet_thread (ownet_handler, std::ref(ownet));
#endif
#ifdef BUILD_XANTREX
    std::thread xantrex_tread (xantrex_handler, std::ref(ownet));
#endif
#ifdef BUILD_OUTBACK
    std::thread outback_thread (outback_handler, std::ref(ownet));
//    std::thread forth (msg_handler, std::ref(pdb));
#endif
    
#ifdef BUILD_OUTBACK
    // Network daemon/client mode. Normally we're a network daemon that
    // responses to requests by a remote client. Many house networks
    // are behind a firewall, so the daemon can also connect to a
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
                if (xml.parseMem(str) == ERROR) {
                    continue;
                }
            }
            messages.clear();
        }
    }
#endif

#if 0
    // Open a console for user input
    Console con;
    con.openCon();
    con.makeRaw();
    int ch = 0;
    while ((ch = con.getcCon()) != 'q') {
        if (ch > 0) {                // If we have something, process it
            con.putcCon (ch);          // echo inputted character to screen
            switch (ch) {
              case 'Q':
              case 'q':
                  dbglogfile << "Qutting PowerGuru due to user input!" << std::endl;
                  break;
              case '?':
                  con.putsCon("PowerGuru client\n");
                  con.putsCon("\t? - help\r\n");
                  con.putsCon("\tq - Quit\r\n");
                  con.putsCon("\tQ - Quit\r\n");
                  sleep(1);
              default:
                  break;
            };
        }
    }

    con.resetCon();
#endif

    // Commands from the client via the client_handler get processed here
    // so messages can be passed between threads.
    while (true) {
        std::unique_lock<std::mutex> guard(queue_lock);
        queue_cond.wait(guard, [] { return !tqueue.empty(); });
        XML xml = tqueue.front();
        tqueue.pop();
        // if (xml[0]->nameGet() == "list") {
            std::vector<std::string> devs;
            ownet.listDevices(devs);
            std::vector<std::string>::iterator sit;
            for (sit = devs.begin(); sit != devs.end(); sit++) {
                std::cerr << "FIXME: " << *sit <<std::endl;
            }
                    //}
    }

    // synchronize threads:
    dbglogfile << "Killing all threads..." << std::endl;
    client_thread.join();                // pauses until first finishes
#ifdef BUILD_OWNET
    ownet_thread.join();                // pauses until second finishes
#endif
#ifdef BUILD_XANTREX
    xantrex_thread.join();               // pauses until third finishes
#endif
#ifdef BUILD_OUTBACK
    outback_thread.join();               // pauses until second finishes
#endif

    exit(0);
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
    datasrc.RecvPacket((unsigned char *)&ch, 1);
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
#if 0
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
#endif

    // Display the Database options
    cerr << "Database Options:" << endl;
    cerr << "\t-m hostname\t\t\tSpecify Database hostname or IP" << endl;
    cerr << "\t-u username\t\t\tSpecify User Name" << endl;
    cerr << "\t-b dbname\t\t\tSpecify the Database" << endl;
    cerr << "\t-w password\t\t\tSpecify the password" << endl;

    exit (-1);
}

// local Variables:
// mode: C++
// indent-tabs-mode: nil
// End:
