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
// along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
//
// DESCRIPTION:
//      OpenCraft client connection class - sends and receives packets
//
//-----------------------------------------------------------------------------

#include <cstdlib>
#include <memory>
#include <utility>

#include <common.h>
#include <opencraft_client_connection.h>
#include <packet_handlers.h>

#include <utils.h>
#include <bbuff.h>

void opencraft_client_connection::start() {
     LOG(info) << "Installing packet handlers";

     packet_callbacks[packet_id_t(HANDSHAKING,PACKET_ID_INIT_HANDSHAKE_UPSTREAM)]        = &handle_handshake;
     packet_callbacks[packet_id_t(LOGIN,      PACKET_ID_LOGIN_LOGIN_START_UPSTREAM)]     = &handle_login_start;
     packet_callbacks[packet_id_t(STATUS,     PACKET_ID_STATUS_STATUS_REQUEST_UPSTREAM)] = &handle_status_request;

     LOG(info) << "Started connection handling!";
     cur_proto_mode = HANDSHAKING;
     do_read();
}

void opencraft_client_connection::do_read() {
     _socket.async_read_some(boost::asio::buffer(_data, 1024),
         [this](boost::system::error_code ec, std::size_t length) {
            LOG(debug) << "do_read callback with " << length << " bytes";
            if (!ec) {
                mtx_.lock();
                recv_buf.write(_data, length);
                if(length >= 1) do_packet_read();
                do_read();
                mtx_.unlock();
            }
         });
}

void opencraft_client_connection::do_packet_read() {
     // this will attempt to read a packet if there is one
     // basically if we don't have enough bytes to parse a packet, it returns

     LOG(debug) << "do_packet_read start";

     // peek at the packet length, and return if we don't have the full packet in buffer yet
     int max_bits = 21;
     if(cur_proto_mode == PLAY) max_bits = 32;
     int32_t packlen = parse_var_int(recv_buf.peek(max_bits/8),max_bits/8);
     if(recv_buf.size() < packlen) return;

     packlen = recv_buf.read_varint(max_bits);
     LOG(debug) << "Got a packet of size " << packlen;
     
     int32_t pack_id = recv_buf.read_varint(32);
     
     LOG(debug) << "Packet ID " << pack_id << " received";

     if(packet_callbacks.find(packet_id_t(cur_proto_mode,pack_id)) == packet_callbacks.end()) {
     } else {
       packet_callbacks[packet_id_t(cur_proto_mode,pack_id)]((void*)this,packlen);
     }
     recv_buf.clear_backlog();
     
}
