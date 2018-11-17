// 
// Copyright (C) 2005, 2006 - 2018
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

#include <sys/types.h>
#include <sys/stat.h>
#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <cstring>
#include <cstdlib>
#include <signal.h>
#include <iostream>

#include "proc.h"

using namespace std;

static int verbosity;

Proc::Proc (void) {
}

Proc::~Proc (void) {
}

bool
Proc::Start (void) {
    cerr << __PRETTY_FUNCTION__ << ": not implemented!" << endl;
    return false;
}

bool
Proc::Start (string procname) {
    return Start (procname, false);
}

bool
Proc::Start (string procname, bool b) {
    struct stat procstats;
    pid_t childpid;
    char *cmd_line[20];

    output[procname] = b;

    // simple debug junk
    if (verbosity)
	cout << "Starting \"" << procname << "\"" << endl;

    // See if the file actually exists, otherwise we can't spawn it
    if (stat(procname.c_str(), &procstats) == -1) {
        cerr << "Invalid filename \"" << procname << "\"" <<endl;
        perror(procname.c_str());
	return (false);
    }

    // setup a command line. By default, argv[0] is the name of the process
    cmd_line[0] = new char(50);
    strcpy(cmd_line[0], procname.c_str());

    // fork ourselves silly
    childpid = fork();
    
    // childpid is a positive integer, if we are the parent, and fork() worked
    if (childpid > 0) {
	tbl[procname] = childpid;
        return (true);
    }
    
    // childpid is -1, if the fork failed, so print out an error message
    if (childpid == -1) {
        /* fork() failed */
	perror(procname.c_str());
	return (false);
    }

    // If we are the child, exec the new process, then go away
    if (childpid == 0) {
	// Turn off all output, if requested
	if (b == false) {
	    close(1);
	    open("/dev/null", O_WRONLY);
	    close(2);
	    open("/dev/null", O_WRONLY);
	}
	// Start the desired executable
	execv(procname.c_str(), cmd_line);
	perror(procname.c_str());
	exit(0);
    }
    
    return (true);
}

int
Proc::Find (string procname) {
    // simple debug junk
    if (verbosity)
	cout << "Finding \"" << procname << "\"" << endl;

    return tbl[procname];
}

bool
Proc::Stop (void) {
    cerr << __PRETTY_FUNCTION__ << ": not implemented!" << endl;
    return false;
}
    
bool
Proc::Stop (string procname) {
    pid_t pid;

    // simple debug junk
    if (verbosity)
	cout << "Stopping \"" << procname << "\"" << endl;

    pid = tbl[procname];
    
    if (kill (pid, SIGQUIT) == -1)
	return (false);
    else
	return (true);
}
 
bool
Proc::SetOutput (string procname, bool b) {
    output[procname] = b;
    return (true);
}

bool
Proc::GetOutput (string procname) {
    return output[procname];
}

// local Variables:
// mode: C++
// indent-tabs-mode: nil
// End:
