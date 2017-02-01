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

#include <libopencraft/base_packet.h>
#include <libopencraft/packet_reader.h>
#include <libopencraft/packet_writer.h>
#include <libopencraft/proto_constants.h>
#include <libopencraft/packets.autogen.h>

#include <client_connection.h>

using namespace opencraft::packets;

client_connection::client_connection(int sock_fd) {
    this->proto_mode    = OPENCRAFT_STATE_HANDSHAKING;
    this->_sock_fd      = sock_fd;
    this->client_reader = new packet_reader(sock_fd,this->proto_mode,false);
    this->client_writer = new packet_writer(sock_fd);
}

void client_connection::send_packet(opencraft::packets::opencraft_packet* pack) {
    packet_writer tmp_writer(this->_sock_fd);
    tmp_writer.write_pack(pack);
}

void client_connection::handle_handshaking() {
     opencraft_packet *inpack = NULL;
     inpack = this->client_reader->read_pack();

     handshake_handshaking_upstream* hspack = inpack;
     int32_t client_proto_version           = hspack->a;
     std::string server_addr                = hspack->b;
     uint16_t server_port                   = hspack->c;
     int32_t next_state                     = hspack->d;
     
     LOG(info) << "Got handshake from client";
     this->proto_mode                = next_state;
     this->client_reader->proto_mode = next_state;
}

void client_connection::handle_status() {
}

void client_connection::handle_login() {
}

void client_connection::handle_play() {
}

void client_connection::handle_client() {
     switch(this->proto_mode) {
        case OPENCRAFT_STATE_HANDSHAKING:
          this->handle_handshaking();
        break;
        case OPENCRAFT_STATE_STATUS:
          this->handle_status();
        break;
        case OPENCRAFT_STATE_LOGIN:
          this->handle_login();
        break;
        case OPENCRAFT_STATE_PLAY:
          this->handle_play();
        break;
     }
}

