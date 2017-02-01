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

class client_connection {
   public:
     client_connection(int sock_fd);
     void send_packet(opencraft::packets::opencraft_packet* pack);
     void handle_client();
     void handle_handshaking();
     void handle_status();
     void handle_login();
     void handle_play();
     int proto_mode;
     bool active;
   private:
     int _sock_fd;
     opencraft::packets::packet_reader *client_reader;
     opencraft::packets::packet_writer *client_writer;

};
