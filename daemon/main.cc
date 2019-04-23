// 
// Copyright (C) 2005, 2006, 2007, 2008, 2009, 2010, 2011, 2012, 2013
//               2014, 2015, 2016, 2017, 2018, 2019
// Free Software Foundation, Inc.
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
//

/// \copyright GNU Public License.
/// \file main.cc Main loop for the daemon.

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

#include <boost/asio.hpp>

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
#include "database.h"
#ifdef HAVE_SNMP
# include "snmp.h"
#endif
#include "rc.h"
#include "commands.h"
#include "onewire.h"
using namespace boost::asio;
using namespace boost::asio::ip;
#include <boost/system/error_code.hpp>
using namespace boost::system;

static void usage (const char *);

// Protoypes for the threads entry point
extern void onewire_handler(Onewire &ow);
extern void client_handler(boost::asio::ip::tcp::socket &tcp_socket);
extern void ownet_handler(Ownet &);
extern void outback_handler(Ownet &);
extern void xantrex_handler(Ownet &);

// This queue is used to pass data between the threads.
std::mutex queue_lock;
std::queue <XML> tqueue;
std::condition_variable queue_cond;

// Note that an entry for 'pi' needs to be in /etc/hosts.
const char DEFAULT_ARGV[] = "-s pi:4304";
const char *DEFAULT_UART = "/dev/ttyS0";

#include <boost/system/error_code.hpp>
using namespace boost::system;

int
main(int argc, char *argv[])
{
    int c, i;
    std::string item, str;
    const char *filespec;
    std::string hostname = "lccalhost";
    std::string owserver = "localhost";
    bool snmp;
    bool daemon;
    bool client;
    std::condition_variable alldone;

    log_init("pgd");
    
    // scan for the two main standard GNU options
    for (c = 0; c < argc; c++) {
        if (strcmp("--help", argv[c]) == 0) {
            usage(argv[0]);
            exit(0);
        }
        if (strcmp("--version", argv[c]) == 0) {
            std::cerr << "PowerGuru version: " << VERSION << std::endl;
            exit(0);
        }
    }

    // Set the option flags to default values. We do it this way to
    // shut up GCC complaining they're not used.
    //daemon = true;
    //client = false;
    snmp = false;
    // Load the database config variable so they can be overridden by
    // the command line arguments.
    //RCinitFile config;
    //config.load_files();
    //if (config.deviceGet().size() > 0) {
    //    filespec = (char *)config.deviceGet().c_str();
    //}
    
    filespec = DEFAULT_UART;
    // Process the command line arguments.
    while ((c = getopt (argc, argv, "d:ahvw:s:")) != -1) {
        switch (c) {
          case 'd':
              filespec = optarg;
              break;

          case 'h':
              usage (argv[0]);
              exit;
              break;
          case 'w':
              // this string needs to include the port if it's
              // not using the default one.
              owserver = optarg;
              break;

#ifdef USE_SNMP
          case 'j':
              snmp = true;
              break;
#endif
          case 'v':
              // verbosity++;
              //dbglogfile.set_verbosity();
              // BOOST_LOG(lg) << "FIXME: Verbose output NOT turned on";
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

    io_service ioservice;
//    tcp::endpoint tcp_endpoint{tcp::v4(), 7654};
//    tcp::acceptor tcp_acceptor{ioservice, tcp_endpoint};
    tcp::socket tcp_socket{ioservice};
//    std::string data;
//    tcp::resolver resolv{ioservice};
//    std::array<char, 4096> bytes;
    std::thread client_thread (client_handler, std::ref(tcp_socket));

    Onewire ow;
    std::thread onewire_thread (onewire_handler, std::ref(ow));
#ifdef BUILD_OWNET
    Ownet ownet(owserver);
    std::thread ownet_thread (ownet_handler, std::ref(ownet));
#endif
#ifdef BUILD_XANTREX
    std::thread xantrex_tread (xantrex_handler, std::ref(ownet));
#endif
#ifdef BUILD_OUTBACK
    std::thread outback_thread (outback_handler, std::ref(ownet));
//    std::thread forth (msg_handler, std::ref(pdb));
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
    BOOST_LOG(lg) << "Killing all threads...";

    onewire_thread.join();      // pauses until first finishes
    client_thread.join();       // pauses until first finishes
#ifdef BUILD_OWNET
    ownet_thread.join();        // pauses until second finishes
#endif
#ifdef BUILD_XANTREX
    xantrex_thread.join();      // pauses until third finishes
#endif
#ifdef BUILD_OUTBACK
    outback_thread.join();      // pauses until second finishes
#endif

    exit(0);
}

static void
usage (const char *prog)
{
    std::cerr <<"This program implements a command line interface" << std::endl; 
    std::cerr << "for an inverter or charge controller" << std::endl;
    std::cerr << "Usage: " << prog << " [sglvphmdcx]" << std::endl;

#if 0
    // enable SNMP daemon mode
    std::cerr << "SNMP Mode:" << std::endl;
    std::cerr << "\t-j\t\t\t\tEnable SNMP agent mode" << std::endl;
    std::cerr << "\t-r\t\t\t\trun in the background as a daemon." << std::endl;
    // Display the End User options
    std::cerr << "User Options:" << std::endl;
    // cerr << "\t-s [heading:item or name]\tSet Item value" << std::endl;
    // cerr << "\t-g [heading:item or name]\tGet Item value" << std::endl;
    std::cerr << "\t-p\t\t\t\tPoll the Meters" << std::endl;
    std::cerr << "\t-l\t\t\t\tLogfile name" << std::endl;
    std::cerr << "\t-v\t\t\t\tVerbose mode" << std::endl;
    std::cerr << "\t-d [filespec]\t\t\tSpecify Serial Port" << std::endl;
    std::cerr << "\t-h\t\t\t\tHelp (this display)" << std::endl;
    std::cerr << "\t-a\t\t\t\tDisplay Command names" << std::endl;

    // Display the Maintainer options
    std::cerr << "Maintainer Options:" << std::endl;
    std::cerr << "\t-m head:item\t\t\tMenu Item index" << std::endl;
    std::cerr << "\t-d\t\t\t\tDump internal data" << std::endl;
    std::cerr << "\t-x\t\t\t\tXantrex Console mode" << std::endl;
    std::cerr << "\t-o\t\t\t\tOutback Console mode" << std::endl;
    std::cerr << "\t-e\t\t\t\tEcho Input Mode" << std::endl;
#endif

    // Display the Database options
    std::cerr << "Database Options:" << std::endl;
    std::cerr << "\t-m hostname\t\t\tSpecify Database hostname or IP" << std::endl;
    exit(0);
}

// local Variables:
// mode: C++
// indent-tabs-mode: nil
// End:
