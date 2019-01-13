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

#include <sys/stat.h>
#include <unistd.h>
#include <cstring>
#include <ltdl.h>
#include <dlfcn.h>

#include "log.h"
#include "err.h"
#include "sharedlib.h"

using namespace std;
#include <boost/system/error_code.hpp>
using namespace boost::system;

extern LogFile dbglogfile;

boost::system::error_code
SharedLib::CloseLib (ErrCond &err)
{
    lt_dlclose (dlhandle);
}

boost::system::error_code
SharedLib::OpenLib (string &filespec, ErrCond &Err)
{
    DEBUGLOG_REPORT_FUNCTION;

    boost::system::error_code (*FuncAddr)(void);
    int errors = 0;
    char *errmsg;
    char pwd[512];

#if 0
    struct stat ostats;
    if (stat (filespec.c_str(), &ostats)) {
        switch (errno) {
          case EBADF:
          case ENOENT:
              Err.SetMsg("Specified shared library doesn't exist");
              BOOST_LOG(lg) << "ERROR: Dynamic library, " << filespec << " doesn't exist!" << endl;
              errc::make_error_code(errc::not_supported);
              break;
        }
    }
#endif  

#if 0
    // ltdl should use the same mallocation as us
    lt_dlmalloc = (lt_ptr (*) (size_t)) xmalloc;
    lt_dlfree = (void (*) (lt_ptr)) free;

#endif

    // Make sure preloaded modules are initialised
    LTDL_SET_PRELOADED_SYMBOLS();

    // Initialize libtool's dynamic library loader
    errors = lt_dlinit ();

    if (errors) {
        Err << "Couldn't initialize ltdl";
        errc::make_error_code(errc::not_supported);
    }

    BOOST_LOG(lg) << "Initialized ltdl" << endl;
  
    // Get the path to look for libraries in, or force a default one 
    // if the ABELMON environment variable isn't set.
    const char *abelmon = (char *)getenv ("ABELMON");
    if (abelmon == NULL) {
        getcwd((char *)&pwd, 512);
        abelmon = pwd;
        BOOST_LOG(lg) << "WARNING: using default DL search path" << endl;
    }
  
    errors = lt_dladdsearchdir (abelmon);
    if (errors) {
        Err << lt_dlerror();
        errc::make_error_code(errc::not_supported);
    }

    BOOST_LOG(lg) << "Added " << abelmon << " to the search paths" << endl;
  
    BOOST_LOG(lg) << "Trying to open shared library " << filespec << endl;

    dlhandle = lt_dlopenext (filespec.c_str());

    if (dlhandle == NULL) {
        Err << lt_dlerror();
        errc::make_error_code(errc::not_supported);
    }

    dlname = filespec;
  
    BOOST_LOG(lg) << "Opened dynamic library " << filespec << endl;
    errc::make_error_code(errc::success);
}

entrypoint *
SharedLib::GetSymbol (std::string &symbol, ErrCond &err)
{
    DEBUGLOG_REPORT_FUNCTION;

    entrypoint *run = NULL;
 
    run  = (entrypoint *) lt_dlsym (dlhandle, symbol.c_str());

    // Realistically, we should never get a valid pointer with a value of 0
    if (run == (entrypoint *)0) {
        err << "Couldn't find symbol" << symbol << endl;
        return NULL;
    } else {
        BOOST_LOG(lg) << "Found symbol " << symbol << " @ " << (void *)run << endl;
    }

    return run;
}

#if 0
// Open the database
boost::system::error_code
SharedLib::ScanDir (void) {
    DEBUGLOG_REPORT_FUNCTION;
  
    int i;
    struct device_info *info;
    struct dirent *entry;
    lt_dlhandle dlhandle;
    boost::system::error_code (*InitDBaddr)(void);
    lt_ptr_t addr;
    struct errcond err;

    // Initialize libdl
    lt_dlinit ();
    LTDL_SET_PRELOADED_SYMBOLS();

    // Get the path to look for libraries in, or force a default one 
    // if the ABELMON environment variable isn't set.
    const char *abelmon = (char *)getenv ("ABELMON");
    if (abelmon == NULL) {
        abelmon = "/usr/local/lib/abelmon";
        BOOST_LOG(lg) << "ERROR: You need to set ABELMON" << endl;
    }
  
    lt_dladdsearchdir (abelmon);
    // BOOST_LOG(lg) << timestamp << "Searching in " << abelmon << "for database drivers" << endl;
  
    DIR *library_dir = opendir (abelmon);

    // By convention, the first two entries in each directory are for . and
    // .. (``dot'' and ``dot dot''), so we ignore those.
    entry = readdir(library_dir);
    entry = readdir(library_dir);
  
    for (i=0; entry>0; i++) {
        // We only want shared libraries than end with the suffix, otherwise
        // we get all the duplicates.
        entry = readdir(library_dir);
        if ((int)entry < 1)
            errc::make_error_code(errc::success);

        //    handle = dlopen (entry->d_name, RTLD_NOW|RTLD_GLOBAL);
        dlhandle = lt_dlopen (entry->d_name);
        if (dlhandle == NULL) {
            continue;
        }
        cout << "Opening " << entry->d_name << endl;
        //    InitDBaddr = (boost::system::error_code (*)(...))dlsym (handle, "InitDB");
        (lt_ptr_t) InitDBaddr = lt_dlsym (dlhandle, "InitDB");
        if (InitDBaddr != NULL) {
            //      BOOST_LOG(lg) << "Found OpenDB in " << entry->d_name << endl;
            cout << "Found InitDB in " << entry->d_name << " at " << addr << endl;
            InitDBaddr();
        } else {
            //      BOOST_LOG(lg) << "Didn't find OpenDB in " << entry->d_name << endl;
            cout << "Didn't find InitDB in " << entry->d_name << endl;
        }
        lt_dlclose (dlhandle);
    }
    closedir(library_dir);

}
#endif

// local Variables:
// mode: C++
// indent-tabs-mode: nil
// End:
