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

#include <libopencraft/base_packet.h>
#include <libopencraft/packet_reader.h>
#include <libopencraft/packet_writer.h>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp> 

#include <string>

class client_connection {
   public:
     client_connection(int sock_fd, std::string client_addr);
     void send_packet(opencraft::packets::opencraft_packet* pack);
     void pinger_thread();
     void handle_client();
     void handle_handshaking();
     void handle_status();
     void handle_login();
     void handle_play();
     int proto_mode;
     bool active;
     std::string _client_addr;
   private:
     double last_sent_ping;
     double last_recv_ping;
     int _sock_fd;
     int entity_id;
     unsigned char game_mode;
     int dimension;
     unsigned char difficulty;
     boost::uuids::uuid uuid;
     std::string username;
     opencraft::packets::packet_reader *client_reader;
     opencraft::packets::packet_writer *client_writer;

};
