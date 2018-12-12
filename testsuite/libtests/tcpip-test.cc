// 
//   Copyright (C) 2005, 2006-2018 Free Software Foundation, Inc.
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

int verbosity;
static void usage (void);
bool waitforgdb = false;
static void cntrlc_handler (int);
static void alarm_handler (int);
static bool loop = true;

int
start_proc (const std::string &procname);

TestState runtest;
class Test : public Tcpip
{
public:
    Test(void) {
        // Get some system information needed to test the classes.
        std::memset(_thost, 0, MAXHOSTNAMELEN);
        if (gethostname(_thost, MAXHOSTNAMELEN) != 0) {
            std::cerr << "ERROR: gethostname() failed!" << std::endl;
            exit(-1);
        } 
        if (_thost == 0) {
            std::cerr << "ERROR: gethostbyname() failed!" << std::endl;
            exit(-1);        
        }
        _tservice = getservbyname("git", NULL);
        if (_tservice == 0) {
            std::cerr << "ERROR: getservbyname() failed!" << std::endl;
            exit(-1);        
        }
    };

    ~Test(void) {
    };

    int test(void) {
        // See if we can do service lookups
        _tservice = lookupService("powerguru", "tcp");
        if (_tservice != 0) {
            if (strcmp(_tservice->s_name, "powerguru") == 0 &&
                strcmp(_tservice->s_proto, "tcp") == 0 &&
                _tservice->s_port == htons(7654))
                runtest.pass ("lookupService(powerguru)");
            else
                runtest.fail ("lookupService(powerguru)");
        } else {
            runtest.fail ("lookupService(powerguru)");
        }

        createNetServer(7654);
        int pid = start_proc("./childtcpip");

        // sleep so the child process has time to run, and we have time
        // to debug it. We setup a handler for ^C, so we can get out of
        // this sleep when we're done.
        if (waitforgdb) {
            struct sigaction  act2;
            act2.sa_handler = cntrlc_handler;
            sigaction (SIGINT, &act2, NULL);
            sleep(300);
        } else {
            sleep(1);
        }

        int retries = 3;
        while (retries-- > 0) {
            retcode_t ts = newNetConnection(true);
            if (ts) {
                break;
            } else {
                // Under purify, things are very slow, so wait before retries
                sleep(10);
            }
        }
        struct sigaction  act;
        act.sa_handler = alarm_handler;
        sigaction (SIGALRM, &act, NULL);
        alarm(5);
        std::cout << std::endl
             << "Waiting 5 seconds for input from the child process..." << std::endl;

        // Read data from the child process
        std::vector<unsigned char> data;
        readNet(data);

        if (data.size() > 0) {
            runtest.pass("Established netork connection");
        } else {
            runtest.fail("Established netork connection");
        }

        sleep(1);
        closeNet();
    };

protected:
    char                   _thost[MAXHOSTNAMELEN];
    const struct servent  *_tservice;
    const struct protoent *_tproto;
    const in_addr_t       *_taddr;
};

int
main(int argc, char *argv[])
{
    int c;
    bool dump = false;
    std::string filespec;
    std::string procname, memname;
    char buffer[300];

    memset(buffer, 0, 300);
    
    while ((c = getopt (argc, argv, "hdvsm:")) != -1) {
        switch (c) {
          case 'h':
            usage ();
            break;
            
          case 'd':
            dump = true;
            break;
            
          case 's':
            waitforgdb = true;
            break;
                                                                                
          case 'v':
            verbosity++;
            break;
            
          case 'm':
            memname = optarg;
            std::cerr << "Open " << memname << std::endl;
            break;
            
          default:
            usage ();
            break;
        }
    }
    
    // get the file name from the command line
    if (optind < argc) {
        filespec = argv[optind];
        std::cout << "Will use \"" << filespec << "\" for test " << std::endl;
    }

    Test test;
    test.test();
    
#if 0    
    // See if we can do service lookups
    service = tcpip.lookupService("powerguru", "tcp");
    if (service != 0) {
        if (strcmp(service->s_name, "powerguru") == 0 &&
            strcmp(service->s_proto, "tcp") == 0 &&
            service->s_port == htons(7654))
            runtest.pass ("Tcpip::lookupService(powerguru)");
        else
            runtest.fail ("Tcpip::lookupService(powerguru)");
    } else {
        runtest.fail ("Tcpip::lookupService(powerguru)");
    }
    
//    tcpip.toggleDebug(true);
    // See if we can do protocol lookups
    proto = tcpip.protoDataGet();
    if (proto != 0) {
        if (strcmp(proto->p_name, "tcp") == 0 &&
            proto->p_proto == 6) {
            runtest.pass ("Tcpip::protoDataGet()");
        } else {
            runtest.fail ("Tcpip::protoDataGet()");
        }
    } else {
        runtest.fail ("Tcpip::protoDataGet()");    
    }
    if (tcpip.protoNameGet() == "tcp") {
        runtest.pass ("Tcpip::protoNameGet()");
    } else {
        runtest.fail ("Tcpip::protoNameGet()");
    }
    if (tcpip.protoNumGet() == 6) {
        runtest.pass ("Tcpip::protoNumGet()");
    } else {
        runtest.fail ("Tcpip::protoNumGet()");
    }
#endif
    
}
// Run the tests between two processes
int
start_proc (const std::string &procname)
{
    struct stat procstats;
    char *cmd_line[5];
    pid_t childpid;
    int ret = 0;
    
    // See if the file actually exists, otherwise we can't spawn it
    if (stat(procname.c_str(), &procstats) == -1) {
        std::cerr << "Invalid filename \"" << procname << "\"" << std::endl;
        perror(procname.c_str());
        return -1;
    }
    
    // setup a command line. By default, argv[0] is the name of the process
    memset(cmd_line, 0, sizeof(char *)*5);
    cmd_line[0] = new char(50);
    strcpy(cmd_line[0], procname.c_str());
    if (waitforgdb) {
        cmd_line[1] = new char(3);
        strcpy(cmd_line[1], "-s");
    }
 
    // fork ourselves silly
    childpid = fork();
    
    
    // childpid is a positive integer, if we are the parent, and fork() worked
    if (childpid > 0) {
        std::cerr << "Forked sucessfully, child process PID is " << childpid << std::endl;
        return childpid;
    }
    
    // childpid is -1, if the fork failed, so print out an error message
    if (childpid == -1) {
        perror(procname.c_str());
        return -1;
    }
    
    // If we are the child, exec the new process, then go away
    if (childpid == 0) {
        // Start the desired executable
        std::cout << "Starting " << procname << " with " << cmd_line[0] << std::endl;
        ret = execv(procname.c_str(), cmd_line);
        perror(procname.c_str());
        exit(0);
    }
    return 0;
}
 
void
cntrlc_handler (int sig)
{
    std::cerr << "Got a ^C !" << std::endl;
}

void
alarm_handler (int sig)
{
  std::cerr << "Got an alarm signal !" << std::endl;
  std::cerr << "This is OK, we use it to end this test case." << std::endl;
  loop = false;
}

static void
usage (void)
{
    std::cerr << "This program tests the Global memory system." << std::endl;
    std::cerr << "Usage: tglobal [h] filename" << std::endl;
    std::cerr << "-h\tHelp" << std::endl;
    std::cerr << "-d\tDump parsed data" << std::endl;
    exit (-1);
}
