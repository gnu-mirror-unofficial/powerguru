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

#include <iomanip>
#include <fstream>
#include <cstring>
#ifdef _WIN32                   // Required for SYSTEMTIME definitions
#include <windows.h>
#endif // _WIN32
#include <time.h>
#include "log.h"

src::logger lg;                 ///< Global handle to the logger

/// \function log_init
///
/// Write a logfile to the disk and the console
/// @param The name of the log file to write
void
log_init(const std::string &name)
{
    // Change the default timestamp to be less long than the commom_attributes
    logging::formatter formatter = expr::stream
        << expr::format_date_time< boost::posix_time::ptime >
        ("TimeStamp", "[%Y-%m-%d %H:%M:%S] ") << expr::message;

    logging::add_file_log (
        keywords::file_name = name + ".log",
        keywords::rotation_size = 10 * 1024 * 1024,
        keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0),
        keywords::format = "[%TimeStamp%]: %Message%",
        keywords::auto_flush = true
        )->set_formatter(formatter);

    logging::add_console_log()->set_formatter(formatter);

    logging::add_common_attributes();
}

// local Variables:
// mode: C++
// indent-tabs-mode: nil
// End:
