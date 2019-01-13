// 
// Copyright (C) 2005, 2006-2018.
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

#ifndef __TCPUTIL_H__
#define __TCPUTIL_H__

// This is generated by autoconf
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <ifaddrs.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/param.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <cstring>
#include <vector>
#include <map>

#include "log.h"
#include "err.h"

extern const std::string DEFAULTPROTO;
extern const short DEFAULTPORT;

class Tcputil {
public:
    // The interface name is the index field for an array of this data
    struct ipNode {
        std::string ipstr;
        struct saddr;
    };

    Tcputil() {};
    Tcputil(const std::string &host);
    Tcputil(const std::string &host, short port);
    ~Tcputil() {};
    
    // This gets the servent data that contains the port
    // number as specified by it's /etc/services file entry.
    //struct servent *lookupService(int number);
    struct servent *lookupService(const std::string &name) {
        return lookupService(name, "tcp");
    };
    struct servent *lookupService(const std::string &name,
                                  const std::string &protocol);
    struct addrinfo *getAddrInfo(void) {
        return getAddrInfo("localhost", DEFAULTPORT);
    };
    struct addrinfo *getAddrInfo(const std::string& hostname) {
        return getAddrInfo(hostname, DEFAULTPORT);
    };
    struct addrinfo *getAddrInfo(const std::string& hostname, std::uint16_t port);

    const std::string &printIP(struct addrinfo *addr, std::string &str) {
        char address[INET6_ADDRSTRLEN];
        std::memset(address, 0, INET6_ADDRSTRLEN);
        inet_ntop(AF_INET, &((struct sockaddr_in *)addr->ai_addr)->sin_addr,
                  address, INET6_ADDRSTRLEN);
        str = address;
        return str;
    };

    void getInterfaces(void);
    
    int numberOfInterfaces(void) {
        if (_nics.size() == 0) {
            getInterfaces();
        }
        return _nics.size();
    };

    std::string getHostname() {
        return _hostname;
    };
    
    void dump(void);
    
protected:
    std::string                    _hostname;
    struct servent *               _service;
    struct protoent *              _proto;
    std::vector<struct addrinfo *> _addrinfo;
    std::map<std::string, struct ipNode *> _nics;
};

// EOF __TCPUTIL_H__
#endif

// local Variables:
// mode: C++
// indent-tabs-mode: nil
// End:
