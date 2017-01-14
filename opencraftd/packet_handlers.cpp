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
#include <version.h>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <jsoncpp/json/value.h>
#include <jsoncpp/json/writer.h>

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
     opencraft_client_connection* client_conn = (opencraft_client_connection*)client;
     LOG(debug) << "Got a status request, building JSON";
     
     Json::Value Version;
     Version["name"]     = OPENCRAFT_SHORT_VER;
     Version["protocol"] = DEFAULT_PROTO;
     
     Json::Value Players;
     Players["online"]   = 0;   // TODO: fix this to actually count clients connected
     Players["max"]      = 100; // TODO: make this a configurable variable

     Json::Value Description;
     Description["text"] = OPENCRAFT_LONG_VER;

     Json::Value resp_val;
     resp_val["version"]     = Version;
     resp_val["players"]     = Players;
     resp_val["description"] = Description;
     
     Json::FastWriter Writer;
     std::string resp_str = Writer.write(resp_val);
     LOG(debug) << "Status response JSON:\n" << resp_str;

     bound_buffer outpack;
     outpack.write_string(resp_str);

     LOG(debug) << "Sending pack status response packet";
     client_conn->send_packet(outpack.peek(outpack.size()), PACKET_ID_STATUS_STATUS_RESPONSE_DOWNSTREAM, outpack.size());
}

void handle_status_ping(void* client, int32_t packlen) {
     LOG(debug) << "Got a status ping";
}

void handle_login_start(void* client, int32_t packlen) {
     opencraft_client_connection* client_conn = (opencraft_client_connection*)client;

     std::string username  = client_conn->recv_buf.read_string();
     client_conn->username = username;

     LOG(debug) << "Got login start for user " << username;
     client_conn->uuid = boost::uuids::random_generator()();

     LOG(debug) << "Sending login success with UUID " << client_conn->uuid;
     bound_buffer outpack;
     outpack.write_string(username);
     outpack.write_string(to_string(client_conn->uuid));

     client_conn->send_packet(outpack.peek(outpack.size()), PACKET_ID_LOGIN_LOGIN_SUCCESS_DOWNSTREAM, outpack.size());
}


