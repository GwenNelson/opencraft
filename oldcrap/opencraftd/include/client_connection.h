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
//     Class representing a client connection
//
//-----------------------------------------------------------------------------

#pragma once

class opencraft_daemon;

#include <libopencraft/base_packet.h>
#include <libopencraft/packets.autogen.h>
#include <libopencraft/packet_reader.h>
#include <libopencraft/packet_writer.h>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp> 
#include <boost/thread.hpp>

#include <string>

#include <event2/event.h>
#include <event2/thread.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>

#include <user_profile.h>

using namespace opencraft::packets;

class client_connection {
   public:
     client_connection(std::string client_addr,opencraft_daemon* daemon);

     void close();

     void read_cb(evutil_socket_t fd);
     void write_cb(evutil_socket_t fd);

     void send_packet(opencraft_packet *packet);

     void handle_packet(opencraft_packet *pack);

     void handle_login(login_start_login_upstream* login_pack);
     void handle_handshake(handshake_handshaking_upstream* hspack);

     std::string       _client_addr;
     opencraft_daemon* _daemon;
     user_profile*     _profile;

     struct event* read_ev;   // we need to store these so they can be freed later
     struct event* write_ev;
     int proto_mode;
   private:
     bool killme;
     boost::mutex _mtx;
     void _read_cb(evutil_socket_t fd);
     struct evbuffer* _recvbuf;
     struct evbuffer* _sendbuf;

};
