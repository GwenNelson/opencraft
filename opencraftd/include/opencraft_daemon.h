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

class opencraft_daemon {
   public:
     opencraft_daemon(bool debug_mode, bool daemon_mode, std::string pidfile, std::string install_root, int listen_port);
     void run();

   private:
     void accept_clients();
     void handle_client(int client_sock_fd, struct sockaddr_in client_addr);
     void write_pidfile(std::string filename);
     void setup_daemon();
     void configure_stdio();
     void configure_signals();
     void fork_me_baby();

     bool                    _active;
     bool                    _debug_mode;
     bool                    _daemon_mode;
     std::string             _pidfile;
     std::string             _install_root;
     int                     _listen_port;
     int                     _server_sock_fd;
};
