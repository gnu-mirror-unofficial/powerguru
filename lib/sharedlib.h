// 
// Copyright (C) 2005, 2006, 2007, 2008, 2009, 2010, 2011
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

#ifndef __SHAREDLIB__
#define __SHAREDLIB__

#include <cstring>
#include <ltdl.h>

#include "log.h"
#include "err.h"

typedef retcode_t entrypoint (unsigned char *buf, int bytes, struct errcond *err);

typedef retcode_t IOentrypoint (FILE *, struct errcond *err);

class SharedLib
{
public:
    retcode_t OpenLib (std::string &name, ErrCond &Err);
    retcode_t CloseLib (ErrCond &Err);
    entrypoint *GetSymbol (std::string &name, ErrCond &Err);

    // Accessors for the protocol name
    std::string &GetDllName (void) { return dlname; }
    void SetDllName (std::string &x) { dlname = x; }

protected:
    lt_dlhandle dlhandle;
    std::string dlname;
};

// __SHAREDLIB_H__
#endif

// local Variables:
// mode: C++
// indent-tabs-mode: nil
// End:
