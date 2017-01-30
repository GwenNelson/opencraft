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


void basic_client::register_handler(int32_t pack_ident, int32_t _proto_mode, pack_callback_t cb, void* ctx) {
    switch(_proto_mode) {
        case OPENCRAFT_STATE_HANDSHAKING:
          this->pack_callbacks_handshaking[pack_ident].push_back(std::tuple<void*,pack_callback_t>(ctx,cb));
        break;
        case OPENCRAFT_STATE_STATUS:
          this->pack_callbacks_status[pack_ident].push_back(std::tuple<void*,pack_callback_t>(ctx,cb));
        break;
        case OPENCRAFT_STATE_LOGIN:
          this->pack_callbacks_login[pack_ident].push_back(std::tuple<void*,pack_callback_t>(ctx,cb));
        break;
        case OPENCRAFT_STATE_PLAY:
          this->pack_callbacks_play[pack_ident].push_back(std::tuple<void*,pack_callback_t>(ctx,cb));
        break;
     }

}

void basic_client::on_recv(std::vector<unsigned char> data) {
     std::vector<opencraft::packets::raw_packet> inpacks = this->p_stream.on_recv(data);
     std::map<int32_t,std::vector<std::tuple<void*, pack_callback_t> > > callbacks;

     switch(this->proto_mode) {
        case OPENCRAFT_STATE_HANDSHAKING:
          callbacks = this->pack_callbacks_handshaking;
        break;
        case OPENCRAFT_STATE_STATUS:
          callbacks = this->pack_callbacks_status;
        break;
        case OPENCRAFT_STATE_LOGIN:
          callbacks = this->pack_callbacks_login;
        break;
        case OPENCRAFT_STATE_PLAY:
          callbacks = this->pack_callbacks_play;
        break;
     }

     for(int pack_i=0; pack_i < inpacks.size(); pack_i++) {
         opencraft::packets::opencraft_packet *inpack = opencraft::packets::opencraft_packet::unpack_packet(this->proto_mode,true,inpacks[pack_i].pack());

         if(inpack != NULL) if(callbacks.find(inpack->ident()) != callbacks.end()) {
               for(int cb_i=0; cb_i < callbacks[inpack->ident()].size(); cb_i++) {
                      std::get<1>(callbacks[inpack->ident()][cb_i])(std::get<0>(callbacks[inpack->ident()][cb_i]),inpack);
               }
               std::cerr << inpack->name() << std::endl;
               delete inpack;
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
