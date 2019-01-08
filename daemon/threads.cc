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
#include <queue>
#include <boost/shared_ptr.hpp>
#ifdef HAVE_GETOPT_H
#include <getopt.h>
#else
extern int optind;
extern char *optarg;
#endif

#include "log.h"
#include "ownet.h"
#include "console.h"
#include "database.h"
#include "tcpip.h"
#include "xml.h"
#include "commands.h"
#include "onewire.h"
#include <boost/asio/io_service.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio.hpp>
#include <string>
#include <ctime>

using namespace boost::asio;
using namespace boost::asio::ip;
using namespace std::chrono_literals;
using boost::asio::ip::tcp;

// This queue is used to pass data between the threads.
extern std::mutex queue_lock;
extern std::queue <XML> tqueue;
extern std::condition_variable queue_cond;

void
onewire_handler(Onewire &onewire)
{
    DEBUGLOG_REPORT_FUNCTION;
    BOOST_LOG(lg) << "PowerGuru - 1 Wire Mode";

    std::map<std::string, family_t> table;
    initTable(table);

    if (!onewire.isMounted()) {
        BOOST_LOG(lg) << "WARNING: Couldn't open 1wire file system!";
        return;        
    }
    
#ifdef HAVE_LIBPQ
    Database pdb;
    if (!pdb.openDB()) {
        BOOST_LOG(lg) << "ERROR: Couldn't open database!";
        exit(1);
    }
#endif

    // Open the network connection to the database.
    std::string query = "INSERT INTO onewire VALUES(";
    query += "";
    query += ");";

    std::map<std::string, boost::shared_ptr<onewire_t>> sensors = onewire.getSensors();
    std::map<std::string, boost::shared_ptr<onewire_t>>::iterator it;
    while (onewire.getPollSleep() > 0) {
        std::string query;
        for (it = sensors.begin(); it != sensors.end(); it++) {
            if (it->second->type == TEMPERATURE) {
                boost::shared_ptr<temperature_t> temp(onewire.getTemperature(it->first));
                if (temp == 0) {
                    continue;
                }
                if (temp) {
                    pdb.formatQuery(temp, query);
                    pdb.queryInsert(query, "temperature");
                    continue;
                }
            }
            if (it->second->type == BATTERY) {   
                boost::shared_ptr<battery_t> batt(onewire.getBattery(it->first));
                if (batt) {
                    pdb.formatQuery(batt, query);
                    pdb.queryInsert(query, "battery");
                    continue;
                }
            }
        }
            
        // Don't eat up all the cpu cycles!
        std::this_thread::sleep_for(std::chrono::seconds(onewire.getPollSleep()));
    }
}

void
client_handler(Tcpip &net)
{
    DEBUGLOG_REPORT_FUNCTION;

    retcode_t ret;
    Commands cmd;
    int retries = 3;
    std::string hostname;
    std::string user;
    io_service ioservice;
    tcp::endpoint tcp_endpoint{tcp::v4(), 7654};
    tcp::acceptor tcp_acceptor{ioservice, tcp_endpoint};
    tcp::socket tcp_socket{ioservice};
    std::string data;
    tcp::resolver resolv{ioservice};
    std::array<char, 4096> bytes;

    tcp_acceptor.listen();
    tcp_acceptor.accept(tcp_socket);
    ioservice.run();
    bool loop = true;
    boost::system::error_code error;

    BOOST_LOG_SEV(lg, severity_level::info) << "PowerGuru ready for incoming connections";
    
    // Wait for a simple handshake from the client, a HELO message
    try {
        tcp_socket.read_some(buffer(bytes), error);
    } catch (const std::exception& e) {
        BOOST_LOG_SEV(lg, severity_level::error)
            << "Couldn't read data from PowerGuru client! " << e.what();
        exit(-1);
    }
    // Send a HELO back to Acknowledge the client
    std::string str;
    std::string args = boost::asio::ip::host_name();
    args += " ";
    args += std::getenv("USER");
    cmd.createCommand(Commands::HELO, args, str);
    try {
        boost::asio::write(tcp_socket, buffer(str), error);
    } catch (const std::exception& e) {
        BOOST_LOG_SEV(lg, severity_level::error)
            << "Couldn't write data to PowerGuru server! " << e.what();
        exit(-1);
    }
    
    while (loop) {
        std::memset(bytes.data(), 0, bytes.size());
        tcp_socket.read_some(buffer(bytes), error);
        std::cerr << bytes.data();
        // Client dropped connection
        if (error == boost::asio::error::eof)
            break;
        // if the first character is a <, assume it's in XML formst.
        XML xml;
        if (bytes[0] == '<') {
            std::string str(std::begin(bytes), std::end(bytes));
            if (!str.empty()) {
                xml.parseMem(str);
                if (xml[0]->nameGet() == "helo") {
                    hostname = xml[0]->childGet(0)->valueGet();
                    user = "foo";// xml[data]->childGet(1)->valueGet();
                    BOOST_LOG(lg) << "Incoming connection from user " << user
                                  << " on host " << hostname;
                } else {
                    cmd.execCommand(xml, str);
                    std::lock_guard<std::mutex> guard(queue_lock);
                    tqueue.push(xml);
                    queue_cond.notify_one();
                }
                str.clear();
            }
        }
    }
}

#ifdef BUILD_OWNET
void
ownet_handler(Ownet &ownet)
{
    DEBUGLOG_REPORT_FUNCTION;
    BOOST_LOG(lg) << "PowerGuru - 1 Wire Mode";

    if (!ownet.isConnected()) {
        BOOST_LOG(lg) << "WARNING: Not connected to owserver!";
        return;        
    }
    
#ifdef HAVE_LIBPQ
    Database pdb;
    if (!pdb.openDB()) {
        BOOST_LOG(lg) << "ERROR: Couldn't open database!";
        return;
    }
#endif
    std::map<std::string, family_t> table;
    initTable(table);

    // Open the network connection to the database.
    std::string query = "INSERT INTO onewire VALUES(";
    query += "";
    query += ");";

    std::map<std::string, boost::shared_ptr<onewire_t>> sensors = ownet.getSensors();
    std::map<std::string, boost::shared_ptr<onewire_t>>::iterator it;
    while (ownet.getPollSleep() > 0) {
        std::string query;
        for (it = sensors.begin(); it != sensors.end(); it++) {
            if (it->second->type == TEMPERATURE) {
                boost::shared_ptr<temperature_t> temp(ownet.getTemperature(it->first));
                if (temp == 0) {
                    continue;
                }
                if (temp) {
                    pdb.formatQuery(temp, query);
                    pdb.queryInsert(query, "temperature");
                    continue;
                }
            }
            if (it->second->type == BATTERY) {   
                boost::shared_ptr<battery_t> batt(ownet.getBattery(it->first));
                if (batt) {
                    pdb.formatQuery(batt, query);
                    pdb.queryInsert(query, "battery");
                    continue;
                }
            }
        }
            
        // Don't eat up all the cpu cycles!
        std::this_thread::sleep_for(std::chrono::seconds(ownet.getPollSleep()));
    }
}
#endif

#ifdef BUILD_OUTBACK
void
outback_handler(Ownet &ownet)
{
    DEBUGLOG_REPORT_FUNCTION;
#if 0
    setitem = false;
    getitem = false;
    monitor = false;
    outbackmode = false;
    xantrexmode = false;
    background = false;
    hostname = "localhost";
#endif

#ifdef BUILD_OUTBACK
    // Network daemon/client mode. Normally we're a network daemon that
    // responses to requests by a remote client. Many house networks
    // are behind a firewall, so the daemon can also connect to a
    // publically accessible host to establish the connection the
    // other direction.
    if (daemon || client) {
      
        Msgs msg;
        msg.toggleDebug(true);
        // Make a client connection
        if (client == true) {
            msg.init(hostname);
        }
      
        // Start as a daemon
        if (daemon == true) {
            msg.init(true);
        }
        //msg.methodsDump();          // FIXME: debugging crap
      
        //msg.print_msg(msg.status((meter_data_t *)0));
      
        if (client) {
            msg.writeNet(msg.metersRequestCreate(Msgs::BATTERY_VOLTS));
        }
      
        //      msg.cacheDump();
      
        XML xml;
        unsigned int i;

        vector<const xmlChar *> messages;        
        bool loop = true;
        while (loop) {
            ret = msg.anydata(messages);
            if (ret == ERROR) {
                BOOST_LOG(lg) << "ERROR: Got error from socket " << endl;
                msg.closeNet();
                // wait for the next connection
                if ((ret = msg.newNetConnection(true))) {
                    BOOST_LOG(lg) << "New connection started for remote client."
                               << msg.remoteIP().c_str()
                               << msg.remoteName().c_str() << endl;
                    ret = SUCCESS;        // the error has been handled
                    continue;
                }
            }
            if (messages.size() == 0) {
                BOOST_LOG(lg) << "ERROR: client socket shutdown! " << endl;
            }
            for (i=0; i < messages.size(); i++) {
                cerr << "Got (" << messages.size() << ") messages " << messages[i] << endl;
                string str = (const char *)messages[i];
                delete messages[i];
                if (msg.findTag("command")) {
                    cerr << "Got command message!" << endl;
                }
                if (xml.parseMem(str) == ERROR) {
                    continue;
                }
            }
            messages.clear();
        }
    }
#endif

    BOOST_LOG(lg) << "FIXME: outback_handler() unimplemented";
    // Don't eat up all the cpu cycles!
    std::this_thread::sleep_for(std::chrono::seconds(ownet.getPollSleep()));
}
#endif

#ifdef BUILD_XANTREX
void
xantrex_handler(Ownet &ownet)
{
    DEBUGLOG_REPORT_FUNCTION;
#if 0
    setitem = false;
    getitem = false;
    monitor = false;
    outbackmode = false;
    xantrexmode = false;
    background = false;
    hostname = "localhost";
#endif

    BOOST_LOG(lg) << "FIXME: xantrext_handler() unimplemented";
    // Don't eat up all the cpu cycles!
    std::this_thread::sleep_for(std::chrono::seconds(ownet.getPollSleep()));
}
#endif

// local Variables:
// mode: C++
// indent-tabs-mode: nil
// End:
