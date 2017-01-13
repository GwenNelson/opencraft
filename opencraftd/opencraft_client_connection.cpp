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
#include <utils.h>

void opencraft_client_connection::start() {
     LOG(info) << "Started connection handling!";
     cur_proto_mode = INIT;
     do_read();
}

void opencraft_client_connection::do_read() {
     _socket.async_read_some(boost::asio::buffer(_data, 1024),
         [this](boost::system::error_code ec, std::size_t length) {
            LOG(debug) << "do_read callback with " << length << " bytes";
            if (!ec) {
                mtx_.lock();
                pending_recv.insert(pending_recv.end(), _data, _data+length);
                do_packet_read();
                do_read();
                mtx_.unlock();
            }
         });
}

void opencraft_client_connection::do_packet_read() {
     // this will attempt to read a packet if there is one
     // basically if we don't have enough bytes to parse a packet, it returns

     LOG(debug) << "do_packet_read start";
     if(pending_recv.size() <= 4)  {
        LOG(debug) << "not enough bytes! Only got " << pending_recv.size();
        return; // not enough bytes yet
     }

     int max_bits = 21;
     if(cur_proto_mode == PLAY) max_bits=32;
     int32_t packlen = parse_var_int(&(pending_recv[0]),(max_bits/8));
     int packlen_len = varint_size(packlen);
     LOG(debug) << "Got a packet coming of size " << packlen;
     
     if(pending_recv.size() >= packlen) {
       LOG(debug) << "Reading in " << packlen << " bytes";
       int packid_offset = packlen_len;
       int32_t pack_id = parse_var_int(&(pending_recv[packid_offset]),4);
       LOG(debug) << "Got a packet of ID " << pack_id << " in protocol mode " << cur_proto_mode;
       int packid_len = varint_size(pack_id);
       int packbody_offset = packid_offset + packid_len;

       // now we build a packet and shove it off to an async handler
       std::vector<unsigned char> new_pack;
       int bodylen = packlen - packlen_len - packid_len;
       new_pack.resize(bodylen);
       new_pack.insert(new_pack.end(), pending_recv[packbody_offset], bodylen);
       pending_recv.erase(pending_recv.begin(), pending_recv.begin() + packlen);

     } else {
       LOG(debug) << "Not enough bytes to read packet yet!";
     }

}
