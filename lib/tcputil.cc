// 
// Copyright (C) 2005, 2006 - 2018      Free Software Foundation, Inc.
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

#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/if_ether.h>
#include <netdb.h>
#include <unistd.h>
#include <iostream>
#ifndef SIOCGIFCONF
#include <sys/sockio.h>
#endif
#include <map>
#include "tcputil.h"
#include "tcpip.h"
#include "log.h"
#include "err.h"

extern LogFile dbglogfile;

Tcputil::Tcputil(void) : Tcputil("localhost", DEFAULTPORT)
{
//    DEBUGLOG_REPORT_FUNCTION;
}

Tcputil::Tcputil(const std::string &host) : Tcputil(host, DEFAULTPORT)
{
//    DEBUGLOG_REPORT_FUNCTION;
}

Tcputil::Tcputil(const std::string &host, short port)
    :_service(0),
     _proto(0),
     _hostname("localhost")
{
//    DEBUGLOG_REPORT_FUNCTION;
    std::string portstr = std::to_string(port);
    _hostname = host;

    _proto = getprotobyname("tcp");
    _proto = getprotobynumber(IPPROTO_TCP);

#if 0
    // Chances are we don't have an entry in the /etc/services file.
    _service = getservbyport(htons(port), _proto->p_name);
    if (_service) {
        dbglogfile <<  "Found service file entry for " << _service->s_name << std::endl;
    } else {
        dbglogfile << "Services file entry for port " << DEFAULTPORT << " was not found, using defaults" << std::endl;
    }
#endif
}

struct addrinfo *
Tcputil::getAddrInfo(const std::string& hostname, std::uint16_t port)
{
//    DEBUGLOG_REPORT_FUNCTION;

    struct addrinfo hints, *ans = nullptr;
    ::memset(&hints, 0, sizeof(struct addrinfo));

    // 
    if (hostname.empty()) {
        hints.ai_family = AF_UNSPEC;    // Allow IPv4 or IPv6
        hints.ai_flags = AI_PASSIVE;
        //hints.ai_protocol = IPPROTO_TCP; // any protocol
    } else {
        hints.ai_family = AF_INET;  // Allow IPv4 only for now
        hints.ai_flags = 0;
        _hostname = hostname;
    }
    hints.ai_protocol = _proto->p_proto;
    hints.ai_socktype = SOCK_STREAM; // TCP

    std::string portstr = std::to_string(port);
    int ret = 0;
    if (hostname.empty()) {
        ret = ::getaddrinfo(nullptr, portstr.c_str(), &hints, &ans);
    } else {
        ret = ::getaddrinfo(hostname.c_str(), portstr.c_str(), &hints, &ans);
    }
    if (ret != 0) {
        std::cerr << "ERROR: getaddrinfo(" << hostname << ") failed! "
                  << gai_strerror(ret) << std::endl;
        return nullptr;
    }

    // convert the linked list to an array to make it easier to access.
    struct addrinfo *addr = ans;
    if (addr->ai_next == 0) {
        _addrinfo.push_back(addr);
    }
    while (addr->ai_next != 0) {
        std::string str;
        dbglogfile << "The IP number for this connection is " << printIP(addr, str) << std::endl;
        _addrinfo.push_back(addr);
        addr = addr->ai_next;
    }

    return ans;
}

void
Tcputil::getInterfaces(void) {
    // DEBUGLOG_REPORT_FUNCTION;

    struct ifaddrs *addrs,*tmp;
    char address[INET6_ADDRSTRLEN];
    std::string str;

    getifaddrs(&addrs);
    tmp = addrs;
    while (tmp) {
        if (tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_PACKET) {
            struct ipNode *ip = new ipNode;
            std::memset(address, 0, INET6_ADDRSTRLEN);
            struct sockaddr_in *sa = reinterpret_cast<struct sockaddr_in *>(tmp->ifa_addr);
            inet_ntop(AF_INET, &sa->sin_addr, address, INET6_ADDRSTRLEN);
            ip->ipstr = address;
            //std::memcpy((void *)&ip->saddr, (void *)tmp->ifa_addr, sizeof(struct sockaddr));
            _nics[tmp->ifa_name] = ip;
        }
        tmp = tmp->ifa_next;
    }
    freeifaddrs(addrs);
};

Tcputil::~Tcputil(void)
{
    DEBUGLOG_REPORT_FUNCTION;

    std::map<std::string, struct addrinfo *>::iterator it;
//    for (it = _addrinfo.begin(); it != _addrinfo.end(); it++) {
//        freeaddrinfo(*it);
//    }
}

struct servent *
Tcputil::lookupService(const std::string &name, const std::string &protocol)
{
    DEBUGLOG_REPORT_FUNCTION;

    // Get the service entry from  /etc/services for this 
    struct servent *serv = getservbyname(name.c_str(), protocol.c_str());

    if (serv) {
        dbglogfile <<  "Found service file entry for " << name << std::endl;
    } else {
        dbglogfile << "Services file entry " << name << " was not found!" << std::endl;
    }

    return serv;
}

void
Tcputil::dump(void)
{
    DEBUGLOG_REPORT_FUNCTION;

    std::cerr << "Hostname for this connection is: " << _hostname << std::endl;

    std::map<std::string, struct ipNode *>::iterator nit;
    for (nit = _nics.begin(); nit != _nics.end(); nit++) {
        std::cerr << "\tInterface: " << nit->first << " @ " << nit->second->ipstr << std::endl;
    }

    // dump servent data
    if (_service) {
        std::cerr << "Service data:" << std::endl;
        //std::cerr << "\t" << _service->s_name << std::endl;
        //char **s_aliases;
        std::cerr << "\t" << _service->s_port << std::endl;
        std::cerr << "\t" << _service->s_proto << std::endl;
    }
    
    if (_proto) {
        std::cerr << "Proto data:" << std::endl;
        // dump protoent data
        //char **p_aliases;
        std::cerr << "\tNumber: " << (_proto->p_proto ? _proto->p_proto : 0)
                  <<  " (" << (_proto->p_name ? _proto->p_name : "none") << ")"
                  << std::endl;
    }

    // Dump addrinfo data
    const std::string st[] = { "UNSPEC", "SOCK_STREAM", "SOCK_DGRAM", "SOCK_RAW" };
    const std::string ft[] = { "AF_UNSPEC", "AF_LOCAL", "AF_INET", "AF_AX25",
                               "AF_IPX", "AF_APPLETALK", "AF_NETROM",
                               "AF_BRIDGE", "AF_ATMPVC", "AF+X25", "AF_INET6" };
    // We only care about these two values
    char *pt[18];
    pt[getprotobyname("tcp")->p_proto] = "tcp";
    pt[getprotobyname("udp")->p_proto] = "udp";

    std::cerr << "Addrinfo data has: " << _addrinfo.size() << " entries" << std::endl;
    std::vector<struct addrinfo *>::iterator ait;
    for (ait = _addrinfo.begin(); ait != _addrinfo.end(); ait++) {
        if ((*ait)->ai_canonname != 0) {
            std::cerr << "\Hostname: " << (*ait)->ai_canonname << std::endl;
        }
        std::cerr << "\tFlags: " << (*ait)->ai_flags << std::endl;
        std::cerr << "\tFamily: " << ft[(*ait)->ai_family] << std::endl;
        std::cerr << "\tsocktype: " << st[(*ait)->ai_socktype] << std::endl;
        std::cerr << "\tprotocol: " << pt[(*ait)->ai_protocol]<< std::endl;
        //socklen_t _addrinfo->ai_addrlen;
        //sockaddr _addrinfo->ai_addr;
        //std::cerr << *ait->ai_canonname << std::endl;
    }
}

// local Variables:
// mode: C++
// indent-tabs-mode: nil
// End:
