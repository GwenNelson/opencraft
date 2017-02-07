//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 by Gareth Nelson (gareth@garethnelson.com)
//
// This file is part of the OpenCraft server.
//
// The OpenCraft server is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// The OpenCraft server is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with OpenCraft.  If not, see <http://www.gnu.org/licenses/>.
//
// DESCRIPTION:
//     The daemon class
//
//-----------------------------------------------------------------------------

#pragma once

#include <string>
#include <event2/event.h>
#include <event2/listener.h>

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>

#include <map>

#include <client_connection.h>

class opencraft_daemon {
   public:
     opencraft_daemon(bool debug_mode, bool daemon_mode, std::string pidfile, std::string install_root, int listen_port);
     void run();
     double mticks();
     void accept_client_cb(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *address, int socklen);
     boost::asio::io_service _io_service;
   private:
     void write_pidfile(std::string filename);
     void setup_daemon();
     void configure_stdio();
     void configure_signals();
     void fork_me_baby();

     struct event_base      *ev_base;
     struct evconnlistener  *ev_listener;
     std::map<std::string, client_connection*> _clients;

     boost::thread_group     _thread_pool;

     bool                    _active;
     bool                    _debug_mode;
     bool                    _daemon_mode;
     std::string             _pidfile;
     std::string             _install_root;
     int                     _listen_port;
     int                     _server_sock_fd;
};
