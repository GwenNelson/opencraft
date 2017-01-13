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
//     Packet handler functions - where the magic happens
//
//-----------------------------------------------------------------------------

#include <common.h>
#include <bbuff.h>

#include <packet_handlers.h>

void handle_handshake(void* client, int32_t packlen) {
     opencraft_client_connection* client_conn = (opencraft_client_connection*)client;

     int32_t proto_ver       = client_conn->recv_buf.read_varint(32);
     std::string server_addr = client_conn->recv_buf.read_string();
     uint16_t server_port    = client_conn->recv_buf.read_ushort();
     int32_t next_state      = client_conn->recv_buf.read_varint(32);

     LOG(debug) << "Got handshake packet: " <<  
                   "Protocol version=" << proto_ver   << ", " <<
                   "Server address="   << server_addr;
     LOG(debug) << "Server port: "      << server_port << ", " <<
                   "Next state: "       << next_state;
     client_conn->cur_proto_mode = (proto_mode_t)next_state;
}

void handle_status_request(void* client, int32_t packlen) {
     LOG(debug) << "Got a status request";
}

void handle_login_start(void* client, int32_t packlen) {
     opencraft_client_connection* client_conn = (opencraft_client_connection*)client;

     std::string username  = client_conn->recv_buf.read_string();
     client_conn->username = username;
     LOG(debug) << "Got login start for user " << username;
}


