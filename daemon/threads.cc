// 
// Copyright (C) 2018 Free Software Foundation, Inc.
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
#include <condition_variable>
#include <mutex>
#include <chrono>

#ifdef HAVE_GETOPT_H
#include <getopt.h>
#else
extern int optind;
extern char *optarg;
#endif

#include "log.h"
#include "err.h"
#include "ownet.h"

extern LogFile dbglogfile;

using namespace std::chrono_literals;

#ifdef BUILD_OWNET
void
ownet_handler(pdev::Ownet &ownet)
{
    DEBUGLOG_REPORT_FUNCTION;
    dbglogfile << "PowerGuru - 1 Wire Mode" << std::endl;
    //dev::Ownet ownet;
    //ownet.dump();
    bool poll = true;
    
    std::map<std::string, pdev::ownet_t> sensors = ownet.getSensors();
    std::map<std::string, pdev::ownet_t>::iterator it;
    while (poll) {
        for (it = sensors.begin(); it != sensors.end(); it++) {
            if (it->second.family == "10") {
                ownet.getTemperature(it->first.c_str());
            }
        }
        ownet.dump();
        
        // Don't eat up all the cpu cycles!
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}
#endif

#ifdef BUILD_OUTBACK
void
outback_handler(pdev::Ownet &ownet)
{
    DEBUGLOG_REPORT_FUNCTION;

    dbglogfile << "FIXME: unimplementd"<< std::endl;
}
#endif

#ifdef BUILD_XANTREX
void
xantrex_handler(pdev::Ownet &ownet)
{
    DEBUGLOG_REPORT_FUNCTION;

    dbglogfile << "FIXME: unimplemented"<< std::endl;
}
#endif

// local Variables:
// mode: C++
// indent-tabs-mode: nil
// End:
