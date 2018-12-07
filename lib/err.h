// 
// Copyright (C) 2005, 2006 - 2018.
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

#ifndef __ERR_H__
#define __ERR_H__

// This is the config file as produced by autoconf
#include "config.h"
        
//#include <stdio.h>
#include <errno.h>
#include <cstring>

// having to malloc() any memory. And this is it's size.
#define ERRMSGSIZE 78
        
// Since NULL may have various values depending on the system,
// we define our own string terminator.
#define STRTERM '\0'

// This is a set of common error codes used by Abelmon
#define MEM_ERR         3               // memory read/write error
#define IO_ERR          5               // I/O error
#define TIMEOUT_ERR     6               // timeout
#define UNIMP_ERR       7               // unimplemented
#define NOSUPP_ERR      8               // unsupported
#define NOSTOP_ERR      11              // wouldn't stop
#define BADINT_ERR      12              // bad interface
#define NO_DATA         13          // there is no data present

// This hold all the data for an error condition
// We use a statically allocate string so the C API can be used wihout
// having to malloc() any memory. Too bad C doesn't support constructors.
struct errcond {
    int line;
    char *func;
    char *file;
    char msg[ERRMSGSIZE+1];
    int code;
};

// This is the data returned by all intelligent functions. It's similar
// to the retcode produced by ILU, but we wanted ours to be different,
// so we can add more to this as time goes on.
typedef enum retcode {
    ERROR,
    SUCCESS
} retcode_t;

// Clear the error code and message
static inline void CLEARERR (struct errcond *x) {
    x->code=0;
    x->msg[0] = STRTERM;
};

// Set an error message based on just an error message
static inline void SETERRMSG (struct errcond *x, const char *y) {
    x->code=-1;
    strncpy (x->msg, y, ERRMSGSIZE);
    x->msg[ERRMSGSIZE] = STRTERM;
};

// Get the error message
static inline char *GETERRMSG (struct errcond *x) {
    return x->msg;
};

// Set an error message based on an error code
static inline void SETERRCODE (struct errcond *x, int y) {
    x->code=y;
    x->msg[0] = STRTERM;
};

// Get the error code
static inline int GETERRCODE (struct errcond *x) {
    return x->code;
};

// Get the error code
static inline int ERRCOND (struct errcond *x) {
    return ((x->code > 0 || x->msg[0] > 'a') ? x->code : 0);
};

// Set an error message based on the message and an error code
static inline void SETERR(struct errcond *x, const char *y, int z) {
    x->code=z;
    if (y!=NULL) {
        strncpy (x->msg, y, ERRMSGSIZE);
        x->msg[ERRMSGSIZE] = STRTERM;
    }
};

#ifdef __cplusplus
#include <cstring>
#include <iostream>

#define WARNMSG (sys_nerr+1)

#define SETERRCOND(err, txt) err.SetMsg(__FILE__, __LINE__, __PRETTY_FUNCTION__, errno, txt)
#define SETWARNCOND(err, txt) err.SetMsg(__FILE__, __LINE__, __PRETTY_FUNCTION__, WARNMSG, txt)

// Do this in C++ instead
class ErrCond {
public:
    ErrCond(void) {
        ecode = 0;
        line = 0;
    }
    ErrCond(int x) {
        ecode = x;
    }

    ~ErrCond(void);
    ErrCond(const std::string &filein, int linein,
            const std::string &funcin, int codein, const std::string &s);
    // methods
    void ClearErr(void);
  
    ErrCond &SetMsg (const std::string &filein, int linein, const std::string &funcin,
                     int codein, const std::string &s);

    void SetMsg (const std::string &s);
    void SetCode(int x) { 
        ecode = x;
    }
    int GetCode(void) { 
        return ecode;
    }
    int GetLine(void) { 
        return line;
    }
    const std::string &GetFile(void) { 
        return file;
    }
  
    const std::string &GetFunc(void) { 
        return func;
    }
  
    const std::string &GetMsg(void) { 
        return emsg;
    }

    ErrCond &operator << (ErrCond &);
    ErrCond &operator << (int x);
    ErrCond &operator << (char const *str);
    ErrCond &operator << (std::string &str);
    ErrCond &operator << (void *addr);

    friend std::ostream & operator << (std::ostream &os, ErrCond& e);
    const std::ostream& operator << (std::ostream & (&)(std::ostream &));
private:
    std::string file;
    std::string func;
    int line;
    int ecode;
    std::string emsg;
};

#endif
#endif		// __ERR_H__

// local Variables:
// mode: C++
// indent-tabs-mode: nil
// End:
