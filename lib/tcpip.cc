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

// This is generated by autoconf
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <vector>
#include <iostream>
#include <sys/param.h>
#include <fcntl.h>
#ifdef HAVE_LIBXML
# include <libxml/encoding.h>
# include <libxml/xmlwriter.h>
# include <libxml/debugXML.h>
#endif

#include "tcpip.h"
#include "tcputil.h"
#include "xml.h"
#include "log.h"
#include "err.h"

using namespace std;

//static const char *SERVICENAME = "powerguru";
static const char *DEFAULTPROTO = "tcp";
static const short DEFAULTPORT  = 7654;
static const int INBUF          = 2048;
static const int DEFAULTTIMEOUT = 5;
const int BLOCKING_TIMEOUT      = -1;

extern LogFile dbglogfile;

#ifndef INADDR_NONE
#define INADDR_NONE  0xffffffff
#endif

// FIXME: This is so methods executed via a pointer can still do
// I/O. It's probably a gross memory corruption problem too.
int  Tcpip::_sockfd;
int  Tcpip::_sockIOfd;

Tcpip::Tcpip(void)
    : _ipaddr(INADDR_ANY),
      _proto(0),
      _debug(false),
      _console(false)
      // _sockfd(-1), _sockIOfd(-1), 
{
    // Get the low level host data for this machine
    hostDataGet();
}

Tcpip::~Tcpip(void)
{    
}

const string
Tcpip::remoteIP(void)
{
    return inet_ntoa(_client.sin_addr);
}

const string
Tcpip::remoteIP(struct in_addr sockin)
{
    return inet_ntoa(sockin);
}

const string
Tcpip::remoteName(void)
{
    return hostByAddrGet(inet_ntoa(_client.sin_addr));
}

const string
Tcpip::remoteName(struct in_addr sockin)
{
    return hostByAddrGet(inet_ntoa(sockin));
}

// Description: Create a tcp/ip network server. This creates a server
//              that listens for incoming socket connections. This
//              support IP aliasing on the host, and will sequntially
//              look for IP address to bind this port to.
retcode_t
Tcpip::createNetServer(void)
{
    return createNetServer(DEFAULTPORT);
}

retcode_t
Tcpip::createNetServer(short port)
{
    string str = DEFAULTPROTO;
    return createNetServer(port, str);
}

retcode_t
Tcpip::createNetServer(string &service)
{
    string str = DEFAULTPROTO;
    return createNetServer(service, str);
}

retcode_t
Tcpip::createNetServer(string &service, string &proto)
{
    DEBUGLOG_REPORT_FUNCTION;

    const struct servent  *serv;

    serv = lookupService(service);
  
    // See if we got a service data structure
    if (serv == 0) {
        dbglogfile << "ERROR: unable to get " << service
                   << " service entry" << endl;
        _port = 0;
        return ERROR;
    }

    dbglogfile << "Port number is " << serv->s_port
               << ", byte swapped is " << htons(serv->s_port) << endl;

    // Store the port number
    _port = serv->s_port;
    _proto = serv->s_proto;
  
    return createNetServer(serv->s_port, proto);
}

retcode_t
Tcpip::createNetServer(short port, string &protocol)
{
    DEBUGLOG_REPORT_FUNCTION;
  
    struct protoent *ppe;
    struct sockaddr_in sock_in;
    int             on, type;
    int             retries = 0;
    const struct hostent  *host;
    struct in_addr  *thisaddr, newaddr;
    in_addr_t       nodeaddr, netaddr;
  
    host = hostDataGet("localhost");
    thisaddr = reinterpret_cast<struct in_addr *>(host->h_addr_list[0]);
    _ipaddr = thisaddr->s_addr;
    memset(&sock_in, 0, sizeof(sock_in));
  
    //  sock_in.sin_addr.s_addr = thisaddr->s_addr;
    sock_in.sin_addr.s_addr = INADDR_ANY;
  
    _ipaddr = sock_in.sin_addr.s_addr;
    sock_in.sin_family = AF_INET;
    sock_in.sin_port = htons(port);
  
  
    //    dbglogfile << "Trying to make a server for service "
    //               <<
    //      srvname << " at port " <<  ntohs(sock_in.sin_port) << nedl;
  
    if ((ppe = getprotobyname(protocol.c_str())) == 0) {
        // error, wasn't able to get a protocol entry
        dbglogfile << "WARNING: unable to get " << protocol
                   << " protocol entry" << endl;
        return ERROR;
    }
  
    // set protocol type
    if (protocol == "udp") {
        type = SOCK_DGRAM;
    } else {
        type = SOCK_STREAM;
    }
  
    // Get a file descriptor for this socket connection
    _sockIOfd = socket(PF_INET, type, ppe->p_proto);
  
    // error, wasn't able to create a socket
    if (_sockIOfd < 0) {
        dbglogfile << "unable to create socket: " << strerror(errno) << endl;
        return SUCCESS;
    }

    on = 1;
    if (setsockopt(_sockIOfd, SOL_SOCKET, SO_REUSEADDR,
                   (char *)&on, sizeof(on)) < 0) {
        dbglogfile << "setsockopt SO_REUSEADDR failed" << endl;
        return ERROR;
    }

    retries = 0;
  
    nodeaddr = inet_lnaof(*thisaddr);
    while (retries < 5) {
        if (bind(_sockIOfd, reinterpret_cast<struct sockaddr *>(&sock_in),
                 sizeof(sock_in)) == -1) {
            dbglogfile << "WARNING: unable to bind to"
                       << inet_ntoa(sock_in.sin_addr)
                       << " port!" << strerror(errno) << endl;
      
            // If there is something already bound to this IP number,
            // then we increment the number to be the next one in the
            // range. This lets multiple tcp/ip servers operate on the
            // same machine using the same tcp/ip port.
            if (errno == EADDRINUSE) {
                //            nodeaddr = inet_lnaof(*thisaddr);
                netaddr = inet_netof(*thisaddr);
                nodeaddr++;
                newaddr = inet_makeaddr(netaddr, nodeaddr);
                sock_in.sin_addr.s_addr = newaddr.s_addr;
                // There are always two interfaces on any machine, the loopback
                // device, and the default ethernet port. With IP aliasing, there
                // will be more devices, but if not, we have an error, and can't
                // continue.
                if (numberOfInterfaces() - 2 > retries) {
                    retries++;
                    continue;
                } else {
                    dbglogfile <<
                        "ERROR: There is another process already bound to this port!" << endl;
                    return ERROR;
                }
            }
            retries++;
        }
    
        _hostname = hostByAddrGet(inet_ntoa(sock_in.sin_addr));
    
#if 0
        char                ascip[32];
        inet_ntop(AF_INET, &_ipaddr, ascip, INET_ADDRSTRLEN);
        dbglogfile << "Host Name is " << host->h_name << " IP is " <<
            ascip << endl;
#endif
    
        dbglogfile << "Server bound to service "
                   << " on port: " << ntohs(sock_in.sin_port)
                   << " on IP " << inet_ntoa(sock_in.sin_addr)
                   << " using fd #" << _sockIOfd << endl;
    
        if (type == SOCK_STREAM && listen(_sockIOfd, 5) < 0) {
            dbglogfile << "ERROR: unable to listen on port: "
                       << port << ": " <<  strerror(errno) << endl; 
            return ERROR;
        }

        _port = port;
    
#if 0
        dbglogfile << "Listening for net traffic on fd #\n" << _sockfd << endl;
#endif
    
        return SUCCESS;
    }
    return ERROR;
}

// Description: Accept a new network connection for the port we have
//              created a server for.
// The default is to block.
retcode_t
Tcpip::newNetConnection(void)
{
    DEBUGLOG_REPORT_FUNCTION;
    return newNetConnection(true);
}

retcode_t
Tcpip::newNetConnection(bool block)
{
    DEBUGLOG_REPORT_FUNCTION;
    struct sockaddr	fsin;
    socklen_t		alen;
    int			ret;
    struct timeval        tval;
    fd_set                fdset;
    int                   retries = 3;
  
    alen = sizeof(struct sockaddr_in);
  
#ifdef NET_DEBUG
    dbglogfile << "Trying to accept net traffic on fd #" << _sockfd << endl;
#endif
  
    if (_sockIOfd <= 2) {
        return ERROR;
    }
  
    while (retries--) {
        // We use select to wait for the read file descriptor to be
        // active, which means there is a client waiting to connect.
        FD_ZERO(&fdset);
        // also return on any input from stdin
        if (_console) {
            FD_SET(fileno(stdin), &fdset);
        }
        FD_SET(_sockIOfd, &fdset);
    
        // Reset the timeout value, since select modifies it on return. To
        // block, set the timeout to zero.
        tval.tv_sec = 1;
        tval.tv_usec = 0;
    
        if (block) {
            ret = select(_sockIOfd+1, &fdset, NULL, NULL, NULL);
        } else {
            ret = select(_sockIOfd+1, &fdset, NULL, NULL, &tval);
        }
    
        if (FD_ISSET(0, &fdset)) {
            dbglogfile << "There is data at the console for stdin!" << endl;
            return SUCCESS;
        }

        // If interupted by a system call, try again
        if (ret == -1 && errno == EINTR) {
            dbglogfile <<
                "The accept() socket for fd #%d was interupted by a system call!"
                       << _sockIOfd << endl;
        }
    
        if (ret == -1) {
            dbglogfile << "ERROR: The accept() socket for fd " << _sockIOfd
                       << " never was available for writing!" << endl;
            return ERROR;
        }
    
        if (ret == 0) {
            if (_debug) {
                dbglogfile <<
                    "ERROR: The accept() socket for fd #%d timed out waiting to write!"
                           << _sockIOfd << endl;
            }
        }
    }
  
    fcntl(_sockIOfd, F_SETFL, O_NONBLOCK); // Don't let accept() block
    _sockfd = accept(_sockIOfd, &fsin, &alen);
  
    if (_sockfd < 0) {
        dbglogfile << "unable to accept : " << strerror(errno) << endl;
        return ERROR;
    }
  
    dbglogfile << "Accepting tcp/ip connection on fd #"
               << _sockfd << endl;

    memcpy(&_client, &fsin, sizeof(struct sockaddr));
  
    return SUCCESS;
}

// Description: Create a new conection to a tcp/ip server.
retcode_t
Tcpip::createNetClient(void)
{
    DEBUGLOG_REPORT_FUNCTION;
    string str = DEFAULTPROTO;
    string host = "localhost";
  
    return createNetClient(host, DEFAULTPORT, str);
}

retcode_t
Tcpip::createNetClient(short port)
{
    DEBUGLOG_REPORT_FUNCTION;
    string str = DEFAULTPROTO;
    string host = "localhost";
  
    return createNetClient(host, port, str);
}

retcode_t
Tcpip::createNetClient(string &hostname, short port)
{
    DEBUGLOG_REPORT_FUNCTION;
    string str = DEFAULTPROTO;
    return createNetClient(hostname, port, str);
}

retcode_t
Tcpip::createNetClient(string &hostname)
{
    DEBUGLOG_REPORT_FUNCTION;
    string str = DEFAULTPROTO;
    return createNetClient(hostname, DEFAULTPORT, str);
}

retcode_t
Tcpip::createNetClient(string &hostname, string &srvname, string &protocol)
{
    DEBUGLOG_REPORT_FUNCTION;
    //    return createNetClient(hostname, srvname, "tcp");
}

retcode_t
Tcpip::createNetClient(string &hostname, short port, string &protocol)
{
    DEBUGLOG_REPORT_FUNCTION;
    struct sockaddr_in	sock_in;
    int             	type;
    char                thishostname[MAXHOSTNAMELEN];
    fd_set              fdset;
    struct timeval      tval;
    int                 ret;
    int                 retries;
  
    memset(&sock_in, 0, sizeof(struct sockaddr_in));
    memset(&thishostname, 0, MAXHOSTNAMELEN);
  
    // If there is no supplied hostname, assume it's a local process
    //  if ((hostname.size() == 0) || (hostname == "localhost")) {
    if (hostname.size() == 0) {
        if (gethostname(thishostname, MAXHOSTNAMELEN) == 0) {
            if (_debug) {
                dbglogfile << "The hostname for this machine is "
                           << thishostname;
            }
        } else {
            dbglogfile << "WARNING: Couldn't get the hostname for this machine!" << endl;
        }
    } else {
        strcpy(thishostname, hostname.c_str());
    }
  
    const struct hostent *hent = hostByNameGet(thishostname);
    memcpy(&sock_in.sin_addr, hent->h_addr, hent->h_length);
  
    sock_in.sin_family = AF_INET;
  
#if 0
    char                ascip[32];
    inet_ntop(AF_INET, &sock_in.sin_addr.s_addr, ascip, INET_ADDRSTRLEN);
    dbglogfile << "The IP address for this client socket is " << ascip << endl;
#endif
  
    sock_in.sin_port = htons(port);
    //    sock_in.sin_port = (strtol(srvname.c_str(), &end, 0)) & 0xffff;
  
    // Set the protocol type
    if (protocol == "udp") {
        type = SOCK_DGRAM;
    } else {
        type = SOCK_STREAM;
    }
  
    if (protoDataGet(protocol) == 0) {
        dbglogfile << "unable to get <%s> protocol entry\n"
                   << protoNameGet() << endl;
        return ERROR;
    }
  
    _sockfd = socket(PF_INET, type, protoNumGet());
  
    if (_sockfd < 0) {
        dbglogfile << "WARNING: unable to create socket: "
                   << strerror(errno) << endl;
        return ERROR;
    }

    if (connect(_sockfd,
                reinterpret_cast<struct sockaddr *>(&sock_in),
                sizeof(sock_in)) < 0) {
        retries = 1;
        while (retries-- > 0) {
            // We use select to wait for the read file descriptor to be
            // active, which means there is a client waiting to connect.
            FD_ZERO(&fdset);
            FD_SET(_sockfd, &fdset);
      
            // Reset the timeout value, since select modifies it on return. To
            // block, set the timeout to zero.
            tval.tv_sec = 5;
            tval.tv_usec = 0;
      
            ret = select(_sockfd+1, &fdset, NULL, NULL, &tval);
      
            // If interupted by a system call, try again
            if (ret == -1 && errno == EINTR) {
                dbglogfile <<
                    "The connect() socket for fd #%d was interupted by a system call!"
                           << _sockfd << endl;
            }
      
            if (ret == -1) {
                dbglogfile <<
                    "The connect() socket for fd #%d never was available for writing!"
                           << _sockfd << endl;
                shutdown(_sockfd, SHUT_RDWR);
                return ERROR;
            }
      
            if (ret == 0) {
                if (_debug) {
                    dbglogfile <<
                        "WARNING: The connect() socket for fd #%d timed out waiting to write!"
                               << _sockfd << endl;
                }
            }
        }
    
        if (connect(_sockfd,
                    reinterpret_cast<struct sockaddr *>(&sock_in),
                    sizeof(sock_in)) < 0) {
            dbglogfile << "unable to connect to "
                       << thishostname
                       << ", port " << port
                       << ": " << strerror(errno) << endl;
            close(_sockfd);
            return ERROR;
        }
    }

    dbglogfile << "Client connected to service at port " << port
               << " at IP " << inet_ntoa(sock_in.sin_addr)
               << " using fd #" << _sockfd << endl;
  
    // For a client, the IO file descriptor is the same as the default one
    _sockIOfd = _sockfd;

    memcpy(&_client, &sock_in, sizeof(struct sockaddr));

    return SUCCESS;
}

// Description: Close an open socket connection.
retcode_t
Tcpip::closeConnection(void)
{
    DEBUGLOG_REPORT_FUNCTION;

    if (_sockfd > 0) {
        closeConnection(_sockfd);
        _sockIOfd = 0;
    }
  
    return ERROR;
}

retcode_t
Tcpip::closeConnection(int fd)
{
    DEBUGLOG_REPORT_FUNCTION;

    if (fd > 0) {
        closeConnection(fd);
    }
  
    return ERROR;
}

retcode_t
Tcpip::closeNet(void)
{
    DEBUGLOG_REPORT_FUNCTION;
  
    closeNet(_sockfd);
    _sockfd = 0;
  
    return ERROR;
}

retcode_t
Tcpip::closeNet(int sockfd)
{
    DEBUGLOG_REPORT_FUNCTION;
    int retries = 0;
  
    // If we can't close the socket, other processes must be
    // locked on it, so we wait a second, and try again. After a
    // few tries, we give up, cause there must be something
    // wrong.

    if (sockfd <= 0) {
        return SUCCESS;
    }
  
    while (retries < 3) {
        if (sockfd) {
            // Shutdown the socket connection
#if 0
            if (shutdown(sockfd, SHUT_RDWR) < 0) {
                if (errno != ENOTCONN) {
                    dbglogfile << "WARNING: Unable to shutdown socket for fd #"
                               << sockfd << strerror(errno) << endl;
                } else {
                    dbglogfile << "The socket using fd #" << sockfd
                               << " has been shut down successfully." << endl;
                    return SUCCESS;
                }
            }
#endif 
            if (close(sockfd) < 0) {
                dbglogfile <<
                    "WARNING: Unable to close the socket for fd "
                           <<	sockfd << strerror(errno) << endl;
                sleep(1);
                retries++;
            } else {
                dbglogfile << "Closed the socket for "
                           << serviceNameGet()
                           << " on fd " << sockfd << endl;
                return SUCCESS;
            }
        }
    }

  
    return ERROR;
}

void
Tcpip::toggleDebug(bool val)
{
    // Turn on our own debugging
    _debug = val;

    // Turn on debugging for the utility methods
    Tcputil::toggleDebug(true);
}

// Return true if there is data in the socket, otherwise return false.
retcode_t
Tcpip::anydata(vector<const xmlChar *> &msgs)
{
    DEBUGLOG_REPORT_FUNCTION;
    //printf("%s: \n", __FUNCTION__);
    return anydata(_sockfd, msgs);
}

// This waits for data on the socket, and on stdin. This way we can sit
// here in a kernel sleep instead of polling all devices.
retcode_t
Tcpip::anydata(int fd, vector<const xmlChar *> &msgs)
{
    DEBUGLOG_REPORT_FUNCTION;
    fd_set                fdset;
    struct timeval        tval;
    int                   ret = 0;
    char                  buf[INBUF];
    char                  *packet;
    int                   retries = 10;
    char                  *ptr, *eom;
    int                   cr, index = 0;
    static char           *leftover = 0;
    int                   adjusted_size;

    if (fd <= 0) {
        return ERROR;
    }
  
    //fcntl(_sockfd, F_SETFL, O_NONBLOCK);

    //msgs = (char **)realloc(msgs, sizeof(char *));
    while (retries-- > 0) {
        FD_ZERO(&fdset);
        // also return on any input from stdin
        if (_console) {
            FD_SET(fileno(stdin), &fdset);
        }
        FD_SET(fd, &fdset);
    
        tval.tv_sec = 10;
        tval.tv_usec = 10;
    
        ret = ::select(fd+1, &fdset, NULL, NULL, &tval);

        // If interupted by a system call, try again
        if (ret == -1 && (errno == EINTR || errno == EAGAIN)) {
            dbglogfile << "The socket for fd #%d was interupted by a system call!"
                       << fd << endl;
            continue;
        }
        if (ret == 0) {
            dbglogfile << "There is no data in the socket for fd #" << fd << endl;
            msgs.clear();
            return SUCCESS;
        }
        if (ret == -1) {
            dbglogfile << "The socket for fd #%d never was available!"
                       << fd << endl;
            return ERROR;
        }
        if (ret > 0) {
            if (FD_ISSET(fileno(stdin), &fdset)) {
                dbglogfile << "There is data at the console for stdin!" << endl;
                msgs.clear();
                return SUCCESS;
            }
            dbglogfile << "There is data in the socket for fd #" << fd << endl;
        }
        memset(buf, 0, INBUF);
        if (FD_ISSET(_sockfd, &fdset)) {
            ret = ::read(_sockfd, buf, INBUF-2);
        } else {
            return ERROR;
        }
    
        if (ret == 0) {
            return ERROR;
        }
    
        if (ret == -1) {
            switch (errno) {
              case EAGAIN:
                  continue;
                  break;
              default:
                  break;
            };
            return ERROR;
        }
        cr = strlen(buf);
//     dbglogfile << "read " << ret << " bytes, first msg terminates at " << cr << endl;
//     dbglogfile << "read " << buf << endl;
        ptr = buf;
        // If we get a single XML message, do less work
        if (ret == cr + 1) {
            adjusted_size = memadjust(ret + 1);
            packet = new char[adjusted_size];
            //printf("Packet size is %d at %p\n", ret + 1, packet);
            memset(packet, 0, adjusted_size);
            strcpy(packet, ptr);
            eom = strrchr(packet, '\n'); // drop the CR off the end if there is one
            if (eom) {
                *eom = 0;
            }
            //    msgs[index] = (const xmlChar *)packet;
            msgs.push_back((const xmlChar *)packet);
            //      msgs[index+1] = 0;
            //printf("%d: Pushing Packet of size %d at %p\n", __LINE__, strlen(packet), packet);
            //processing(false);
      
            return SUCCESS;
        }

        // If we get multiple messages in a single transmission, break the buffer
        // into separate messages.
        while ((strchr(ptr, '\0') > 0) && (ptr > 0)) {
            if (leftover) {
                //processing(false);
                printf("%s: The remainder is: \"%s\"\n", __FUNCTION__, leftover);
                printf("%s: The rest of the message is: \"%s\"\n", __FUNCTION__, ptr);
                adjusted_size = memadjust(cr + strlen(leftover) + 1);
                packet = new char[adjusted_size];
                memset(packet, 0, adjusted_size);
                strcpy(packet, leftover);
                strcat(packet, ptr);
                eom = strrchr(packet, '\0'); // drop the CR off the end there is one
                if (eom) {
                    *eom = 0;
                }
                printf("%s: The whole message is: \"%s\"\n", __FUNCTION__, packet);
                ptr = strchr(ptr, '\0') + 2; // messages are delimited by a "\n\0"
                delete leftover;
                leftover = 0;
            } else {
                adjusted_size = memadjust(cr + 1);
                packet = new char[adjusted_size];
                memset(packet, 0, adjusted_size);
                strcpy(packet, ptr);
                ptr += cr + 1;
                //dbglogfile << "Packet is: " << packet << endl;
            } // end of if remainder
            if (*packet == '<') {
                eom = strrchr(packet, '\n'); // drop the CR off the end there is one
                if (eom) {
                    *eom = 0;
                }
                //printf("Allocating new packet at %p\n", packet);
                //data.push_back(packet);
                //        msgs[index++] = (const xmlChar *)packet;
                msgs.push_back((const xmlChar *)packet);
            } else {
                if (*packet == *ptr) {
                    // dbglogfile << "Read all XML messages in packet " << packet << endl;
                    break;
                } else { 
                    //dbglogfile << "WARNING: Throwing out partial packet " << endl;
                    dbglogfile << "WARNING: Throwing out partial packet " << packet << endl;
                    break;
                }
            }
      
            cr = strlen(ptr);
        } // end of while (cr)
    
        if (strlen(ptr) > 0) {
            leftover = new char[strlen(ptr) + 1];
            strcpy(leftover, ptr);
            //processing(true);
            printf("%s: Adding remainder: \"%s\"\n", __FUNCTION__, leftover);
        }
    
        //processing(false);
        if (msgs.size() == 0) {
            return ERROR;
            //      printf("Returning %d messages\n", msgs.size());
        }
        return SUCCESS;
    
    } // end of while (retires)

    return SUCCESS;
}

// Read from the socket
int
Tcpip::readNet(char *buffer, int nbytes)
{
    return readNet(_sockfd, buffer, nbytes, DEFAULTTIMEOUT);
}

int
Tcpip::readNet(int fd, char *buffer, int nbytes)
{
    return readNet(fd, buffer, nbytes, DEFAULTTIMEOUT);
}

int
Tcpip::readNet(char *buffer, int nbytes, int timeout)
{
    return readNet(_sockfd, buffer, nbytes, timeout);
}

int
Tcpip::readNet(int fd, char *buffer, int nbytes, int timeout)
{
    DEBUGLOG_REPORT_FUNCTION;
    fd_set              fdset;
    int                 ret = 0;
    struct timeval      tval;
  
    // have ensible limits...
    if (fd == 0 || fd > 100) {
        return -1;
    }
    
    // Wait for the socket to be ready for reading
    if (fd > 2) {
        FD_ZERO(&fdset);
        FD_SET(fd, &fdset);
    } else {
        dbglogfile << "WARNING: Can't do anything with socket fd #"
                   << fd << endl;
        return -1;
    }
  
    // Reset the timeout value, since select modifies it on return
    if (timeout >= 0) {
        tval.tv_sec = timeout;
    } else {
        tval.tv_sec = DEFAULTTIMEOUT;
    }
    tval.tv_usec = 0;
    if (timeout != BLOCKING_TIMEOUT) {
        ret = select(fd+1, &fdset, NULL, NULL, &tval);
    } else {
        ret = select(fd+1, &fdset, NULL, NULL, NULL);        
    }
  

    // If interupted by a system call, try again
    if (ret == -1 && errno == EINTR) {
        dbglogfile <<
            "The socket for fd #" << fd << " we interupted by a system call!"
                                  << endl;
        dbglogfile << "WARNING: error is " << strerror(errno) << endl;
        return 0;
    }
  
    if (ret == -1) {
        dbglogfile <<
            "The socket for fd #" << fd << " never was available for reading!"
                                  << endl;
        return -1;
    }
  
    if (ret == 0) {
#if 0                           // FIXME: too verbose
        dbglogfile <<
            "The socket for fd #" << fd << " timed out waiting to read!" << endl;
#endif
        return 0;
    }

    ret = read(fd, buffer, nbytes);
#if 0
    if (ret != 0) {
        dbglogfile << "Read " << ret << " bytes from fd #" << fd << endl;
        dbglogfile << "Buffer says " << buffer << endl;
    }
#endif
    return ret;
}

// Write data to the socket. We first make sure the socket is ready for
// data.
int
Tcpip::writeNet(string buffer)
{
    return writeNet(_sockfd, buffer.c_str(), buffer.size(), DEFAULTTIMEOUT);
}

int
Tcpip::writeNet(char const *buffer)
{
    return writeNet(_sockfd, buffer, strlen(buffer), DEFAULTTIMEOUT);
}

int
Tcpip::writeNet(char const *buffer, int nbytes)
{
    return writeNet(_sockfd, buffer, nbytes,DEFAULTTIMEOUT );
}

int
Tcpip::writeNet(int fd, char const *buffer)
{
    return writeNet(fd, buffer, strlen(buffer), DEFAULTTIMEOUT);
}

int
Tcpip::writeNet(int fd, char const *buffer, int nbytes)
{
    return writeNet(fd, buffer, nbytes, DEFAULTTIMEOUT);
}

int
Tcpip::writeNet(int fd, char const *buffer, int nbytes, int timeout)
{
    DEBUGLOG_REPORT_FUNCTION;
    fd_set              fdset;
    int                 ret = 0;
    const char         *bufptr;
    struct timeval      tval;
    int                 retries = 3;
  
    bufptr = buffer;

    dbglogfile << "Writing to socket: \r\n\t" << buffer << endl;

    while (retries-- > 1) {
        // Wait for the socket to be ready for writing
        if (_sockfd > 2) {
            FD_ZERO(&fdset);
            FD_SET(fd, &fdset);
        } else {
            dbglogfile << "WARNING: Can't do anything with socket fd #!"
                       << fd << endl;
            return -1;
        }
    
        // Reset the timeout value, since select modifies it on return
        if (timeout) {
            tval.tv_sec = timeout;
        } else {
            tval.tv_sec = DEFAULTTIMEOUT;
        }
        tval.tv_usec = 100;
        ret = select(fd+1, NULL, &fdset, NULL, &tval);
    
        // If interupted by a system call, try again
        if (ret == -1 && errno == EINTR) {
            dbglogfile <<
                "The socket for fd #" << fd << " we interupted by a system call!" << endl;
        }
    
        if (ret == -1) {
            dbglogfile << "The socket for fd #" << fd
                       << " never was available for writing!" << endl;
            continue;
        }
    
        if (ret == 0) {
            dbglogfile << "The socket for fd #"
                       << fd << " timed out waiting to write!" << endl;
            continue;
        }
        ret = write(fd, bufptr, nbytes);
        // Add a LF/CR to flush the buffer.
        // write(fd, "\r\n", 2);
    
    
        if (ret == 0) {
            dbglogfile << "Couldn't write any bytes to fd #" << fd << endl;
            return ret;
        }
    
        if (ret < 0) {
            dbglogfile << "Couldn't write " << nbytes << " bytes to fd #"
                       << fd << endl;
            return ret;
        }
    
        if (ret > 0) {
            bufptr += ret;            
            if (ret != nbytes) {
                dbglogfile << "wrote " << ret << " bytes to fd #"
                           << fd << " expected " <<  nbytes << endl;
                retries++;
            } else {
#if 0
                dbglogfile << "Wrote " << ret << " bytes to fd #" << fd << endl;
#endif
                return ret;
            }
      
            if (ret == 0) {
                dbglogfile << "Wrote 0 bytes to fd #" << fd << endl;
            }
        }
    }
  
    return ret;
}

Tcpip &
Tcpip::operator = (Tcpip &tcp) 
{
#if 1
    _sockfd = _sockfd;
    _sockIOfd = _sockIOfd;
    _ipaddr = _ipaddr;
    _hostname = _hostname;
    //memcpy(_client, tcp._client, sizeof(sockaddr_in);
    //_proto = strdup(_proto);
    _port = _port;
    _debug = _debug;
#endif
}

#if 0
// Description: Get the hostname of this machine.
const string
Tcpip::netNameGet(void)
{
    char hostname[MAXHOSTNAMELEN];

    if (_hostname.size() == 0) {
        gethostname(hostname, MAXHOSTNAMELEN);
        _hostname = hostname;
    }
    
    return _hostname;
}
#endif

// local Variables:
// mode: C++
// indent-tabs-mode: nil
// End:
