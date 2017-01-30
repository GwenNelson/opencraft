//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 by Gareth Nelson (gareth@garethnelson.com)
//
// This file is part of OpenCraft.
//
// The OpenCraft client is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// The OpenCraft client is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with OpenCraft.  If not, see <http://www.gnu.org/licenses/>.
//
// DESCRIPTION:
//     A basic client
//
//-----------------------------------------------------------------------------

#include <libopencraft/common.h>
#include <libopencraft/basic_client.h>
#include <libopencraft/packets.autogen.h>

#include <string>
#include <map>
#include <vector>
#include <iostream>

namespace opencraft {
  namespace client {


void basic_client::register_handler(int32_t pack_ident, pack_callback_t cb, void* ctx) {
     this->pack_callbacks[pack_ident].push_back(std::tuple<void*,pack_callback_t>(ctx,cb));
}

void basic_client::on_recv(std::vector<unsigned char> data) {
     std::vector<opencraft::packets::raw_packet> inpacks = this->p_stream.on_recv(data);
     for(int a=0; a< inpacks.size(); a++) {
        if(this->pack_callbacks.find(inpacks[a].pack_ident) != this->pack_callbacks.end()) {
           opencraft::packets::opencraft_packet *inpack = opencraft::packets::opencraft_packet::unpack_packet(this->proto_mode,true,inpacks[a].pack());
           for(int b=0; b != this->pack_callbacks[inpacks[a].pack_ident].size(); b++) {
               if(inpack != NULL) { 
                  std::get<1>(this->pack_callbacks[inpacks[a].pack_ident][b])(std::get<0>(this->pack_callbacks[inpacks[a].pack_ident][b]),inpack);
               }
           }
           if(inpack != NULL) delete inpack;
        }
     }
}

void basic_client::send_pack(opencraft::packets::opencraft_packet *pack) {
     opencraft::packets::raw_packet raw_pack(pack->ident(),pack->pack());
     std::vector<unsigned char> packed = raw_pack.pack();
     for(int i=0; i<packed.size(); i++) {
         this->sendbuf.push_back(packed[i]);
     }
}

void basic_client::send_hs(std::string hostname, int port, int new_proto_mode) {
     opencraft::packets::handshake_handshaking_upstream hspack(OPENCRAFT_PROTOCOL_VERSION,hostname,port,new_proto_mode);
     this->send_pack(&hspack);
     this->proto_mode = new_proto_mode;
}

void basic_client::set_compression(int32_t new_threshold) {
     this->compression_threshold = new_threshold;
     this->compression_enabled = false;
     if(new_threshold >0) this->compression_enabled = true;
}

std::vector<unsigned char> basic_client::on_send() {
     std::vector<unsigned char> retval(this->sendbuf);
     this->sendbuf.clear();
     return retval;
}


}
}
