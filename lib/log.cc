#include <iomanip>
#include <fstream>
#include <cstring>
// #include <strstream>

// Target Manager internal logging sub-system
#include "log.h"

// Required for SYSTEMTIME definitions
#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

#include <time.h>

# if 0
std::ofstream LogFile::outstream;
string LogFile::logentry;
bool LogFile::stamp;
enum LogFile::file_state LogFile::state;
#endif
std::ofstream LogFile::console;
int LogFile::verbose;

const unsigned char hexchars[]="0123456789abcdef";

// Instantiate the log file
LogFile dbglogfile;

extern "C" char digit2hex(int);
extern "C" int hex2digit (int);


std::ostream& operator<<(std::ostream& os, Verbose& b) {
    // dbglogfile.verbosity(b);
    return os;
}

#if 0
std::ostream& stampon(std::ostream& x) {
    LogFile::stamp(true);
    return x;
}

std::ostream& stampoff(std::ostream& x) {
    LogFile::SetStamp(false);
    return x;
}
#endif

std::ostream& timestamp(std::ostream& x) {
    time_t t;
    char buf[10];

    memset (buf, '0', 10);	// this terminates the string
    time (&t);			// get the current time
    strftime (buf, sizeof(buf), "%H:%M:%S", localtime (&t));

    return x << buf << ": ";
}

std::string &timestamp(std::string &str) {
    time_t t;
    char buf[10];

    memset (buf, '0', 10);	// this terminates the string
    time (&t);			// get the current time
    strftime (buf, sizeof(buf), "%H:%M:%S", localtime (&t));
    str = buf;

    return str;
}

std::ostream& datetimestamp(std::ostream& x) {
    time_t t;
    char buf[20];

    memset (buf, '0', 20);	// this terminates the string
    time (&t);			// get the current time
    strftime (buf, sizeof(buf), "%Y-%m-%d %H:%M:%S ", localtime (&t));

    return x << buf;
}

// FIXME: This controls the entire output stream
#if 0
std::ostream & operator << (std::ostream &os, std::string x) {
    if (dbglogfile.get_verbosity() > 0)
	cout << timestamp << x;
    return os << x.c_str();
}
#endif

std::ostream &operator<<(std::ostream &os, LogFile &l) {
    return os << l.GetEntry();
}

// Make sure we can print outself to an output stream

const char *
LogFile::GetEntry(void) {
    return logentry.c_str();
}

// Default constructor
LogFile::LogFile (void) {
    verbose = 0;
    stamp = true;
    LogFile::outstream.open (DEFAULT_LOGFILE, std::ios::out);
    state = OPEN;
}

LogFile::LogFile (const char *filespec) {
    verbose = 0;
    stamp = true;
    if (state == OPEN)
        LogFile::outstream.close ();
    LogFile::outstream.open (filespec, std::ios::out);
    state = OPEN;
}

retcode_t
LogFile::Open (const char *filespec) {
    if (state == OPEN)
        LogFile::outstream.close ();
    LogFile::outstream.open (filespec, std::ios::out);
    state = OPEN;
  
    // LogFile::outstream << "Opened " << filespec << std::endl;
  
    return SUCCESS;
}

retcode_t
LogFile::Close (void) {
    LogFile::outstream.flush ();
    if (state == OPEN)
        LogFile::outstream.close ();
    //    state = CLOSED;
    
    return SUCCESS;
}

// Overload the input operator, so we can stick a timestamp on
// the message that gets printed.
LogFile&
LogFile::operator << (ErrCond& e) {
    if (verbose > 0)
        std::cout << e << std::endl;
    LogFile::outstream << e;
    state = INPROGRESS;

    return *this;
}

LogFile&
LogFile::operator << (long x)
{
    if (verbose > 0)
        std::cout << x;
    LogFile::outstream << x;
    state = INPROGRESS;
  
    return *this;
}

LogFile&
LogFile::operator << (unsigned int x)
{
    if (verbose > 0)
        std::cout << x;
    LogFile::outstream << x;
    state = INPROGRESS;
  
    return *this;
}

LogFile&
LogFile::operator << (float x)
{ 
    LogFile::outstream << x;
    if (verbose > 0) {
        std::cout << x;
    }
    state = INPROGRESS;

    return *this;
}

LogFile&
LogFile::operator << (double &x)
{
    LogFile::outstream << x; 
    if (verbose > 0) {
        std::cout << x;
    }
  
    state = INPROGRESS;

    return *this;
}

LogFile&
LogFile::operator << (int x) {
    
    if (verbose > 0)
        std::cout << x;
    LogFile::outstream << x;
    state = INPROGRESS;

    return *this;
}

LogFile&
LogFile::operator << (void *ptr) {
    
    if (verbose > 0)
        std::cout << ptr;
    LogFile::outstream << ptr;
    state = INPROGRESS;

    return *this;
}

LogFile& 
LogFile::operator << (const std::string &s) {
    outstream << s;
    return *this;
}

LogFile& 
LogFile::operator << (const char *c) {
    std::string str;
    logentry = timestamp(str);
    logentry += ": ";

    if (stamp == true && (state == IDLE || state == OPEN)) {
        LogFile::state = INPROGRESS;
	if (verbose > 0)
	    std::cout << logentry  << c;
        outstream << logentry << c;
    } else {
	if (verbose > 0)
	    std::cout << c;
        outstream << c;
    }
    logentry += c;

    return *this;
}

LogFile& 
LogFile::operator << (const unsigned char *c) {
    std::string str;
    logentry = timestamp(str);
    logentry += ": ";

    if (c == NULL) {
        return *this;
    }
    
    if (stamp == true && (state == IDLE || state == OPEN)) {
        LogFile::state = INPROGRESS;
	if (verbose > 0)
	    std::cout << logentry  << c;
        outstream << logentry << c;
    } else {
	if (verbose > 0)
	    std::cout << c;
        outstream << c;
    }
    logentry += (const char*)c;

    return *this;
}

// This grabs the endl operator;
std::ostream&
LogFile::operator << (std::ostream & (&)(std::ostream &)) {
    if (verbose > 0)
        std::cout << "\r" << std::endl;
    LogFile::outstream << std::endl;;
    LogFile::outstream.flush();
    LogFile::state = IDLE;

    // FIXME: This is probably not the value to return
    return std::cout;
}

#if 0
// This is a logging function. All messages get logged as a time stamped
// entry to the disk log file. The optionally get displayed via the
// printf_filtered() function, which is defined by the program that links
// in the target manager client library.
retcode_t
LogFile::note (int level, const char *fmt, ...) {
    va_list ap;
    char tmp[LOGBUFFERSIZE];
    char buf[TIMESTAMP_LENGTH];
#ifdef _WIN32
    SYSTEMTIME system_time, local_time;
#else
    time_t t;
#endif // _WIN32
    const char *errmsg;

    va_start (ap, fmt);
    vsprintf (tmp, fmt, ap);
    
// use SYSTEMTIME under Windows for millisecond resolution
#ifdef _WIN32
    GetSystemTime (&system_time); // get the current time
    SystemTimeToTzSpecificLocalTime (NULL, &system_time, &local_time);
    sprintf (buf, "%02u:%02u:%02u.%03u", local_time.wHour,

             local_time.wMinute, local_time.wSecond, local_time.wMilliseconds);
#else
    time (&t);			// get the current time
    strftime (buf, sizeof(buf), "%H:%M:%S", localtime (&t));
#endif // _WIN32
    // Some messages already come with a newline on the end, which we
    // don't want.
    if (*(tmp+strlen(tmp)-1) == '\n')
        *(tmp+strlen(tmp)-1) = '\0';

    // Since we're running either the curse UI or a X11 GUI, we
    // don't ever print log messages to the screen.
#if 0
    if (verbose >= level) {
        std::cout << tmp;       ;
        std::cout.flush();
    }
#endif

    // write the message with a timestamp to the log file
    if (state == OPEN) {
        logout << buf << ":" << tmp << std::endl;
        logout.flush();
    }

    va_end (ap);
    return SUCCESS;
}

retcode_t
LogFile::printf (int level, const char *fmt, ...) {
    va_list ap;
    char tmp[LOGBUFFERSIZE];
    char buf[10];
    const char *errmsg;
    
    va_start (ap, fmt);
    vsprintf (tmp, fmt, ap);
    
    // Since we're running either the curse UI or a X11 GUI, we
    // don't ever print log messages to the screen.
#if 0
    if (verbose >= level) {
        std::cout << tmp;
        std::cout.flush();
    }
#endif    

    // write the message with a timestamp to the log file
    if (state == OPEN) {
        logout << tmp;
        logout.flush();
    }
    
    va_end (ap);    
    return SUCCESS;
}

// Unconditionally print an error message
void
LogFile::error (const char *fmt, ...) {
    va_list ap;
    char tmp[LOGBUFFERSIZE];
    char buf[10];
    
    va_start (ap, fmt);
    vsprintf (tmp, fmt, ap);
    
    LogFile::note (0, "ERROR: %s", tmp);
    
    va_end (ap);
}

// Unconditionally print a warning message
void
LogFile::warning (const char *fmt, ...) {
    va_list ap;
    char tmp[LOGBUFFERSIZE];
    char buf[10];
    
    va_start (ap, fmt);
    vsprintf (tmp, fmt, ap);
    
    LogFile::note (0, "WARNING: %s", tmp);
    
    va_end (ap);
}

// Print an informational message, which is printed at level 1
void
LogFile::info (const char *fmt, ...) {
    va_list ap;
    char tmp[LOGBUFFERSIZE];
    char buf[10];
    
    va_start (ap, fmt);
    vsprintf (tmp, fmt, ap);
    
    LogFile::note (1, "%s", tmp);
    
    va_end (ap);
}

// Print an informational message, which is printed at level 1
void
LogFile::info (char *str) {
    LogFile::note (1, "%s", str);
}

// These wrap a C API around the C++ one
// C wrappers for the C++ functions
extern "C" void
debuglog_error (const char *fmt, ...) 
{
    va_list ap;
    va_start (ap, fmt);
    dbglogfile.note (0, fmt, ap);
    va_end (ap);
}

extern "C" void
debuglog_warning (const char *fmt, ...) 
{
    va_list ap;
    va_start (ap, fmt);
    dbglogfile.note (0, fmt, ap);
    va_end (ap);
}

extern "C" void
debuglog_info (const char *fmt, ...) 
{
    va_list ap;
    va_start (ap, fmt);
    dbglogfile.note (1, fmt, ap);
    va_end (ap);
}

extern "C" void
debuglog_note (unsigned int level, const char *fmt, ...) 
{
    va_list ap;
    va_start (ap, fmt);
    dbglogfile.note (level, fmt, ap);
    va_end (ap);
}

extern "C" void
debuglog_set_verbosity (void) 
{
    dbglogfile.set_verbosity ();
}

extern "C" int
debuglog_get_verbosity (void) 
{
    return dbglogfile.get_verbosity ();
}

extern "C" int 
debuglog_open (const char *filespec) {
    return dbglogfile.open (filespec);
}

extern "C" void
logfile_printf (int level, const char *fmt, ...) {
    va_list ap;
    char tmp[3000];
    
    // FIXME: for some odd reason, uncommenting this causes an
    // infinite loop.
    // REPORT_FUNCTION;
    va_start (ap, fmt);
    vsprintf (tmp, fmt, ap);
    
    dbglogfile.printf (level, "%S", tmp);
}

// Convert a buffer into an ascified representation. ie.. ^C is 0x03, etc...
extern "C" unsigned char *
ascify_buffer (unsigned char *p, const unsigned char *s, int x) {
    int i;
    unsigned char *p1 = p;

//    dbglogfile.note (3, "ascify_buffer: convert %d bytes.", x);
    // convert some characters so it'll look right in the log
    for (i=0 ; i<x; i++) {
//        dbglogfile.note (3, "\tascify 0x%x", s[i]);
        switch (s[i]) {
          case '\012':		// newlines
              *p++ = '\\';
              *p++ = 'n';
              continue;
          case '\015':		// carriage returns
              *p++ = '\\';
              *p++ = 'r';
              continue;
          case '\033':		// escape
              *p++ = '\\';
              *p++ = 'e';
              continue;
          case '\011':		// tab
              *p++ = '\\';
              *p++ = 't';
              continue;
          case '\010':		// backspace
              *p++ = '\\';
              *p++ = 'b';
              continue;
        }
        
        if (s[i] < 26) {	// modify control characters
            *p++ = '^';
            *p++ = s[i] + '@';
            continue;
        }
        if (s[i] >= 128) {	// modify control characters
            *p++ = '!';
            *p++ = s[i] + '@';
            continue;
        }
        if (s[i] > 26 && s[i] < 128) { // no change
            *p++ = s[i];
            continue;
        }
    }

    *p = '\0';			// terminate the string

    return p1;
}

// Convert each byte into it's hex represntation
unsigned char *
hexify_buffer (unsigned char *p, const unsigned char *s, int x) {
    int i;
    unsigned char *p1 = p;

    dbglogfile.note (3, "hexify_buffer: convert %d bytes.", x);
    // convert some characters so it'll look right in the log
    for (i=0 ; i<x; i++) {
        // use the hex value
        *p++ = hexchars[s[i] >> 4];
        *p++ = hexchars[s[i] & 0xf];
        *p++ = ' ';
//        dbglogfile.note (3, "\thexify s[%d] = 0x%02x", i, s[i]);
    }

    *p = '\0';                                  // terminate the string

    return p1;
}

// convert number NIB to a hex digit.
//      param is a decimal digit.
//      returns a hex digit.
extern "C" char
digit2hex(int digit) {
    if (digit < 10)
        return '0' + digit;
    else
        return 'a' + digit - 10;
}

// convert an ascii hex digit to a number.
//      param is hex digit.
//      returns a decimal digit.
extern "C" int
hex2digit (int digit)
{
    if (digit == 0)
        return 0;

    if (digit >= '0' && digit <= '9')
        return digit - '0';
    if (digit >= 'a' && digit <= 'f')
        return digit - 'a' + 10;
    if (digit >= 'A' && digit <= 'F')
        return digit - 'A' + 10;

    /* shouldn't ever get this far */
    return -1;
}

#endif

// local Variables:
// mode: C++
// indent-tabs-mode: nil
// End:
