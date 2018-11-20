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

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "err.h"
#include "log.h"
#include "ownet.h"
#include <string>
#include <boost/algorithm/string.hpp>

extern LogFile dbglogfile;

namespace pdev {

Ownet::~Ownet(void) {
    OW_finish();
};

} // end of namespace

// local Variables:
// mode: C++
// indent-tabs-mode: nil
// End:
